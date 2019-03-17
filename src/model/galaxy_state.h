/* model/galaxy_state.h: Class representation of a savegame file.
 *
 * Copyright 2019 by Adrian "ArdiMaster" Welcker, distributed
 * under the MIT License - see file "LICENSE" for details.
 */

#pragma once

#ifndef STELLARIS_STAT_VIEWER_GALAXY_STATE_H
#define STELLARIS_STAT_VIEWER_GALAXY_STATE_H

#include <QtCore/QObject>
#include <QtCore/QMap>

#include "../parser/parser.h"

namespace Galaxy {
	class Empire;
	class Fleet;

	class State : public QObject {
		Q_OBJECT
		/* No destructor necessary: Empire, etc. all inherit from QObject, and their
		 * instances will be created with the State instance as their parent, so
		 * the QObject system will take care of deleting the children. */
	public:
		State(QObject *parent = nullptr);
		Empire *getEmpireWithId(qint64 id);
		const QMap<qint64, Empire *> &getEmpires() const;
	private:
		QMap<qint64, Empire *> empires;
		QMap<qint64, Fleet *> fleets;

		friend class StateFactory;
	};

	class StateFactory : public QObject {
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
