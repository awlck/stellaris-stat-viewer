/* settingsdialog.h: Drawing a tech tree.
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

#include "techtreedialog.h"

#include <QtCore/QDirIterator>
#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include <QtCore/QTemporaryDir>
#include <QtSvg/QSvgWidget>
#include <QtWidgets/QApplication>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>

#include "model/galaxy_model.h"
#include "model/technology.h"
#include "parser/parser.h"

using Parsing::AstNode;
using Parsing::Parser;

TechTreeDialog::TechTreeDialog(QWidget *parent) : QDialog(parent) {
	mainLayout = new QGridLayout;
	setLayout(mainLayout);

	treeTypeBox = new QGroupBox(tr("Tree type"));
	treeTypeBoxLayout = new QHBoxLayout;
	treeTypeBox->setLayout(treeTypeBoxLayout);
	treeCompleteRadio = new QRadioButton(tr("Full"));
	treeReducedRadio = new QRadioButton(tr("Reduced"));
	treeCompleteRadio->setEnabled(false);
	treeCompleteRadio->setToolTip(tr("Not yet available."));
	treeReducedRadio->setToolTip(tr("Only render the prerequisite relation."));
	treeTypeBoxLayout->addWidget(treeReducedRadio);
	treeTypeBoxLayout->addWidget(treeCompleteRadio);
	mainLayout->addWidget(treeTypeBox, 0, 0);

	statusLabel = new QLabel(tr("Ready."));
	mainLayout->addWidget(statusLabel, 1, 0, Qt::AlignCenter);

	buttons = new QDialogButtonBox;
	goButton = buttons->addButton(tr("Go"), QDialogButtonBox::ActionRole);
	closeButton = buttons->addButton(QDialogButtonBox::Close);
	connect(goButton, &QPushButton::pressed, this, &TechTreeDialog::goClicked);
	connect(closeButton, &QPushButton::pressed, this, &QDialog::accept);
	mainLayout->addWidget(buttons, 2, 0);
}

void TechTreeDialog::goClicked() {
	QTemporaryDir dir;
	if (!dir.isValid()) {
		QMessageBox message(this);
		message.setText(tr("Unable to create temporary directory"));
		message.setInformativeText(tr("I can think of several potential reasons for this, "
			"but perhaps the most likely one is that your system has run out of disk space."));
		message.setIcon(QMessageBox::Critical);
		message.setStandardButtons(QMessageBox::Ok);
		message.exec();
		return;
	}
	QSettings settings;
	QDir techDir(settings.value("game/folder").toString() + "/common/technology");
	// TODO: Error handling for wrong selected folder
	if (!techDir.exists()) {
		QMessageBox message(this);
		message.setText(tr("Unable to find technologies directory"));
		message.setInformativeText(tr("I was looking for files defining technologies inside "
			"the folder %1, but an error occurred. You either do not have permission to access "
			"that directory, or (most likely) the game folder was incorrectly set in the "
			"settings.").arg(techDir.absolutePath()));
		message.setIcon(QMessageBox::Critical);
		message.setStandardButtons(QMessageBox::Ok);
		message.exec();
		return;
	}

	QDirIterator it(techDir.absolutePath(), QStringList("*.txt"), QDir::Files);
	if (!it.hasNext()) {
		statusLabel->setText(tr("No technologies found."));
		return;
	}
	Galaxy::Model model;
	while (it.hasNext()) {
		QFileInfo f(it.next());
		statusLabel->setText(tr("Reading %1.").arg(f.fileName()));
		QApplication::processEvents();
		Parser parser(f, Parsing::FileType::GameFile, this);
		AstNode *tree = parser.parse();
		if (tree) model.addTechnologies(tree);
		delete tree;
	}
}