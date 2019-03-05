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

	enum NodeType {
		NT_INDETERMINATE = 0,
		NT_COMPOUND,
		NT_STRING,
		NT_BOOL,
		NT_INT,
		NT_DOUBLE,
		NT_INTLIST,
		NT_INTLIST_MEMBER,
		NT_DOUBLELIST,
		NT_DOUBLELIST_MEMBER,
		NT_EMPTY
	};

	enum RelationType {
		RT_NONE = 0,
		RT_EQ,
		RT_GT,
		RT_GE,
		RT_LT,
		RT_LE
	};

	struct AstNode {
		~AstNode();
		char myName[64] = {'\0'};
		NodeType type = NT_INDETERMINATE;
		AstNode *nextSibling = nullptr;
		RelationType relation = RT_NONE;
		union {
			char Str[64];
			bool Bool;
			int64_t Int;
			double Double;
			AstNode *Child;
		} val = {{'\0'}};
	};

	class Parser : public QObject {
		Q_OBJECT
	public:
		explicit Parser(QString *text);
		Parser(const QFileInfo &fileInfo, FileType ftype);
		Parser(QTextStream *stream, const QString &filename, FileType ftype);
		~Parser() override;

	signals:
		void progress(unsigned long current, unsigned long total);
	private:
		int lex(int atLeast = 0);

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
