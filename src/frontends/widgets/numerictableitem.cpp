/* numerictableitem.cpp: Numerically sortable table item.
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

#include "numerictableitem.h"

NumericTableItem::NumericTableItem(qint64 val) : QTableWidgetItem(QString("%1").arg(val)) {};
NumericTableItem::NumericTableItem(double val) : QTableWidgetItem(QString("%1").arg(val)) {};

bool NumericTableItem::operator<(const QTableWidgetItem &other) const {
	if (text() == "") return true;
	bool ok1, ok2;
	bool result = text().toDouble(&ok1) < other.text().toDouble(&ok2);
	if (!(ok1 && ok2)) return true;
	return result;
}