/*
	SPDX-FileCopyrightText: 2009 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef CONNECTAGRAM_CELL_H
#define CONNECTAGRAM_CELL_H

class Letter;
class Word;

#include <QPoint>

/**
 * @brief The Cell class defines a position on the board and holds a letter.
 */
class Cell
{
public:
	/**
	 * Constructs a cell instance.
	 * @param position the location on the board
	 * @param letter the starting letter held by the cell
	 */
	Cell(const QPoint& position, Letter* letter);

	/**
	 * @return the position on the board
	 */
	QPoint position() const
	{
		return m_position;
	}

	/**
	 * @return the letter currently held by the cell
	 */
	Letter* letter() const
	{
		return m_letter;
	}

	/**
	 * Sets the letter held by the cell.
	 * @param letter the letter to be held by the cell
	 */
	void setLetter(Letter* letter);

	/**
	 * @return the word the letter belongs to
	 */
	Word* word() const
	{
		return m_word;
	}

	/**
	 * Sets the word the letter belongs to.
	 * @param word the word the letter belongs to
	 */
	void setWord(Word* word)
	{
		m_word = word;
	}

private:
	QPoint m_position; /**< location on the board */
	Word* m_word; /**< which word the cell is part of */
	Letter* m_letter; /**< letter held by the cell */
};

#endif // CONNECTAGRAM_CELL_H
