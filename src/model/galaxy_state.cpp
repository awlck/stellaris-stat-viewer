//
// Created by Adrian Welcker on 2019-03-15.
//

#include "galaxy_state.h"

#include "empire.h"
#include "model_private_macros.h"

using Parsing::AstNode;

namespace Galaxy {
	State::State(QObject *parent) : QObject(parent) {}

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
