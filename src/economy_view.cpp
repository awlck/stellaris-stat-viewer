#include "economy_view.h"

#include "model/empire.h"
#include "model/galaxy_state.h"
#include "numerictableitem.h"

EconomyView::EconomyView(QWidget *parent) : QTableWidget(parent) {
	setColumnCount(8);
	QStringList headers;
	headers << tr("Name") << tr("Energy") << tr("Minerals") << tr("Food") << tr("Influence")
			<< tr("Unity") << tr("Alloys") << tr("Consumer Goods");
	setHorizontalHeaderLabels(headers);
}

void EconomyView::modelChanged(const Galaxy::State *newState) {
	setSortingEnabled(false);
	const QMap<qint64, Galaxy::Empire *> empires = newState->getEmpires();
	setRowCount(empires.size());
	int i = 0;
	for (auto it = empires.cbegin(); it != empires.cend(); it++) {
		Galaxy::Empire *empire = it.value();
		const QMap<QString, double> &incomes = empire->getIncomes();
		QTableWidgetItem *nameItem = new QTableWidgetItem(empire->getName());
		setItem(i, 0, nameItem);
		NumericTableItem *energyItem = new NumericTableItem(incomes["energy"]);
		setItem(i, 1, energyItem);
		NumericTableItem *mineralsItem = new NumericTableItem(incomes["minerals"]);
		setItem(i, 2, mineralsItem);
		NumericTableItem *foodItem = new NumericTableItem(incomes["food"]);
		setItem(i, 3, foodItem);
		NumericTableItem *influenceItem = new NumericTableItem(incomes["influence"]);
		setItem(i, 4, influenceItem);
		NumericTableItem *unityItem = new NumericTableItem(incomes["unity"]);
		setItem(i, 5, unityItem);
		NumericTableItem *alloysItem = new NumericTableItem(incomes["alloys"]);
		setItem(i, 6, alloysItem);
		NumericTableItem *consumerGoodsItem = new NumericTableItem(incomes["consumer_goods"]);
		setItem(i++, 7, consumerGoodsItem);
	}
	setSortingEnabled(true);
}