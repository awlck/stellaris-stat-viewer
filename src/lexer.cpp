/* lexer.cpp: A lexer for PDS script/declaration files.
 *
 * Copyright 2019 by Adrian "ArdiMaster" Welcker, distributed
 * under the MIT License - see file "LICENSE" for details.
 */

#include "lexer.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QTextStream>

namespace Parsing {
	void Lexer::lex(QString *text) {
		QTextStream *stream = new QTextStream(text, QIODevice::ReadOnly);
		lex(stream, QCoreApplication::tr("<string>"), FileType::NoFile);
		delete stream;
	}

	void Lexer::lex(const QFileInfo &fileInfo, FileType type) {
		QFile file(fileInfo.absoluteFilePath());
		file.open(QIODevice::ReadOnly);
		QTextStream *stream = new QTextStream(&file);
		lex(stream, fileInfo.absoluteFilePath(), type);
		delete stream;
		file.close();
	}

	void Lexer::lex(QTextStream *stream, const QString &filename, FileType type) {
		QString fname = filename;
		/* switch(type) {  // TODO: Shorten file paths according to type
			case FileType::SaveFile: {
				QFileInfo f(filename);

				break;
			}
			case FileType::NoFile:
				fname = filename;
				break;
		} */
		QString current;
		QChar c;
		TokenType assumption = TT_NONE;
		unsigned int len = 0;
		unsigned long lineno = 1;
		unsigned long cpos = 0;
		bool haveOpenQuote = false;
		while (!stream->atEnd()) {
			c = stream->read(1)[0];
			cpos++;
			if (haveOpenQuote || (!c.isSpace() && c != '{' && c != '}' && c != '=' && c != '<' && c != '>')) {
				if (c == '"') {
					if (haveOpenQuote) {  // This ends a quoted string
						haveOpenQuote = false;
						goto finish_off_token;  // Immediately finish this token, don't add the quotation mark to the result.
					} else {  // This begins a quoted string
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
				} else if (assumption == TT_INT && c == '.') assumption = TT_DOUBLE;
			} else {  finish_off_token:
				Token token;
				token.fname = fname;  // TODO: Very memory inefficient!
				token.line = lineno;
				token.firstChar = cpos-len;
				switch (assumption) {
					case TT_STRING:
						// Check if our "string" might be a bool after all
						if (current.compare("yes", Qt::CaseInsensitive)) {
							token.type = TT_BOOL;
							token.tok_bool = true;
						} else if (current.compare("no", Qt::CaseInsensitive)) {
							token.type = TT_BOOL;
							token.tok_bool = false;
						} else {
							QByteArray next = current.toUtf8();
							Q_ASSERT_X(next.size() < 64, "Lexer::lex", "string too long");  // TODO: Error handling (string too long)
							qstrcpy(token.tok_string, next.data());
						}
						break;
					case TT_INT:
						token.type = TT_INT;
						bool int_ok;
						token.tok_int = static_cast<int64_t>(current.toLongLong(&int_ok));
						Q_ASSERT_X(int_ok, "Lexer::lex", "int token invalid");  // TODO: Error handling (int token invalid)
						break;
					case TT_DOUBLE:
						token.type = TT_DOUBLE;
						bool double_ok;
						token.tok_double = current.toDouble(&double_ok);
						Q_ASSERT_X(double_ok, "Lexer::lex", "double token invalid");  // TODO: Error handling (double token invalid)
						break;
					case TT_NONE:  // Special character
						break;
					default:
						Q_UNREACHABLE();
						abort();  // TODO: Error handling (invalid token type!)
				}
				if (assumption != TT_NONE) result.push_back(token);

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
				}
				if (specialType != TT_NONE) {
					Token stok;
					stok.fname = fname;
					stok.line = lineno;
					stok.firstChar = cpos;
					stok.type = specialType;
					result.push_back(stok);
				}
				assumption = TT_NONE;
				current = "";
				len = 0;
			}
			if (c == '\n') {
				lineno++;
				cpos = 0;
			}
		}

		Q_ASSERT_X(assumption == TT_NONE, "Lexer::lex", "unexpected EOF");  // TODO: Error handling (EOF while token incomplete)
	}

	void Lexer::clear() {
		result.clear();
	}

	const QVector<Token>& Lexer::getResult() const {
		return result;
	}
}
