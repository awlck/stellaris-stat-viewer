/* model/empire.cpp: Extracting empire information from an AST.
 *
 * Copyright 2019 by Adrian "ArdiMaster" Welcker, distributed
 * under the MIT License - see file "LICENSE" for details.
 */

#include "empire.h"

#include "model_private_macros.h"
#include "galaxy_state.h"
#include "../parser/parser.h"

using Parsing::AstNode;

namespace Galaxy {
	Empire::Empire(State *parent) : QObject(parent) {}

	qint64 Empire::getIndex() const {
		return index;
	}

	const QString& Empire::getName() const {
		return this->name;
	}

	double Empire::getMilitaryPower() const {
		return this->militaryPower;
	}

	double Empire::getEconomyPower() const {
		return this->economyPower;
	}

	double Empire::getTechPower() const {
		return this->techPower;
	}

	quint32 Empire::getOwnedSystemsCount() const {
		return ownedSystems;
	}

	Empire *Empire::createFromAst(AstNode *tree, State *parent) {
		Empire *state = new Empire(parent);
		state->index = static_cast<qint64>(QString(tree->myName).toLongLong());
		AstNode *nameNode = tree->findChildWithName("name");
		CHECK_PTR(nameNode);
		state->name = nameNode->val.Str;
		AstNode *powerNode = tree->findChildWithName("military_power");
		CHECK_PTR(powerNode);
		state->militaryPower = powerNode->val.Double;
		powerNode = powerNode->nextSibling;
		if (qstrcmp(powerNode->myName, "economy_power") != 0) {
			powerNode = tree->findChildWithName("economy_power");
			CHECK_PTR(powerNode);
		}
		state->economyPower = powerNode->val.Double;
		powerNode = powerNode->nextSibling->nextSibling->nextSibling;
		if (qstrcmp(powerNode->myName, "tech_power") != 0) {
			powerNode = tree->findChildWithName("tech_power");
			CHECK_PTR(powerNode);
		}
		state->techPower = powerNode->val.Double;

		AstNode *planetsNode = tree->findChildWithName("controlled_planets");
		state->ownedSystems = planetsNode ? static_cast<quint32>(planetsNode->countChildren()) : 0;
		return state;
	}
}
