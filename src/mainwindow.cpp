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

#include <QtCore/QDebug>
#include <QtGui/QDesktopServices>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QProgressDialog>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>

#include "views/economy_view.h"
#include "views/fleets_view.h"
#include "model/galaxy_state.h"
#include "parser/parser.h"
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

	helpMenu = theMenuBar->addMenu(tr("Help"));
	helpMenu->setToolTipsVisible(true);
	checkForUpdatesAction = helpMenu->addAction(tr("Check for updates..."));
	checkForUpdatesAction->setToolTip(tr("Open your default browser to check GitHub for new releases."));
	connect(checkForUpdatesAction, &QAction::triggered, this, &MainWindow::checkForUpdatesSelected);
	aboutQtAction = helpMenu->addAction(tr("About Qt"));
	connect(aboutQtAction, &QAction::triggered, this, &MainWindow::aboutQtSelected);
	aboutSsvAction = helpMenu->addAction(tr("About Stellaris Stat Viewer"));
	connect(aboutSsvAction, &QAction::triggered, this, &MainWindow::aboutSsvSelected);

	powerRatingView = new OverviewView(this);
	connect(this, &MainWindow::modelChanged, powerRatingView, &OverviewView::modelChanged);
	tabs->addTab(powerRatingView, "Overview");

	militaryView = new FleetsView(this);
	connect(this, &MainWindow::modelChanged, militaryView, &FleetsView::modelChanged);
	tabs->addTab(militaryView, "Fleets");

	economyView = new EconomyView(this);
	connect(this, &MainWindow::modelChanged, economyView, &EconomyView::modelChanged);
	tabs->addTab(economyView, "Economy");

	techView = new TechView(this);
	connect(this, &MainWindow::modelChanged, techView, &TechView::modelChanged);
	tabs->addTab(techView, "Technologies");

	statusLabel = new QLabel(tr("No file loaded."));
	statusBar()->addPermanentWidget(statusLabel);
}

void MainWindow::aboutQtSelected() {
	QMessageBox::aboutQt(this);
}

void MainWindow::aboutSsvSelected() {
	QMessageBox::about(this, tr("About Stellars Stat Viewer"), tr("Stellaris Stat Viewer: EU4-inspired "
		"statistics and rankings for Stellaris.\n\nVersion: " SSV_VERSION "\n(c) 2019 Adrian "
		"\"ArdiMaster\" Welcker, Licensed under the Apache License version 2.0\n\nCheck out the source "
		"code and contribute at\nhttps://github.com/ArdiMaster/stellaris-stat-viewer"));
}

void MainWindow::checkForUpdatesSelected() {
	bool ok = QDesktopServices::openUrl(tr("https://github.com/ArdiMaster/stellaris-stat-viewer/releases"));
	if (!ok) {
		QMessageBox::warning(this, tr("Update Check Failed"), tr("An error occurred, and I was unable to open "
			"the releases page in any browser."));
	}
}

void MainWindow::openFileSelected() {
	QString which = QFileDialog::getOpenFileName(this, tr("Select gamestate file"), QString(),
			tr("Stellaris Game State Files (gamestate)"));
	if (which == "") return;  // Cancel was clicked
	delete state;
	gamestateLoadBegin();
	Parsing::Parser parser(QFileInfo(which), Parsing::FileType::SaveFile, this);
	connect(&parser, &Parsing::Parser::progress, this, &MainWindow::parserProgressUpdate);
	Parsing::AstNode *result = parser.parse();
	if (!result) {
		gamestateLoadDone();
		Parsing::ParserError error = parser.getLatestParserError();
		QMessageBox::critical(this, tr("Parse Error"), tr("A parse error occurred:\n") + which +
			":" + QString::number(error.erroredToken.line) + ":" + QString::number(error.erroredToken.firstChar) +
			".");
		return;
	}

	gamestateLoadSwitch();
	Galaxy::StateFactory stateFactory;
	connect(&stateFactory, &Galaxy::StateFactory::progress, this, &MainWindow::galaxyProgressUpdate);
	state = stateFactory.createFromAst(result, this);
	if (!state) {
		gamestateLoadDone();
		QMessageBox::critical(this, tr("Galaxy Creation Error"), tr("An error occurred while trying to extract "
			"inforation from ") + which + tr(". Perhaps something isn't right with the input file."));
		return;
	}

	gamestateLoadFinishing();
	delete result;
	emit modelChanged(state);
	statusLabel->setText(state->getDate());
	statusBar()->showMessage(tr("Loaded ") + which, 5000);
	gamestateLoadDone();
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