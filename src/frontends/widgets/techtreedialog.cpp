/* techtreedialog.cpp: Drawing a tech tree.
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

#include "techtreedialog.h"

#include <QtCore/QDirIterator>
#include <QtCore/QFileInfo>
#include <QtCore/QProcess>
#include <QtCore/QSettings>
#include <QtCore/QTemporaryDir>
#include <QtGui/QDesktopServices>
#include <QtWidgets/QApplication>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>

#include "../../core/galaxy_model.h"
#include "../../core/technology.h"
#include "../../core/parser.h"
#include "gametranslator.h"

using Galaxy::Technology;
using Parsing::AstNode;
using Parsing::Parser;

static void writeTechTreeNodes(QFile *out, const QMap<QString, Technology *> &techs, GameTranslator *translator) {
	for (auto it = techs.cbegin(); it != techs.cend(); it++) {
		Technology *tech = it.value();
		out->write(it.key().toUtf8());
		switch (tech->getArea()) {
			case Galaxy::TechArea::Physics:
				out->write("[color=blue");
				break;
			case Galaxy::TechArea::Society:
				out->write("[color=forestgreen");
				break;
			case Galaxy::TechArea::Engineering:
				out->write("[color=orange");
				break;
		}
		out->write(",label=\"");
		out->write(translator->getTranslationOf(it.key()).toUtf8());
		out->write("\"");
		if (tech->getIsStartingTech()) {
			out->write(",style=filled,fillcolor=green");
		} else if (tech->getIsWeightZero()) {
			out->write(",style=filled,fillcolor=gray");
		} else if (tech->getIsRare()) {
			out->write(",style=filled,fillcolor=darkorchid");
		}
		out->write("];\n");
	}
}

static void writeTechTreeRelations(QFile *out, const QMap<QString, Technology *> &techs, bool enableWeights) {
	for (auto it = techs.cbegin(); it != techs.cend(); it++) {
		const QStringList &reqs = it.value()->getRequirements();
		const QString &tech = it.key();
		for (const auto &aReq: reqs) {
			out->write(aReq.toUtf8().data());
			out->write("->");
			out->write(tech.toUtf8().data());
			out->write(";\n");
		}
		if (enableWeights) {
			const auto &wms = it.value()->getWeightModifyingTechs();
			for (const auto &wm: wms) {
				out->write(wm.tech.toUtf8().data());
				out->write(" -> ");
				out->write(tech.toUtf8().data());
				out->write(" [style=dashed, color=");
				if (wm.modifier < 1.0) {
					out->write("red");
				} else {
					out->write("forestgreen");
				}
				out->write("];\n");
			}
		}
	}
}

TechTreeDialog::TechTreeDialog(GameTranslator *translator, QWidget *parent)
		: QDialog(parent), translator(translator) {
	mainLayout = new QGridLayout;
	setLayout(mainLayout);

	treeTypeBox = new QGroupBox(tr("Tree type"));
	treeTypeBoxLayout = new QHBoxLayout;
	treeTypeBox->setLayout(treeTypeBoxLayout);
	treeCompleteRadio = new QRadioButton(tr("Advanced"));
	treeReducedRadio = new QRadioButton(tr("Standard"));
	// treeCompleteRadio->setEnabled(false);
	treeCompleteRadio->setToolTip(tr("Attempt to draw how technologies in more detail. Experimental."));
	treeReducedRadio->setToolTip(tr("Only render the prerequisite relation."));
	treeReducedRadio->setChecked(true);
	treeTypeBoxLayout->addWidget(treeReducedRadio);
	treeTypeBoxLayout->addWidget(treeCompleteRadio);
	mainLayout->addWidget(treeTypeBox, 0, 0);

	statusLabel = new QLabel(tr("Ready."));
	statusLabel->setMinimumWidth(statusLabel->fontMetrics().width(tr("Reading 00_strategic_resources_tech.txt")));
	statusLabel->setAlignment(Qt::AlignCenter);
	mainLayout->addWidget(statusLabel, 1, 0, Qt::AlignCenter);

	buttons = new QDialogButtonBox;
	goButton = buttons->addButton(tr("Go"), QDialogButtonBox::ActionRole);
	closeButton = buttons->addButton(QDialogButtonBox::Close);
	connect(goButton, &QPushButton::pressed, this, &TechTreeDialog::goClicked);
	connect(closeButton, &QPushButton::pressed, this, &QDialog::accept);
	mainLayout->addWidget(buttons, 2, 0);
}

#define UPDATE_STATUS(text) do { statusLabel->setText((text)); update(); QApplication::processEvents(); } while (0)

void TechTreeDialog::goClicked() {
	QTemporaryDir dir;
	if (!dir.isValid()) {
		QMessageBox message(this);
		message.setText(tr("Unable to create temporary directory"));
		message.setInformativeText(tr("I can think of several potential reasons for this, "
			"but perhaps the most likely one is that your system has run out of disk space."));
		message.setIcon(QMessageBox::Critical);
		message.setStandardButtons(QMessageBox::Ok);
		message.exec();
		return;
	}
	QSettings settings;
	QDir techDir(settings.value("game/folder").toString() + "/common/technology");
	// TODO: Error handling for wrong selected folder
	if (!techDir.exists()) {
		QMessageBox message(this);
		message.setText(tr("Unable to find technologies directory"));
		message.setInformativeText(tr("I was looking for files defining technologies inside "
			"the folder %1, but an error occurred. You either do not have permission to access "
			"that directory, or (most likely) the game folder was incorrectly set in the "
			"settings.").arg(techDir.absolutePath()));
		message.setIcon(QMessageBox::Critical);
		message.setStandardButtons(QMessageBox::Ok);
		message.exec();
		return;
	}

	QDirIterator it(techDir.absolutePath(), QStringList("*.txt"), QDir::Files);
	if (!it.hasNext()) {
		statusLabel->setText(tr("No technologies found."));
		return;
	}
	Galaxy::Model model;
	while (it.hasNext()) {
		QFileInfo f(it.next());
		UPDATE_STATUS(tr("Reading %1").arg(f.fileName()));
		Parser parser(f, Parsing::FileType::GameFile, this);
		AstNode *tree = parser.parse();
		if (tree) model.addTechnologies(tree);
	}
	UPDATE_STATUS(tr("Writing nodes"));
	const QMap<QString, Technology *> &techs = model.getTechnologies();
	QFile outfile(dir.filePath("techs.dot"));
	if (!outfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QMessageBox message(this);
		message.setText(tr("Unable to write tech tree"));
		message.setInformativeText(tr("I can think of several potential reasons for this, "
			"but perhaps the most likely one is that your system has run out of disk space."));
		message.setIcon(QMessageBox::Critical);
		message.setStandardButtons(QMessageBox::Ok);
		message.exec();
		return;
	}
	outfile.write("strict digraph techs {\nnode[shape=box];ranksep=1.5;concentrate=true;rankdir=LR;\n");
	writeTechTreeNodes(&outfile, techs, translator);
	UPDATE_STATUS(tr("Writing connections"));
	writeTechTreeRelations(&outfile, techs, treeCompleteRadio->isChecked());
	outfile.write("}\n");
	outfile.close();
	UPDATE_STATUS(tr("Running dot"));
	QProcess dot(this);
	dot.setWorkingDirectory(dir.path());
	QStringList args;
	args << "techs.dot" << "-Tpdf" << "-otechs.pdf";
	dot.start(settings.value("tools/dot").toString(), args);
	dot.waitForFinished();
	if (dot.exitStatus() == QProcess::CrashExit || dot.exitCode() != 0) {
		QMessageBox message(this);
		message.setText(tr("Error drawing graph"));
		message.setInformativeText(tr("Some error occurred, and the <code>dot</code> utility did not finish successfully."));
		message.setIcon(QMessageBox::Critical);
		message.setStandardButtons(QMessageBox::Ok);
		message.exec();
		return;
	} else {
		QString saveTo = QFileDialog::getSaveFileName(this, tr("Select target location"), QString(),
				tr("Portable Document Format (*.pdf)"));
		if (saveTo == "") {
			UPDATE_STATUS(tr("Cancelled"));
			return;
		}
		// Allow overwriting exisiting files -- a "file exists" query should be provided
		// by the OS's "save file" dialog.
		if (QFile::exists(saveTo)) QFile::remove(saveTo);
		QFile::copy(dir.filePath("techs.pdf"), saveTo);
		QDesktopServices::openUrl(QUrl::fromLocalFile(saveTo));
	}
	UPDATE_STATUS(tr("Ready."));
}