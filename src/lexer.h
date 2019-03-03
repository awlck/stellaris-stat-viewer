/* lexer.h: A lexer for PDS script/declaration files (header file).
 *
 * Copyright 2019 by Adrian "ArdiMaster" Welcker, distributed
 * under the MIT License - see file "LICENSE" for details.
 */


#ifndef STELLARIS_STAT_VIEWER_LEXER_H
#define STELLARIS_STAT_VIEWER_LEXER_H

#include <QtCore/QFileInfo>
#include <QtCore/QString>
#include <QtCore/QVector>
class QTextStream;

namespace Parsing {
	enum TokenType {
		TT_STRING,
		TT_OBRACE,
		TT_CBRACE,
		TT_EQUALS,
		TT_BOOL,
		TT_INT,
		TT_DOUBLE,
		TT_LT,
		TT_GT,
		TT_NONE
	};

	struct Token {
		QString fname;
		unsigned long line;
		unsigned long firstChar;
		TokenType type;
		union {
			char tok_string[64];
			bool tok_bool;
			int64_t tok_int;
			double tok_double;
		};
	};

	enum class FileType {
		SaveFile,
		GameFile,
		ModFile,
		NoFile
	};

	class Lexer : public QObject {
		Q_OBJECT
	public:
		void lex(QString *text);
		void lex(const QFileInfo &fileInfo, FileType type);
		void lex(QTextStream *stream, const QString &filename, FileType type);
		void clear();
		const QVector<Token> &getResult() const;
	protected:
		QVector<Token> result;
	};
}

#endif //STELLARIS_STAT_VIEWER_LEXER_H
