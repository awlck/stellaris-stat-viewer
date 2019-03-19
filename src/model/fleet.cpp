/* model/fleet.cpp: Extracting fleet information from an AST.
 *
 * Copyright 2019 by Adrian "ArdiMaster" Welcker, distributed
 * under the MIT License - see file "LICENSE" for details.
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
			CHECK_PTR(stationNode);
		}
		state->isStation = stationNode->val.Bool;

		AstNode *powerNode = stationNode->nextSibling->nextSibling->nextSibling->nextSibling->nextSibling;
		if (qstrcmp(powerNode->myName, "military_power") != 0) {
			powerNode = tree->findChildWithName("military_power");
			CHECK_PTR(powerNode);
		}
		state->militaryPower = powerNode->val.Double;
		
		return state;
	}
}