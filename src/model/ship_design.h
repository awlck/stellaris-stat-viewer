/* model/ship_design.h: Class representation of a ship design.
 *
 * Copyright 2019 by Adrian "ArdiMaster" Welcker, distributed
 * under the MIT License - see file "LICENSE" for details.
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
