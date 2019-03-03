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

#include "parser.h"

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
	Parsing::Parser parser(QFileInfo(which), Parsing::FileType::SaveFile);
	connect(&parser, &Parsing::Parser::progress, this, &MainWindow::parserProgressUpdate);
	gamestateLoadDone();
}

void MainWindow::parserProgressUpdate(unsigned long current, unsigned long max) {
	currentProgressDialog->setMaximum(max);
	currentProgressDialog->setValue(current);
}

void MainWindow::gamestateLoadBegin() {
	currentProgressDialog = new QProgressDialog(tr("(1/2) Loading gamestate file..."), tr("Cancel"), 0, 0, this);
	currentProgressDialog->setWindowModality(Qt::WindowModal);
	currentProgressDialog->setMinimumDuration(500);
}

void MainWindow::gamestateLoadSwitch() {
	currentProgressDialog->setLabelText(tr("(2/3) Building Galaxy..."));
}

void MainWindow::gamestateLoadDone() {
	currentProgressDialog->close();
	delete currentProgressDialog;
	currentProgressDialog = nullptr;
}