/* fleets_view.h: Display empire fleet statistics as a table.
 *
 * Copyright 2019 by Adrian "ArdiMaster" Welcker, distributed
 * under the MIT License - see file "LICENSE" for details.
 */

#pragma once

#ifndef STELLARIS_STAT_VIEWER_MILITARY_VIEW_H
#define STELLARIS_STAT_VIEWER_MILITARY_VIEW_H

#include <QtWidgets/QTableWidget>

namespace Galaxy { class State; }

class FleetsView : public QTableWidget {
	Q_OBJECT
public:
	FleetsView(QWidget *parent = nullptr);

public slots:
	void modelChanged(const Galaxy::State *newState);
};

#endif