/* fleets_view.cpp: Display empire technologies as a list.
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

#ifndef STELLARIS_STAT_VIEWER_TECHS_VIEW_H
#define STELLARIS_STAT_VIEWER_TECHS_VIEW_H

#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtWidgets/QWidget>
class QGridLayout;
class QLabel;
class QListWidget;

namespace Galaxy {
	class State;
}

class TechView : public QWidget {
	Q_OBJECT
public:
	TechView(QWidget *parent = nullptr);
public slots:
	void modelChanged(const Galaxy::State *newModel);

private slots:
	void selectedEmpireChanged(const QString &newEmpireName);
private:
	QGridLayout *layout;
	QLabel *empireListLabel;
	QLabel *techsListLabel;
	QListWidget *empireList;
	QListWidget *techsList;

	QMap<QString, QStringList> empireTechs;
};

#endif //STELLARIS_STAT_VIEWER_TECHS_H