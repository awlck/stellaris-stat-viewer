#pragma once

#ifndef STELLARIS_STAT_VIEWER_TECHS_VIEW_H
#define STELLARIS_STAT_VIEWER_TECHS_VIEW_H

#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtWidgets/QWidget>
class QGridLayout;
class QLabel;
class QListWidget;

namespace Galaxy {
	class State;
}

class TechView : public QWidget {
	Q_OBJECT
public:
	TechView(QWidget *parent = nullptr);
public slots:
	void modelChanged(const Galaxy::State *newModel);

private slots:
	void selectedEmpireChanged(const QString &newEmpireName);
private:
	QGridLayout *layout;
	QLabel *empireListLabel;
	QLabel *techsListLabel;
	QListWidget *empireList;
	QListWidget *techsList;

	QMap<QString, QStringList> empireTechs;
};

#endif //STELLARIS_STAT_VIEWER_TECHS_H