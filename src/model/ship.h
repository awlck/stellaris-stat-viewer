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