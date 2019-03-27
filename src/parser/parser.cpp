/* parser.cpp: A parser for PDS script/declaration files.
 *
 * Copyright 2019 Adrian "ArdiMaster" Welcker
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

#include <QtCore/QStack>
#include <QtCore/QTextStream>

#include <stdio.h>  // TODO: Not use stdlib

#define everyNth(which, n, what) do { if ((((which)++) % (n)) == 0) {(what); (which) = 1;} } while (0)

namespace Parsing {
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

	AstNode::~AstNode() {
		if (typeHasChildren(type)) delete val.firstChild;
		if (nextSibling) {
			AstNode *sib, *tmp;
			sib = nextSibling;
			do {
				tmp = sib->nextSibling;
				sib->nextSibling = nullptr;
				delete sib;
				sib = tmp;
			} while (sib);
		}
	}

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
		delete other;
	}

	AstNode* AstNode::findChildWithName(const char *name) const {
		if (type != NT_COMPOUND) return nullptr;
		if (this->val.firstChild == nullptr) return nullptr;
		AstNode *child = this->val.firstChild;
		do {
			if (qstrcmp(child->myName, name) == 0) return child;
			child = child->nextSibling;
		} while (child);
		return nullptr;
	}

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

	// TODO: Not use stdlib
	void printParseTree(AstNode *tree, int indent) {
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
			case NT_COMPUNDLIST_MEMBER:
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
		if (tree->nextSibling) {
			// shoddily avoid recursion
			tree = tree->nextSibling;
			goto comeagain;
		}
	}

	Parser::Parser(QString *text, QObject *parent) : QObject::QObject(parent),
			shouldDeleteStream(true), fileType(FileType::NoFile),
			file(nullptr), filename(tr("<string>")) {
		filename = tr("<string>");
		stream = new QTextStream(text, QIODevice::ReadOnly);
		totalSize = text->toUtf8().size();
	}

	Parser::Parser(const QFileInfo &fileInfo, FileType ftype, QObject *parent) : QObject::QObject(parent),
			shouldDeleteStream(true), fileType(ftype) {
		filename = fileInfo.absoluteFilePath();
		file = new QFile(filename);
		file->open(QIODevice::ReadOnly);
		stream = new QTextStream(file);
		totalSize = file->size();
	}

	Parser::Parser(QTextStream *stream, const QString &filename, FileType ftype, QObject *parent) :
			QObject::QObject(parent), shouldDeleteStream(false), fileType(ftype),
			file(nullptr), filename(filename), stream(stream) {
		// Get the total size of stuff to be processed, if possible.
		QIODevice *device = stream->device();
		totalSize = device ? device->size() : 0;
	}

	Parser::~Parser() {
		if (shouldDeleteStream) delete stream;
		if (file) {
			file->close();
			delete file;
		}
	}

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

#define PARSE_ERROR(error) do { latestParserError = { (error), currentToken }; delete root; return nullptr; } while (0)

	AstNode* Parser::parse() {
		lex();  // Initially fill token queue
		// Create a root node that will encompass the entire file.
		AstNode *root = new AstNode;
		root->type = NT_COMPOUND;
		qstrcpy(root->myName, "tree_root");
		QStack<AstNode *> things;  // Explicitly use a stack instead of using recursion.
		things.push(root);
		State state = State::CompoundRoot;
		Token currentToken = {0, 0, TT_NONE, {{'\0'}}};

		while ((!lexerDone || !lexQueue.empty()) && !shouldCancel) {
			try {
				currentToken = getNextToken();
			} catch (const ParserError &e) {
				latestParserError = e;
				delete root;
				return nullptr;
			}
			switch (state) {
			case State::CompoundRoot:
				if (currentToken.type == TT_STRING) {
					state = State::HaveName;
					AstNode *nextNode = new AstNode;
					qstrcpy(nextNode->myName, currentToken.tok.String);
					ADD_AS_CHILD(nextNode);
					things.push(nextNode);
				} else if (currentToken.type == TT_INT) {
					state = State::HaveName;
					AstNode * nextNode = new AstNode;
					QString tmp = QString::number(currentToken.tok.Int);
					qstrcpy(nextNode->myName, tmp.toUtf8().data());
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
					qstrcpy(things.top()->val.Str, currentToken.tok.String);
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
					try {  // compound or string list
						if (lookahead(1) == TT_EQUALS) {
							things.top()->type = NT_COMPOUND;
							AstNode *nextNode = new AstNode;
							nextNode->type = NT_INDETERMINATE;
							state = State::HaveName;
							qstrcpy(nextNode->myName, currentToken.tok.String);
							ADD_AS_CHILD(nextNode);
							things.push(nextNode);
						} else if (lookahead(1) == TT_STRING || lookahead(1) == TT_CBRACE) {
							state = State::BegunStringList;
							things.top()->type = NT_STRINGLIST;
							AstNode *member = new AstNode;
							member->type = NT_STRINGLIST_MEMBER;
							qstrcpy(member->val.Str, currentToken.tok.String);
							ADD_AS_CHILD(member);
						} else PARSE_ERROR(PE_INVALID_COMBO_AFTER_OPEN);
					} catch (const ParserError &e) {
						latestParserError = e;
						delete root;
						return nullptr;
					}
					break;
				case TT_INT: 
					try {
						if (lookahead(1) == TT_INT || lookahead(1) == TT_CBRACE) {
							state = State::BegunIntList;
							things.top()->type = NT_INTLIST;
							AstNode *member = new AstNode;
							member->type = NT_INTLIST_MEMBER;
							member->val.Int = currentToken.tok.Int;
							ADD_AS_CHILD(member);
						} else if (lookahead(1) == TT_EQUALS) {
							things.top()->type = NT_COMPOUND;
							AstNode *nextNode = new AstNode;
							nextNode->type = NT_INDETERMINATE;
							state = State::HaveName;
							QString tmp = QString::number(currentToken.tok.Int);
							qstrcpy(nextNode->myName, tmp.toUtf8().data());
							ADD_AS_CHILD(nextNode);
							things.push(nextNode);
						} else PARSE_ERROR(PE_INVALID_COMBO_AFTER_OPEN);
					} catch (const ParserError &e) {
						latestParserError = e;
						delete root;
						return nullptr;
					}
					break;
				case TT_DOUBLE: {
					state = State::BegunDoubleList;
					things.top()->type = NT_DOUBLELIST;
					AstNode *member = new AstNode;
					member->type = NT_DOUBLELIST_MEMBER;
					member->val.Double = currentToken.tok.Double;
					ADD_AS_CHILD(member);
				}
					break;
				case TT_BOOL: {
					state = State::BegunBoolList;
					things.top()->type = NT_BOOLLIST;
					AstNode *member = new AstNode;
					member->type = NT_BOOLLIST_MEMBER;
					member->val.Bool = currentToken.tok.Bool;
					ADD_AS_CHILD(member);
				}
					break;
				case TT_OBRACE: {
					state = State::CompoundRoot;
					things.top()->type = NT_COMPOUNDLIST;
					AstNode *member = new AstNode;
					member->type = NT_COMPUNDLIST_MEMBER;
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
					AstNode *member = new AstNode;
					member->type = NT_INTLIST_MEMBER;
					member->val.Int = currentToken.tok.Int;
					ADD_AS_CHILD(member);
				} else PARSE_ERROR(PE_INVALID_IN_INT_LIST);
				break;
			case State::BegunDoubleList:
				if (currentToken.type == TT_CBRACE) {
					state = State::CompoundRoot;
					things.pop();
				} else if (currentToken.type == TT_DOUBLE) {
					AstNode *member = new AstNode;
					member->type = NT_DOUBLELIST_MEMBER;
					member->val.Double = currentToken.tok.Double;
					ADD_AS_CHILD(member);
				} else PARSE_ERROR(PE_INVALID_IN_DOUBLE_LIST);
				break;
			case State::BegunCompoundList:
				if (currentToken.type == TT_OBRACE) {
					state = State::CompoundRoot;
					AstNode *member = new AstNode;
					member->type = NT_COMPUNDLIST_MEMBER;
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
					AstNode *member = new AstNode;
					member->type = NT_STRINGLIST_MEMBER;
					qstrcpy(member->val.Str, currentToken.tok.String);
					ADD_AS_CHILD(member);
				} else if (currentToken.type == TT_CBRACE) {
					state = State::CompoundRoot;
					things.pop();
				} else PARSE_ERROR(PE_INVALID_IN_STRING_LIST);
				break;
			case State::BegunBoolList:
				if (currentToken.type == TT_BOOL) {
					AstNode *member = new AstNode;
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

		// alternatively, if all input is consumed but the parser isn't "at rest"...
		if (things.size() > 1 || state != State::CompoundRoot) {
			PARSE_ERROR(PE_UNEXPECTED_END);
		}
		if (shouldCancel) PARSE_ERROR(PE_CANCELLED);

		return root;
	}

	void Parser::cancel() {
		shouldCancel = true;
	}

	ParserError Parser::getLatestParserError() {
		return latestParserError;
	}

	Token Parser::getNextToken() {
		if (lexQueue.isEmpty()) {
			lex();
			everyNth(lexCalls1, 100, emit progress(this, totalProgress, totalSize));
		}
		return lexQueue.dequeue();
	}

	int Parser::lex(int atLeast) {
		if (atLeast == 0) atLeast = queueCapacity;
		QString current;
		QChar c;
		TokenType assumption = TT_NONE;
		int tokensRead = 0;
		unsigned int len = 0;
		bool haveOpenQuote = false;
		bool comment = false;
		while (!stream->atEnd() && tokensRead < atLeast && lexQueue.count() < queueCapacity-1) {
			c = stream->read(1)[0];

			// keep track of where we are in the file so we can report our progress and the locations of errors
			charPos++;
			totalProgress++;

			// if in comment, ignore everything until the end of the line
			if (comment) {
				if (c == '\n') comment = false;
				continue;
			}

			// According to the wiki, the game uses only unix-style line endings (LF), so ignore CR characters.
			if (c == '\r') continue;
			if (haveOpenQuote || (!c.isSpace() && c != '{' && c != '}' && c != '=' && c != '<' && c != '>' && c != '#')) {
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
				current += c;
				len++;

				// update our assumption of what the currently-read token is, if necessary.
				if (assumption == TT_NONE) {
					if (c.isDigit()) assumption = TT_INT;
					else if (c == '-') assumption = TT_INT;
					else assumption = TT_STRING;
				}
				else if (assumption == TT_INT && c == '.') assumption = TT_DOUBLE;
			} else { finish_off_token:
				Token token;
				token.line = line;
				token.firstChar = charPos - len;
				switch (assumption) {
				case TT_STRING:
					// Check if our "string" might be a bool after all
					if (current.compare("yes", Qt::CaseInsensitive) == 0) {
						token.type = TT_BOOL;
						token.tok.Bool = true;
					}
					else if (current.compare("no", Qt::CaseInsensitive) == 0) {
						token.type = TT_BOOL;
						token.tok.Bool = false;
					} else {  // nope, it really is a string
						token.type = TT_STRING;
						QByteArray next = current.toUtf8();
						// string length is currently limited to 64 bytes
						qstrncpy(token.tok.String, next.data(), 64);
					}
					break;
				case TT_INT:
					token.type = TT_INT;
					bool int_ok;
					token.tok.Int = static_cast<qint64>(current.toLongLong(&int_ok));
					if (!int_ok) {
						token.type = TT_NONE;
						qstrncpy(token.tok.String, current.toUtf8().data(), 64);
						throw ParserError{ LE_INVALID_INT, token };
					}
					break;
				case TT_DOUBLE:
					token.type = TT_DOUBLE;
					bool double_ok;
					token.tok.Double = current.toDouble(&double_ok);
					if (!double_ok) {
						token.type = TT_NONE;
						qstrncpy(token.tok.String, current.toUtf8().data(), 64);
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
					Token stok;
					stok.line = line;
					stok.firstChar = charPos;
					stok.type = specialType;
					lexQueue.append(stok);
					tokensRead++;
				}
				assumption = TT_NONE;
				current = "";
				len = 0;
			}
			if (c == '\n') {
				line++;
				charPos = 0;
			}
		}

		if (stream->atEnd()) {
			lexerDone = true;
			if (assumption != TT_NONE) {
				Token currentToken{ line, charPos - len, TT_NONE, {0} };
				qstrncpy(currentToken.tok.String, current.toUtf8().data(), 64);
				throw ParserError{ PE_UNEXPECTED_END, currentToken };
			}
		}
		everyNth(lexCalls2, 1000, totalProgress = totalSize != 0 ? stream->pos() : 0);
		return tokensRead;
	}

	TokenType Parser::lookahead(int n) {
		Q_ASSERT_X(n >= 1, "Parser::lookahead", "invalid lookahead");
		if (lexQueue.size() < n) lex(n);
		if (lexQueue.size() < n) return TT_NONE;
		return lexQueue[n-1].type;
	}
}