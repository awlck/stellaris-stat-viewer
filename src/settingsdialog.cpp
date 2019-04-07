/* settingsdialog.cpp: Setting up a settings dialog.
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


#include "settingsdialog.h"

#include <QtCore/QSettings>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent) {
	mainLayout = new QGridLayout;
	setLayout(mainLayout);

	buttonBox = new QDialogButtonBox;
	okButton = buttonBox->addButton(QDialogButtonBox::Ok);
	cancelButton = buttonBox->addButton(QDialogButtonBox::Cancel);
	connect(okButton, &QPushButton::pressed, this, &SettingsDialog::okClicked);
	connect(cancelButton, &QPushButton::pressed, this, &QDialog::reject);

	gameFolderEdit = new QLineEdit;
	gameFolderSelect = new QPushButton(tr("Select"));
	gameFolderLabel = new QLabel(tr("Game Folder:"));
	gameFolderLabel->setBuddy(gameFolderEdit);
	connect(gameFolderSelect, &QPushButton::pressed, this, &SettingsDialog::selectGameClicked);

	dotProgramEdit = new QLineEdit;
	dotProgramSelect = new QPushButton(tr("Select"));
	dotProgramLabel = new QLabel(tr("Dot:"));
	dotProgramLabel->setBuddy(dotProgramEdit);
	connect(dotProgramSelect, &QPushButton::pressed, this, &SettingsDialog::selectDotClicked);

	mainLayout->addWidget(gameFolderLabel, 0, 0, 1, 1);
	mainLayout->addWidget(gameFolderEdit, 0, 1, 1, 3);
	mainLayout->addWidget(gameFolderSelect, 0, 4, 1, 1);
	mainLayout->addWidget(dotProgramLabel, 1, 0, 1, 1);
	mainLayout->addWidget(dotProgramEdit, 1, 1, 1, 3);
	mainLayout->addWidget(dotProgramSelect, 1, 4, 1, 1);
	mainLayout->addWidget(buttonBox, 2, 2, 1, 2);

	QSettings settings;
	gameFolderEdit->setText(settings.value("game/folder", QString()).toString());
	dotProgramEdit->setText(settings.value("tools/dot", QString()).toString());
}

void SettingsDialog::okClicked() {
	QSettings settings;
	settings.setValue("game/folder", gameFolderEdit->text());
	settings.setValue("tools/dot", dotProgramEdit->text());
	accept();
}

void SettingsDialog::selectDotClicked() {
	#ifdef Q_OS_WIN
	const QString &folder = QFileDialog::getOpenFileName(this, tr("Select Dot executable"), QString(),
			tr("Execuables (*.exe)"));
	#else
	const QString &folder = QFileDialog::getOpenFileName(this, tr("Select Dot executable"));
	#endif
	if (folder != "") gameFolderEdit->setText(folder);
}

void SettingsDialog::selectGameClicked() {
	const QString &folder = QFileDialog::getExistingDirectory(this, tr("Select Stellaris game folder"));
	if (folder != "") gameFolderEdit->setText(folder);
}