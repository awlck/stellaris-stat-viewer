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

#include <iostream>

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QSettings>
#include <QtCore/QTextStream>
#include <QtGui/QDesktopServices>
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
	tabs = new QTabWidget;
	setCentralWidget(tabs);

	theMenuBar = menuBar();
	fileMenu = theMenuBar->addMenu(tr("File"));
	openFileAction = fileMenu->addAction(tr("Open Save File"));
	connect(openFileAction, &QAction::triggered, this, &MainWindow::openFileSelected);

	toolsMenu = theMenuBar->addMenu(tr("Tools"));
	techTreeAction = toolsMenu->addAction(tr("Draw Tech Tree..."));
	settingsAction = toolsMenu->addAction(tr("Settings"));
	connect(techTreeAction, &QAction::triggered, this, &MainWindow::techTreeSelected);
	connect(settingsAction, &QAction::triggered, this, &MainWindow::settingsSelected);

	helpMenu = theMenuBar->addMenu(tr("Help"));
	helpMenu->setToolTipsVisible(true);
	checkForUpdatesAction = helpMenu->addAction(tr("Check for updates..."));
	checkForUpdatesAction->setToolTip(tr("Open your default browser to check GitHub for new releases."));
	connect(checkForUpdatesAction, &QAction::triggered, this, &MainWindow::checkForUpdatesSelected);
	aboutQtAction = helpMenu->addAction(tr("About Qt"));
	connect(aboutQtAction, &QAction::triggered, this, &MainWindow::aboutQtSelected);
	aboutSsvAction = helpMenu->addAction(tr("About Stellaris Stat Viewer"));
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
	QString which = QFileDialog::getOpenFileName(this, tr("Select save file"), QString(),
			tr("Stellaris Compressed Save Files (*.sav);;Stellaris Game State Files (gamestate)"));
	if (which == "") return;  // Cancel was clicked
	delete state;
	gamestateLoadBegin();
	Parsing::Parser *parser;
	bool isCompressedFile = which.endsWith(QStringLiteral(".sav"));
	unsigned char *dest;  // where the extracted gamestate file will go, if necessary
	QTextStream *stream;
	if (isCompressedFile) {
		unsigned long destsize;
		QFile f(which);
		f.open(QIODevice::ReadOnly);
		int result = extractGamestate(f, &dest, &destsize);
		f.close();
		if (result != 0) {
			QMessageBox::critical(this, tr("Compression Error"), tr("An error occurred while inflating the selected "
				"file:\n%1\nPlease make sure that you have selected a valid save file. If the selected file loads fine "
				"in the game, please report this issue "
				"to the developer.").arg(getInflateErrmsg(result)));

			if (result <= 2) free(dest);
			return;
		}
		stream = new QTextStream(QByteArray((char *) dest, destsize));
		parser = new Parsing::Parser(stream, which, Parsing::FileType::SaveFile, this);
	} else {
		stream = nullptr;
		dest = nullptr;
		parser = new Parsing::Parser(QFileInfo(which), Parsing::FileType::SaveFile, this);
	}
	connect(parser, &Parsing::Parser::progress, this, &MainWindow::parserProgressUpdate);
	Parsing::AstNode *result = parser->parse();
	if (isCompressedFile) {
		delete stream;
		free(dest);
	}
    if (!result) {
        gamestateLoadDone();
        Parsing::ParserError error(parser->getLatestParserError());
        if (error.etype != Parsing::PE_CANCELLED)
            QMessageBox::critical(this, tr("Parse Error"),
                                  tr("%1:%2:%3: %4 (error #%5)").arg(which).arg(error.erroredToken.line)
                                          .arg(error.erroredToken.firstChar).arg(
                                                  Parsing::getErrorDescription(error.etype)).arg(error.etype));
        delete parser;
        return;
    }

	gamestateLoadSwitch();
	Galaxy::StateFactory stateFactory;
	connect(&stateFactory, &Galaxy::StateFactory::progress, this, &MainWindow::galaxyProgressUpdate);
	state = stateFactory.createFromAst(result, this);
	if (!state) {
		gamestateLoadDone();
		QMessageBox::critical(this, tr("Galaxy Creation Error"), tr("An error occurred while trying to extract "
			"information from %1. Perhaps something isn't right with the input file.").arg(which));
		return;
	}

	gamestateLoadFinishing();
	delete parser;
	emit modelChanged(state);
	statusLabel->setText(state->getDate());
	statusBar()->showMessage(tr("Loaded ") + which, 5000);
	gamestateLoadDone();
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

void MainWindow::parserProgressUpdate(Parsing::Parser *parser, qint64 current, qint64 max) {
	if (currentProgressDialog->wasCanceled()) {
		parser->cancel();
		return;
	}
	currentProgressDialog->setMaximum(max);
	currentProgressDialog->setValue(current);
}

void MainWindow::galaxyProgressUpdate(Galaxy::StateFactory *factory, int current, int max) {
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

void MainWindow::gamestateLoadSwitch() {
	currentProgressDialog->setLabelText(tr("(2/3) Building Galaxy..."));
	currentProgressDialog->setValue(0);
	currentProgressDialog->setMaximum(0);
}

void MainWindow::gamestateLoadFinishing() {
	currentProgressDialog->setLabelText(tr("(3/3) Finishing work..."));
}

void MainWindow::gamestateLoadDone() {
	currentProgressDialog->close();
	delete currentProgressDialog;
	currentProgressDialog = nullptr;
}
