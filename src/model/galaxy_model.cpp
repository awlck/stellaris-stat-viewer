/* model/galaxy_state.cpp: Building the underlying model of the galaxy.
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

#include "galaxy_model.h"

#include "model_private_macros.h"
#include "technology.h"
#include "../parser/parser.h"

using Parsing::AstNode;

namespace Galaxy {
	Model::Model(QObject *parent) : QObject(parent) {}

	const QMap<QString, Technology *> &Model::getTechnologies() const {
		return techs;
	}

	const Technology *Model::getTechnology(const QString &name) const {
		return techs.value(name, nullptr);
	}

	void Model::addTechnologies(const AstNode *tree) {
		if (!tree || !(tree->type == Parsing::NT_COMPOUND)) return;
		ITERATE_CHILDREN(tree, aTech) {
			Technology *newTech = Technology::createFromAst(aTech, this);
			if (newTech) techs[newTech->getName()] = newTech;
		}
	}
}