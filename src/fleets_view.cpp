/* fleets_view.cpp: Put empire fleet statistics into a table.
 *
 * Copyright 2019 by Adrian "ArdiMaster" Welcker, distributed
 * under the MIT License - see file "LICENSE" for details.
 */

#include "fleets_view.h"

#include "model/empire.h"
#include "model/fleet.h"
#include "model/galaxy_state.h"
#include "model/ship.h"
#include "model/ship_design.h"
#include "numerictableitem.h"

FleetsView::FleetsView(QWidget *parent) : QTableWidget(parent) {
	setColumnCount(9);
	QStringList headers;
	headers << tr("Name") << tr("Total Fleet Power") << tr("# Corvettes") << tr("# Destroyers")
			<< tr("# Cruisers") << tr("# Battleships") << ("# Titans") << ("# Colossi") << ("# FE ships");
	setHorizontalHeaderLabels(headers);
}

struct FleetData {
	double power;
	unsigned long corvettes;
	unsigned long destroyers;
	unsigned long cruisers;
	unsigned long battleships;
	unsigned int titans;
	unsigned int colossi;
	unsigned long fallen;
};

void FleetsView::modelChanged(const Galaxy::State *newState) {
	using Galaxy::ShipSize;
	setSortingEnabled(false);
	const QMap<qint64, Galaxy::Fleet*> &fleets = newState->getFleets();
	const QMap<qint64, Galaxy::Ship *> &ships = newState->getShips();
	
	// QMap<Galaxy::Empire *, double> empireTotalMilitary;

	QMap<Galaxy::Empire *, FleetData> empireTotals;
	for (auto fit = fleets.cbegin(); fit != fleets.cend(); fit++) {
		Galaxy::Empire *owner = fit.value()->getOwner();
		// empireTotals[owner] = empireTotals.value(owner, { 0, 0, 0, 0, 0, 0, 0 }).power + fit.value()->getMilitaryPower();
		FleetData data = empireTotals.value(owner, { 0 });
		data.power += fit.value()->getMilitaryPower();
		empireTotals[owner] = data;
	}
	setRowCount(empireTotals.size());
	for (auto sit = ships.cbegin(); sit != ships.cend(); sit++) {
		Galaxy::Ship *ship = sit.value();
		Galaxy::ShipDesign *design = ship->getDesign();
		Galaxy::Fleet *fleet = ship->getFleet();
		Galaxy::Empire *owner = fleet->getOwner();
		switch (design->getSize()) {
		case ShipSize::Corvette:
			empireTotals[owner].corvettes += 1;
			break;
		case ShipSize::Destroyer:
			empireTotals[owner].destroyers += 1;
			break;
		case ShipSize::Cruiser:
			empireTotals[owner].cruisers += 1;
			break;
		case ShipSize::Battleship:
			empireTotals[owner].battleships += 1;
			break;
		case ShipSize::Titan:
			empireTotals[owner].titans += 1;
			break;
		case ShipSize::Colossus:
			empireTotals[owner].colossi += 1;
			break;
		case ShipSize::FallenSmallShip:
		case ShipSize::FallenLargeShip:
		case ShipSize::FallenMassiveShip:
			empireTotals[owner].fallen += 1;
			break;
		/* case ShipSize::INVALID:
			Q_UNREACHABLE(); */  // meh
		default:
			continue;
		}
	}
	int i = 0;
	for (auto it = empireTotals.cbegin(); it != empireTotals.cend(); it++) {
		QTableWidgetItem *itemName = new QTableWidgetItem(it.key()->getName());
		setItem(i, 0, itemName);
		NumericTableItem *itemMilitary = new NumericTableItem(tr("%1").arg(it.value().power));
		setItem(i, 1, itemMilitary);
		NumericTableItem *itemCorvettes = new NumericTableItem(tr("%1").arg(it.value().corvettes));
		setItem(i, 2, itemCorvettes);
		NumericTableItem *itemDestroyers = new NumericTableItem(tr("%1").arg(it.value().destroyers));
		setItem(i, 3, itemDestroyers);
		NumericTableItem *itemCruisers = new NumericTableItem(tr("%1").arg(it.value().cruisers));
		setItem(i, 4, itemCruisers);
		NumericTableItem *itemBattleships = new NumericTableItem(tr("%1").arg(it.value().battleships));
		setItem(i, 5, itemBattleships);
		NumericTableItem *itemTitans = new NumericTableItem(tr("%1").arg(it.value().titans));
		setItem(i, 6, itemTitans);
		NumericTableItem *itemColossi = new NumericTableItem(tr("%1").arg(it.value().colossi));
		setItem(i, 7, itemColossi);
		NumericTableItem *itemFeShips = new NumericTableItem(tr("%1").arg(it.value().fallen));
		setItem(i++, 8, itemFeShips);
	}
	setSortingEnabled(true);
}
