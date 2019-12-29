/* frontends/json/json_main.cpp: Entry point for the SSV json frontend.
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

#include <stdio.h>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QLinkedList>
#include <QtCore/QTextStream>
#include "../../core/empire.h"
#include "../../core/fleet.h"
#include "../../core/galaxy_state.h"
#include "../../core/ship.h"
#include "../../core/parser.h"
#include "../../core/extract_gamestate.h"

#include "dataextraction.h"

using namespace Parsing;

int frontend_json_begin(int argc, char **argv) {
	if (argc != 3) {
		fprintf(stderr, "USAGE: %s --frontend=json <FILE>\n\n"
				  "  Read the gamestate file FILE and dump json stats to stdout\n", argv[0]);
		return 1;
	}
	QString filename(argv[2]);
	Parser *parser;
	bool isCompressed = filename.endsWith(QStringLiteral(".sav"));
	unsigned char *dest;
	QTextStream *stream;
	if (isCompressed) {
		fprintf(stderr, "Inflating file ...\n");
		unsigned long destsize;
		QFile f(filename);
		f.open(QIODevice::ReadOnly);
		int result = extractGamestate(f, &dest, &destsize);
		f.close();

		if (result != 0) {
			fprintf(stderr, "%s:\n%s\n\nPlease make sure you have selected a valid save file. If the selected file "
				   "loads fine in the game, please report this issue to the developer.\n",
				   argv[2], getInflateErrmsg(result).toLocal8Bit().data());
			if (result <= 2) free(dest);
			return 3;
		}

		stream = new QTextStream(QByteArray((char *) dest, destsize));
		parser = new Parser(stream, filename, Parsing::FileType::SaveFile);
	} else {
		dest = nullptr;
		stream = nullptr;
		parser = new Parser(QFileInfo(argv[2]), FileType::SaveFile);
	}
	fprintf(stderr, "Parsing file ...\n");
	AstNode *node = parser->parse();
	if (node == nullptr) {
		ParserError err = parser->getLatestParserError();
		fprintf(stderr, "Parser Error on %s:%lu:%lu: Error#%d\n",
				argv[2], err.erroredToken.line, err.erroredToken.firstChar, err.etype);
		return 2;
	} else if (node->countChildren() == 0) {
		fprintf(stderr, "%s: Unknown parse error.\n", argv[2]);
		return 2;
	}
	if (isCompressed) {
		delete stream;
		free(dest);
	}
	fprintf(stderr, "Building galaxy ...\n");
	Galaxy::StateFactory sf;
	Galaxy::State *state = sf.createFromAst(node, nullptr);
	if (state == nullptr) {
		fprintf(stderr, "Error extracting data from the save file.\n");
		return 3;
	}
	delete parser;

	fprintf(stderr, "Extracting data ... ");
	QJsonObject toplevelObj(createJsonFromState(state));
	QJsonDocument outdoc(toplevelObj);
	printf("%s\n", outdoc.toJson().data());
	return 0;
}
