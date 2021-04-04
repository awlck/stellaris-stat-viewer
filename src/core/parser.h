/* parser.h: A parser for PDS script/declaration files (header file)
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

#pragma once

#ifndef STELLARIS_STAT_VIEWER_PARSER_H
#define STELLARIS_STAT_VIEWER_PARSER_H

#include <forward_list>

#include <QtCore/QFileInfo>
#include <QtCore/QObject>
#include <QtCore/QQueue>
#include <QtCore/QString>
class QFile;

namespace Parsing {
	// Indicates the type of a lexed token.
	enum TokenType {
		TT_STRING,  // e.g. "hello"
		TT_OBRACE,  // {
		TT_CBRACE,  // }
		TT_EQUALS,  // =
		TT_BOOL,    // e.g. yes
		TT_INT,     // e.g. 3
		TT_DOUBLE,  // e.g. 3.5
		TT_LT,      // <
		TT_GT,      // >
		TT_NONE     // lexer initial state: not yet determined
	};

	// Represents a token
	struct Token {
		// the line in which this token begun
		unsigned long line;
		// the character position whithin that line at which this token begun
		unsigned long firstChar;
		// The type of this token (see above)
		TokenType type;
		// Potential values of this token. The active element is indicated by the `type'.
		// (Not all TokenTypes have a value.)
		union {
			char String[64];
			bool Bool;
			qint64 Int;
			double Double;
		} tok;
	};
	
	// indicates the type of file being read
	enum class FileType {
		SaveFile,
		GameFile,
		ModFile,
		NoFile
	};

	// Represents the types of AstNode.
	enum NodeType {
		NT_INDETERMINATE = 0,  // not yet set
		NT_COMPOUND,  // a compound node, e.g. test = { hello = world }
		NT_STRING,  // a string, e.g. "test"
		NT_BOOL,  // a boolean, e.g. yes
		NT_INT,  // an integer, e.g. 3
		NT_DOUBLE,  // a double, e.g. 3.5
		NT_INTLIST,  // a list of integers, e.g. test = { 3 4 5 }
		NT_INTLIST_MEMBER,  // a single element within a list of integers
		NT_DOUBLELIST,  // a list of doubles, e.g. test = { 3.5 4.97 7.6 }
		NT_DOUBLELIST_MEMBER,  // a single element within a list of doubles
		NT_COMPOUNDLIST,  // a list of compunds, e.g. test = { { hello = there } { general = kenobi } }
		NT_COMPUNDLIST_MEMBER,  // a single element within a list of compunds
		NT_STRINGLIST,  // a list of strings, e.g. test = { "hello" "there" "general" "kenobi" }
		NT_STRINGLIST_MEMBER,  // a single element within a list of strings
		NT_BOOLLIST,  // a list of booleans, e.g. test = { yes no yes }
		NT_BOOLLIST_MEMBER,  // a single member within a list of strings
		NT_EMPTY  // an empty node, e.g. test = {} -- the exact type (compund, int list, etc.) can't be determined.
	};

	// The relation within and int or double node: =, >, <, >=, <=
	enum RelationType {
		RT_NONE = 0,
		RT_EQ,
		RT_GT,
		RT_GE,
		RT_LT,
		RT_LE
	};

	// Represents a node in the parse tree.
	struct AstNode {
		/** Merge 'other' into this tree
		 *
		 * All children of 'other' will become children of this.
		 *
		 * If this and other are not of the same type, nothing will happen.
		 * Also works if 'this' and/or 'other' does not have any children.
		 *
		 * @param other The tree to merge into this tree.
		 */
		void merge(AstNode *other);
		/** Finds the first child of this node with the given name.
		 *
		 * @param name The name of the child to search for.
		 * @return The child searched for, or nullptr if no child of that name exists.
		 */
		AstNode *findChildWithName(const char *name) const;
		qint64 countChildren() const;

		// the name of this node.
		char myName[64] = {'\0'};
		// The type of this node
		NodeType type = NT_INDETERMINATE;
		// The next sibling of this node.
		AstNode *nextSibling = nullptr;
		// The relation type in this node (see above).
		RelationType relation = RT_NONE;
		// The value of this node. The active union member is indicated by the NodeType.
		union NodeValue {
			// for string nodes
			char Str[64];
			// for boolean nodes
			bool Bool;
			// for integer nodes
			qint64 Int;
			// for double nodes
			double Double;
			// for compound and list nodes.
			struct { AstNode *firstChild; AstNode *lastChild; };
		} val = {{'\0'}};
	};

	/** For debugging purposes, print the parse tree. */
	void printParseTree(const AstNode *tree, int indent = 0, bool toplevel = true);

	// Represemts the types of parser errors.
	enum ParseErr {
		PE_NONE,
		PE_INVALID_IN_COMPOUND,
		PE_INVALID_AFTER_NAME,
		PE_INVALID_AFTER_EQUALS,
		PE_INVALID_AFTER_RELATION,
		PE_INVALID_AFTER_OPEN,
		PE_INVALID_COMBO_AFTER_OPEN,
		PE_INVALID_IN_INT_LIST,
		PE_INVALID_IN_DOUBLE_LIST,
		PE_INVALID_IN_COMPOUND_LIST,
		PE_INVALID_IN_STRING_LIST,
		PE_INVALID_IN_BOOL_LIST,
		PE_UNEXPECTED_END,
		PE_TOO_MANY_CLOSE_BRACES,
		LE_INVALID_INT,
		LE_INVALID_DOUBLE,
		PE_CANCELLED
	};

	/** Get a textual representation of the given error type. */
	QString getErrorDescription(ParseErr etype);

	/** Represents a parser error, consisting of a type and the token causing the error. */
	struct ParserError {
		ParseErr etype;
		Token erroredToken;
	};

	/** A memory buffer for reading files, with an interface modeled after stdio.h */
	class MemBuf {
		Q_DISABLE_COPY(MemBuf)
	public:
		MemBuf(char *area, size_t size);
		explicit MemBuf(const QByteArray &arr);
		explicit MemBuf(QFile &file);
		~MemBuf();

		/** Gets the next character from the file, or indicate EOF */
		inline char getc() {
			if (Q_UNLIKELY(location == _size)) return EOF;
			return buf[location++];
		}
		/** Whether or not the end of the file has been reached. */
		inline bool eof() {
			return location == _size;
		}
		/** Go back to the beginning of the buffer */
		inline void rewind() {
			location = 0;
		}
		/** Get the current position in the buffer */
		inline size_t tell() {
			return location;
		}
		/** Get the total size of the buffer */
		inline size_t size() {
			return _size;
		}
	private:
		char *buf;
		size_t location;
		size_t _size;
	};

	/** Where parsing and lexing take place. */
	class Parser : public QObject {
		Q_OBJECT
	public:
		Parser(MemBuf &data, FileType ftype, QString filename = QString(), QObject *parent = nullptr);
		/** Parse the file and return a pointer to the root node */
		AstNode *parse();
		/** Cancel parsing at the next possible occasion */
		void cancel();
		/** Get the stored parser error */
		ParserError getLatestParserError() const;

	signals:
		/** Emitted periodically to indicate the current parse progress. */
		void progress(Parser *parser, qint64 current, qint64 total);

	private:
		Token getNextToken();
		int lex(int atLeast = 0);
		TokenType lookahead(int n);
		AstNode *createNode();

		static void fixListType(AstNode *list);

		bool lexerDone = false;
		bool shouldCancel = false;

		MemBuf &data;
		FileType fileType;
		QString filename;
		QQueue<Token> lexQueue;
		qint64 totalProgress = 0;
		qint64 totalSize;
		std::forward_list<AstNode> allCreatedNodes;

		static constexpr int queueCapacity = 50;
		unsigned long line = 1;
		unsigned long charPos = 0;

		ParserError latestParserError{PE_NONE, {0, 0, TT_NONE, {{0}}}};

		unsigned int lexCalls1 = 1;
	};
}

#endif //STELLARIS_STAT_VIEWER_PARSER_H
