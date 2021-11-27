/* parser.cpp: A parser for PDS script/declaration files.
 *
 * Copyright 2019-2021 Adrian "ArdiMaster" Welcker
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "parser.h"

#include <stack>
#include <utility>

#include <stdio.h>
#include <locale.h>

#define everyNth(which, n, what) do { if ((((which)++) % (n)) == 0) {(what); (which) = 1;} } while (0)

namespace Parsing {
	// Indicates whether this AST node type can have children.
	static inline bool typeHasChildren(NodeType t) {
		switch (t) {
			case NT_COMPOUND:
			case NT_INTLIST:
			case NT_DOUBLELIST:
			case NT_COMPOUNDLIST:
			case NT_STRINGLIST:
			case NT_BOOLLIST:
				return true;
			default:
				return false;
		}
	}

	// Merges the given AstNode into this by appending all its children to our list of children.
	// This is not a copy operation: the other node is left childless.
	void AstNode::merge(Parsing::AstNode *other) {
		if (type != other->type || !typeHasChildren(type)) return;
		if (other->val.firstChild != nullptr) {
			if (this->val.firstChild != nullptr) {
				this->val.lastChild->nextSibling = other->val.firstChild;
			} else {
				this->val.firstChild = other->val.firstChild;
			}
			this->val.lastChild = other->val.lastChild;
			other->val.firstChild = nullptr;
			other->val.lastChild = nullptr;
		}
	}

	// Iterates through our children to find the one called `name', if any.
	AstNode* AstNode::findChildWithName(const char *name) const {
		if (type != NT_COMPOUND) return nullptr;
		if (this->val.firstChild == nullptr) return nullptr;
		AstNode *child = this->val.firstChild;
		do {
			if (strcmp(child->myName, name) == 0) return child;
			child = child->nextSibling;
		} while (child);
		return nullptr;
	}

	// Counts the children of this node.
	qint64 AstNode::countChildren() const {
		if (!typeHasChildren(type)) return -1;
		if (this->val.firstChild == nullptr) return 0;
		qint64 childCount = 0;
		AstNode *child = this->val.firstChild;
		do {
			childCount++;
			child = child->nextSibling;
		} while (child);
		return childCount;
	}

	// For debugging: print the parse tree starting at this node.
	void printParseTree(const AstNode *tree, int indent, bool toplevel) {
		comeagain:
		if (!tree) return;
		for (int i = 0; i < indent; i++) {
			printf("| ");
		}
		printf("+ ");
		switch (tree->type) {
			case NT_COMPOUND:
			case NT_STRING:
			case NT_BOOL:
			case NT_INT:
			case NT_DOUBLE:
			case NT_INTLIST:
			case NT_DOUBLELIST:
			case NT_COMPOUNDLIST:
			case NT_STRINGLIST:
			case NT_BOOLLIST:
			case NT_EMPTY:
				printf("%s", tree->myName);
				break;
			case NT_INDETERMINATE:
				printf("??? (BUG: Indeterminate Node)\n");
				return;
			default: break;
		}
		switch (tree->type) {
			case NT_COMPOUND:
				printf(" (Compound)\n");
				printParseTree(tree->val.firstChild, indent + 1);
				break;
			case NT_STRING:
				printf(" (String) = %s\n", tree->val.Str);
				break;
			case NT_BOOL:
				printf(" (Bool) = %s\n", tree->val.Bool ? "yes" : "no");
				break;
			case NT_INT:
				printf(" (Int) ");
				switch (tree->relation) {
					case RT_EQ:
						printf("=");
						break;
					case RT_GT:
						printf(">");
						break;
					case RT_GE:
						printf(">=");
						break;
					case RT_LT:
						printf("<");
						break;
					case RT_LE:
						printf("<=");
						break;
					case RT_NONE:
						printf("(BUG: RT_NONE)");
						break;
				}
				printf(" %lld\n", tree->val.Int);
				break;
			case NT_DOUBLE:
				printf(" (Double) ");
				switch (tree->relation) {
					case RT_EQ:
						printf("=");
						break;
					case RT_GT:
						printf(">");
						break;
					case RT_GE:
						printf(">=");
						break;
					case RT_LT:
						printf("<");
						break;
					case RT_LE:
						printf("<=");
						break;
					case RT_NONE:
						printf("(BUG: RT_NONE)");
				}
				printf(" %f\n", tree->val.Double);
				break;
			case NT_INTLIST:
				printf(" (Int List)\n");
				printParseTree(tree->val.firstChild, indent + 1);
				break;
			case NT_INTLIST_MEMBER:
				printf("%llu\n", tree->val.Int);
				break;
			case NT_DOUBLELIST:
				printf(" (Double List)\n");
				printParseTree(tree->val.firstChild, indent + 1);
				break;
			case NT_DOUBLELIST_MEMBER:
				printf("%f\n", tree->val.Double);
				break;
			case NT_COMPOUNDLIST:
				printf(" (Compound List)\n");
				printParseTree(tree->val.firstChild, indent + 1);
				break;
			case NT_COMPOUNDLIST_MEMBER:
				printf("(Compound List Member)\n");
				printParseTree(tree->val.firstChild, indent + 1);
				break;
			case NT_STRINGLIST:
				printf(" (String List)\n");
				printParseTree(tree->val.firstChild, indent + 1);
				break;
			case NT_STRINGLIST_MEMBER:
				printf("%s\n", tree->val.Str);
				break;
			case NT_BOOLLIST:
				printf(" (Bool List)\n");
				printParseTree(tree->val.firstChild, indent + 1);
				break;
			case NT_BOOLLIST_MEMBER:
				printf("%s\n", tree->val.Bool ? "yes" : "no");
				break;
			case NT_EMPTY:
				printf(" (Empty)\n");
				break;
			case NT_INDETERMINATE:
				Q_UNREACHABLE();
		}
		if (tree->nextSibling && toplevel) {
			// shoddily avoid recursion
			tree = tree->nextSibling;
			goto comeagain;
		}
	}

	// Returns a textual description for the given `ParseErr' enum entry.
	QString getErrorDescription(ParseErr etype) {
		switch(etype) {
			case PE_NONE:
				return QObject::tr("No error.");
			case PE_INVALID_IN_COMPOUND:
				return QObject::tr("Invalid token in Compound node.");
			case PE_INVALID_AFTER_NAME:
				return QObject::tr("Invalid token after name.");
			case PE_INVALID_AFTER_EQUALS:
				return QObject::tr("Invalid token after equals.");
			case PE_INVALID_AFTER_RELATION:
				return QObject::tr("Invalid token after relation.");
			case PE_INVALID_AFTER_OPEN:
				return QObject::tr("Invalid token after open brace.");
			case PE_INVALID_COMBO_AFTER_OPEN:
				return QObject::tr("Invalid combination of tokens after open brace.");
			case PE_INVALID_IN_INT_LIST:
				return QObject::tr("Invalid token in int list.");
			case PE_INVALID_IN_DOUBLE_LIST:
				return QObject::tr("Invalid token in double list.");
			case PE_INVALID_IN_COMPOUND_LIST:
				return QObject::tr("Invalid token in compound list.");
			case PE_INVALID_IN_STRING_LIST:
				return QObject::tr("Invalid token in string list.");
			case PE_INVALID_IN_BOOL_LIST:
				return QObject::tr("Invalid token in bool list.");
			case PE_UNEXPECTED_END:
				return QObject::tr("Unexpected end of file.");
			case PE_TOO_MANY_CLOSE_BRACES:
				return QObject::tr("Too many closing braces.");
			case LE_INVALID_INT:
				return QObject::tr("Invalid int literal.");
			case LE_INVALID_DOUBLE:
				return QObject::tr("Invalid double literal.");
			case PE_CANCELLED:
				return QObject::tr("Parsing cancelled (no error).");
		}
		return QStringLiteral("??? (BUG: unknown error type.)");
	}

	MemBuf::MemBuf(char *area, size_t size) : buf(area), location(0), _size(size) {}
	MemBuf::MemBuf(const QByteArray &arr) {
		_size = arr.size();
		buf = static_cast<char *>(malloc(_size));
		memcpy(buf, arr.data(), _size);
		location = 0;
	}
	MemBuf::MemBuf(QFile &file) {
		_size = file.size();
		buf = static_cast<char *>(malloc(_size));
		file.read(buf, _size);
		location = 0;
	}
	MemBuf::~MemBuf() {
		free(buf);
	}

	Parser::Parser(Parsing::MemBuf &data, Parsing::FileType ftype, QString filename, QObject *parent)
		: QObject(parent), data(data), fileType(ftype), filename(std::move(filename)), totalSize(data.size()) {}

	Parser::~Parser() {
		for (auto *block: nodeStorageBlocks) {
			delete[] block;
		}
	}

	// create a new AstNode, adding it to the list of all nodes.
	inline AstNode* Parser::createNode() {
		if (nextNodeToUse == nullptr || nextNodeToUse > lastNodeInBlock) {
			nextNodeToUse = new AstNode[nodesAtOnce];
			lastNodeInBlock = nextNodeToUse + nodesAtOnce - 1;
			nodeStorageBlocks.push_back(nextNodeToUse);
		}
		return nextNodeToUse++;
	}

	// Represents the parser's internal state.
	enum class State {
		CompoundRoot,
		HaveName,
		HaveNameEquals,
		HaveNameGt,
		HaveNameGtEq,
		HaveNameLt,
		HaveNameLtEq,
		HaveNameOpen,
		BegunIntList,
		BegunDoubleList,
		BegunCompoundList,
		BegunStringList,
		BegunBoolList
	};

#define ADD_AS_CHILD(node) do { \
if (things.top()->val.firstChild) { things.top()->val.lastChild->nextSibling = (node); things.top()->val.lastChild = (node); } \
else { things.top()->val.firstChild = (node); things.top()->val.lastChild = (node); } \
} while (0)

#define PARSE_ERROR(error) do { latestParserError = { (error), currentToken }; return nullptr; } while (0)

	// This somewhat elephantine function is responsible for constructing the parse tree from the lexer output.
	AstNode* Parser::parse() {
		try {
			lex();  // Initially fill token queue
		} catch (const ParserError &e) {
			latestParserError = e;
			return nullptr;
		}
		// Create a root node that will encompass the entire file.
		AstNode *root = createNode();
		root->type = NT_COMPOUND;
		strcpy(root->myName, "tree_root");
		std::stack<AstNode *> things;  // Explicitly use a stack instead of using recursion.
		things.push(root);
		State state = State::CompoundRoot;
		Token currentToken = {0, 0, TT_NONE, {{'\0'}}};

		while ((!lexerDone || !lexQueue.empty()) && !shouldCancel) {
			try {
				currentToken = getNextToken();
			} catch (const ParserError &e) {
				if (lexerDone) break;  // in case the first token the lexer encounters is EOF
				latestParserError = e;
				return nullptr;
			}
			switch (state) {
			case State::CompoundRoot:
				if (currentToken.type == TT_STRING) {
					state = State::HaveName;
					AstNode *nextNode = createNode();
					memcpy(nextNode->myName, currentToken.tok.String, 64);
					ADD_AS_CHILD(nextNode);
					things.push(nextNode);
				} else if (currentToken.type == TT_INT) {
					state = State::HaveName;
					AstNode *nextNode = createNode();
					QString tmp = QString::number(currentToken.tok.Int);
					strcpy(nextNode->myName, tmp.toUtf8().data());
					ADD_AS_CHILD(nextNode);
					things.push(nextNode);
				} else if (currentToken.type == TT_CBRACE) {
					things.pop();
					if (things.empty()) PARSE_ERROR(PE_TOO_MANY_CLOSE_BRACES);
					if (things.top()->type == NT_COMPOUNDLIST) state = State::BegunCompoundList;
				} else {
					PARSE_ERROR(PE_INVALID_IN_COMPOUND);
				}
				break;
			case State::HaveName:  // Having read a name
				if (currentToken.type == TT_EQUALS) state = State::HaveNameEquals;
				else if (currentToken.type == TT_GT) state = State::HaveNameGt;
				else if (currentToken.type == TT_LT) state = State::HaveNameLt;
				else if (currentToken.type == TT_OBRACE) {
					/*   HACKY FIX FOR 3.0 SAVE FILES
					* In the intel section of the save file, the game generates weird structures like this:
					* intel={  # type: funky pair list...?
					* 	{  # type: funky pair...?
					* 		56 {  # note the lack of an equals sign
					* 			intel=30
					* 			stale_intel={
					* 			}
					* 		}
					* 	}
					* 	# and so on
					* }
					* Since we aren't really interested in that part of the file and just want to read past it,
					* act as if the equals sign was present, making `intel' a compound list.
					*/
					// (But only if our grandparent node is indeed called `intel' or `federation_intel'.)
					AstNode *tmpSelf = things.top();
					things.pop();
					AstNode *tmpParent = things.top();
					things.pop();
					if (strncmp(things.top()->myName, "intel", 64) == 0 ||
						strncmp(things.top()->myName, "federation_intel", 64) == 0) {  // hack applies
						things.push(tmpParent);
						things.push(tmpSelf);
						// act as if we'd read an equals sign as well.
						state = State::HaveNameOpen;
					}
				}
				else PARSE_ERROR(PE_INVALID_AFTER_NAME);
				break;
			case State::HaveNameEquals:  // Having read a name immediately followed by an equals sign
				switch (currentToken.type) {
				case TT_OBRACE:  // Could be compound or list
					state = State::HaveNameOpen;
					break;
				case TT_INT:  // something simple like "stuff = 30"
					state = State::CompoundRoot;
					things.top()->type = NT_INT;
					things.top()->val.Int = currentToken.tok.Int;
					things.top()->relation = RT_EQ;
					things.pop();
					break;
				case TT_DOUBLE:  // something similarly simple like "stuff = 23.5"
					state = State::CompoundRoot;
					things.top()->type = NT_DOUBLE;
					things.top()->val.Double = currentToken.tok.Double;
					things.top()->relation = RT_EQ;
					things.pop();
					break;
				case TT_BOOL:  // something even simpler like "stuff = yes"
					state = State::CompoundRoot;
					things.top()->type = NT_BOOL;
					things.top()->val.Bool = currentToken.tok.Bool;
					// "stuff > yes" wouldn't really make sense, but for the sake of completeness...
					things.top()->relation = RT_EQ;
					things.pop();
					break;
				case TT_STRING:
					state = State::CompoundRoot;
					things.top()->type = NT_STRING;
					memcpy(things.top()->val.Str, currentToken.tok.String, 64);
					// again, kinda redundant, but...
					things.top()->relation = RT_EQ;
					things.pop();
					break;
				default:
					PARSE_ERROR(PE_INVALID_AFTER_EQUALS);
				}
				break;
			case State::HaveNameGt:
				if (currentToken.type == TT_EQUALS) state = State::HaveNameGtEq;
				else if (currentToken.type == TT_INT) {  // "stuff > 30"
					state = State::CompoundRoot;
					things.top()->type = NT_INT;
					things.top()->val.Int = currentToken.tok.Int;
					things.top()->relation = RT_GT;
					things.pop();
				} else if (currentToken.type == TT_DOUBLE) {  // "stuff > 23.5"
					state = State::CompoundRoot;
					things.top()->type = NT_DOUBLE;
					things.top()->val.Double = currentToken.tok.Double;
					things.top()->relation = RT_GT;
					things.pop();
				} else PARSE_ERROR(PE_INVALID_AFTER_RELATION);
				break;
			case State::HaveNameGtEq:
				if (currentToken.type == TT_INT) {  // "stuff >= 30"
					state = State::CompoundRoot;
					things.top()->type = NT_INT;
					things.top()->val.Int = currentToken.tok.Int;
					things.top()->relation = RT_GE;
					things.pop();
				} else if (currentToken.type == TT_DOUBLE) {  // "stuff >= 23.5"
					state = State::CompoundRoot;
					things.top()->type = NT_DOUBLE;
					things.top()->val.Double = currentToken.tok.Double;
					things.top()->relation = RT_GE;
					things.pop();
				} else PARSE_ERROR(PE_INVALID_AFTER_RELATION);
					break;
			case State::HaveNameLt:
				if (currentToken.type == TT_EQUALS) state = State::HaveNameLtEq;
				else if (currentToken.type == TT_INT) {  // "stuff < 30"
					state = State::CompoundRoot;
					things.top()->type = NT_INT;
					things.top()->val.Int = currentToken.tok.Int;
					things.top()->relation = RT_LT;
					things.pop();
				} else if (currentToken.type == TT_DOUBLE) {  // "stuff < 23.5"
					state = State::CompoundRoot;
					things.top()->type = NT_DOUBLE;
					things.top()->val.Double = currentToken.tok.Double;
					things.top()->relation = RT_LT;
					things.pop();
				} else PARSE_ERROR(PE_INVALID_AFTER_RELATION);
				break;
			case State::HaveNameLtEq:
				if (currentToken.type == TT_INT) {  // "stuff <= 30"
					state = State::CompoundRoot;
					things.top()->type = NT_INT;
					things.top()->val.Int = currentToken.tok.Int;
					things.top()->relation = RT_LE;
					things.pop();
				} else if (currentToken.type == TT_DOUBLE) {  // "stuff <= 23.5"
					state = State::CompoundRoot;
					things.top()->type = NT_DOUBLE;
					things.top()->val.Double = currentToken.tok.Double;
					things.top()->relation = RT_LE;
					things.pop();
				} else PARSE_ERROR(PE_INVALID_AFTER_RELATION);
				break;
			case State::HaveNameOpen:  // "stuff = {"
				switch (currentToken.type) {
				case TT_STRING:
					TokenType nextType;
					try {  // compound or string list
						nextType = lookahead(1);
					} catch (const ParserError &e) {
						latestParserError = e;
						return nullptr;
					}
					if (nextType == TT_EQUALS || nextType == TT_GT || nextType == TT_LT) {
						things.top()->type = NT_COMPOUND;
						AstNode *nextNode = createNode();
						nextNode->type = NT_INDETERMINATE;
						state = State::HaveName;
						memcpy(nextNode->myName, currentToken.tok.String, 64);
						ADD_AS_CHILD(nextNode);
						things.push(nextNode);
					} else if (lookahead(1) == TT_STRING || lookahead(1) == TT_CBRACE) {
						state = State::BegunStringList;
						things.top()->type = NT_STRINGLIST;
						AstNode *member = createNode();
						member->type = NT_STRINGLIST_MEMBER;
						memcpy(member->val.Str, currentToken.tok.String, 64);
						ADD_AS_CHILD(member);
					} else PARSE_ERROR(PE_INVALID_COMBO_AFTER_OPEN);
					break;
				case TT_INT: 
					try {
						if (lookahead(1) == TT_INT || lookahead(1) == TT_DOUBLE || lookahead(1) == TT_CBRACE) {
							state = State::BegunIntList;
							things.top()->type = NT_INTLIST;
							AstNode *member = createNode();
							member->type = NT_INTLIST_MEMBER;
							member->val.Int = currentToken.tok.Int;
							ADD_AS_CHILD(member);
						} else if (lookahead(1) == TT_EQUALS) {
							things.top()->type = NT_COMPOUND;
							AstNode *nextNode = createNode();
							nextNode->type = NT_INDETERMINATE;
							state = State::HaveName;
							QString tmp = QString::number(currentToken.tok.Int);
							strcpy(nextNode->myName, tmp.toUtf8().data());
							ADD_AS_CHILD(nextNode);
							things.push(nextNode);
						} else PARSE_ERROR(PE_INVALID_COMBO_AFTER_OPEN);
					} catch (const ParserError &e) {
						latestParserError = e;
						return nullptr;
					}
					break;
				case TT_DOUBLE: {
					state = State::BegunDoubleList;
					things.top()->type = NT_DOUBLELIST;
					AstNode *member = createNode();
					member->type = NT_DOUBLELIST_MEMBER;
					member->val.Double = currentToken.tok.Double;
					ADD_AS_CHILD(member);
				}
					break;
				case TT_BOOL: {
					state = State::BegunBoolList;
					things.top()->type = NT_BOOLLIST;
					AstNode *member = createNode();
					member->type = NT_BOOLLIST_MEMBER;
					member->val.Bool = currentToken.tok.Bool;
					ADD_AS_CHILD(member);
				}
					break;
				case TT_OBRACE: {
					state = State::CompoundRoot;
					things.top()->type = NT_COMPOUNDLIST;
					AstNode *member = createNode();
					member->type = NT_COMPOUNDLIST_MEMBER;
					ADD_AS_CHILD(member);
					things.push(member);
				}
					break;
				case TT_CBRACE:
					state = State::CompoundRoot;
					things.top()->type = NT_EMPTY;
					things.pop();
					break;
				default:
					PARSE_ERROR(PE_INVALID_AFTER_OPEN);
				}
				break;
			// now follow the various list types, such as "stuff = { 1 2 3 }"
			case State::BegunIntList:
				if (currentToken.type == TT_CBRACE) {
					state = State::CompoundRoot;
					things.pop();
				} else if (currentToken.type == TT_INT) {
					AstNode *member = createNode();
					member->type = NT_INTLIST_MEMBER;
					member->val.Int = currentToken.tok.Int;
					ADD_AS_CHILD(member);
				} else if (currentToken.type == TT_DOUBLE) {
					// perhaps this should have been a double list all along, but all entries
					// so far were integer numbers for some reason written without a decimal point
					fixListType(things.top());
					AstNode *member = createNode();
					member->type = NT_DOUBLELIST_MEMBER;
					member->val.Double = currentToken.tok.Double;
					state = State::BegunDoubleList;
					ADD_AS_CHILD(member);
				} else PARSE_ERROR(PE_INVALID_IN_INT_LIST);
				break;
			case State::BegunDoubleList:
				if (currentToken.type == TT_CBRACE) {
					state = State::CompoundRoot;
					things.pop();
				} else if (currentToken.type == TT_DOUBLE) {
					AstNode *member = createNode();
					member->type = NT_DOUBLELIST_MEMBER;
					member->val.Double = currentToken.tok.Double;
					ADD_AS_CHILD(member);
				} else if (currentToken.type == TT_INT) {
					// sometimes the game writes integers (especially '0') into a double list...
					AstNode *member = createNode();
					member->type = NT_DOUBLELIST_MEMBER;
					member->val.Double = static_cast<double>(currentToken.tok.Int);
					ADD_AS_CHILD(member);
				} else PARSE_ERROR(PE_INVALID_IN_DOUBLE_LIST);
				break;
			case State::BegunCompoundList:
				if (currentToken.type == TT_OBRACE) {
					state = State::CompoundRoot;
					AstNode *member = createNode();
					member->type = NT_COMPOUNDLIST_MEMBER;
					ADD_AS_CHILD(member);
					things.push(member);
				} else if (currentToken.type == TT_CBRACE) {
					state = State::CompoundRoot;
					things.pop();
				}
				else PARSE_ERROR(PE_INVALID_IN_COMPOUND_LIST);
				break;
			case State::BegunStringList:
				if (currentToken.type == TT_STRING) {
					AstNode *member = createNode();
					member->type = NT_STRINGLIST_MEMBER;
					memcpy(member->val.Str, currentToken.tok.String, 64);
					ADD_AS_CHILD(member);
				} else if (currentToken.type == TT_CBRACE) {
					state = State::CompoundRoot;
					things.pop();
				} else PARSE_ERROR(PE_INVALID_IN_STRING_LIST);
				break;
			case State::BegunBoolList:
				if (currentToken.type == TT_BOOL) {
					AstNode *member = createNode();
					member->type = NT_BOOLLIST_MEMBER;
					member->val.Bool = currentToken.tok.Bool;
					ADD_AS_CHILD(member);
				} else if (currentToken.type == TT_CBRACE) {
					state = State::CompoundRoot;
					things.pop();
				} else PARSE_ERROR(PE_INVALID_IN_BOOL_LIST);
				break;
			}
			if (things.empty()) {  // an extraneous closing brace caused our implicit root node to be closed
				PARSE_ERROR(PE_TOO_MANY_CLOSE_BRACES);
			}
		}

		// Bail out if user cancelled.
		if (shouldCancel) PARSE_ERROR(PE_CANCELLED);
		// alternatively, if all input is consumed but the parser isn't "at rest"...
		if (things.size() > 1 || state != State::CompoundRoot) {
			PARSE_ERROR(PE_UNEXPECTED_END);
		}

		return root;
	}

	// Called by the GUI frontend when the user clicks the Cancel button.
	// Sets a flag indicating the parse function to quit at the next possible moment.
	void Parser::cancel() {
		shouldCancel = true;
	}

	// Gets the stored parser error.
	ParserError Parser::getLatestParserError() const {
		return latestParserError;
	}

	// Gets the next token from the queue, calling the lexer if necessary.
	// Throws an "unexpected end" parser error if at end of file.
	Token Parser::getNextToken() {
		if (lexQueue.isEmpty()) {
			lex();
			everyNth(lexCalls1, 100, emit progress(this, totalProgress, totalSize));
		}
		if (lexQueue.isEmpty()) {
			throw ParserError{ PE_UNEXPECTED_END, {line, charPos, TT_NONE, {{0}}} };
		}
		return lexQueue.dequeue();
	}

#ifdef Q_OS_MAC
#define LEXER_SETUP(locale) const auto locale = setlocale(LC_NUMERIC, nullptr); setlocale(LC_NUMERIC, "C")
#define LEXER_TEARDOWN(locale) setlocale(LC_NUMERIC, (locale))
#else
#define LEXER_SETUP(locale)
#define LEXER_TEARDOWN(locale)
#endif

	// Lex into the queue. Attempt to provide `atLeast' many tokens: can be more if the last
	// token ends in a special character, can be less if end of file is reached.
	// Returns the number of tokens lexed.
	int Parser::lex(int atLeast) {
		if (atLeast == 0) atLeast = queueCapacity;
		char buf[64];
		char c;
		TokenType assumption = TT_NONE;
		int tokensRead = 0;
		unsigned int len = 0;
		bool haveOpenQuote = false;
		bool comment = false;
		bool haveEscape = false;
		LEXER_SETUP(oldLocale);

		while (!data.eof() && tokensRead < atLeast && lexQueue.count() < queueCapacity-1) {
			c = data.getc();
			// keep track of where we are in the file so we can report our progress and the locations of errors
			charPos++;
			totalProgress++;

			// if in comment, ignore everything until the end of the line
			if (comment) {
				if (c == '\n') {
					comment = false;
					line++;
				}
				continue;
			}
			// According to the wiki, the game uses only unix-style line endings (LF), so ignore CR characters.
			if (c == '\r') continue;

			if (haveOpenQuote || (!isspace(c) && c != '{' && c != '}' && c != '=' && c != '<' && c != '>' && c != '#')) {
				if (haveOpenQuote) {
					if (!haveEscape) {
						haveEscape = (c == '\\');
					} else {
						if ((c == '"' || c == '\\') && len < 63) {
							buf[len] = c;
							buf[++len] = '\0';
						}
						haveEscape = false;
						continue;
					}
				}
				if (c == '"') {
					if (haveOpenQuote) {  // This ends a quoted string
						haveOpenQuote = false;
						goto finish_off_token;  // Immediately finish this token, don't add the quotation mark to the result.
					}
					else {  // This begins a quoted string
						haveOpenQuote = true;
						assumption = TT_STRING;
						continue;  // Don't add the quotation mark to the result
					}
				}
				if (len < 63) {
					buf[len] = c;
					buf[++len] = '\0';

					// update our assumption of what the currently-read token is, if necessary.
					if (assumption == TT_NONE) {
						if (isdigit(c) || c == '-') assumption = TT_INT;
						else assumption = TT_STRING;
					} else if (assumption == TT_INT && c == '.') {
						assumption = TT_DOUBLE;
					} else if (assumption == TT_DOUBLE && c == '.') {
						// most likely a date, but we don't really use those so we treat it as a string.
						assumption = TT_STRING;
					}
				}
			} else { finish_off_token:
				Token token{};
				token.line = line;
				token.firstChar = charPos - len;
				char *eptr;
				switch (assumption) {
				case TT_STRING:
					// Check if our "string" might be a bool after all
					if (strcmp(buf, "yes") == 0 || strcmp(buf, "YES") == 0) {
						token.type = TT_BOOL;
						token.tok.Bool = true;
					}
					else if (strcmp(buf, "no") == 0 || strcmp(buf, "NO") == 0) {
						token.type = TT_BOOL;
						token.tok.Bool = false;
					} else {  // nope, it really is a string
						token.type = TT_STRING;
						// string length is currently limited to 64 bytes
						memcpy(token.tok.String, buf, 64);
					}
					break;
				case TT_INT:
					token.type = TT_INT;
					token.tok.Int = strtoll(buf, &eptr, 10);
					if (Q_UNLIKELY(token.tok.Int == 0 && eptr == buf)) {
						token.type = TT_NONE;
						memcpy(token.tok.String, buf, 64);
						LEXER_TEARDOWN(oldLocale);
						throw ParserError{ LE_INVALID_INT, token };
					}
					break;
				case TT_DOUBLE:
					token.type = TT_DOUBLE;
					token.tok.Double = strtod(buf, &eptr);
					if (Q_UNLIKELY(eptr == buf)) {
						token.type = TT_NONE;
						memcpy(token.tok.String, buf, 64);
						LEXER_TEARDOWN(oldLocale);
						throw ParserError{ LE_INVALID_DOUBLE, token };
					}
					break;
				case TT_NONE:  // Special character
					break;
				default:
					// We never assign any other value, so this should never be reached.
					Q_UNREACHABLE();
				}
				if (assumption != TT_NONE) {
					lexQueue.append(token);
					tokensRead++;
				}

				// Check whether the most recently read character (which caused the last token to be terminated)
				// is a special character. Note that whitespace is not itself a token.
				TokenType specialType = TT_NONE;
				if (c == '=') {
					specialType = TT_EQUALS;
				} else if (c == '{') {
					specialType = TT_OBRACE;
				} else if (c == '}') {
					specialType = TT_CBRACE;
				} else if (c == '<') {
					specialType = TT_LT;
				} else if (c == '>') {
					specialType = TT_GT;
				} else if (c == '#') {
					comment = true;
				}
				if (specialType != TT_NONE) {
					Token stok{};
					stok.line = line;
					stok.firstChar = charPos;
					stok.type = specialType;
					lexQueue.append(stok);
					tokensRead++;
				}
				assumption = TT_NONE;
				buf[0] = '\0';
				len = 0;
			}
			if (c == '\n') {
				line++;
				charPos = 0;
			}
		}

		if (data.eof()) {
			lexerDone = true;
			// produce an error if the file ended in the middle of a token.
			if (assumption != TT_NONE) {
				Token currentToken{ line, charPos - len, TT_NONE, {{0}} };
				memcpy(currentToken.tok.String, buf, 64);
				LEXER_TEARDOWN(oldLocale);
				throw ParserError{PE_UNEXPECTED_END, currentToken};
			}
		}
		totalProgress = data.tell();
		LEXER_TEARDOWN(oldLocale);
		return tokensRead;
	}

	// Attempt to look ahead by `n' tokens.
	TokenType Parser::lookahead(int n) {
		Q_ASSERT_X(n >= 1 && n < queueCapacity, "Parser::lookahead", "invalid lookahead");
		// Refill queue if necessary
		if (lexQueue.size() < n) lex(n);
		// Requested token beyond end of file
		if (lexQueue.size() < n) return TT_NONE;
		return lexQueue[n-1].type;
	}

	// Fixup list types: when a double appears in an int list, transform the entire thing into a double list.
	void Parser::fixListType(Parsing::AstNode *list) {
		Q_ASSERT_X(list->type == NT_INTLIST, "Parser::fixListType", "Attempted to transform non-integer list.");
		for (AstNode *node = list->val.firstChild; node; node = node->nextSibling) {
			node->val.Double = (double) node->val.Int;
			node->type = NT_DOUBLELIST_MEMBER;
		}
		list->type = NT_DOUBLELIST;
	}
}
