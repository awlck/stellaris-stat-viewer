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

#include "research_view.h"

#include "../../../core/empire.h"
#include "../../../core/galaxy_state.h"
#include "../numerictableitem.h"

ResearchView::ResearchView(QWidget* parent) : QTableWidget(parent) {
	setColumnCount(4);
	QStringList headers;
	headers << tr("Name") << tr("Physics") << tr("Society") << tr("Engineering");
	setHorizontalHeaderLabels(headers);
}

void ResearchView::modelChanged(const Galaxy::State* newState) {
	setSortingEnabled(false);
	const QMap<qint64, Galaxy::Empire*>& empires = newState->getEmpires();
	setRowCount(empires.size());
	int i = 0;
	for (auto it = empires.cbegin(); it != empires.cend(); it++) {
		Galaxy::Empire* empire = it.value();
		const QMap<QString, double>& incomes = empire->getIncomes();
		QTableWidgetItem* nameItem = new QTableWidgetItem(empire->getName());
		setItem(i, 0, nameItem);
		NumericTableItem* physicsItem = new NumericTableItem(incomes.value("physics_research", 0.0));
		setItem(i, 1, physicsItem);
		NumericTableItem* societyItem = new NumericTableItem(incomes.value("society_research", 0.0));
		setItem(i, 2, societyItem);
		NumericTableItem* engineeringItem = new NumericTableItem(incomes.value("engineering_research", 0.0));
		setItem(i++, 3, engineeringItem);
	}
	setRowCount(i);
	setSortingEnabled(true);
}