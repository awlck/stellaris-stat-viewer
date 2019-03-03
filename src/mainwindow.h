/* mainwindow.h: Header file for stellaris_stat_viewer main window.
 *
 * Copyright 2019 by Adrian "ArdiMaster" Welcker, distributed
 * under the MIT License - see file "LICENSE" for details.
 */

#ifndef STELLARIS_STAT_VIEWER_MAINWINDOW_H
#define STELLARIS_STAT_VIEWER_MAINWINDOW_H

#include <QtWidgets/QMainWindow>
enum class LoadStage;
class QAction;
class QMenu;
class QMenuBar;
class QProgressDialog;
class QTabWidget;

class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	MainWindow(QWidget *parent = nullptr);

private slots:
	void openFileSelected();

	void parserProgressUpdate(int current, int max);
	void galaxyProgressUpdate(int current, int max);

private:
	void gamestateLoadBegin();
	void gamestateLoadUpdate(LoadStage stage, int current, int max);
	void gamestateLoadDone();

	QAction *openFileAction;
	QMenuBar *theMenuBar;
	QMenu *fileMenu;
	QProgressDialog *currentProgressDialog;
	QTabWidget *tabs;
};

#endif //STELLARIS_STAT_VIEWER_MAINWINDOW_H
