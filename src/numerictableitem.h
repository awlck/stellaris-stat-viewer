/* numerictableitem.h: QTableWidgetItem that sorts numerically.
 *
 * Copyright 2019 by Adrian "ArdiMaster" Welcker, distributed
 * under the MIT License - see file "LICENSE" for details.
 */

#pragma once

#ifndef STELLARIS_STAT_VIEWER_NUMERICTABLEITEM_H
#define STELLARIS_STAT_VIEWER_NUMERICTABLEITEM_H

#include <QtWidgets/QTableWidgetItem>

class NumericTableItem : public QTableWidgetItem {
public:
	bool operator<(const QTableWidgetItem &other) const override;
};

#endif //STELLARIS_STAT_VIEWER_NUMERICTABLEITEM_H
