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
#include "../../core/empire.h"
#include "../../core/fleet.h"
#include "../../core/galaxy_state.h"
#include "../../core/ship.h"
#include "../../core/parser.h"

#include "dataextraction.h"

using namespace Parsing;

int frontend_json_begin(int argc, char **argv) {
	if (argc != 3) {
		fprintf(stderr, "USAGE: %s --frontend=json <FILE>\n\n"
				  "  Read the gamestate file FILE and dump json stats to stdout\n", argv[0]);
		return 1;
	}
	fprintf(stderr, "Parsing file ...\n");
	Parser parser(QFileInfo(argv[2]), FileType::SaveFile);
	AstNode *node = parser.parse();
	if (node == nullptr) {
		ParserError err = parser.getLatestParserError();
		fprintf(stderr, "Parser Error on %s:%lu:%lu: Error#%d\n",
				argv[2], err.erroredToken.line, err.erroredToken.firstChar, err.etype);
		return 2;
	} else if (node->countChildren() == 0) {
		fprintf(stderr, "%s: Unknown parse error.\n", argv[2]);
		return 2;
	}
	fprintf(stderr, "Building galaxy ...\n");
	Galaxy::StateFactory sf;
	Galaxy::State *state = sf.createFromAst(node, nullptr);
	if (state == nullptr) {
		fprintf(stderr, "Error extracting data from the save file.\n");
		return 3;
	}
	delete node;

	fprintf(stderr, "Extracting data ... ");
	QJsonObject toplevelObj;
	toplevelObj["date"] = state->getDate();
	QJsonObject dataObj;

	const QMap<qint64, Galaxy::Empire *> &empires = state->getEmpires();
	const QMap<qint64, Galaxy::Ship *> &ships = state->getShips();
	QMap<Galaxy::Empire *, QLinkedList<Galaxy::Ship *>> shipsPerEmpire;

	for (auto it = ships.cbegin(); it != ships.cend(); it++) {
		shipsPerEmpire[it.value()->getFleet()->getOwner()].append(it.value());
	}

	for (auto it = empires.cbegin(); it != empires.cend(); it++) {
		dataObj[it.value()->getName()] = createDataForEmpire(it.value(), shipsPerEmpire[it.value()]);
	}
	fprintf(stderr, "done.\n");

	toplevelObj["content"] = dataObj;
	QJsonDocument outdoc(toplevelObj);
	printf("%s\n", outdoc.toJson().data());
	return 0;
}
