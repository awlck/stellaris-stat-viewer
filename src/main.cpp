/* main.cpp: Entry point for stellaris_stat_viewer.
 *
 * Copyright 2019 by Adrian "ArdiMaster" Welcker, distributed
 * under the MIT License - see file "LICENSE" for details.
 */

#include "mainwindow.h"

#include <QtWidgets/QApplication>

#ifdef Q_OS_WIN
int WinMain(int argc, char *argv[]) {
#else
int main(int argc, char *argv[]) {
#endif
	QApplication app(argc, argv);
	MainWindow window;
	window.show();
	return app.exec();
}