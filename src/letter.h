/*
	SPDX-FileCopyrightText: 2009 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef CONNECTAGRAM_LETTER_H
#define CONNECTAGRAM_LETTER_H

#include <QGraphicsPathItem>
class QGraphicsSimpleTextItem;
class Board;
class Cell;

class Letter : public QGraphicsPathItem
{
public:
	Letter(const QChar& character, Board* board);

	QChar character() const
	{
		return m_character;
	}

	bool isMovable() const
	{
		return m_movable;
	}

	void setCell(Cell* cell);
	void setCorrect();
	void setHighlight(bool highlight = true);
	void setJoin();
	void setPaused(bool paused);

protected:
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

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

#endif // CONNECTAGRAM_LETTER_H
