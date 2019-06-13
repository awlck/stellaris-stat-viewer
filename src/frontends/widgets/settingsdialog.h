/* settingsdialog.h: Settings Dialog.
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

#include <QtCore/QStringList>
#include <QtWidgets/QDialog>
class QComboBox;
class QDialogButtonBox;
class QGridLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;

class SettingsDialog : public QDialog {
	Q_OBJECT
public:
	SettingsDialog(QWidget *parent = nullptr);
public slots:
	void gameDirChanged();
	void okClicked();
	void selectDotClicked();
	void selectGameClicked();
private:
	QComboBox *gameLanguage;
	QDialogButtonBox *buttonBox;
	QGridLayout *mainLayout;
	QLabel *dotProgramLabel, *gameFolderLabel, *gameLanguageLabel, *styleLabel;
	QLineEdit *dotProgramEdit, *gameFolderEdit;
	QPushButton *okButton, *cancelButton;
	QPushButton *dotProgramSelect, *gameFolderSelect;
	QRadioButton *appStyleNative, *appStyleStellaris;
};