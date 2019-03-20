/* model/galaxy_state.cpp: Extracting data from a savegame file.
 *
 * Copyright 2019 by Adrian "ArdiMaster" Welcker, distributed
 * under the MIT License - see file "LICENSE" for details.
 */

#include "galaxy_state.h"

#include "empire.h"
#include "fleet.h"
#include "model_private_macros.h"
#include "ship.h"
#include "ship_design.h"

using Parsing::AstNode;

namespace Galaxy {
	State::State(QObject *parent) : QObject(parent) {}

	Empire* State::getEmpireWithId(qint64 id) {
		return empires.value(id, nullptr);
	}

	const QMap<qint64, Empire *>& State::getEmpires() const {
		return empires;
	}

	const QMap<qint64, Fleet *>& State::getFleets() const {
		return fleets;
	}

	const QMap<qint64, Ship *>& State::getShips() const {
		return ships;
	}

	const QMap<qint64, ShipDesign *>& State::getShipDesigns() const {
		return shipDesigns;
	}

	State *StateFactory::createFromAst(Parsing::AstNode *tree, QObject *parent) {
		// figure out how many objects we need to create so we can display a proper progress bar
		int done = 0;
		int toDo = 0;
		AstNode *ast_countries = tree->findChildWithName("country");
		AstNode *ast_fleets = tree->findChildWithName("fleet");
		AstNode *ast_shipDesigns = tree->findChildWithName("ship_design");
		AstNode *ast_ships = tree->findChildWithName("ships");
		toDo += ast_countries->countChildren();
		toDo += ast_fleets->countChildren();
		toDo += ast_shipDesigns->countChildren();
		toDo += ast_ships->countChildren();

		emit progress(this, done, toDo);
		if (shouldCancel) return nullptr;

		State *state = new State(parent);
		CHECK_COMPOUND(ast_countries);
		ITERATE_CHILDREN(ast_countries, aCountry) {
			Empire *created = Empire::createFromAst(aCountry, state);
			if (created) {
				// sometimes, the save file contains nonsensical lines such as "16777248=none"
				state->empires.insert(created->getIndex(), created);
			}
			emit progress(this, ++done, toDo);
			if (shouldCancel) { delete state; return nullptr; }
		}

		CHECK_COMPOUND(ast_fleets);
		ITERATE_CHILDREN(ast_fleets, aFleet) {
			Fleet *created = Fleet::createFromAst(aFleet, state);
			if (created) {
				state->fleets.insert(created->getIndex(), created);
			}
			emit progress(this, ++done, toDo);
			if (shouldCancel) { delete state; return nullptr; }
		}

		CHECK_COMPOUND(ast_shipDesigns);
		ITERATE_CHILDREN(ast_shipDesigns, aDesign) {
			ShipDesign *created = ShipDesign::createFromAst(aDesign, state);
			if (created) {
				state->shipDesigns.insert(created->getIndex(), created);
			}
			emit progress(this, ++done, toDo);
			if (shouldCancel) { delete state; return nullptr; }
		}

		CHECK_COMPOUND(ast_ships);
		ITERATE_CHILDREN(ast_ships, aShip) {
			Ship *created = Ship::createFromAst(aShip, state);
			if (created) {
				state->ships.insert(created->getIndex(), created);
			}
			emit progress(this, ++done, toDo);
			if (shouldCancel) { delete state; return nullptr; }
		}
		return state;
	}

	void StateFactory::cancel() {
		shouldCancel = true;
	}
}
