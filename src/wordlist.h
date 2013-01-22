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

#ifndef WORD_LIST_H
#define WORD_LIST_H

#include <QHash>
#include <QStringList>

class WordList {
public:
	WordList();

	bool isEmpty() const {
		return m_all_words.isEmpty();
	}

	QString language() const {
		return m_langcode;
	}

	int maximumLength() const {
		return m_maximum_length;
	}

	QStringList filter(const QString& known_letters) const;

	QStringList spellings(const QString& word) const {
		return m_spellings.value(word, QStringList(word.toLower()));
	}

	void addAnagramFilter(const QString& word);
	void resetAnagramFilters();
	void setLanguage(const QString& langcode);
	void setLength(int length);

private:
	void resetWords();

private:
	QString m_langcode;
	QHash<int, QStringList> m_all_words;
	QHash<QString, QStringList> m_spellings;
	QStringList m_words;
	QStringList m_anagram_filters;
	int m_maximum_length;
	int m_length;
};

#endif