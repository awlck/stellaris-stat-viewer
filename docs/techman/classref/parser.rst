========================
Core Classes Reference
========================

Parser
------

(Beginning the documentation effort for real now, here comes the first class. Structure
subject to change with future releases. I suppose this can also serve as a reference for
the the syntax of the Paradox script file format)

.. namespace:: Parsing

All the parser-related declarations live in the :any:`Parsing` namespace.

Tokens
******

.. struct:: Token

   A token as produced by the lexer.

   .. member:: unsigned long line

	       The line in which this token begun

   .. member:: unsigned long firstChar

	       The character position within that line at which this token begun

   .. member:: TokenType type

	       The type of this token.

   .. union:: @tokenValue

      The value of this token, with the active element being determined by the
      :member:`Token::type`. Some types, like :enumerator:`TokenType::TT_EQUALS`,
      have no active element at all.

      .. var:: char String[64]
	       
      .. var:: bool Bool

      .. var:: qint64 Int

      .. var:: double Double

   .. member:: @tokenValue tok


.. enum:: TokenType

    Represents the type of a lexed token.

    .. enumerator:: TT_STRING

       Token is a string, e.g. ``"hello"``

    .. enumerator:: TT_OBRACE

       An opening brace ``{``

    .. enumerator:: TT_CBRACE

       A closing brace ``}``

    .. enumerator:: TT_EQUALS

       An equals sign ``=``

    .. enumerator:: TT_BOOL

       A boolean value, `yes` or `no` (stored as a C++ ``bool``)

    .. enumerator:: TT_INT

       An integer value, e.g. ``3``

    .. enumerator:: TT_DOUBLE

       A floating point value, e.g. ``3.5`` (stored as a C++ ``double``)

    .. enumerator:: TT_LT

       A less-than sign ``<``

    .. enumerator:: TT_GT

       A greater-than sign ``>``

    .. enumerator:: TT_NONE

       A non-type used upon initialisation of a :struct:`Token` to indicate that
       the type has not yet been set.

AST Nodes
*********

.. struct:: AstNode

    Represents a node in the parse tree.
   
	.. function:: AstNode *findChildWithName(const char *name) const
   
		Find the first child of this node with the given name.
	  
		:param name: The name of the child to search for
		:return: pointer to the child searched for, or `nullptr` if no child of that name exists.
	
	.. function:: qint64 countChildren() const
		
		Count the children of this node.
	
	.. function:: void merge(AstNode *other)
	
		Merge `other` into this tree. All children of `other` become children of `this`,
		and other is left without any children.
		
		Does nothing if the two nodes don't have the same type.
		
		:param other: The node whose children to adopt.
	
	.. member:: char myName[64] = {'\0'}
	
		The name label of this node.
		
	.. member:: NodeType type = NT_INDETERMINATE
	
		The type of this node.
	
	.. member:: AstNode *nextSibling = nullptr
	
		The next sibling of this node, or `nullptr` if there are no more siblings.
		
	.. member:: RelationType relation = RT_NONE
	
		The relation type of this node (whether the node represents, say, ``hello = 3``
		or ``hello < 3``). Only meaningful for numeric types.
		
	.. union:: NodeValue
	
		The value of this node. The active element is determindes by :member:`AstNode::type`.
		
		.. member:: char Str[64]
		
		.. member:: bool Bool
		
		.. member:: qint64 Int
		
		.. member:: double Double
		
		.. struct:: @childStruct
		
			For compound and list types, this struct stores pointers to the first and last child
			of the node. (The other children are navigates as a singly-linked list via
			:member:`AstNode::nextSibling`.)
			
			.. member:: AstNode *firstChild
			
			.. member:: AstNode *lastChild
	
	.. member:: NodeValue val = {{'\0'}}

.. enum:: NodeType

	Indicates what a given :struct:`AstNode` represents.
	
	.. enumerator:: NT_INDETERMINATE = 0
	
		Not yet set: should appear only while the parser is still constructing the node.
		
	.. enumerator:: NT_COMPOUND
	
		A (named) "compound node" -- this could also be called a "structure" or an "object",
		but I wanted to avoid confusion with the C++ concepts of those names, so "compound"
		will be good enough. Like a JSON ``{ }`` object.
		
		Example: ``test = { hello = world }``
		
	.. enumerator:: NT_STRING
	
		A (named) string node. Example: ``hello = "test"``
	
	.. enumerator:: NT_BOOL
	
		A (named) boolean node. Example: ``hello = yes``
	
	.. enumerator:: NT_INT
	
		A (named) integer node. Example: ``hello = 3``
	
	.. enumerator:: NT_DOUBLE
	
		A (named) floating-point number node. Example: ``hello = 3.5``
		
	.. enumerator:: NT_INTLIST
	
		A (named) integer list node. Example: ``hello = { 1 2 3 }``
		
		.. note::
			If a floating-point number is encountered within an integer list, it is assumed
			that the list should have been a floating-point list all along -- see
			:func:`Parser::fixListType`.
			
	.. enumerator:: NT_INTLIST_MEMBER
	
		A (nameless) integer as a member of an integer list.
		
	.. enumerator:: NT_DOUBLELIST
	
		A (named) floating-point number list node. Example: ``hello = { 1.5 2.5 3.5 }``

		.. note::
			It is legal for integers to appear within a list of floating-point numbers:
			when constructing such a list, ``3`` is simply read as ``3.0``.
		
	.. enumerator:: NT_DOUBLELIST_MEMBER
	
		A (nameless) floating-point number as a member of a list of floating point numbers.
	
	.. enumerator:: NT_COMPOUNDLIST
	
		A (named) list of compounds. Written as ``hello = { { a = b } { c = d } }``.
		
		.. note::
			Unlike JSON arrays, list types in Paradox scripts cannot be arbitrarily nested.
			So the above example is allowed, but we couldn't add more brace pairs around
			``a = b``.
	
	.. enumerator:: NT_COMPOUNTLIST_MEMBER
	
		A (nameless) compound within a list of compounds.
	
	.. enumerator:: NT_STRINGLIST
	
		A (named) string list nome. Example: ``hello = { "there" "general" "kenobi" }``
	
	.. enumerator:: NT_STRINGLIST_MEMBER
	
		A (nameless) string as a member of a string list.
	
	.. enumerator:: NT_BOOLLIST
	
		A (named) boolean list node. Example: ``hello = { yes no yes }``
		
	.. enumerator:: NT_BOOLLIST_MEMBER
	
		A (nameless) boolean as a member of a list of booleans.
		
	.. enumerator:: NT_EMPTY
	
		An empty compound (named), list (named), or compound list member (nameless) node.
		The exact type can't be determined. Example: ``hello = { }``

.. enum:: RelationType

	Indicates the relation type stored in this node. Only meaningful for numeric types,
	and only used within conditionals.
	
	.. enumerator:: RT_NONE = 0
	
		No relation type. Used in list members.
	
	.. enumerator:: RT_EQ
	
		``hello = 3``. This is also the relation type set for all non-numeric node types
		other than list members.
	
	.. enumerator:: RT_GT
	
		``hello > 3``
	
	.. enumerator:: RT_GE
	
		``hello >= 3``
	
	.. enumerator:: RT_LT
	
		``hello < 3``
	
	.. enumerator:: RT_LE
	
		``hello <= 3``
	
Parser Errors
*************

.. enum:: ParseErr

	Represents the types of parser (and lexer) errors that can occur.
	
	.. enumerator:: PE_NONE
	
		No error.
	
	.. enumerator:: PE_INVALID_IN_COMPOUND
	
		Invalid token while constructing a compound
	
	.. enumerator:: PE_INVALID_AFTER_NAME
	
		Invalid token after a name (expected =, >, <)
	
	.. enumerator:: PE_INVALID_AFTER_EQUALS
	
		Invalid token after equals (expected a value or ``{``)
	
	.. enumerator:: PE_INVALID_AFTER_RELATION
	
		Invalid token after or within a relation.
	
	.. enumerator:: PE_INVALID_AFTER_OPEN
	
		Invalid token after an open brace.
	
	.. enumerator:: PE_INVALID_COMBO_AFTER_OPEN
	
		Invalid second token after open brace. Occurs when types of the two
		tokens after an open brace can't combine into any node type.
	
	.. enumerator:: PE_INVALID_IN_INT_LIST
	
		Invalid token in integer list. Occurs when anything other than an
		integer, a floating-point number, or a closing brace appears within
		an integer list.
	
	.. enumerator:: PE_INVALID_IN_DOUBLE_LIST
	
		Invalid token in double list. Occurs when anything other than a
		floating-point number, an integer, or a closing brace appears within
		a floating-point number list.
	
	.. enumerator:: PE_INVALID_IN_COMPOUND_LIST
	
		Invalid token in compound list. Occurs when anything other than an
		open brace or a closing brace appears within a compound list. (Errors
		within the nested compounds are reported as this.)
	
	.. enumerator:: PE_INVALID_IN_STRING_LIST
	
		Invalid token in string list. Occurs when anything other than a string
		or a closing brace appears within a sting list.
	
	.. enumerator:: PE_INVALID_IN_BOOL_LIST
	
		Invalid token in bool list. Occurs when anything other than a boolean or
		a closing brace appears within a boolean list.
	
	.. enumerator:: PE_UNEXPECTED_END
	
		Unexpected end of input. Occurs when end of input is reached while there
		are still unfinished nodes on the parser's stack (other than the implicit
		root node).
	
	.. enumerator:: PE_TOO_MANY_CLOSE_BRACES
	
		Too many closing braces. Occurs when extraneous closing braces are present
		in the input that cause the implicit root node to be closed.
	
	.. enumerator:: LE_INVALID_INT
	
		Lexer error: invalid integer literal.
	
	.. enumerator:: LE_INVALID_DOUBLE
	
		Lexer error: invalid floating-point literal.
		
	.. enumerator:: PE_CANCELLED
	
		Not an error, but handled in the same manner. Occurs when the frontend
		indicates that the user wishes to cancel the ongoing operation.

.. function:: QString getErrorDescription(ParseError etype)

	Get a textual description of the given error that can be used in error messages.
	
	:param etype: The error type to describe.

.. struct:: ParserError

	Stores information about a parser error.
	
	Occasionally thrown from the lexer back to the main parser function, but does not
	inherit from :class:`std::Exception`.
	
	.. member:: ParseErr etype
	
		The type of error that has occurred
	
	.. member:: Token erroredToken
	
		The token that caused the error.
		
		.. note::
			For lexer errors, this will have a type of :enumerator:`TokenType::TT_NONE`,
			with the problematic literal stored in :var:`Token::tok.String <Token::@tokenValue::String>`.

Memory Buffers
**************

Until version 0.4.1 (included), SSV used :class:`QTextStream`, a wrapper which
provides text I/O both from files and from byte arrays in memory. This was
convenient, but very slow -- in fact, profiling showed that the parser was
spending most of its time in functions related to :class:`QTextStream`. This
replacement loads the entire file into memory (or creates a copy of the source
array) and has a few simple ``stdio.h``-like functions for accessing the array
one byte at a time. (These functions are defined right in the header and declared
``inline`` for maximum performance.)

.. class:: MemBuf

	An array and a couple of functions for accessing it one byte at a time.
	
	A :class:`MemBuf` is not copyable.
	
	.. function:: MemBuf(char *area, size_t size)
	
		Constructor. Takes ownership of the memory pointed to by ``area``, which
		will be freed when the `MemBuf` is destroyed.
		
		:param area: Pointer to a memory area of which to take ownership
		:param size: Extent of that memory area
	
	.. function:: MemBuf(const QByteArray &arr)
	
		Constructor. Copy the memory managed by the :class:`QByteArray`.
		
		:param arr: The array to copy
	
	.. function:: MemBuf(QFile &file)
	
		Constructor. Read the entirety of `file`.
		
		:param file: The file to read, assuming the file has been opened and is ready for reading.
	
	.. function:: char getc()
	
		Get the next character from the file (buffer), or indicate EOF
	
	.. function:: bool eof()
	
		Whether or not the end of the file (buffer) has been reached.
	
	.. function:: void rewind()
	
		Return to the beginning of the buffer.
	
	.. function:: size_t size()
	
		Get the total size of the buffer.
	
	.. member:: private char *buf
	
		The memory area referenced by this :class:`MemBuf`.
	
	.. member:: private size_t location
	
		The current read position in the buffer. Set to 0 by :func:`rewind` and
		advances every time :func:`getc` is called.
	
	.. member:: private size_t size_
	
		The total size of the buffer.
	
Parser Proper
*************

The parsing action takes place within the following parser class.

.. class:: Parser : public QObject

	.. function:: Parser(MemBuf &data, FileType ftype, QString filename = QString(), QObject *parent = nullptr)
	
		Constructor. Prepare to parse the text in the given buffer, assuming it
		is content from a file of the given :enum:`FileType`.
	
	.. function:: AstNode *parse()
	
		This lengthy function is responsible for the main parsing work. On success,
		a pointer to the root node is returned. On failure, `nullptr` is returned,
		and the caller should call :func:`getLatestParserError` to find out what
		went wrong.
		
		.. note::
			Do any processing with the results before deleting the Parser object --
			doing so will also deallocate the entire parse tree.
	
	.. function:: void cancel()
	
		Indicate that the user wishes for the parsing process to be aborted.
	
	.. function:: ParserError getLatestParserError() const
	
		Gets the latest parser error.
		
	.. function:: void progress(Parser *parser, qint64 current, qint64 total)
	
		A signal occasionally emitted by the parser to indicate the current progress.
		
		The widgets frontend connects to this to update the progress bar.
	
	.. function:: private Token getNextToken()
	
		Get the next token from the queue, running the lexer to refill it if necessary.
	
	.. function:: private int lex(int atLeast = 0)
	
		The lexing function. It puts tokens into the :member:`lexQueue`.
		
		:param atLeast: Attempt to provide this many tokens. Can be more if the last \
			token ends in a special character, or less if the file ends. If this is zero, \
			use :member:`queueCapacity`.
		:returns: The number of tokens actually read.
	
	.. function:: private TokenType lookahead(int n)
	
		Attempt to look ahead and determine the type of the token `n` positions ahead, with
		``1`` meaning the first token in the queue. Runs the lexer if that many tokens aren't
		available.
		
		:param n: Number of tokens to look ahead.
		:return: Type of that token, or :enumerator:`TokenType::TT_NONE` if that many tokens \
			can't be provided.
	
	.. function:: private AstNode *createNode()
	
		Allocates a new AstNode and adds it to :member:`the list of nodes <allCreatedNodes>`.

	.. function:: static void fixListType(AstNode *list)

		If the given node is an integer list, change its type to a floating-point
		list and convert all its list members as well.

		This is necessary because game version 2.6. "Verne" (and presumably newer
		ones as well), floating-point numbers that happen to be "round" integers
		(like ``3.0``) are stored as integers (``3``), so the list ```l = { 1 2 3.5 }``
		would initially be created as an integer list. Upon reading ``3.5``, the parser
		calls this function to convert the list, and all subsequently read integers
		will be automatically converted to floating-point numbers as well.
		
	.. member:: private bool lexerDone = false
	
		Whether or not the lexer has reached the end of the file.
	
	.. member:: private bool shouldCancel = false
	
		Whether or not the parsing process needs to be aborted.
	
	.. member:: private MemBuf &data
	
		The memory buffer from which to read.
	
	.. member:: private FileType fileType
	
		The type of file being read. Currently, this setting makes no difference.
	
	.. member:: private QString filename
	
		The name of the file being read. Useful for error reporting.
	
	.. member:: private QQueue<Token> lexQueue
	
		The queue in which the lexer puts the tokens it reads
	
	.. member:: private qint64 totalProgress = 0
		
		The number of characters that the lexer has processed. Used in progress reporting.
	
	.. member:: private qint64 totalSize
	
		The total size of the file that needs to be read. Used in progress reporting.
	
	.. member:: private std::forward_list<AstNode> allCreatedNodes
	
		Stores all created nodes so we can deallocate them once the Parser object is deleted.
	
	.. member:: private static constexpr int queueCapacity = 50
	
		The maximum size of the lexer queue.
	
	.. member:: private unsigned long line = 1
	.. member:: private unsigned long charPos = 0
	
		The current line and character position within the line, persisted here between
		lexer calls. Used for error reporting.
	
	.. member:: private ParserError latestParserError
	
		The current parser errror.
	
	.. member:: private unsigned int lexCalls1 = 0
	
		The number of times the lexer has been called. This is used to limit the rate at
		which progress updates are made, as emitting a Qt signal is a somewhat expensive
		operation.
		
		Currently, a progress update is sent by :func:`getNextToken` after every 100th call
		to :func:`lex`.
		
.. enum-class:: FileType

	Indicates what type a file has.
	
	.. enumerator:: SaveFile
	
		A save-game file.
	
	.. enumerator:: GameFile
	
		A file provided with the game itself
	
	.. enumerator:: ModFile
	
		A file provided by a mod. Currently unused.
	
	.. enumerator:: NoFile
	
		No file at all. Currently used only for the unit tests.

The 3.0 Hack
^^^^^^^^^^^^

The 3.0 update brought the revamped intel and espionage systems. While generally
well-regarded by players, it has made parsing the save file somewhat more awkward,
as the intel info is stored in a structure like this:

.. code-block:: python

	intel={  # [1]
	    {  # [2]
	        56 {  # [3] -- note the absence of an equals sign
	            intel=30
	            stale_intel={
	            }
	        }
	    }
		# ... and so on ...
	}

So... what is the type of the node that begins at ``[2]``? Some kind of *funky pair*,
consisting of a (nameless) integer and a (nameless) compound? And I suppose that
would then make ``intel`` (beginning at ``[1]``) a *funky pair list*.

But since we're not currently interested in the intel data, we don't really need to
worry about what that syntax actually means to the game -- we can simply fudge the
syntax enough to read past these sections. The "fix", as it were, is relatively simple:
when an open brace directly follows a name (in this case, ``56``), and the name of the
grandparent node is either ``intel`` or ``federation_intel``, then continue parsing as
if an equals sign were present. (If the grandparent node's name doesn't match either of
the two, throw the error as per usual.) That way, the node at ``[3]`` is read as a
compound, the node at ``[2]`` is a compound list member (with one entry), and the node
at ``[1]`` (``intel`` or ``federation_intel``) is a compound list, which seems like a
reasonable enough approximation.

Internals
*********

These are declared within ``parser.cpp`` and therefore not available to other components.

.. function:: static bool typeHasChildren(NodeType t)

	Indicates whether it is legal for the given :enum:`NodeType` to have children.

.. enum-class:: State

	Represents the parser's internal state. We use this, along with a giant ``switch``
	statement and an :class:`std::stack`, to keep track of the parser's state, instead
	of making function calls like a traditional recursive-descent parser might.
