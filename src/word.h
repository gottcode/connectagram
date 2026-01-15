/*
	SPDX-FileCopyrightText: 2009 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef CONNECTAGRAM_WORD_H
#define CONNECTAGRAM_WORD_H

class Board;
class WordList;

#include <QChar>
#include <QList>
#include <QPoint>
#include <QString>
class QGraphicsItem;

#include <random>

/**
 * @brief The Word class defines a word on the board.
 */
class Word
{
public:
	/**
	 * Constructs a word instance.
	 * @param word the initial correctly spelled word
	 * @param position the location on the board
	 * @param orientation the direction of the word
	 * @param random the random number generator
	 */
	Word(const QString& word, const QPoint& position, Qt::Orientation orientation, std::mt19937& random);

	/**
	 * Returns a character in first spelling of the word.
	 * @param i index for the character
	 * @return character at index
	 */
	QChar at(int i) const
	{
		return m_solutions.at(0).at(i);
	}

	/**
	 * Handle player clicking on the word.
	 */
	void click();

	/**
	 * Handles determining if the player has solved the anagram.
	 */
	void check();

	/**
	 * @return finds the first misspelled letter and shows where it should go
	 */
	QGraphicsItem* hint();

	/**
	 * @return has the word been solved by the player
	 */
	bool isCorrect() const
	{
		return m_correct;
	}

	/**
	 * @return direction of the word
	 */
	Qt::Orientation orientation() const
	{
		return m_orientation;
	}

	/**
	 * @return locations of each letter in word on the board
	 */
	QList<QPoint> positions() const
	{
		return m_positions;
	}

	/**
	 * @return the valid spellings of the word
	 */
	QList<QString> solutions() const
	{
		return m_solutions;
	}

	/**
	 * Repositions the word.
	 * @param delta how much to move the word by
	 */
	void moveBy(const QPoint& delta);

	/**
	 * Creates the word from a shuffled word.
	 * @param shuffled the misspelled word to start from
	 */
	void fromString(const QString& shuffled);

	/**
	 * @return the word in its current shuffled form
	 */
	QString toString() const;

	/**
	 * Sets the game board the word belongs to.
	 * @param board the game board
	 */
	void setBoard(Board* board)
	{
		m_board = board;
	}

	/**
	 * Highlights a word.
	 * @param highlight whether the word is highlighted
	 */
	void setHighlight(bool highlight);

	/**
	 * Creates a word by looking at the locations on the board and finding a word that fits.
	 * Then it randomizes the letters to become an anagram of the solution.
	 * @param words the word list to use to find the word and valid solutions
	 */
	void shuffle(const WordList* words);

private:
	Board* m_board; /**< the game area */
	bool m_correct; /**< is the word spelled correctly */
	QList<QPoint> m_positions; /**< locations on the board */
	QList<QString> m_solutions; /**< valid solutions of the anagram */
	Qt::Orientation m_orientation; /**< direction of word */
	std::mt19937& m_random; /**< the random number generator */
};

#endif // CONNECTAGRAM_WORD_H
