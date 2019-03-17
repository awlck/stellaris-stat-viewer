/* powerrating_view.cpp: Put empire power ratings into a table.
 *
 * Copyright 2019 by Adrian "ArdiMaster" Welcker, distributed
 * under the MIT License - see file "LICENSE" for details.
 */

#include "powerrating_view.h"

#include "model/galaxy_state.h"
#include "model/empire.h"
#include "numerictableitem.h"

PowerRatingView::PowerRatingView(QWidget *parent) : QTableWidget(parent) {
	setColumnCount(4);
	QStringList headers;
	headers << tr("Name")<< tr("Military") << tr("Economic") << tr("Technology");
	setHorizontalHeaderLabels(headers);
}

void PowerRatingView::modelChanged(const Galaxy::State *newState) {
	setSortingEnabled(false);
	setRowCount(newState->getEmpires().size());
	const QMap<qint64, Galaxy::Empire*> &empires = newState->getEmpires();
	int i = 0;
	for (auto it = empires.cbegin(); it != empires.cend(); it++) {
		QTableWidgetItem *itemName = new QTableWidgetItem(it.value()->getName());
		setItem(i, 0, itemName);
		NumericTableItem *itemMilitary = new NumericTableItem(tr("%1").arg(it.value()->getMilitaryPower()));
		setItem(i, 1, itemMilitary);
		NumericTableItem *itemEconomy = new NumericTableItem(tr("%1").arg(it.value()->getEconomyPower()));
		setItem(i, 2, itemEconomy);
		NumericTableItem *itemTechnology = new NumericTableItem(tr("%1").arg(it.value()->getTechPower()));
		setItem(i++, 3, itemTechnology);
	}
	setSortingEnabled(true);
}