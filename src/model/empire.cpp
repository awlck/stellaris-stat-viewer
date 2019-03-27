/* model/empire.cpp: Extracting empire information from an AST.
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

	const QMap<QString, double> &Empire::getIncomes() const {
		return incomes;
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

		AstNode *budgetNode = tree->findChildWithName("budget");
		if (budgetNode && budgetNode->type == Parsing::NT_COMPOUND) {
			AstNode *lastMonthNode = budgetNode->findChildWithName("last_month");
			if (lastMonthNode && lastMonthNode->type == Parsing::NT_COMPOUND) {
				AstNode *balanceNode = lastMonthNode->findChildWithName("balance");
				if (balanceNode && balanceNode->type == Parsing::NT_COMPOUND) {
					ITERATE_CHILDREN(balanceNode, anItem) {
						if (anItem->type == Parsing::NT_COMPOUND) {
							ITERATE_CHILDREN(anItem, aResource) {
								state->incomes[QString(aResource->myName)] += aResource->val.Double;
							}
						}
					}
				}
			}
		}
		return state;
	}
}
