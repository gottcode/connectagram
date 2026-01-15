/*
	SPDX-FileCopyrightText: 2009 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef CONNECTAGRAM_VIEW_H
#define CONNECTAGRAM_VIEW_H

class Board;

#include <QGraphicsView>
#include <QTime>

/**
 * @brief The View class displays the game area.
 */
class View : public QGraphicsView
{
	Q_OBJECT

public:
	/**
	 * Constructs a view instance.
	 * @param board the game area to display
	 * @param parent the QWidget that manages the view
	 */
	explicit View(Board* board, QWidget* parent = nullptr);

protected:
	/**
	 * Override focusOutEvent to pause the game.
	 * @param event details of the focus out event
	 */
	void focusOutEvent(QFocusEvent* event) override;

	/**
	 * Override mouseReleaseEvent to reset mouse cursor if dropping outside of word.
	 * @param event details of mouse release event
	 */
	void mouseReleaseEvent(QMouseEvent* event) override;

private Q_SLOTS:
	/**
	 * Center the board when the game starts.
	 */
	void gameStarted();

private:
	Board* m_board; /**< the game area */
};

#endif // CONNECTAGRAM_VIEW_H
