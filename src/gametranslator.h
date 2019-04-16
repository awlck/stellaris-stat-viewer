#pragma once

#ifndef STELLARIS_STAT_VIEWER_GAMETRANSLATOR_H
#define STELLARIS_STAT_VIEWER_GAMETRANSLATOR_H

#include <QtCore/QDir>
#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QString>
class QFile;

class GameTranslator : public QObject {
	Q_OBJECT
public:
	GameTranslator(const QString &gameFolder, const QString &language = QString(), QObject *parent = nullptr);
	const QString &getLanguage() const;
	QString getTranslationOf(const QString &key) const;
	void setLanguage(const QString &newLanguage);
private:
	void readTranslationFilesForLanguage();
	void readSingleTranslationFile(QFile *f);

	QDir gameDirectory;
	QString language;
	QMap<QString, QString> translations;
};

#endif //STELLARIS_STAT_VIEWER_GAMETRANSLATOR_H