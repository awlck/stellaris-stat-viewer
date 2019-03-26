/* overview_view.h: Display an overview in a table.
 *
 * Copyright 2019 by Adrian "ArdiMaster" Welcker, distributed
 * under the MIT License - see file "LICENSE" for details.
 */

#pragma once

#ifndef STELLARIS_STAT_VIEWER_POWERRATING_VIEW_H
#define STELLARIS_STAT_VIEWER_POWERRATING_VIEW_H

#include <QtWidgets/QTableWidget>

namespace Galaxy { class State; }

class OverviewView : public QTableWidget {
	Q_OBJECT
public:
	OverviewView(QWidget *parent = nullptr);
	QSize sizeHint() const override;

public slots:
	void modelChanged(const Galaxy::State *newState);
};

#endif //STELLARIS_STAT_VIEWER_POWERRATING_VIEW_H
