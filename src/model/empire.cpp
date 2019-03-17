/* model/empire.cpp: Extracting empire information from an AST.
 *
 * Copyright 2019 by Adrian "ArdiMaster" Welcker, distributed
 * under the MIT License - see file "LICENSE" for details.
 */

#include "empire.h"

#include "model_private_macros.h"
#include "galaxy_state.h"

using namespace Parsing;

namespace Galaxy {
	Empire::Empire(State *parent) : QObject(parent) {}

	qint64 Empire::getIndex() {
		return index;
	}

	const QString& Empire::getName() {
		return this->name;
	}

	double Empire::getMilitaryPower() {
		return this->militaryPower;
	}

	double Empire::getEconomyPower() {
		return this->economyPower;
	}

	double Empire::getTechPower() {
		return this->techPower;
	}

	Empire *Empire::createFromAst(Parsing::AstNode *tree, State *parent) {
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
		return state;
	}
}
