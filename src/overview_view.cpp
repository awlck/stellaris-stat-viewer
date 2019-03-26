/* overview_view.cpp: Put empire power ratings into a table.
 *
 * Copyright 2019 by Adrian "ArdiMaster" Welcker, distributed
 * under the MIT License - see file "LICENSE" for details.
 */

#include "overview_view.h"

#include "model/galaxy_state.h"
#include "model/empire.h"
#include "numerictableitem.h"

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
