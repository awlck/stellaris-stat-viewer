/* core/ship.cpp: Extracting ship information from an AST.
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

#include "ship.h"

#include "fleet.h"
#include "galaxy_state.h"
#include "model_private_macros.h"
#include "ship_design.h"
#include "parser.h"

using Parsing::AstNode;

namespace Galaxy {
	Ship::Ship(State *parent) : QObject(parent) {}

	qint64 Ship::getIndex() const {
		return index;
	}

	const QString &Ship::getName() const {
		return name;
	}

	Fleet *Ship::getFleet() const {
		return fleet;
	}

	ShipDesign *Ship::getDesign() const {
		return design;
	}

	Ship *Ship::createFromAst(AstNode *tree, State *parent) {
		Ship *state = new Ship(parent);
		state->index = static_cast<qint64>(QString(tree->myName).toLongLong());
		
		AstNode *fleetNode = tree->findChildWithName("fleet");
		CHECK_PTR(fleetNode);
		state->fleet = parent->getFleets().value(fleetNode->val.Int);
		// Q_ASSERT_X(state->fleet != nullptr, "Ship::createFromAst", "invalid fleet");
		CHECK_PTR(state->fleet);

		AstNode *nameNode = fleetNode->nextSibling;
		if (qstrcmp(nameNode->myName, "name") != 0) {
			nameNode = tree->findChildWithName("name");
			CHECK_PTR(nameNode);
		}
		state->name = nameNode->val.Str;

		AstNode *designNode = nameNode->nextSibling->nextSibling;
		if (qstrcmp(designNode->myName, "ship_design") != 0) {
			designNode = tree->findChildWithName("ship_design");
			CHECK_PTR(designNode);
		}
		state->design = parent->getShipDesigns().value(designNode->val.Int);
		// Q_ASSERT_X(state->design != nullptr, "Ship::createFromAst", "invalid design");
		CHECK_PTR(state->design);

		return state;
	}
}