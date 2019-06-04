//
// Created by Adrian Welcker on 2019-06-04.
//

#include <QtWidgets/QApplication>
#include "mainwindow.h"

extern "C"
#ifdef _MSC_VER
__declspec(dllexport)
#endif
int frontend_begin(int argc, char **argv) {
	QApplication app(argc, argv);
	MainWindow window;
	window.show();
	return app.exec();
}