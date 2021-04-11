/*
	SPDX-FileCopyrightText: 2009-2021 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "definitions.h"

#include "wordlist.h"

#include <QDesktopServices>
#include <QSettings>
#include <QUrl>

//-----------------------------------------------------------------------------

Definitions::Definitions(const WordList* wordlist, QWidget* parent)
	: QMenu(parent)
	, m_wordlist(wordlist)
{
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

	QByteArray url = m_url;
	url.replace("%s", QUrl::toPercentEncoding(word));
	QDesktopServices::openUrl(QUrl::fromEncoded(url));
}

//-----------------------------------------------------------------------------

void Definitions::setLanguage(const QString& langcode)
{
	QSettings settings(QString("connectagram:%1/language.ini").arg(langcode), QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
	settings.setIniCodec("UTF-8");
#endif
	QString url = settings.value("Language/Dictionary").toString();
	if (url.isEmpty()) {
		url = QString("https://%1.wiktionary.org/wiki/%s").arg(langcode);
	}
	m_url = QUrl::toPercentEncoding(url, "#$%&+,/:;=?@~");
}

//-----------------------------------------------------------------------------
