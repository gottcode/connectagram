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

#include "definitions.h"

#include "wordlist.h"

#include <QDesktopServices>

//-----------------------------------------------------------------------------

Definitions::Definitions(const WordList* wordlist, QWidget* parent)
	: QMenu(parent)
	, m_wordlist(wordlist)
{
	m_url.setScheme("https");
	m_url.setPath("/wiki/");
	connect(wordlist, &WordList::languageChanged, this, &Definitions::setLanguage);

	connect(this, &QMenu::triggered, this, &Definitions::defineWord);
}

//-----------------------------------------------------------------------------

void Definitions::clearWords()
{
	m_word_table.clear();
	clear();
}

//-----------------------------------------------------------------------------

void Definitions::addWord(const QString& word)
{
	QAction* action = addAction(QString(word.length(), QChar('?')));
	m_word_table.insert(word, action);
}

//-----------------------------------------------------------------------------

void Definitions::solveWord(const QString& original_word, const QString& current_word)
{
	QAction* action = m_word_table.value(original_word);
	if (!action) {
		return;
	}
	if (original_word != current_word) {
		m_word_table.remove(original_word);
		m_word_table.insert(current_word, action);
	}
	action->setText(current_word);

	// Sort actions
	removeAction(action);
	QAction* before = nullptr;
	const QList<QAction*> acts = actions();
	for (QAction* check : acts) {
		if (check->text().localeAwareCompare(current_word) >= 0) {
			before = check;
			break;
		}
	}
	insertAction(before, action);
}

//-----------------------------------------------------------------------------

void Definitions::selectWord(const QString& word)
{
	QAction* action = m_word_table.value(word);
	if (action) {
		defineWord(action);
	}
}

//-----------------------------------------------------------------------------

void Definitions::defineWord(QAction* action)
{
	if (!action || (action->text().at(0) == QChar('?'))) {
		return;
	}

	const QString word = m_wordlist->spellings(action->text()).constFirst();

	QUrl url = m_url;
	url.setPath("/wiki/" + word);
	QDesktopServices::openUrl(url);
}

//-----------------------------------------------------------------------------

void Definitions::setLanguage(const QString& langcode)
{
	m_url.setHost(langcode + ".wiktionary.org");
}

//-----------------------------------------------------------------------------
