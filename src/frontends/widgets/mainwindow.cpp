/* mainwindow.cpp: Implementation of stellaris_stat_viewer main window.
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

#ifndef SSV_VERSION
#define SSV_VERSION "<unknown>"
#endif

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QMimeData>
#include <QtCore/QSettings>
#include <QtCore/QTextStream>
#include <QtGui/QDesktopServices>
#include <QtGui/QDragEnterEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QProgressDialog>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>

#include "../../core/gametranslator.h"
#include "../../core/galaxy_state.h"
#include "../../core/empire.h"
#include "../../core/parser.h"
#include "../../core/extract_gamestate.h"
#include "settingsdialog.h"
#include "techtreedialog.h"
#include "views/economy_view.h"
#include "views/fleets_view.h"
#include "views/overview_view.h"
#include "views/techs_view.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	setWindowTitle(tr("Stellaris Stat Viewer"));
	setAcceptDrops(true);
	tabs = new QTabWidget;
	setCentralWidget(tabs);

	QMenuBar *theMenuBar = menuBar();
	fileMenu = theMenuBar->addMenu(tr("File"));
	openFileAction = fileMenu->addAction(tr("Open Save File"));
	openFileAction->setShortcut(QKeySequence::Open);
	connect(openFileAction, &QAction::triggered, this, &MainWindow::openFileSelected);
#ifdef SSV_BUILD_JSON
	exportStatsAction = fileMenu->addAction(tr("Export Data"));
	exportStatsAction->setEnabled(false);
	exportStatsAction->setToolTip(tr("Export the currently loaded statistics in JSON format."));
	connect(exportStatsAction, &QAction::triggered, this, &MainWindow::exportStatsSelected);
#endif
	quitAction = fileMenu->addAction(tr("Exit"));
	quitAction->setMenuRole(QAction::QuitRole);
	quitAction->setShortcut(QKeySequence::Quit);
	connect(quitAction, &QAction::triggered, this, &MainWindow::quitSelected);

	toolsMenu = theMenuBar->addMenu(tr("Tools"));
	techTreeAction = toolsMenu->addAction(tr("Draw Tech Tree..."));
	settingsAction = toolsMenu->addAction(tr("Settings"));
	settingsAction->setMenuRole(QAction::PreferencesRole);
	settingsAction->setShortcut(QKeySequence::Preferences);
	connect(techTreeAction, &QAction::triggered, this, &MainWindow::techTreeSelected);
	connect(settingsAction, &QAction::triggered, this, &MainWindow::settingsSelected);

	helpMenu = theMenuBar->addMenu(tr("Help"));
	helpMenu->setToolTipsVisible(true);
	checkForUpdatesAction = helpMenu->addAction(tr("Check for updates..."));
	checkForUpdatesAction->setToolTip(tr("Open your default browser to check GitHub for new releases."));
	connect(checkForUpdatesAction, &QAction::triggered, this, &MainWindow::checkForUpdatesSelected);
	aboutQtAction = helpMenu->addAction(tr("About Qt"));
	aboutQtAction->setMenuRole(QAction::AboutQtRole);
	connect(aboutQtAction, &QAction::triggered, this, &MainWindow::aboutQtSelected);
	aboutSsvAction = helpMenu->addAction(tr("About Stellaris Stat Viewer"));
	aboutSsvAction->setMenuRole(QAction::AboutRole);
	connect(aboutSsvAction, &QAction::triggered, this, &MainWindow::aboutSsvSelected);

	QSettings settings;
	translator = new GameTranslator(settings.value("game/folder").toString(), settings.value("game/language", "english").toString());

	powerRatingView = new OverviewView(this);
	connect(this, &MainWindow::modelChanged, powerRatingView, &OverviewView::modelChanged);
	tabs->addTab(powerRatingView, tr("Overview"));

	militaryView = new FleetsView(this);
	connect(this, &MainWindow::modelChanged, militaryView, &FleetsView::modelChanged);
	tabs->addTab(militaryView, tr("Fleets"));

	economyView = new EconomyView(this);
	connect(this, &MainWindow::modelChanged, economyView, &EconomyView::modelChanged);
	tabs->addTab(economyView, tr("Economy"));

	techView = new TechView(translator, this);
	connect(this, &MainWindow::modelChanged, techView, &TechView::modelChanged);
	tabs->addTab(techView, tr("Technologies"));

	statusLabel = new QLabel(tr("No file loaded."));
	statusBar()->addPermanentWidget(statusLabel);
}

void MainWindow::aboutQtSelected() {
	QMessageBox::aboutQt(this);
}

void MainWindow::aboutSsvSelected() {
	QMessageBox::about(this, tr("About Stellars Stat Viewer"), tr("Stellaris Stat Viewer: EU4-inspired "
		"statistics and rankings for Stellaris.\n\nVersion: %1\n(c) 2019 Adrian "
		"\"ArdiMaster\" Welcker, Licensed under the Apache License version 2.0\n\nCheck out the source "
		"code and contribute at\nhttps://gitlab.com/ArdiMaster/stellaris-stat-viewer").arg(SSV_VERSION));
}

void MainWindow::checkForUpdatesSelected() {
	bool ok = QDesktopServices::openUrl(tr("https://github.com/ArdiMaster/stellaris-stat-viewer/releases"));
	if (!ok) {
		QMessageBox::warning(this, tr("Update Check Failed"), tr("An error occurred, and I was unable to open "
			"the releases page in any browser."));
	}
}

void MainWindow::openFileSelected() {
#if defined(Q_OS_WIN) || defined(Q_OS_MACOS)
	QString which = QFileDialog::getOpenFileName(this, tr("Select save file"),
			QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
			tr("Stellaris Compressed Save Files (*.sav);;Stellaris Game State Files (gamestate)"));
#else
	QString which = QFileDialog::getOpenFileName(this, tr("Select save file"), QDir::homePath(),
			tr("Stellaris Compressed Save Files (*.sav);;Stellaris Game State Files (gamestate)"));
#endif
	if (which == "") return;  // Cancel was clicked
	loadFromFile(QFileInfo(which));
}

#ifdef SSV_BUILD_JSON
#include "../json/dataextraction.h"

void MainWindow::exportStatsSelected() {
	QString saveTo = QFileDialog::getSaveFileName(this, tr("Select target location"), QString(),
	                                              tr("JSON files (*.json)"));
	if (saveTo == "") {
		return;
	}

	QJsonObject result = createJsonFromState(state);
	// Allow overwriting exisiting files -- a "file exists" query should already be provided
	// by the OS's "save file" dialog.
	if (QFile::exists(saveTo)) QFile::remove(saveTo);

	QFile out(saveTo);
	if (!out.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QMessageBox message(this);
		message.setText(tr("Unable to open export file"));
		message.setInformativeText(tr("I can think of several potential reasons for this, "
		                              "but perhaps the most likely one is that your system has run out of disk space."));
		message.setIcon(QMessageBox::Critical);
		message.setStandardButtons(QMessageBox::Ok);
		message.exec();
		return;
	}
	qint64 written = out.write(QJsonDocument(result).toJson());
	out.close();

	if (written == -1) {
		QMessageBox message(this);
		message.setText(tr("Unable to write export file"));
		message.setInformativeText(tr("I can think of several potential reasons for this, "
		                              "but perhaps the most likely one is that your system has run out of disk space."));
		message.setIcon(QMessageBox::Critical);
		message.setStandardButtons(QMessageBox::Ok);
		message.exec();
		return;
	}
	statusBar()->showMessage(tr("Wrote %1").arg(saveTo), 5000);
}
#endif

void MainWindow::quitSelected() const {
	QApplication::exit();
}

void MainWindow::settingsSelected() {
	SettingsDialog dialog(this);
	if (dialog.exec()) {
		QSettings settings;
		if (settings.value("game/folder").toString() != "") {
			int tc = translator->setFolderAndLanguage(settings.value("game/folder").toString(),
					settings.value("game/language").toString());
			statusBar()->showMessage(tr("Loaded %1 strings for language %2.").arg(tc).arg(translator->getLanguage()), 5000);
			// Cause techView to reload translations
			if (state) emit modelChanged(state);
		}
	}
}

void MainWindow::techTreeSelected() {
	QSettings settings;
	if (settings.value("game/folder", QString()).toString() == "") {
		QMessageBox messageBox;
		messageBox.setText(tr("Game folder not set"));
		messageBox.setInformativeText(tr("Would you like to open settings and set the game folder now?"));
		messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		messageBox.setDefaultButton(QMessageBox::Yes);
		messageBox.setIcon(QMessageBox::Warning);
		int selected = messageBox.exec();
		if (selected == QMessageBox::Yes) this->settingsSelected();
		return;
	}
	if (settings.value("tools/dot", QString()).toString() == "") {
		QMessageBox messageBox;
		messageBox.setText(tr("Dot utility not found"));
		messageBox.setInformativeText(tr("The tech tree functionality relies on the <code>dot</code> utility "
		                                 "from the GraphViz suite, but I was unable to locate it on your system. Would you like to open "
		                                 "settings and look for it manually right now?"));
		messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		messageBox.setDefaultButton(QMessageBox::Yes);
		messageBox.setIcon(QMessageBox::Warning);
		int selected = messageBox.exec();
		if (selected == QMessageBox::Yes) this->settingsSelected();
		return;
	}
	TechTreeDialog ttd(translator, this);
	ttd.exec();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
	if (event->mimeData()->hasUrls()) event->accept();
}

void MainWindow::dropEvent(QDropEvent *event) {
	const QMimeData *mimeData = event->mimeData();
	if (mimeData->hasUrls()) {
		auto file = QFileInfo(mimeData->urls()[0].toLocalFile());
		if (file.fileName() != "gamestate" && !file.fileName().endsWith(QStringLiteral(".sav"))) {
			QMessageBox box;
			box.setText(tr("Peculiar file. Load anyways?"));
			box.setInformativeText(tr("The file '%1' doesn't look like a Stellaris save file to me. "
							 "Load anyways?").arg(file.fileName()));
			box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			box.setDefaultButton(QMessageBox::No);
			box.setIcon(QMessageBox::Question);
			int selected = box.exec();
			if (selected == QMessageBox::No) return;
		}
		loadFromFile(file);
	} else event->ignore();
}

void MainWindow::loadFromFile(const QFileInfo& file) {
	delete state;
	gamestateLoadBegin();
	Parsing::MemBuf *buf;
	bool isCompressedFile = file.fileName().endsWith(QStringLiteral(".sav"));
	QFile f(file.absoluteFilePath());
	f.open(QIODevice::ReadOnly);
	if (isCompressedFile) {
		unsigned char *content;  // where the extracted gamestate file will go, if necessary
		unsigned long contentSize;
		int result = extractGamestate(f, &content, &contentSize);
		f.close();
		if (result != 0) {
			QMessageBox::critical(this, tr("Compression Error"), tr("An error occurred while inflating the selected "
			                                                        "file:\n%1\nPlease make sure that you have selected a valid save file. If the selected file loads fine "
			                                                        "in the game, please report this issue "
			                                                        "to the developer.").arg(getInflateErrmsg(result)));

			if (result <= 2) free(content);
			return;
		}
		buf = new Parsing::MemBuf((char *) content, contentSize);
	} else {
		buf = new Parsing::MemBuf(f);
	}

	Parsing::Parser parser(*buf, Parsing::FileType::SaveFile, file.absoluteFilePath(), this);
	connect(&parser, &Parsing::Parser::progress, this, &MainWindow::parserProgressUpdate);
	Parsing::AstNode *result = parser.parse();

	if (!result) {
		gamestateLoadDone();
		Parsing::ParserError error(parser.getLatestParserError());
		if (error.etype != Parsing::PE_CANCELLED)
			QMessageBox::critical(this, tr("Parse Error"),
			                      tr("%1:%2:%3: %4 (error #%5)").arg(file.absoluteFilePath()).arg(error.erroredToken.line)
					                      .arg(error.erroredToken.firstChar).arg(
							                      Parsing::getErrorDescription(error.etype)).arg(error.etype));
		delete buf;
		return;
	}

	gamestateLoadSwitch();
	Galaxy::StateFactory stateFactory;
	connect(&stateFactory, &Galaxy::StateFactory::progress, this, &MainWindow::galaxyProgressUpdate);
	state = stateFactory.createFromAst(result, this);
	if (!state) {
		gamestateLoadDone();
		QMessageBox::critical(this, tr("Galaxy Creation Error"), tr("An error occurred while trying to extract "
		                                                            "information from %1. Perhaps something isn't right with the input file.").arg(file.absoluteFilePath()));
		return;
	}

	gamestateLoadFinishing();
	delete buf;
	emit modelChanged(state);
	statusLabel->setText(state->getDate());
	statusBar()->showMessage(tr("Loaded %1").arg(file.absoluteFilePath()), 5000);
#ifdef SSV_BUILD_JSON
	exportStatsAction->setEnabled(true);
#endif
	gamestateLoadDone();
}

void MainWindow::parserProgressUpdate(Parsing::Parser *parser, qint64 current, qint64 max) const {
	if (currentProgressDialog->wasCanceled()) {
		parser->cancel();
		return;
	}
	currentProgressDialog->setMaximum(max);
	currentProgressDialog->setValue(current);
}

void MainWindow::galaxyProgressUpdate(Galaxy::StateFactory *factory, int current, int max) const {
	if (currentProgressDialog->wasCanceled()) {
		factory->cancel();
		return;
	}
	currentProgressDialog->setMaximum(max);
	currentProgressDialog->setValue(current);
}

void MainWindow::gamestateLoadBegin() {
	currentProgressDialog = new QProgressDialog(tr("(1/3) Loading gamestate file..."), tr("Cancel"), 0, 0, this);
	currentProgressDialog->setWindowModality(Qt::WindowModal);
	currentProgressDialog->setMinimumDuration(500);
}

void MainWindow::gamestateLoadSwitch() const {
	currentProgressDialog->setLabelText(tr("(2/3) Building Galaxy..."));
	currentProgressDialog->setValue(0);
	currentProgressDialog->setMaximum(0);
}

void MainWindow::gamestateLoadFinishing() const {
	currentProgressDialog->setLabelText(tr("(3/3) Finishing work..."));
}

void MainWindow::gamestateLoadDone() {
	currentProgressDialog->close();
	delete currentProgressDialog;
	currentProgressDialog = nullptr;
}
