#include "settingsdialog.h"

#include <QtCore/QSettings>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent) {
	mainLayout = new QGridLayout;
	setLayout(mainLayout);

	buttonBox = new QDialogButtonBox;
	okButton = buttonBox->addButton(QDialogButtonBox::Ok);
	cancelButton = buttonBox->addButton(QDialogButtonBox::Cancel);
	connect(okButton, &QPushButton::pressed, this, &SettingsDialog::okClicked);
	connect(cancelButton, &QPushButton::pressed, this, &QDialog::rejected);

	gameFolderEdit = new QLineEdit;
	gameFolderSelect = new QPushButton(tr("Select"));
	gameFolderLabel = new QLabel(tr("Game Folder:"));
	gameFolderLabel->setBuddy(gameFolderEdit);
	connect(gameFolderSelect, &QPushButton::pressed, this, &SettingsDialog::selectClicked);

	mainLayout->addWidget(gameFolderLabel, 0, 0, 1, 1);
	mainLayout->addWidget(gameFolderEdit, 0, 1, 1, 3);
	mainLayout->addWidget(gameFolderSelect, 0, 4, 1, 1);
	mainLayout->addWidget(buttonBox, 1, 2, 1, 2);

	QSettings settings;
	gameFolderEdit->setText(settings.value("game/folder", QString()).toString());
}

void SettingsDialog::okClicked() {
	QSettings settings;
	settings.setValue("game/folder", gameFolderEdit->text());
	accept();
}

void SettingsDialog::selectClicked() {
	const QString &folder = QFileDialog::getExistingDirectory(this, tr("Select Stellaris game folder"));
	if (folder != "") gameFolderEdit->setText(folder);
}