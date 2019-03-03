//
// Created by Adrian Welcker on 2019-03-03.
//

#pragma once

#ifndef STELLARIS_STAT_VIEWER_PARSER_H
#define STELLARIS_STAT_VIEWER_PARSER_H

#include <QtCore/QFileInfo>
#include <QtCore/QObject>
#include <QtCore/QQueue>
#include <QtCore/QString>
class QFile;
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

	class Parser : public QObject {
		Q_OBJECT
	public:
		explicit Parser(QString *text);
		Parser(const QFileInfo &fileInfo, FileType ftype);
		Parser(QTextStream *stream, const QString &filename, FileType ftype);
		~Parser();
	signals:
		void progress(unsigned long current, unsigned long total);
	private:
		unsigned int lex(unsigned int atLeast = 0);

		bool shouldDeleteStream;
		FileType fileType;
		QFile *file;
		QString filename;
		QQueue<Token> lexQueue;
		QTextStream *stream;
		const int queueCapacity = 50;
		unsigned long line = 1;
		unsigned long charPos = 0;
	};
}

#endif //STELLARIS_STAT_VIEWER_PARSER_H
