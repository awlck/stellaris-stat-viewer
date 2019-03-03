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

#include "lexer.h"

enum class LoadStage {
	Lexing,
	Parsing,
	Galaxy
};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	tabs = new QTabWidget;
	setCentralWidget(tabs);
	theMenuBar = menuBar();
	fileMenu = theMenuBar->addMenu(tr("File"));
	openFileAction = fileMenu->addAction(tr("Open Save File"));
	connect(openFileAction, &QAction::triggered, this, &MainWindow::openFileSelected);
}

void MainWindow::openFileSelected() {
	QString which = QFileDialog::getOpenFileName(this, tr("Select gamestate file"), QString(), tr("Stellaris Game State Files (gamestate)"));
	if (which == "") return;  // Cancel was clicked
	gamestateLoadBegin();
	Parsing::Lexer lexer;
	lexer.lex(QFileInfo(which), Parsing::FileType::SaveFile);
	gamestateLoadDone();
}

void MainWindow::parserProgressUpdate(int current, int max) {
	gamestateLoadUpdate(LoadStage::Parsing, current, max);
}

void MainWindow::galaxyProgressUpdate(int current, int max) {
	gamestateLoadUpdate(LoadStage::Galaxy, current, max);
}

void MainWindow::gamestateLoadBegin() {
	currentProgressDialog = new QProgressDialog(tr("(1/3) Lexing gamestate file..."), tr("Cancel"), 0, 0, this);
	currentProgressDialog->setWindowModality(Qt::WindowModal);
	currentProgressDialog->setMinimumDuration(500);
}

void MainWindow::gamestateLoadUpdate(LoadStage stage, int current, int max) {
	static LoadStage previousLoadStage = LoadStage::Lexing;
	if (previousLoadStage != stage) {
		switch (stage) {
			case LoadStage::Lexing:
				currentProgressDialog->setLabelText(tr("(1/3) Lexing gamestate file..."));
				currentProgressDialog->setMaximum(max);
				break;
			case LoadStage::Parsing:
				currentProgressDialog->setLabelText(tr("(2/3) Parsing gamestate file..."));
				currentProgressDialog->setMaximum(max);
				break;
			case LoadStage::Galaxy:
				currentProgressDialog->setLabelText(tr("(3/3) Building galaxy..."));
				currentProgressDialog->setMaximum(max);
				break;
		}
		previousLoadStage = stage;
	}
	currentProgressDialog->setValue(current);
}

void MainWindow::gamestateLoadDone() {
	currentProgressDialog->close();
	delete currentProgressDialog;
	currentProgressDialog = nullptr;
}