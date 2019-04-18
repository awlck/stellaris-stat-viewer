/* gametranslator.h: Class to collect translated game strings
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
	int setGameFolder(const QString &newFolder);
	int setLanguage(const QString &newLanguage);
	int setFolderAndLanguage(const QString &newFolder, const QString &newLanguage);
private:
	void readTranslationFilesForLanguage();
	void readSingleTranslationFile(QFile *f);

	QDir gameDirectory;
	QString language;
	QMap<QString, QString> translations;
};

#endif //STELLARIS_STAT_VIEWER_GAMETRANSLATOR_H