#include "ship.h"

#include "fleet.h"
#include "galaxy_state.h"
#include "model_private_macros.h"
#include "ship_design.h"
#include "../parser/parser.h"

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
		Q_ASSERT_X(state->fleet != nullptr, "Ship::createFromAst", "invalid fleet");

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
		Q_ASSERT_X(state->design != nullptr, "Ship::createFromAst", "invalid design");

		return state;
	}
}