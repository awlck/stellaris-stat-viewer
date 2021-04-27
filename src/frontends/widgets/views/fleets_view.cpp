/* fleets_view.cpp: Put empire fleet statistics into a table.
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

#include "fleets_view.h"

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>

#include "../../../core/empire.h"
#include "../../../core/fleet.h"
#include "../../../core/galaxy_state.h"
#include "../../../core/ship.h"
#include "../../../core/ship_design.h"
#include "../numerictableitem.h"

class FleetsViewInternal : public QTableWidget {
	Q_OBJECT
public:
	FleetsViewInternal(QWidget *parent = nullptr);
	void recalculate(const Galaxy::State *state, bool includeStations);
};

FleetsView::FleetsView(QWidget *parent) : QWidget(parent) {
	layout = new QVBoxLayout(this);
	view = new FleetsViewInternal;
	includeStations = new QCheckBox(tr("Include stations in fleet power"));
	layout->addWidget(includeStations);
	layout->addWidget(view);
	connect(includeStations, &QCheckBox::stateChanged, this, &FleetsView::onCheckboxChanged);
}

void FleetsView::modelChanged(const Galaxy::State *newState) {
	currentState = newState;
	view->recalculate(currentState, includeStations->checkState() == Qt::Checked);
}

void FleetsView::onCheckboxChanged([[maybe_unused]] int newState) {
	if (currentState) view->recalculate(currentState, includeStations->checkState() == Qt::Checked);
}

FleetsViewInternal::FleetsViewInternal(QWidget *parent) : QTableWidget(parent) {
	setColumnCount(9);
	QStringList headers;
	headers << tr("Name") << tr("Total Fleet Power") << tr("# Corvettes") << tr("# Destroyers")
			<< tr("# Cruisers") << tr("# Battleships") << ("# Titans") << ("# Colossi") << ("# FE ships");
	setHorizontalHeaderLabels(headers);
}

void FleetsViewInternal::recalculate(const Galaxy::State *state, bool includeStations) {
	using Galaxy::ShipSize;
	using Galaxy::FleetData;
	setSortingEnabled(false);
	const QMap<qint64, Galaxy::Fleet*> &fleets = state->getFleets();
	const QMap<qint64, Galaxy::Ship *> &ships = state->getShips();

	QMap<Galaxy::Empire *, FleetData> empireTotals;
	for (auto fit = fleets.cbegin(); fit != fleets.cend(); fit++) {
		Galaxy::Empire *owner = fit.value()->getOwner();
		FleetData data = empireTotals.value(owner, { 0, 0, 0, 0, 0, 0, 0, 0 });
		if (includeStations || !fit.value()->getIsStation()) {
			data.power += fit.value()->getMilitaryPower();
		}
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
		NumericTableItem *itemMilitary = new NumericTableItem((qint64) it.value().power);
		setItem(i, 1, itemMilitary);
		NumericTableItem *itemCorvettes = new NumericTableItem((qint64) it.value().corvettes);
		setItem(i, 2, itemCorvettes);
		NumericTableItem *itemDestroyers = new NumericTableItem((qint64) it.value().destroyers);
		setItem(i, 3, itemDestroyers);
		NumericTableItem *itemCruisers = new NumericTableItem((qint64) it.value().cruisers);
		setItem(i, 4, itemCruisers);
		NumericTableItem *itemBattleships = new NumericTableItem((qint64) it.value().battleships);
		setItem(i, 5, itemBattleships);
		NumericTableItem *itemTitans = new NumericTableItem((qint64) it.value().titans);
		setItem(i, 6, itemTitans);
		NumericTableItem *itemColossi = new NumericTableItem((qint64) it.value().colossi);
		setItem(i, 7, itemColossi);
		NumericTableItem *itemFeShips = new NumericTableItem((qint64) it.value().fallen);
		setItem(i++, 8, itemFeShips);
	}
	setSortingEnabled(true);
}

#include "fleets_view.moc"
