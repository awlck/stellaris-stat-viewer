/* model/ship.h: Class representation of a ship.
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

#ifndef STELLARIS_STAT_VIEWER_SHIP_H
#define STELLARIS_STAT_VIEWER_SHIP_H

#include <QtCore/QObject>

namespace Parsing { struct AstNode; }

namespace Galaxy {
	class Fleet;
	class ShipDesign;
	class State;

	class Ship : public QObject {
		Q_OBJECT
	public:
		Ship(State *parent);
		qint64 getIndex() const;
		const QString &getName() const;
		Fleet *getFleet() const;
		ShipDesign *getDesign() const;
		static Ship *createFromAst(Parsing::AstNode *tree, State *parent);
	private:
		qint64 index;
		QString name;
		Fleet *fleet;
		ShipDesign *design;
	};
}

#endif