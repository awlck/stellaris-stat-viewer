/* core/ship_design.cpp: Extracting ship design information from an AST.
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

#include "ship_design.h"

#include "galaxy_state.h"
#include "model_private_macros.h"
#include "parser.h"

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
			{ "military_station_small_fallen_empire", ShipSize::FallenSmallStation },
			{ "military_station_large_fallen_empire", ShipSize::FallenLargeStation },      // speculation
			{ "military_station_massive_fallen_empire", ShipSize::FallenMassiveStation },  // speculation
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
			{ "small_ship_fallen_empire", ShipSize::FallenSmallShip },
			{ "large_ship_fallen_empire", ShipSize::FallenLargeShip },
			{ "massive_ship_fallen_empire", ShipSize::FallenMassiveShip },
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
			state->name = QString(nameNode->val.Str);

			sizeNode = nameNode->nextSibling;
			if (qstrcmp(sizeNode->myName, "ship_size") != 0) {
				sizeNode = tree->findChildWithName("ship_size");
				CHECK_PTR(sizeNode);
			}
		} else {
			state->name = tr("<anonymous design>");
			sizeNode = tree->findChildWithName("ship_size");
			CHECK_PTR(sizeNode);
		}
		state->size = shipSizes.value(sizeNode->val.Str, ShipSize::INVALID);

		AstNode *autoGenNode = tree->findChildWithName("auto_gen_design");
		state->isAutogen = autoGenNode != nullptr ? autoGenNode->val.Bool : false;

		return state;
	}
}
