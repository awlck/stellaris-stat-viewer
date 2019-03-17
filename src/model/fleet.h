/* model/fleet.h: Class representation of an interstellar fleet.
 *
 * Copyright 2019 by Adrian "ArdiMaster" Welcker, distributed
 * under the MIT License - see file "LICENSE" for details.
 */

#pragma once

#ifndef STELLARIS_STAT_VIEWER_FLEET_H
#define STELLARIS_STAT_VIEWER_FLEET_H

#include <QtCore/QObject>

namespace Parsing { struct AstNode; }

namespace Galaxy {
	class Empire;
	class State;

	class Fleet : public QObject {
		Q_OBJECT
	public:
		Fleet(State *parent);
		qint64 getIndex() const;
		const QString &getName() const;
		Empire *getOwner() const;
		bool getIsStation() const;
		double getMilitaryPower() const;
		static Fleet *createFromAst(Parsing::AstNode *tree, State *parent);
	private:
		qint64 index;
		QString name;
		Empire *owner;
		bool isStation;
		double militaryPower;
	};
}

#endif //STELLARIS_STAT_VIEWER_FLEET_H