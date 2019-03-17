/* powerrating_view.h: Display the three empire power ratings in a table.
 *
 * Copyright 2019 by Adrian "ArdiMaster" Welcker, distributed
 * under the MIT License - see file "LICENSE" for details.
 */

#pragma once

#ifndef STELLARIS_STAT_VIEWER_POWERRATING_VIEW_H
#define STELLARIS_STAT_VIEWER_POWERRATING_VIEW_H

#include <QtWidgets/QTableWidget>

namespace Galaxy { class State; }

class PowerRatingView : public QTableWidget {
	Q_OBJECT
public:
	PowerRatingView(QWidget *parent = nullptr);

public slots:
	void modelChanged(const Galaxy::State *newState);
};

#endif //STELLARIS_STAT_VIEWER_POWERRATING_VIEW_H
