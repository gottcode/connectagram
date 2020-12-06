/***********************************************************************
 *
 * Copyright (C) 2013-2020 Graeme Gott <graeme@gottcode.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "wordlist.h"

#include <QFile>
#include <QLocale>
#include <QRegularExpression>
#include <QTextStream>

#include <algorithm>

//-----------------------------------------------------------------------------

WordList::WordList(QObject* parent)
: QObject(parent), m_length(0) {
}

//-----------------------------------------------------------------------------

QStringList WordList::filter(const QString& known_letters) const {
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

void WordList::addAnagramFilter(const QString& word) {
	QString sorted_letters = word;
	std::sort(sorted_letters.begin(), sorted_letters.end());
	m_anagram_filters.append(sorted_letters);
}

//-----------------------------------------------------------------------------

void WordList::resetAnagramFilters() {
	m_anagram_filters.clear();
}

//-----------------------------------------------------------------------------

void WordList::setLanguage(const QString& langcode) {
	if (m_langcode == langcode) {
		return;
	}
	m_langcode = langcode;

	static QHash<QString, QSharedPointer<WordListData> > languages;
	if (!languages.contains(langcode)) {
		languages.insert(langcode, QSharedPointer<WordListData>(new WordListData(langcode)));
	}
	m_data = languages[langcode];
	resetWords();

	emit languageChanged(m_langcode);
}

//-----------------------------------------------------------------------------

void WordList::setLength(int length) {
	m_length = length;
	resetWords();
}

//-----------------------------------------------------------------------------

QString WordList::defaultLanguage() {
	QString language = QLocale().name();
	if (!QFile::exists("connectagram:" + language)) {
		language = language.left(2);
		if (!QFile::exists("connectagram:" + language)) {
			language = "en";
		}
	}
	return language;
}

//-----------------------------------------------------------------------------

void WordList::resetWords() {
	m_words = m_data->words(m_length);
}

//-----------------------------------------------------------------------------

WordList::WordListData::WordListData(const QString& language) :
	m_maximum_length(0)
{
	// Read words from disk
	QFile file("connectagram:" + language + "/words");
	if (!file.open(QFile::ReadOnly | QIODevice::Text)) {
		return;
	}

	QTextStream in(&file);
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
	in.setCodec("UTF-8");
#endif
	while (!in.atEnd()) {
#if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
		QStringList spellings = in.readLine().simplified().split(" ", Qt::SkipEmptyParts);
#else
		QStringList spellings = in.readLine().simplified().split(" ", QString::SkipEmptyParts);
#endif
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
