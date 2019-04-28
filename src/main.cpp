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

#include <QtCore/QSettings>
#include <QtWidgets/QApplication>

void setupStellarisStyle(QApplication *app) {
	app->setStyleSheet(
		"* {"
			"background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #161f1c, stop: 1 #293a34);"
			"color: white;"
			"margin: 5px;"
		"}"

		"QHeaderView { margin: 0; }"
		"QHeaderView::section {"
			"border: 1px solid rgba(108, 255, 224, 255);"
			"background-color: rgba(108, 255, 224, 13);"
			"padding: 10 5 10 5;"
		"}"

		"QLabel, QMenuBar, QStatusBar, QHeaderView, QRadioButton { background: transparent; }"
		
		"QListView, QTableView {"
			"background: transparent;"
			"border: 2px solid rgba(108, 255, 224, 64);"
		"}"

		"QListView::item {"
			"margin-right: 20px;"
			"margin-top: 2px;"
			"margin-bottom: 2px;"
			"border: 1px solid rgba(108, 255, 224, 64);"
		"}"
		"QListView::item:selected {"
			"border-color: rgba(243, 143, 87, 255);"
			"background-color: rgba(255, 150, 80, 25);"
		"}"

		"QPushButton, QMenu::item, QGroupBox, QTabBar::tab { "
			"border: 2px solid rgba(108, 255, 224, 64);"
			"padding: 5 10 5 10;"
			"border-radius: 0px;"
			"background-color: rgba(108, 255, 224, 13);"
		"}"
		"QPushButton:pressed {"
			"border-color: rgba(108, 255, 224, 255);"
		"}"
		"QPushButton:hover:!pressed, QMenu::item:selected, QHeaderView::section:checked {"
			"border-color: rgba(243, 143, 87, 255);"
			"background-color: rgba(255, 150, 80, 25);"
		"}"

		"QProgressBar {"
			"text-align: center;"
			"font-family: 'Orbitron';"
			"font-size: 11pt;"
			"background-color: #101c1c;"
			"border: 1px solid #595154;"
		"}"
		"QProgressBar::Chunk:horizontal {"
			"background-color: qlineargradient(x1: 0, y1: 1, x2: 1, y2: 0, stop: 0 #3e614b, stop: 1 #679d79);"
		"}"
		
		"QScrollBar { background-color: #172e26; }"
		"QScrollBar:horizontal { height: 20; }"
		"QScrollBar:vertical { width: 20; }"
		"QScrollBar::add-line, QScrollBar::sub-line {"
			"border: none;"
			"background: none;"
			"width: 0px;"
			"height: 0px;"
		"}"
		"QScrollBar::handle { background-color: #4a9380; }"
		"QScrollBar::handle:horizontal { min-width: 100; }"
		"QScrollBar::handle:vertical { min-height: 100; }"

		"QTabWidget::pane { border: 2px solid rgba(108, 255, 224, 64); }"
		"QTabWidget::tab-bar { left: 5px; }"
		"QTabBar::tab {"
			"background-color: qlineargradient(x1: 1, y1: 0, x2: 0, y2: 1, stop: 0 #172820, stop: 1 #1e2f27);"
			"min-width: 8ex;"
			"padding: 7 15 7 15;"
		"}"
		"QTabBar::tab:selected {"
			"background-color: qlineargradient(x1: 1, y1: 0, x2: 0, y2: 1, stop: 0 #1c2d27, stop: 1 #273c33);"
		"}"
		"QTabBar::tab:!selected { margin-top: 2px; }"
		"QTabBar::tab:selected {"
			"margin-left: -4px;"
			"margin-right: -4px;"
		"}"
		"QTabBar::tab:first:selected { margin-left: 0; }"
		"QTabBar::tab:last:selected { margin-right: 0; }"
		"QTabBar::tab:only-one { margin: 0; }"
	);
}

#ifdef Q_OS_WIN
int __stdcall WinMain(int argc, char *argv[]) {
#else
int main(int argc, char *argv[]) {
#endif
	QApplication app(argc, argv);
	QCoreApplication::setApplicationName("Stellaris Stat Viewer");
	QCoreApplication::setOrganizationName("ArdiMaster");
	QCoreApplication::setOrganizationDomain("diepixelecke.de");

	QSettings settings;
	if (settings.value("app/useStellarisStyle", false).toBool()) {
		setupStellarisStyle(&app);
	}

	MainWindow window;
	window.show();
	return app.exec();
}