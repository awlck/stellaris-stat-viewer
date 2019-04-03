#pragma once

#include <QtWidgets/QDialog>
class QDialogButtonBox;
class QGridLayout;
class QLabel;
class QLineEdit;
class QPushButton;

class SettingsDialog : public QDialog {
	Q_OBJECT
public:
	SettingsDialog(QWidget *parent = nullptr);
public slots:
	void okClicked();
	void selectClicked();
private:
	QDialogButtonBox *buttonBox;
	QGridLayout *mainLayout;
	QLabel *gameFolderLabel;
	QLineEdit *gameFolderEdit;
	QPushButton *okButton, *cancelButton;
	QPushButton *gameFolderSelect;
};