//
// Created by Adrian Welcker on 2019-03-16.
//

#ifndef STELLARIS_STAT_VIEWER_EMPIRE_H
#define STELLARIS_STAT_VIEWER_EMPIRE_H

#include <QtCore/QObject>
#include "../parser/parser.h"

namespace Galaxy {
	class Empire : public QObject {
		Q_OBJECT
	public:
		Empire(QObject *parent = nullptr);
		qint64 getIndex();
		const QString &getName();
		double getMilitaryPower();
		double getEconomyPower();
		double getTechPower();
		static Empire *createFromAst(Parsing::AstNode *tree, QObject *parent = nullptr);
	private:
		qint64 index;
		QString name;
		double militaryPower;
		double economyPower;
		double techPower;
	};
}

#endif //STELLARIS_STAT_VIEWER_EMPIRE_H
