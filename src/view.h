/*
	SPDX-FileCopyrightText: 2009-2021 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef CONNECTAGRAM_VIEW_H
#define CONNECTAGRAM_VIEW_H

class Board;

#include <QGraphicsView>
#include <QTime>

class View : public QGraphicsView
{
	Q_OBJECT

public:
	explicit View(Board* board, QWidget* parent = nullptr);

protected:
	void focusOutEvent(QFocusEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;

private slots:
	void gameStarted();

private:
	Board* m_board;
};

#endif // CONNECTAGRAM_VIEW_H
