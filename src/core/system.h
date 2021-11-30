/* core/system.h: Class representation of a star system.
 *
 * Copyright 2021 Adrian "ArdiMaster" Welcker
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

#ifdef SSV_WITH_GALAXY_MAP
#ifndef STELLARIS_STAT_VIEWER_SYSTEM_H
#define STELLARIS_STAT_VIEWER_SYSTEM_H

#include <stdint.h>
#include <QtCore/QObject>
#include <QtGui/QVector2D>

namespace Parsing { struct AstNode; }

namespace Galaxy {
	class State;
	class Starbase;

	class System : public QObject {
		Q_OBJECT
	public:
		System(State* parent);
		inline int64_t getIndex() const {
			return index;
		};
		const QString& getName() const;
		const QVector2D& getPosition() const;
		inline Starbase *getStarbase() const {
			return starbase;
		}
		static System* createFromAst(const Parsing::AstNode* tree, State* parent);

	private:
		int64_t index;
		QString name;
		QVector2D position;
		Starbase *starbase;
	};
}

#endif  // !STELLARIS_STAT_VIEWER_SYSTEM_H
#endif  // SSV_WITH_GALAXY_MAP