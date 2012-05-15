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

#ifndef LETTER_H
#define LETTER_H

#include <QGraphicsPathItem>
class QGraphicsSimpleTextItem;
class Board;
class Cell;

class Letter : public QGraphicsPathItem {
	public:
		Letter(const QChar& character, Board* board);

		QChar character() const {
			return m_character;
		}

		bool isMovable() const {
			return m_movable;
		}

		void setCell(Cell* cell);
		void setCorrect();
		void setHighlight(bool highlight = true);
		void setJoin();
		void setPaused(bool paused);

	protected:
		virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
		virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

	private:
		void setText(const QChar& character);

	private:
		Board* m_board;
		QChar m_character;
		Cell* m_cell;
		bool m_correct;
		bool m_movable;
		bool m_dragged;
		QGraphicsSimpleTextItem* m_text;
		QGraphicsPathItem* m_shadow;
};

#endif
