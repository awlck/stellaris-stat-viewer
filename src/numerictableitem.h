/* numerictableitem.h: QTableWidgetItem that sorts numerically.
 *
 * Copyright 2019 Adrian "ArdiMaster" Welcker
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#ifndef STELLARIS_STAT_VIEWER_NUMERICTABLEITEM_H
#define STELLARIS_STAT_VIEWER_NUMERICTABLEITEM_H

#include <QtWidgets/QTableWidgetItem>

class NumericTableItem : public QTableWidgetItem {
public:
	NumericTableItem(qint64 val);
	NumericTableItem(double val);
	bool operator<(const QTableWidgetItem &other) const override;
};

#endif //STELLARIS_STAT_VIEWER_NUMERICTABLEITEM_H
