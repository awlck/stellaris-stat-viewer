/* model/galaxy_state.cpp: Extracting data from a savegame file.
 *
 * Copyright 2019 by Adrian "ArdiMaster" Welcker, distributed
 * under the MIT License - see file "LICENSE" for details.
 */

#include "galaxy_state.h"

#include "empire.h"
#include "model_private_macros.h"

using Parsing::AstNode;

namespace Galaxy {
	State::State(QObject *parent) : QObject(parent) {}

	Empire* State::getEmpireWithId(qint64 id) {
		return empires.value(id, nullptr);
	}

	State *State::createFromAst(Parsing::AstNode *tree, QObject *parent) {
		State *state = new State(parent);
		AstNode *ast_countries = tree->findChildWithName("country");
		CHECK_COMPOUND(ast_countries);
		ITERATE_CHILDREN(ast_countries, aCountry) {
			Empire *created = Empire::createFromAst(aCountry);
			CHECK_PTR(created);
			state->empires.insert(created->getIndex(), created);
		}
		return state;
	}
}
