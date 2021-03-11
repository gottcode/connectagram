/*
	SPDX-FileCopyrightText: 2009 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef CELL_H
#define CELL_H

#include <QPoint>
class Board;
class Letter;
class Word;

class Cell
{
public:
	Cell(const QPoint& position, Letter* letter);

	QPoint position() const
	{
		return m_position;
	}

	Letter* letter() const
	{
		return m_letter;
	}

	void setLetter(Letter* letter);

	Word* word() const
	{
		return m_word;
	}

	void setWord(Word* word)
	{
		m_word = word;
	}

private:
	QPoint m_position;
	Word* m_word;
	Letter* m_letter;
};

#endif
