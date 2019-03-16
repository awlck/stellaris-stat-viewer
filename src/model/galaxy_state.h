//
// Created by Adrian Welcker on 2019-03-15.
//

#ifndef STELLARIS_STAT_VIEWER_GALAXY_STATE_H
#define STELLARIS_STAT_VIEWER_GALAXY_STATE_H

#include <QtCore/QObject>
#include <QtCore/QMap>

#include "../parser/parser.h"

namespace Galaxy {
	class Empire;

	class State : public QObject {
		Q_OBJECT
	public:
		State(QObject *parent = nullptr);
		static State *createFromAst(Parsing::AstNode *tree, QObject *parent = nullptr);
	private:
		QMap<qint64, Empire *> empires;
	};
}

#endif //STELLARIS_STAT_VIEWER_GALAXY_STATE_H
