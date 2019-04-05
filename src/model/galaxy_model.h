/* model/galaxy_model.h: Class representation of the basic world model.
 * This potentially encompasses everything that gets set during the initial
 * loading stage on game load.
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

#ifndef STELLARIS_STAT_VIEWER_GALAXY_MODEL_H
#define STELLARIS_STAT_VIEWER_GALAXY_MODEL_H

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QString>

namespace Parsing { struct AstNode; }

namespace Galaxy {
	class Technology;

	class Model : public QObject {
		Q_OBJECT
	public:
		Model(QObject *parent = nullptr);
		const QMap<QString, Technology *> &getTechnologies() const;
		const Technology *getTechnology(const QString &name) const;
		void addTechnologies(const Parsing::AstNode *tree);
	private:
		QMap<QString, Technology *> techs;
	};
}

#endif //STELLARIS_STAT_VIEWER_GALAXY_MODEL_H