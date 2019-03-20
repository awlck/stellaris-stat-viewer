/* model/ship_design.cpp: Extracting ship design information from an AST.
 *
 * Copyright 2019 by Adrian "ArdiMaster" Welcker, distributed
 * under the MIT License - see file "LICENSE" for details.
 */

#include "ship_design.h"

#include "galaxy_state.h"
#include "model_private_macros.h"
#include "../parser/parser.h"

using Parsing::AstNode;

namespace Galaxy {
	const QMap<QString, ShipSize> ShipDesign::shipSizes = {
			{ "starbase_outpost", ShipSize::StarbaseOutpost },
			{ "starbase_starport", ShipSize::StarbaseStarport },
			{ "starbase_citadel", ShipSize::StarbaseCitadel },
			{ "starbase_starhold", ShipSize::StarbaseStarhold },
			{ "starbase_starfortress", ShipSize::StarbaseStarfortress },
			{ "military_station_small", ShipSize::DefensePlatSmall },
			{ "military_station_medium", ShipSize::DefensePlatMedium },
			{ "military_station_large", ShipSize::DefensePlatLarge },
			{ "ion_cannon", ShipSize::IonCannon },
			{ "mining_station", ShipSize::MiningStation },
			{ "research_station", ShipSize::ResearchStation },
			{ "observation_station", ShipSize::ObservationStation },
			{ "corvette", ShipSize::Corvette },
			{ "destroyer", ShipSize::Destroyer },
			{ "cruiser", ShipSize::Cruiser },
			{ "battleship", ShipSize::Battleship },
			{ "titan", ShipSize::Titan },
			{ "colossus", ShipSize::Colossus },
			{ "transport", ShipSize::TransportShip },
			{ "constructor", ShipSize::ConstructionShip },
			{ "science", ShipSize::ScienceShip },
			{ "colonizer", ShipSize::ColonyShip },
			{ "sponsored_colonizer", ShipSize::ColonyShipPrivate }
	};

	ShipDesign::ShipDesign(QObject *parent) : QObject(parent) {}

	qint64 ShipDesign::getIndex() const {
		return index;
	}

	const QString &ShipDesign::getName() const {
		return name;
	}

	ShipSize ShipDesign::getSize() const {
		return size;
	}

	bool ShipDesign::getIsAutogen() const {
		return isAutogen;
	}

	ShipDesign *ShipDesign::createFromAst(AstNode *tree, Galaxy::State *parent) {
		ShipDesign *state = new ShipDesign(parent);
		state->index = static_cast<qint64>(QString(tree->myName).toLongLong());

		AstNode *nameNode = tree->findChildWithName("name");
		AstNode *sizeNode;
		if (nameNode) {
			state->name = nameNode != nullptr ? QString(nameNode->val.Str) : tr("<anonymous design>");

			sizeNode = nameNode->nextSibling;
			if (qstrcmp(sizeNode->myName, "ship_size") != 0) {
				sizeNode = tree->findChildWithName("ship_size");
				CHECK_PTR(sizeNode);
			}
		} else {
			sizeNode = tree->findChildWithName("ship_size");
			CHECK_PTR(sizeNode);
		}
		state->size = shipSizes.value(sizeNode->val.Str, ShipSize::INVALID);

		AstNode *autoGenNode = tree->findChildWithName("auto_gen_design");
		state->isAutogen = autoGenNode != nullptr ? autoGenNode->val.Bool : false;

		return state;
	}
}
