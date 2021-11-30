#pragma once

#ifdef SSV_WITH_GALAXY_MAP
#ifndef STELLARIS_STAT_VIEWER_STARBASE_H
#define STELLARIS_STAT_VIEWER_STARBASE_H

#include <QtCore/QObject>

namespace Parsing { struct AstNode; }

namespace Galaxy {
	class State;
	class Empire;
	class Ship;

	class Starbase : public QObject {
		Q_OBJECT
	public:
		Starbase(State *parent);
		inline int64_t getIndex() const {
			return index;
		};
		inline const Empire *getOwner() const {
			return owner;
		};
		inline Ship *getStation() const {
			return station;
		}
		static Starbase *createFromAst(const Parsing::AstNode *tree, State *parent);

	private:
		int64_t index;
		Empire *owner;
		Ship *station;
	};
}

#endif  // !STELLARIS_STAT_VIEWER_STARBASE_H
#endif  // SSV_WITH_GALAXY_MAP