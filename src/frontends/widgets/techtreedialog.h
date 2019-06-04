/* techtreedialog.h: Tech Tree Drawing Dialog.
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

#ifndef STELLARIS_STAT_VIEWER_TECHTREEDIALOG_H
#define STELLARIS_STAT_VIEWER_TECHTREEDIALOG_H

#include <QtWidgets/QDialog>
class QDialogButtonBox;
class QGridLayout;
class QGroupBox;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;

class GameTranslator;

class TechTreeDialog : public QDialog {
	Q_OBJECT
public:
	TechTreeDialog(GameTranslator *translator, QWidget *parent = nullptr);

private slots:
	void goClicked();
private:
	QDialogButtonBox *buttons;
	QGridLayout *mainLayout;
	QGroupBox *treeTypeBox;
	QHBoxLayout *treeTypeBoxLayout;
	QLabel *statusLabel;
	QPushButton *closeButton, *goButton;
	QRadioButton *treeCompleteRadio, *treeReducedRadio;

	GameTranslator *translator;
};

#endif //STELLARIS_STAT_VIEWER_TECHTREEDIALOG_H