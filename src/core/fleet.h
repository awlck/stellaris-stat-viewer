/* core/fleet.h: Class representation of an interstellar fleet.
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

#ifndef STELLARIS_STAT_VIEWER_FLEET_H
#define STELLARIS_STAT_VIEWER_FLEET_H

#include <QtCore/QObject>

namespace Parsing { struct AstNode; }

namespace Galaxy {
	class Empire;
	class State;

	struct FleetData {
		double power;
		unsigned long corvettes;
		unsigned long destroyers;
		unsigned long cruisers;
		unsigned long battleships;
		unsigned int titans;
		unsigned int colossi;
		unsigned long fallen;
	};

	class Fleet : public QObject {
		Q_OBJECT
	public:
		Fleet(State *parent);
		qint64 getIndex() const;
		const QString &getName() const;
		Empire *getOwner() const;
		bool getIsStation() const;
		double getMilitaryPower() const;
		static Fleet *createFromAst(const Parsing::AstNode *tree, State *parent);
	private:
		qint64 index;
		QString name;
		Empire *owner;
		bool isStation;
		double militaryPower;
	};
}

#endif //STELLARIS_STAT_VIEWER_FLEET_H