/* fleets_view.h: Display empire fleet statistics as a table.
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

#ifndef STELLARIS_STAT_VIEWER_MILITARY_VIEW_H
#define STELLARIS_STAT_VIEWER_MILITARY_VIEW_H

#include <QtWidgets/QWidget>

class QCheckBox;
class QVBoxLayout;
class FleetsViewInternal;

namespace Galaxy { class State; }

class FleetsView : public QWidget {
	Q_OBJECT;

public:
	FleetsView(QWidget *parent = nullptr);

public slots:
	void modelChanged(const Galaxy::State *newState);
	void onCheckboxChanged(int newState);

private:
	FleetsViewInternal *view;
	QCheckBox *includeStations;
	QVBoxLayout *layout;

	const Galaxy::State *currentState;
};

#endif