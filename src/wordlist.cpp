/***********************************************************************
 *
 * Copyright (C) 2013 Graeme Gott <graeme@gottcode.org>
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
#include <QTextStream>

//-----------------------------------------------------------------------------

WordList::WordList()
: m_maximum_length(0), m_length(0) {
}

//-----------------------------------------------------------------------------

QStringList WordList::filter(const QString& known_letters) const {
	QRegExp filter(known_letters);
	QStringList filtered;
	for (QStringList::const_iterator i = m_words.begin(), end = m_words.end(); i != end; ++i) {
		if (filter.exactMatch(*i)) {
			QString sorted = *i;
			std::sort(sorted.begin(), sorted.end());
			if (m_anagram_filters.contains(sorted)) {
				continue;
			}
			filtered += *i;
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

bool WordList::setLanguage(const QString& langcode) {
	if (m_langcode == langcode) {
		return true;
	}
	m_langcode = langcode;

	m_all_words.clear();
	m_words.clear();

	// Read words from disk
	QFile file("connectagram:" + langcode + "/words");
	if (!file.open(QFile::ReadOnly | QIODevice::Text)) {
		return false;
	}

	QTextStream in(&file);
	in.setCodec("UTF-8");
	while (!in.atEnd()) {
		QStringList spellings = in.readLine().simplified().split(" ", QString::SkipEmptyParts);
		if (spellings.isEmpty()) {
			continue;
		}

		QString word = spellings.takeFirst();
		int length = word.length();
		if (length < 5) {
			continue;
		}

		m_maximum_length = qMax(m_maximum_length, length);
		m_all_words[length - 1].append(word.toUpper());

		if (!spellings.isEmpty()) {
			m_spellings[word] = spellings;
		}
	}

	resetWords();

	// Adjust maximum length to account for maximum amount of words
	for (int i = m_maximum_length - 1; i > 0; --i) {
		if (m_all_words[i].size() >= 20) {
			m_maximum_length = i + 1;
			break;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------

void WordList::setLength(int length) {
	m_length = length;
	resetWords();
}

//-----------------------------------------------------------------------------

void WordList::resetWords() {
	m_words = m_all_words.value(m_length);
}

//-----------------------------------------------------------------------------
