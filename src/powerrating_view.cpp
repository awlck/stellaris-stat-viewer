/* powerrating_view.cpp: Put empire power ratings into a table.
 *
 * Copyright 2019 by Adrian "ArdiMaster" Welcker, distributed
 * under the MIT License - see file "LICENSE" for details.
 */

#include "powerrating_view.h"

#include "model/galaxy_state.h"
#include "model/empire.h"

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
	for (auto it = empires.cbegin(); it != empires.cend(); it++) {
		QTableWidgetItem *itemName = new QTableWidgetItem(it.value()->getName());
		setItem(it.key(), 0, itemName);
		QTableWidgetItem *itemMilitary = new QTableWidgetItem(tr("%1").arg(it.value()->getMilitaryPower()));
		setItem(it.key(), 1, itemMilitary);
		QTableWidgetItem *itemEconomy = new QTableWidgetItem(tr("%1").arg(it.value()->getEconomyPower()));
		setItem(it.key(), 2, itemEconomy);
		QTableWidgetItem *itemTechnology = new QTableWidgetItem(tr("%1").arg(it.value()->getTechPower()));
		setItem(it.key(), 3, itemTechnology);
	}
	setSortingEnabled(true);
}
