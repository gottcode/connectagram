/*
	SPDX-FileCopyrightText: 2009-2023 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef CONNECTAGRAM_LETTER_H
#define CONNECTAGRAM_LETTER_H

class Board;
class Cell;

#include <QGraphicsPathItem>
class QGraphicsSimpleTextItem;

/**
 * @brief The Letter class defines a letter on the board.
 */
class Letter : public QGraphicsPathItem
{
public:
	/**
	 * Constructs a letter instance.
	 * @param character the character displayed by the letter
	 * @param board the board containing the letter
	 */
	Letter(const QChar& character, Board* board);

	/**
	 * @return the character displayed by the letter
	 */
	QChar character() const
	{
		return m_character;
	}

	/**
	 * @return whether the letter can be moved
	 */
	bool isMovable() const
	{
		return m_movable;
	}

	/**
	 * Sets the cell containing the letter.
	 * @param cell the cell containing the letter
	 */
	void setCell(Cell* cell);

	/**
	 * Handles setting the color and making correct letters non-movable.
	 */
	void setCorrect();

	/**
	 * Sets the color of the letter based on if it is highlighted.
	 * @param highlight whether the letter is highlighted
	 */
	void setHighlight(bool highlight = true);

	/**
	 * Handles setting the color and making letters that join words non-movable.
	 */
	void setJoin();

	/**
	 * Displays the character or '?' if the game is paused.
	 * @param paused whether the game is paused
	 */
	void setPaused(bool paused);

protected:
	/**
	 * Override mouseMoveEvent to handle the player clicking on a word.
	 * @param event details of the mouse double click event
	 */
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

	/**
	 * Override mouseMoveEvent to handle the player dragging a letter.
	 * @param event details of the mouse move event
	 */
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

	/**
	 * Override mousePressEvent to handle the player starting to drag a letter.
	 * @param event details of the mouse press event
	 */
	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

	/**
	 * Override mouseReleaseEvent to handle the player dropping a letter.
	 * @param event details of the mouse release event
	 */
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
	/**
	 * Sets the text actually displayed. Handles substituting vowels at end of Hebrew words until
	 * the word has been correctly spelled.
	 * @param character the text to display
	 */
	void setText(const QChar& character);

private:
	Board* m_board; /**< the board the letter belongs to */
	QChar m_character; /**< the character displayed by the letter */
	Cell* m_cell; /**< the cell currently containing the letter */
	bool m_correct; /**< is the word correctly spelled */
	bool m_movable; /**< is the letter movable */
	bool m_dragged; /**< is the letter currently being dragged */
	QGraphicsSimpleTextItem* m_text; /**< the graphics item that displays the character */
	QGraphicsPathItem* m_shadow; /**< the shadow where the letter was dragged from */
};

#endif // CONNECTAGRAM_LETTER_H
