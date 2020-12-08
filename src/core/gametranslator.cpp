/* gametranslator.h: Reading game localization files.
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

#include "gametranslator.h"

#include <QtCore/QDirIterator>
#include <QtCore/QRegularExpression>
#include <QtCore/QTextStream>

GameTranslator::GameTranslator(const QString &gameFolder, const QString &language, QObject *parent)
		: QObject(parent), gameDirectory(QDir(gameFolder)), language(language) {
	if (language != "" && gameFolder != "") readTranslationFilesForLanguage();
}

const QString &GameTranslator::getLanguage() const {
	return language;
}

QString GameTranslator::getTranslationOf(const QString &key) const {
	return translations.value(key, key);
}

int GameTranslator::setLanguage(const QString &newLanguage) {
	translations.clear();
	language = newLanguage;
	if (newLanguage != "") readTranslationFilesForLanguage();
	return translations.count();
}

int GameTranslator::setGameFolder(const QString &newFolder) {
	translations.clear();
	gameDirectory = newFolder;
	if (newFolder != "") readTranslationFilesForLanguage();
	return translations.count();
}

int GameTranslator::setFolderAndLanguage(const QString &newFolder, const QString &newLanguage) {
	translations.clear();
	gameDirectory = newFolder;
	language = newLanguage;
	if (newFolder != "" && newLanguage != "") readTranslationFilesForLanguage();
	return translations.count();
}

void GameTranslator::fixupStrings() {
	QRegularExpression matchexpr("\\$([a-zA-Z0-9._]+)\\$");
	for (auto it = translations.begin(); it != translations.end(); it++) {
		QString backup(it.value());
		QRegularExpressionMatchIterator matchit = matchexpr.globalMatch(backup);
		while (matchit.hasNext()) {
			QRegularExpressionMatch match = matchit.next();
			if (translations.contains(match.captured(1))) {
				it.value().replace(match.captured(0), translations[match.captured(1)]);
			}
		}
	}
}

void GameTranslator::readTranslationFilesForLanguage() {
	QDir localizationDir(gameDirectory.absoluteFilePath("localisation/") + language);

	QDirIterator it(localizationDir);
	while (it.hasNext()) {
		QFile currentFile(it.next());
		readSingleTranslationFile(&currentFile);
	}
	fixupStrings();
}

void GameTranslator::readSingleTranslationFile(QFile *f) {
	if (!(f->exists())) return;
	if (!(f->open(QIODevice::ReadOnly | QIODevice::Text))) return;

	/* @p Reading YML localization files with Regular Expressions.
	 * Fortunately, the game's localization files use only the simplest form of YML, so
	 * this expression is almost surprisingly simple. The files are laid out like so:
	 * l_LANGUAGE:
	 *  KEY:0 "Value"
	 * Note that this implementation discards any line that ends in a comment, but I am
	 * not aware of comments appearing on the end of useful lines: they seem to always
	 * reside in lines of their own.
	 */
	QRegularExpression matchexpr("^[ \\t]+([a-zA-Z0-9._]+):[01]?[ \\t]+\"(.+)\"\\r?$", QRegularExpression::MultilineOption);
	QRegularExpressionMatchIterator it = matchexpr.globalMatch(f->readAll());
	while (it.hasNext()) {
		QRegularExpressionMatch line = it.next();
		translations[line.captured(1)] = line.captured(2);
	}
	f->close();
}