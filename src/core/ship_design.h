/* core/ship_design.h: Class representation of a ship design.
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

#pragma once

#ifndef STELLARIS_STAT_VIEWER_SHIP_DESIGN_H
#define STELLARIS_STAT_VIEWER_SHIP_DESIGN_H

#include <QtCore/QObject>

namespace Parsing { struct AstNode; }

namespace Galaxy {
	class State;

	enum class ShipSize {
		StarbaseOutpost,
		StarbaseStarport,
		StarbaseStarhold,
		StarbaseStarfortress,
		StarbaseCitadel,
		DefensePlatSmall,
		DefensePlatMedium,
		DefensePlatLarge,
		IonCannon,
		FallenSmallStation,
		FallenLargeStation,   // speculation
		FallenMassiveStation, // speculation
		MiningStation,
		ResearchStation,
		ObservationStation,
		Corvette,
		Destroyer,
		Cruiser,
		Battleship,
		Titan,
		Colossus,
		TransportShip,
		FallenSmallShip,
		FallenLargeShip,
		FallenMassiveShip,
		ConstructionShip,
		ScienceShip,
		ColonyShip,
		ColonyShipPrivate,
		INVALID
	};

	class ShipDesign : QObject {
		Q_OBJECT
	public:
		ShipDesign(QObject *parent);
		qint64 getIndex() const;
		const QString &getName() const;
		ShipSize getSize() const;
		bool getIsAutogen() const;
		static ShipDesign *createFromAst(Parsing::AstNode *tree, State *parent);
	private:
		qint64 index;
		QString name;
		ShipSize size;
		bool isAutogen;
		static const QMap<QString, ShipSize> shipSizes;
	};
}

#endif //STELLARIS_STAT_VIEWER_SHIP_DESIGN_H
