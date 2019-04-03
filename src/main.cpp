/* main.cpp: Entry point for stellaris_stat_viewer.
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

#include "mainwindow.h"

#include <QtWidgets/QApplication>

#ifdef Q_OS_WIN
int __stdcall WinMain(int argc, char *argv[]) {
#else
int main(int argc, char *argv[]) {
#endif
	QApplication app(argc, argv);
	QCoreApplication::setApplicationName("Stellaris Stat Viewer");
	QCoreApplication::setOrganizationName("ArdiMaster");
	QCoreApplication::setOrganizationDomain("diepixelecke.de");
	MainWindow window;
	window.show();
	return app.exec();
}