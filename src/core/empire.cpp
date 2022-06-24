/* core/empire.cpp: Extracting empire information from an AST.
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

#include <QtCore/QRegularExpression>

#include "model_private_macros.h"
#include "galaxy_state.h"
#include "gametranslator.h"
#include "parser.h"

using Parsing::AstNode;

namespace Galaxy {
	Empire::Empire(State *parent) : QObject(parent), ownedSystems(0) {}

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

	const QStringList &Empire::getTechnologies() const {
		return technologies;
	}

	Empire *Empire::createFromAst(const AstNode *tree, State *parent, const GameTranslator *translator) {
		Empire *state = new Empire(parent);
		state->index = static_cast<qint64>(QString(tree->myName).toLongLong());
		AstNode *nameNode = tree->findChildWithName("name");
		CHECK_PTR(nameNode);
		if (nameNode->type == Parsing::NT_COMPOUND) {
			// Starting with 3.4 (I think), "name" doesn't hold the name directly.
			auto *keyNode = nameNode->findChildWithName("key");
			if (!keyNode || keyNode->type != Parsing::NT_STRING) {
				delete state;
				return nullptr;
			}
			auto* varsNode = nameNode->findChildWithName("variables");
			// For player empires, the "key" is simply the name.
			if (!varsNode || !translator) {
				if (translator) state->name = translator->getTranslationOf(keyNode->val.Str);
				else state->name = keyNode->val.Str;
			} else {  // we need to go deeper.
				QMap<QString, QString> vars;
				// load all the variables into a convenient format
				ITERATE_CHILDREN(varsNode, var) {
					vars[var->val.firstChild->val.Str] = var->val.lastChild->val.firstChild->val.Str;
				}
				// get the format string
				QString format = translator->getTranslationOf(keyNode->val.Str);
				QRegularExpression angles("<(.*?)>");
				QRegularExpression squares("\\[(.*?)\\]");
				QRegularExpressionMatch amatch;
				while ((amatch = angles.match(format)).hasMatch())
					format.replace(amatch.captured(0), translator->getTranslationOf(vars[amatch.captured(1)]));
				auto smatch = squares.match(format);
				if (smatch.hasMatch())
					format.replace(smatch.captured(0), translator->getTranslationOf(vars[smatch.captured(1)]));
				state->name = translator->getTranslationOf(format);
			}
		} else if (nameNode->type == Parsing::NT_STRING) {
			state->name = nameNode->val.Str;
		} else {
			delete state;
			return nullptr;
		}

		AstNode *techNode = nameNode->nextSibling->nextSibling->nextSibling;
		if (qstrcmp(techNode->myName, "tech_status") != 0) {
			techNode = tree->findChildWithName("tech_status");
			CHECK_COMPOUND(techNode);
		}
		ITERATE_CHILDREN(techNode, aTech) {
			if (qstrcmp(aTech->myName, "technology") == 0 && aTech->type == Parsing::NT_STRING) {
				state->technologies.append(QString(aTech->val.Str));
			}
		}

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

		AstNode *budgetNode = tree->findChildWithName("budget");
		if (budgetNode && budgetNode->type == Parsing::NT_COMPOUND) {
			AstNode *lastMonthNode = budgetNode->findChildWithName("last_month");
			if (lastMonthNode && lastMonthNode->type == Parsing::NT_COMPOUND) {
				AstNode *balanceNode = lastMonthNode->findChildWithName("balance");
				if (balanceNode && balanceNode->type == Parsing::NT_COMPOUND) {
					ITERATE_CHILDREN(balanceNode, anItem) {
						if (anItem->type == Parsing::NT_COMPOUND) {
							ITERATE_CHILDREN(anItem, aResource) {
								// For some reason, game version 2.6 stopped writing integers as e.g. '17.0'
								state->incomes[QString(aResource->myName)] += aResource->type == Parsing::NT_DOUBLE ? aResource->val.Double : (double) aResource->val.Int;
							}
						}
					}
				}
			}
		}
		return state;
	}
}
