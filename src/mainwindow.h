/* mainwindow.h: Header file for stellaris_stat_viewer main window.
 *
 * Copyright 2019 by Adrian "ArdiMaster" Welcker, distributed
 * under the MIT License - see file "LICENSE" for details.
 */

#pragma once

#ifndef STELLARIS_STAT_VIEWER_MAINWINDOW_H
#define STELLARIS_STAT_VIEWER_MAINWINDOW_H

#include <QtWidgets/QMainWindow>

class QAction;
class QMenu;
class QMenuBar;
class QProgressDialog;
class QTabWidget;

enum class LoadStage;
class FleetsView;
class OverviewView;
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

private slots:
	void openFileSelected();

	void parserProgressUpdate(Parsing::Parser *parser, qint64 current, qint64 max);
	void galaxyProgressUpdate(Galaxy::StateFactory *factory, int current, int max);

private:
	void gamestateLoadBegin();
	void gamestateLoadSwitch();
	void gamestateLoadFinishing();
	void gamestateLoadDone();

	QAction *openFileAction;
	QMenuBar *theMenuBar;
	QMenu *fileMenu;
	QProgressDialog *currentProgressDialog;
	QTabWidget *tabs;

	Galaxy::State *state = nullptr;
	FleetsView *militaryView;
	OverviewView *powerRatingView;
};

#endif //STELLARIS_STAT_VIEWER_MAINWINDOW_H
