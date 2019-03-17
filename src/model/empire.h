/* model/empire.h: Class representation of a galactic empire.
 *
 * Copyright 2019 by Adrian "ArdiMaster" Welcker, distributed
 * under the MIT License - see file "LICENSE" for details.
 */

#pragma once

#ifndef STELLARIS_STAT_VIEWER_EMPIRE_H
#define STELLARIS_STAT_VIEWER_EMPIRE_H

#include <QtCore/QObject>

namespace Parsing { struct AstNode; }

namespace Galaxy {
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
		static Empire *createFromAst(Parsing::AstNode *tree, State *parent);
	private:
		qint64 index;
		QString name;
		double militaryPower;
		double economyPower;
		double techPower;
		quint32 ownedSystems;
	};
}

#endif //STELLARIS_STAT_VIEWER_EMPIRE_H
