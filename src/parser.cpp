//
// Created by Adrian Welcker on 2019-03-03.
//

#include "parser.h"

#include <QtCore/QTextStream>

namespace Parsing {
	Parser::Parser(QString *text) :
			shouldDeleteStream(true), fileType(FileType::NoFile),
			file(nullptr), filename(tr("<string>")) {
		filename = tr("<string>");
		stream = new QTextStream(text, QIODevice::ReadOnly);
	}

	Parser::Parser(const QFileInfo &fileInfo, FileType ftype) :
			shouldDeleteStream(true), fileType(ftype) {
		filename = fileInfo.absoluteFilePath();
		file = new QFile(filename);
		file->open(QIODevice::ReadOnly);
		stream = new QTextStream(file);
	}

	Parser::Parser(QTextStream *stream, const QString &filename, FileType ftype) :
			shouldDeleteStream(false), fileType(ftype),
			file(nullptr), filename(filename), stream(stream) { }

	Parser::~Parser() {
		if (shouldDeleteStream) delete stream;
		if (file) {
			file->close();
			delete file;
		}
	}

	unsigned int Parser::lex(unsigned int atLeast) {
		if (atLeast == 0) atLeast = queueCapacity;
		QString current;
		QChar c;
		TokenType assumption = TT_NONE;
		unsigned int tokensRead = 0;
		unsigned int len = 0;
		bool haveOpenQuote = false;
		while (!stream->atEnd() && tokensRead < atLeast && lexQueue.count() < queueCapacity-1) {
			c = stream->read(1)[0];
			charPos++;
			if (haveOpenQuote || (!c.isSpace() && c != '{' && c != '}' && c != '=' && c != '<' && c != '>')) {
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
						if (current.compare("yes", Qt::CaseInsensitive)) {
							token.type = TT_BOOL;
							token.tok_bool = true;
						}
						else if (current.compare("no", Qt::CaseInsensitive)) {
							token.type = TT_BOOL;
							token.tok_bool = false;
						}
						else {
							QByteArray next = current.toUtf8();
							Q_ASSERT_X(next.size() < 64, "Parser::lex", "string too long");  // TODO: Error handling (string too long)
							qstrcpy(token.tok_string, next.data());
						}
						break;
					case TT_INT:
						token.type = TT_INT;
						bool int_ok;
						token.tok_int = static_cast<int64_t>(current.toLongLong(&int_ok));
						Q_ASSERT_X(int_ok, "Parser::lex", "int token invalid");  // TODO: Error handling (int token invalid)
						break;
					case TT_DOUBLE:
						token.type = TT_DOUBLE;
						bool double_ok;
						token.tok_double = current.toDouble(&double_ok);
						Q_ASSERT_X(double_ok, "Parser::lex", "double token invalid");  // TODO: Error handling (double token invalid)
						break;
					case TT_NONE:  // Special character
						break;
					default:
						Q_UNREACHABLE();
						abort();  // TODO: Error handling (invalid token type!)
				}
				if (assumption != TT_NONE) {
					lexQueue.append(token);
					tokensRead++;
				}

				TokenType specialType = TT_NONE;
				if (c == '=') {
					specialType = TT_EQUALS;
				}
				else if (c == '{') {
					specialType = TT_OBRACE;
				}
				else if (c == '}') {
					specialType = TT_CBRACE;
				}
				else if (c == '<') {
					specialType = TT_LT;
				}
				else if (c == '>') {
					specialType = TT_GT;
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
			// TODO: Error handling (EOF while token incomplete)
			Q_ASSERT_X(assumption == TT_NONE, "Parser::lex", "unexpected EOF");
		}
		return tokensRead;
	}
}