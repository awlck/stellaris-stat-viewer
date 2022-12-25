/* core/empire.h: Class representation of a galactic empire.
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

#ifndef STELLARIS_STAT_VIEWER_EMPIRE_H
#define STELLARIS_STAT_VIEWER_EMPIRE_H

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QStringList>

class GameTranslator;

namespace Parsing { struct AstNode; }

namespace Galaxy {
	class Ship;
	class State;

	class Empire : public QObject {
		Q_OBJECT
	public:
		Empire(State *parent);
		qint64 getIndex() const;
		const QString &getName() const;
		double getMilitaryPower() const;
		double getEconomyPower() const;
		double getTechPower() const;
		quint32 getOwnedSystemsCount() const;
		const QMap<QString, double> &getIncomes() const;
		const QStringList &getTechnologies() const;
		static Empire *createFromAst(const Parsing::AstNode *tree, State *parent, const GameTranslator *translator);
#ifdef SSV_WITH_GALAXY_MAP
		inline const QString &getMapColor() const {
			return mapColorName;
		}
#endif
	private:
		qint64 index;
		QString name;
		double militaryPower;
		double economyPower;
		double techPower;
		quint32 ownedSystems;
		QMap<QString, double> incomes;
		QStringList technologies;
#ifdef SSV_WITH_GALAXY_MAP
		QString mapColorName;
#endif
		friend class Ship;
	};
}

#endif //STELLARIS_STAT_VIEWER_EMPIRE_H
