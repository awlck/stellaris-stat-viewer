/* frontends/json/dataextraction.cpp: Putting stats into JSON objects.
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

#include "dataextraction.h"

#include "../../core/empire.h"
#include "../../core/fleet.h"
#include "../../core/ship.h"
#include "../../core/ship_design.h"
#include "../../core/galaxy_state.h"

QJsonObject getOverviewForEmpire(const Galaxy::Empire *empire) {
	QJsonObject overview;
	overview["military"] = empire->getMilitaryPower();
	overview["economy"] = empire->getEconomyPower();
	overview["technology"] = empire->getTechPower();
	overview["systemsOwned"] = (qint64) empire->getOwnedSystemsCount();
	return overview;
}

QJsonObject getFleetsForEmpire(const QLinkedList<Galaxy::Ship *> &shipsOfEmpire) {
	QJsonObject fleetObj;
	using Galaxy::ShipSize;
	using Galaxy::FleetData;

	FleetData empireTotals { 0, 0, 0, 0, 0, 0, 0, 0 };
	for (auto ship : shipsOfEmpire) {
		Galaxy::ShipDesign *design = ship->getDesign();
		switch (design->getSize()) {
			case ShipSize::Corvette:
				empireTotals.corvettes += 1;
				break;
			case ShipSize::Destroyer:
				empireTotals.destroyers += 1;
				break;
			case ShipSize::Cruiser:
				empireTotals.cruisers += 1;
				break;
			case ShipSize::Battleship:
				empireTotals.battleships += 1;
				break;
			case ShipSize::Titan:
				empireTotals.titans += 1;
				break;
			case ShipSize::Colossus:
				empireTotals.colossi += 1;
				break;
			case ShipSize::FallenSmallShip:
			case ShipSize::FallenLargeShip:
			case ShipSize::FallenMassiveShip:
				empireTotals.fallen += 1;
				break;
				/* case ShipSize::INVALID:
					Q_UNREACHABLE(); */  // meh
			default:
				continue;
		}
	}

	fleetObj["corvettes"] = (qint64) empireTotals.corvettes;
	fleetObj["destroyers"] = (qint64) empireTotals.destroyers;
	fleetObj["cruisers"] = (qint64) empireTotals.cruisers;
	fleetObj["battleships"] = (qint64) empireTotals.battleships;
	fleetObj["titans"] = (qint64) empireTotals.titans;
	fleetObj["colossi"] = (qint64) empireTotals.colossi;
	fleetObj["fallen"] = (qint64) empireTotals.fallen;
	return fleetObj;
}

QJsonObject getEconomyForEmpire(const Galaxy::Empire *empire) {
	QJsonObject economy;
	const QMap<QString, double> &incomes = empire->getIncomes();
	// Unfortunately, no direct conversion from QMap to QJsonObject.
	economy["energy"] = incomes["energy"];
	economy["minerals"] = incomes["minerals"];
	economy["food"] = incomes["food"];
	economy["influence"] = incomes["influence"];
	economy["unity"] = incomes["unity"];
	economy["alloys"] = incomes["alloys"];
	economy["consumer_goods"] = incomes["consumer_goods"];
	return economy;
}

QJsonObject createDataForEmpire(const Galaxy::Empire *empire, const QLinkedList<Galaxy::Ship *> &shipsOfEmpire) {
	QJsonObject result;

	result["overview"] = getOverviewForEmpire(empire);
	result["military"] = getFleetsForEmpire(shipsOfEmpire);
	result["economy"] = getEconomyForEmpire(empire);

	return result;
}
