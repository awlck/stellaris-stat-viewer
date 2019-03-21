#pragma once

#ifndef STELLARIS_STAT_VIEWER_ECONOMY_VIEW_H
#define STELLARIS_STAT_VIEWER_ECONOMY_VIEW_H

#include <QtWidgets/QTableWidget>

namespace Galaxy { class State; }

class EconomyView : public QTableWidget {
	Q_OBJECT
public:
	EconomyView(QWidget *parent = nullptr);
public slots:
	void modelChanged(const Galaxy::State *newState);
};

#endif //STELLARIS_STAT_VIEWER_ECONOMY_VIEW_H