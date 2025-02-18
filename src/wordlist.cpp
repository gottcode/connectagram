/*
	SPDX-FileCopyrightText: 2013-2025 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "wordlist.h"

#include <QFile>
#include <QLocale>
#include <QRegularExpression>
#include <QSettings>
#include <QTextStream>

#include <algorithm>

//-----------------------------------------------------------------------------

WordList::WordList(QObject* parent)
	: QObject(parent)
	, m_length(0)
{
}

//-----------------------------------------------------------------------------

QStringList WordList::filter(const QString& known_letters) const
{
	const QRegularExpression filter("^" + known_letters + "$");
	QStringList filtered;
	for (const QString& word : m_words) {
		if (filter.match(word).hasMatch()) {
			QString sorted = word;
			std::sort(sorted.begin(), sorted.end());
			if (m_anagram_filters.contains(sorted)) {
				continue;
			}
			filtered += word;
		}
	}
	return filtered;
}

//-----------------------------------------------------------------------------

void WordList::addAnagramFilter(const QString& word)
{
	QString sorted_letters = word;
	std::sort(sorted_letters.begin(), sorted_letters.end());
	m_anagram_filters.append(sorted_letters);
}

//-----------------------------------------------------------------------------

void WordList::resetAnagramFilters()
{
	m_anagram_filters.clear();
}

//-----------------------------------------------------------------------------

void WordList::setLanguage(const QString& langcode)
{
	if (m_langcode == langcode) {
		return;
	}
	m_langcode = langcode;

	static QHash<QString, QSharedPointer<WordListData>> languages;
	if (!languages.contains(langcode)) {
		languages.insert(langcode, QSharedPointer<WordListData>(new WordListData(langcode)));
	}
	m_data = languages[langcode];
	resetWords();

	Q_EMIT languageChanged(m_langcode);
}

//-----------------------------------------------------------------------------

void WordList::setLength(int length)
{
	m_length = length;
	resetWords();
}

//-----------------------------------------------------------------------------

QString WordList::defaultLanguage()
{
	QString language = QLocale().name();
	if (!QFile::exists("connectagram:" + language)) {
		language = language.section('_', 0, 0);
		if (!QFile::exists("connectagram:" + language)) {
			language = "en";
		}
	}
	return language;
}

//-----------------------------------------------------------------------------

QString WordList::languageName(const QString& language)
{
	QSettings settings(QString("connectagram:%1/language.ini").arg(language), QSettings::IniFormat);
	QString name = settings.value("Language/Name").toString();
	if (name.isEmpty()) {
		name = QLocale(language).nativeLanguageName();
	}
	return name;
}

//-----------------------------------------------------------------------------

void WordList::resetWords()
{
	m_words = m_data->words(m_length);
}

//-----------------------------------------------------------------------------

WordList::WordListData::WordListData(const QString& language)
	: m_maximum_length(0)
{
	// Read words from disk
	QFile file("connectagram:" + language + "/words");
	if (!file.open(QFile::ReadOnly)) {
		return;
	}
	const QByteArray data = qUncompress(file.readAll());
	file.close();

	QTextStream in(data);
	while (!in.atEnd()) {
		QStringList spellings = in.readLine().simplified().split(" ", Qt::SkipEmptyParts);
		if (spellings.isEmpty()) {
			continue;
		}

		QString word = spellings.takeFirst();
		int length = word.length();
		if (length < 5) {
			continue;
		}

		m_maximum_length = std::max(m_maximum_length, length);
		m_all_words[length - 1].append(word.toUpper());

		if (!spellings.isEmpty()) {
			m_spellings[word] = spellings;
		}
	}

	// Adjust maximum length to account for maximum amount of words
	for (int i = m_maximum_length - 1; i > 0; --i) {
		if (m_all_words[i].size() >= 20) {
			m_maximum_length = i + 1;
			break;
		}
	}
}

//-----------------------------------------------------------------------------
