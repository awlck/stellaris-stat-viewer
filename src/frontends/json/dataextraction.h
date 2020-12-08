/* frontends/json/dataextraction.h: Header file for JSON data extraction
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

#ifndef STELLARIS_STAT_VIEWER_DATAEXTRACTION_H
#define STELLARIS_STAT_VIEWER_DATAEXTRACTION_H

#include <forward_list>
#include <QtCore/QJsonObject>

namespace Galaxy {
	class Empire;
	class Ship;
	class State;
}

QJsonObject createDataForEmpire(const Galaxy::Empire *e, const std::forward_list<Galaxy::Ship *> &ships);
QJsonObject createJsonFromState(const Galaxy::State *state);

#endif //STELLARIS_STAT_VIEWER_DATAEXTRACTION_H
