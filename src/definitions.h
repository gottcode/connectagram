/***********************************************************************
 *
 * Copyright (C) 2009-2021 Graeme Gott <graeme@gottcode.org>
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

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

class WordList;

#include <QHash>
#include <QMenu>
#include <QUrl>

class Definitions : public QMenu
{
	Q_OBJECT

public:
	Definitions(const WordList* wordlist, QWidget* parent = nullptr);

public slots:
	void clearWords();
	void addWord(const QString& word);
	void solveWord(const QString& original_word, const QString& current_word);
	void selectWord(const QString& word);

private slots:
	void defineWord(QAction* action);
	void setLanguage(const QString& langcode);

private:
	const WordList* m_wordlist;
	QHash<QString, QAction*> m_word_table;
	QUrl m_url;
};

#endif
