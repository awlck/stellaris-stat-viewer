/* research_view.cpp: Display empire research as a table.
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

#include "strategic_resources_view.h"

#include "../../../core/empire.h"
#include "../../../core/galaxy_state.h"
#include "../numerictableitem.h"

StrategicResourcesView::StrategicResourcesView(QWidget* parent) : QTableWidget(parent) {
	setColumnCount(8);
	QStringList headers;
	headers << tr("Name") << tr("Volatile Motes") << tr("Rare Cystals") << tr("Exotic Gases") << tr("Zro") << tr("Dark Matter") << tr("Living Metal") << tr("Nanites");
	setHorizontalHeaderLabels(headers);
}

void StrategicResourcesView::modelChanged(const Galaxy::State* newState) {
	setSortingEnabled(false);
	const QMap<qint64, Galaxy::Empire*>& empires = newState->getEmpires();
	setRowCount(empires.size());
	int i = 0;
	for (auto it = empires.cbegin(); it != empires.cend(); it++) {
		Galaxy::Empire* empire = it.value();
		const QMap<QString, double>& incomes = empire->getIncomes();
		QTableWidgetItem* nameItem = new QTableWidgetItem(empire->getName());
		setItem(i, 0, nameItem);
		NumericTableItem* motesItem = new NumericTableItem(incomes.value("volatile_motes", 0.0));
		setItem(i, 1, motesItem);
		NumericTableItem* crystalsItem = new NumericTableItem(incomes.value("rare_crystals", 0.0));
		setItem(i, 2, crystalsItem);
		NumericTableItem* gasesItem = new NumericTableItem(incomes.value("exotic_gases", 0.0));
		setItem(i, 3, gasesItem);
		NumericTableItem* zroItem = new NumericTableItem(incomes.value("sr_zro", 0.0));
		setItem(i, 4, zroItem);
		NumericTableItem* darkMatterItem = new NumericTableItem(incomes.value("sr_dark_matter", 0.0));
		setItem(i, 5, darkMatterItem);
		NumericTableItem* livingMetalItem = new NumericTableItem(incomes.value("sr_living_metal", 0.0));
		setItem(i, 6, livingMetalItem);
		NumericTableItem* nanitesItem = new NumericTableItem(incomes.value("nanites", 0.0));
		setItem(i++, 7, nanitesItem);
	}
	setRowCount(i);
	setSortingEnabled(true);
}