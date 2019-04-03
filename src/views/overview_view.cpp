/* overview_view.cpp: Put empire power ratings into a table.
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

#include "overview_view.h"

#include "../model/galaxy_state.h"
#include "../model/empire.h"
#include "../numerictableitem.h"

OverviewView::OverviewView(QWidget *parent) : QTableWidget(parent) {
	setColumnCount(5);
	QStringList headers;
	headers << tr("Name")<< tr("Military") << tr("Economy") << tr("Technology") << tr("Systems owned");
	setHorizontalHeaderLabels(headers);
}

QSize OverviewView::sizeHint() const {
	return { 850, 650 };
}

void OverviewView::modelChanged(const Galaxy::State *newState) {
	setSortingEnabled(false);
	setRowCount(newState->getEmpires().size());
	const QMap<qint64, Galaxy::Empire*> &empires = newState->getEmpires();
	int i = 0;
	for (auto it = empires.cbegin(); it != empires.cend(); it++) {
		QTableWidgetItem *itemName = new QTableWidgetItem(it.value()->getName());
		setItem(i, 0, itemName);
		NumericTableItem *itemMilitary = new NumericTableItem(it.value()->getMilitaryPower());
		setItem(i, 1, itemMilitary);
		NumericTableItem *itemEconomy = new NumericTableItem(it.value()->getEconomyPower());
		setItem(i, 2, itemEconomy);
		NumericTableItem *itemTechnology = new NumericTableItem(it.value()->getTechPower());
		setItem(i, 3, itemTechnology);
		NumericTableItem *itemSystems = new NumericTableItem((qint64) it.value()->getOwnedSystemsCount());
		setItem(i++, 4, itemSystems);
	}
	setSortingEnabled(true);
}
