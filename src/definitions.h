/*
	SPDX-FileCopyrightText: 2009-2021 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef CONNECTAGRAM_DEFINITIONS_H
#define CONNECTAGRAM_DEFINITIONS_H

class WordList;

#include <QHash>
#include <QMenu>
#include <QUrl>

class Definitions : public QMenu
{
	Q_OBJECT

public:
	explicit Definitions(const WordList* wordlist, QWidget* parent = nullptr);

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

#endif // CONNECTAGRAM_DEFINITIONS_H
