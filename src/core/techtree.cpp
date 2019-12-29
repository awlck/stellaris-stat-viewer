/* techtree.cpp: Writing dot files from technologies
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

#include "techtree.h"

#include <QtCore/QFile>

#include "galaxy_model.h"
#include "gametranslator.h"
#include "technology.h"

void writeTechTreePreamble(QFile *out) {
	out->write("strict digraph techs {\nnode[shape=box];ranksep=1.5;concentrate=true;rankdir=LR;\n");
}

void writeTechTreeClosing(QFile *out) {
	out->write("}\n");
}

void writeTechTreeNodes(QFile *out, const QMap<QString, Galaxy::Technology *> &techs, GameTranslator *translator) {
	for (auto it = techs.cbegin(); it != techs.cend(); it++) {
		Galaxy::Technology *tech = it.value();
		out->write(it.key().toUtf8());
		switch (tech->getArea()) {
		case Galaxy::TechArea::Physics:
			out->write("[color=blue");
			break;
		case Galaxy::TechArea::Society:
			out->write("[color=forestgreen");
			break;
		case Galaxy::TechArea::Engineering:
			out->write("[color=orange");
			break;
		}
		out->write(",label=\"");
		out->write(translator->getTranslationOf(it.key()).toUtf8());
		out->write("\"");
		if (tech->getIsStartingTech()) {
			out->write(",style=filled,fillcolor=green");
		} else if (tech->getIsWeightZero()) {
			out->write(",style=filled,fillcolor=gray");
		} else if (tech->getIsRare()) {
			out->write(",style=filled,fillcolor=darkorchid");
		}
		out->write("];\n");
	}
}

void writeTechTreeRelations(QFile *out, const QMap<QString, Galaxy::Technology *> &techs, bool enableWeights) {
	for (auto it = techs.cbegin(); it != techs.cend(); it++) {
		const QStringList &reqs = it.value()->getRequirements();
		const QString &tech = it.key();
		for (const auto &aReq: reqs) {
			out->write(aReq.toUtf8().data());
			out->write("->");
			out->write(tech.toUtf8().data());
			out->write(";\n");
		}
		if (enableWeights) {
			const auto &wms = it.value()->getWeightModifyingTechs();
			for (const auto &wm: wms) {
				out->write(wm.tech.toUtf8().data());
				out->write(" -> ");
				out->write(tech.toUtf8().data());
				out->write(" [style=dashed, color=");
				if (wm.modifier < 1.0) {
					out->write("red");
				} else {
					out->write("forestgreen");
				}
				out->write("];\n");
			}
		}
	}
}

