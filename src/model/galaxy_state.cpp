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

	const QMap<qint64, Empire *>& State::getEmpires() const {
		return empires;
	}

	State *StateFactory::createFromAst(Parsing::AstNode *tree, QObject *parent) {
		// figure out how many objects we need to create so we can display a proper progress bar
		int done = 0;
		int toDo = 0;
		AstNode *ast_countries = tree->findChildWithName("country");
		toDo += ast_countries->countChildren();

		emit progress(this, done, toDo);
		if (shouldCancel) return nullptr;

		State *state = new State(parent);
		CHECK_COMPOUND(ast_countries);
		ITERATE_CHILDREN(ast_countries, aCountry) {
			Empire *created = Empire::createFromAst(aCountry, state);
			CHECK_PTR(created);
			state->empires.insert(created->getIndex(), created);
			emit progress(this, ++done, toDo);
			if (shouldCancel) { delete state; return nullptr; }
		}
		return state;
	}

	void StateFactory::cancel() {
		shouldCancel = true;
	}
}
