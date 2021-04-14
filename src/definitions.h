/*
	SPDX-FileCopyrightText: 2009-2021 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef CONNECTAGRAM_DEFINITIONS_H
#define CONNECTAGRAM_DEFINITIONS_H

class WordList;

#include <QByteArray>
#include <QHash>
#include <QMenu>

/**
 * @brief The Definitions class manages the looking up of word definitions.
 */
class Definitions : public QMenu
{
	Q_OBJECT

public:
	/**
	 * Constructs a definitions instance.
	 * @param wordlist the full list of words used for proper spellings
	 * @param parent the QWidget that manages the definitions
	 */
	explicit Definitions(const WordList* wordlist, QWidget* parent = nullptr);

public slots:
	/**
	 * Clears the list of words to define.
	 */
	void clearWords();

	/**
	 * Adds a word to the list of words to define and adds a placeholder element to the menu.
	 * @param word the word to add
	 */
	void addWord(const QString& word);

	/**
	 * Finds the proper spelling of a word and updates it in the menu.
	 * @param original_word the word when the board was generated
	 * @param current_word the anagram of @p original_word found by the player
	 */
	void solveWord(const QString& original_word, const QString& current_word);

	/**
	 * Selects a word and shows its definition.
	 * @param word the word to define
	 */
	void selectWord(const QString& word);

private slots:
	/**
	 * Shows the definition of a word in a web browser.
	 * @param action the word to define
	 */
	void defineWord(QAction* action);

	/**
	 * Sets the dictionary used to define words.
	 * @param langcode the language to select a dictionary for.
	 */
	void setLanguage(const QString& langcode);

private:
	const WordList* m_wordlist; /**< the list of proper spellings of words */
	QHash<QString, QAction*> m_word_table; /**< map of words to their menu actions */
	QByteArray m_url; /**< dictionary location to define words */
};

#endif // CONNECTAGRAM_DEFINITIONS_H
