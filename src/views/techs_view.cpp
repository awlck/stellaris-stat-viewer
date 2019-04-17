/* fleets_view.cpp: Put empire technologies into a list.
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

#include "techs_view.h"

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QLabel>

#include "../gametranslator.h"
#include "../model/galaxy_state.h"
#include "../model/empire.h"

TechView::TechView(GameTranslator *t, QWidget* parent) : QWidget(parent), translator(t) {
	layout = new QGridLayout;
	empireList = new QListWidget;
	connect(empireList, &QListWidget::currentTextChanged, this, &TechView::selectedEmpireChanged);
	empireListLabel = new QLabel(tr("Empires"));
	empireListLabel->setBuddy(empireList);
	techsList = new QListWidget;
	techsListLabel = new QLabel(tr("Researched Technologies"));
	layout->addWidget(empireListLabel, 0, 0);
	layout->addWidget(empireList, 1, 0);
	layout->addWidget(techsListLabel, 0, 1);
	layout->addWidget(techsList, 1, 1);
	setLayout(layout);
}

void TechView::modelChanged(const Galaxy::State *newModel) {
	empireList->clear();
	techsList->clear();
	empireList->setSortingEnabled(false);
	const QMap<qint64, Galaxy::Empire *> &empires = newModel->getEmpires();
	for (auto it = empires.cbegin(); it != empires.cend(); it++) {
		Galaxy::Empire *empire = it.value();
		QStringList translatedTechs;
		for (const auto &tech: empire->getTechnologies()) {
			translatedTechs << translator->getTranslationOf(tech);
		}
		empireTechs.insert(empire->getName(), translatedTechs);
		empireList->addItem(empire->getName());
	}
	empireList->setSortingEnabled(true);
}

void TechView::selectedEmpireChanged(const QString &newEmpire) {
	techsList->clear();
	techsList->addItems(empireTechs.value(newEmpire));
}