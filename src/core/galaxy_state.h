/* core/galaxy_state.h: Class representation of a savegame file.
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

#ifndef STELLARIS_STAT_VIEWER_GALAXY_STATE_H
#define STELLARIS_STAT_VIEWER_GALAXY_STATE_H

#include "ssv_core.h"

#include <QtCore/QObject>
#include <QtCore/QMap>

namespace Parsing { struct AstNode; }

namespace Galaxy {
	class Empire;
	class Fleet;
	class Ship;
	class ShipDesign;

	class DLLEXPORT State : public QObject {
		Q_OBJECT
		/* No destructor necessary: Empire, etc. all inherit from QObject, and their
		 * instances will be created with the State instance as their parent, so
		 * the QObject system will take care of deleting the children. */
	public:
		State(QObject *parent = nullptr);
		const QString &getDate() const;
		Empire *getEmpireWithId(qint64 id);
		const QMap<qint64, Empire *> &getEmpires() const;
		const QMap<qint64, Fleet *> &getFleets() const;
		const QMap<qint64, Ship *> &getShips() const;
		const QMap<qint64, ShipDesign *> &getShipDesigns() const;
	private:
		QString date;
		QMap<qint64, Empire *> empires;
		QMap<qint64, Fleet *> fleets;
		QMap<qint64, Ship *> ships;
		QMap<qint64, ShipDesign *> shipDesigns;

		friend class StateFactory;
	};

	class DLLEXPORT StateFactory : public QObject {
		Q_OBJECT
	public:
		State *createFromAst(Parsing::AstNode *tree, QObject *parent = nullptr);
		void cancel();
	signals:
		void progress(StateFactory *factory, int current, int max);
	private:
		bool shouldCancel = false;
	};
}

#endif //STELLARIS_STAT_VIEWER_GALAXY_STATE_H
