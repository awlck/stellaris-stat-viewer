#include "gametranslator.h"

#include <QtCore/qdebug.h>
#include <QtCore/QDirIterator>
#include <QtCore/QRegularExpression>

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

void GameTranslator::setLanguage(const QString &newLanguage) {
	translations.clear();
	language = newLanguage;
	if (newLanguage != "") readTranslationFilesForLanguage();
}

void GameTranslator::readTranslationFilesForLanguage() {
	QDir localizationDir(gameDirectory.absoluteFilePath("localisation/") + language);

	QDirIterator it(localizationDir);
	while (it.hasNext()) {
		QFile currentFile(it.next());
		readSingleTranslationFile(&currentFile);
	}
}

void GameTranslator::readSingleTranslationFile(QFile *f) {
	if (!(f->exists())) return;
	if (!(f->open(QIODevice::ReadOnly))) return;
	QTextStream stream(f);
	stream.setCodec("utf-8");

	/* @p Reading YML localization files with Regular Expressions.
	 * Fortunately, the game's localization files use only the simplest form of YML, so
	 * this expression is almost surprisingly simple. The files are laid out like so:
	 * l_LANGUAGE:
	 *  KEY:0 "Value"
	 * Note that this implementation discards any line that ends in a comment, but I am
	 * not aware of comments appearing on the end of useful lines: they seem to always
	 * reside in lines of their own.
	 */
	QRegularExpression matchexpr("^ ([a-zA-Z0-9._]+):[01]? \"(.+)\"$", QRegularExpression::MultilineOption);
	QRegularExpressionMatchIterator it = matchexpr.globalMatch(stream.readAll());
	while (it.hasNext()) {
		QRegularExpressionMatch line = it.next();
		translations[line.captured(1)] = line.captured(2);
	}
	f->close();
}