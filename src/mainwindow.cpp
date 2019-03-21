/* mainwindow.cpp: Implementation of stellaris_stat_viewer main window.
 *
 * Copyright 2019 by Adrian "ArdiMaster" Welcker, distributed
 * under the MIT License - see file "LICENSE" for details.
 */

#include "mainwindow.h"

#include <QtCore/QDebug>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressDialog>
#include <QtWidgets/QTabWidget>

#include "economy_view.h"
#include "fleets_view.h"
#include "model/galaxy_state.h"
#include "parser/parser.h"
#include "overview_view.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	setWindowTitle(tr("Stellaris Stat Viewer"));
	tabs = new QTabWidget;
	setCentralWidget(tabs);

	theMenuBar = menuBar();
	fileMenu = theMenuBar->addMenu(tr("File"));
	openFileAction = fileMenu->addAction(tr("Open Save File"));
	connect(openFileAction, &QAction::triggered, this, &MainWindow::openFileSelected);

	powerRatingView = new OverviewView(this);
	connect(this, &MainWindow::modelChanged, powerRatingView, &OverviewView::modelChanged);
	tabs->addTab(powerRatingView, "Overview");

	militaryView = new FleetsView(this);
	connect(this, &MainWindow::modelChanged, militaryView, &FleetsView::modelChanged);
	tabs->addTab(militaryView, "Fleets");

	economyView = new EconomyView(this);
	connect(this, &MainWindow::modelChanged, economyView, &EconomyView::modelChanged);
	tabs->addTab(economyView, "Economy");
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
	Q_ASSERT_X(result != nullptr, "Parser::parse", "a parse error occurred");

	gamestateLoadSwitch();
	Galaxy::StateFactory stateFactory;
	connect(&stateFactory, &Galaxy::StateFactory::progress, this, &MainWindow::galaxyProgressUpdate);
	state = stateFactory.createFromAst(result, this);
	Q_ASSERT_X(state != nullptr, "Galaxy::StateFactory", "an internal error occurred while attempting "
		"to extract information from the parse tree.");

	gamestateLoadFinishing();
	delete result;
	emit modelChanged(state);
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