/***********************************************************************
 *
 * Copyright (C) 2009 Graeme Gott <graeme@gottcode.org>
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
