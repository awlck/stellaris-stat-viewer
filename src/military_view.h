#pragma once

#ifndef STELLARIS_STAT_VIEWER_MILITARY_VIEW_H
#define STELLARIS_STAT_VIEWER_MILITARY_VIEW_H

#include <QtWidgets/QTableWidget>

namespace Galaxy { class State; }

class MilitaryView : public QTableWidget {
	Q_OBJECT
public:
	MilitaryView(QWidget *parent = nullptr);

public slots:
	void modelChanged(const Galaxy::State *newState);
};

#endif