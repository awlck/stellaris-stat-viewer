/* mainwindow.h: Header file for stellaris_stat_viewer main window.
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

#ifndef STELLARIS_STAT_VIEWER_MAINWINDOW_H
#define STELLARIS_STAT_VIEWER_MAINWINDOW_H

#include <QtCore/QFileInfo>
#include <QtWidgets/QMainWindow>

#include "frontends.h"

class QAction;
class QFileSystemWatcher;
class QLabel;
class QMenu;
class QMenuBar;
class QProgressDialog;
class QTabWidget;

enum class LoadStage;
class EconomyView;
class FleetsView;
class GameTranslator;
class OverviewView;
class ResearchView;
class StrategicResourcesView;
class TechView;
namespace Galaxy {
	class State;
	class StateFactory;
}
namespace Parsing { class Parser; }

class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	MainWindow(QWidget *parent = nullptr);

signals:
	void modelChanged(const Galaxy::State *newModel);

#ifndef EMSCRIPTEN
protected:
	void dragEnterEvent(QDragEnterEvent *event) override;
	void dropEvent(QDropEvent *event) override;
#endif  // !EMSCRIPTEN

private slots:
	void aboutQtSelected();
	void aboutSsvSelected();
	void checkForUpdatesSelected();
	void openFileSelected();
	void quitSelected() const;
	void settingsSelected();

#ifndef EMSCRIPTEN
	void techTreeSelected();
    void saveDirModified(const QString &dir);
#endif  // !EMSCRIPTEN

#ifdef SSV_BUILD_JSON
	void exportStatsSelected();
#endif  // SSV_BUILD_JSON

	void parserProgressUpdate(Parsing::Parser *parser, qint64 current, qint64 max) const;
	void galaxyProgressUpdate(Galaxy::StateFactory *factory, int current, int max) const;

private:
	void gamestateLoadBegin();
	void gamestateLoadSwitch() const;
	void gamestateLoadFinishing() const;
	void gamestateLoadDone();

#ifndef EMSCRIPTEN
	void loadFromFile(const QFileInfo& file);
	bool hackilyWaitOnFile(const QString &file);
#endif  // !EMSCRIPTEN
    void loadFromFile(const QString &name, const QByteArray &content);

	QAction *aboutQtAction;
	QAction *aboutSsvAction;
	QAction *checkForUpdatesAction;
	QAction *openFileAction;
	QAction *quitAction;
	QAction *settingsAction;
	QLabel *statusLabel;
	QMenu *fileMenu;
	QMenu *helpMenu;
	QMenu *toolsMenu;
	QProgressDialog *currentProgressDialog;
	QTabWidget *tabs;

#ifdef SSV_BUILD_JSON
	QAction *exportStatsAction;
#endif  // SSV_BUILD_JSON
	
	Galaxy::State *state = nullptr;
	GameTranslator *translator;
	EconomyView *economyView;
	FleetsView *militaryView;
	OverviewView *powerRatingView;
	ResearchView* researchView;
	StrategicResourcesView* strategicResourcesView;
	TechView *techView;

#ifndef EMSCRIPTEN
	QFileSystemWatcher *newSaveWatcher;
	QStringList knownSaveFiles;
	bool isOpeningFile = false;
	qint64 autoOpeningBegun;

    QAction *techTreeAction;
#endif  // !EMSCRIPTEN
};

#endif //STELLARIS_STAT_VIEWER_MAINWINDOW_H
