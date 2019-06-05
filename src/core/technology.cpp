/* core/technology.cpp: Extracting technology information from an AST.
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

#include "technology.h"

#include "model_private_macros.h"
#include "parser.h"

using Parsing::AstNode;

namespace Galaxy {
	Technology::Technology(QObject *parent) : QObject(parent) {}

	const QString &Technology::getName() const {
		return name;
	}

	const QStringList &Technology::getRequirements() const {
		return requirements;
	}

	bool Technology::getIsStartingTech() const {
		return isStartingTech;
	}

	bool Technology::getIsRare() const {
		return isRare;
	}

	bool Technology::getIsRepeatable() const {
		return isRepeatable;
	}

	int Technology::getTier() const {
		return tier;
	}

	TechArea Technology::getArea() const {
		return area;
	}

	Technology *Technology::createFromAst(const Parsing::AstNode *node, QObject *parent) {
		Technology *state = new Technology(parent);
		state->name = QString(node->myName);
		
		AstNode *areaNode = node->findChildWithName("area");
		CHECK_PTR(areaNode);
		if (qstrcmp(areaNode->val.Str, "engineering") == 0) state->area = TechArea::Engineering;
		else if (qstrcmp(areaNode->val.Str, "society") == 0) state->area = TechArea::Society;
		else if (qstrcmp(areaNode->val.Str, "physics") == 0) state->area = TechArea::Physics;
		else { delete state; return nullptr; }
		
		AstNode *startTechNode = node->findChildWithName("start_tech");
		state->isStartingTech = startTechNode ? startTechNode->val.Bool : false;
		
		AstNode *rareNode = node->findChildWithName("is_rare");
		state->isRare = rareNode ? rareNode->val.Bool : false;

		state->isRepeatable = state->name.startsWith("tech_repeatable_");
		if (!state->isRepeatable) {
			AstNode *tierNode = node->findChildWithName("tier");
			CHECK_PTR(tierNode);
			state->tier = tierNode->val.Int;
		} else state->tier = -1;

		AstNode *prerequisitesNode = node->findChildWithName("prerequisites");
		if (prerequisitesNode && prerequisitesNode->type == Parsing::NT_STRINGLIST) {
			ITERATE_CHILDREN(prerequisitesNode, aPrerequisite) {
				state->requirements.append(aPrerequisite->val.Str);
			}
		}
		
		return state;
	}
}