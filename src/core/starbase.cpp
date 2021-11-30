#include "starbase.h"

#include "galaxy_state.h"
#include "model_private_macros.h"
#include "parser.h"

using Parsing::AstNode;

namespace Galaxy {
	Starbase::Starbase(State *parent) : QObject(parent) {}

	Starbase *Starbase::createFromAst(const Parsing::AstNode *tree, State *parent) {
		Starbase *state = new Starbase(parent);
		state->index = static_cast<int64_t>(QString(tree->myName).toLongLong());
		AstNode *stationNode = tree->findChildWithName("station");
		CHECK_PTR(stationNode);
		AstNode *ownerNode = stationNode->nextSibling;
		if (!ownerNode || strcmp(ownerNode->myName, "owner") != 0) {
			ownerNode = tree->findChildWithName("owner");
		}
		CHECK_PTR(ownerNode);

		state->owner = parent->getEmpireWithId(ownerNode->val.Int);
		state->station = parent->getShips().value(stationNode->val.Int, nullptr);

		return state;
	}
}