/* core/technology.h: Extracting technology information from an AST.
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

#ifndef STELLARIS_STAT_VIEWER_TECHNOLOGY_H
#define STELLARIS_STAT_VIEWER_TECHNOLOGY_H

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QObject>

namespace Parsing { struct AstNode; }

namespace Galaxy {
	enum class TechArea {
		Physics,
		Society,
		Engineering
	};

	struct WeightModifier {
		QString tech;
		double modifier;
	};

	class Technology : public QObject {
		Q_OBJECT
	public:
		const QString &getName() const;
		const QStringList &getRequirements() const;
		bool getIsStartingTech() const;
		bool getIsRare() const;
		bool getIsRepeatable() const;
		bool getIsWeightZero() const;
		const QList<WeightModifier> &getWeightModifyingTechs() const;
		int getTier() const;

		TechArea getArea() const;
		static Technology *createFromAst(const Parsing::AstNode *node, QObject *parent);
	private:
		Technology(QObject *parent = nullptr);
		QString name;
		QStringList requirements;
		QList<WeightModifier> weightModifyingTechs;
		bool isStartingTech;
		bool isRare;
		bool isRepeatable;
		bool isWeightZero;
		int tier;
		TechArea area;
	};
}

#endif //STELLARIS_STAT_VIEWER_TECHNOLOGY_H