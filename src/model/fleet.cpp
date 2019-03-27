/* model/fleet.cpp: Extracting fleet information from an AST.
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

#include "fleet.h"

#include "model_private_macros.h"
#include "galaxy_state.h"
#include "../parser/parser.h"

using Parsing::AstNode;

namespace Galaxy {
	Fleet::Fleet(State *parent) : QObject(parent) {}

	qint64 Fleet::getIndex() const {
		return index;
	}

	const QString &Fleet::getName() const {
		return name;
	}

	Empire *Fleet::getOwner() const {
		return owner;
	}

	bool Fleet::getIsStation() const {
		return isStation;
	}

	double Fleet::getMilitaryPower() const {
		return militaryPower;
	}

	Fleet *Fleet::createFromAst(Parsing::AstNode *tree, State *parent) {
		if (tree->type == Parsing::NT_STRING && qstrcmp(tree->val.Str, "none") == 0) return nullptr;

		Fleet *state = new Fleet(parent);
		state->index = static_cast<qint64>(QString(tree->myName).toLongLong());

		AstNode *nameNode = tree->findChildWithName("name");
		CHECK_PTR(nameNode);
		state->name = QString(nameNode->val.Str);

		AstNode *ownerNode = nameNode->nextSibling->nextSibling->nextSibling->nextSibling;
		if (qstrcmp(ownerNode->myName, "owner") != 0) {
			ownerNode = tree->findChildWithName("owner");
			CHECK_PTR(ownerNode);
		}
		Empire *potentialOwner = parent->getEmpireWithId(ownerNode->val.Int);
		CHECK_PTR(potentialOwner);
		state->owner = potentialOwner;

		AstNode *stationNode = ownerNode->nextSibling;
		if (qstrcmp(stationNode->myName, "station") != 0) {
			stationNode = tree->findChildWithName("station");
		}
		state->isStation = stationNode != nullptr ? stationNode->val.Bool : false;

		AstNode *powerNode = tree->findChildWithName("military_power");
		CHECK_PTR(powerNode);
		state->militaryPower = powerNode->val.Double;
		
		return state;
	}
}