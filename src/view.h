/*
	SPDX-FileCopyrightText: 2009-2013 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef VIEW_H
#define VIEW_H

#include <QGraphicsView>
#include <QTime>
class Board;

class View : public QGraphicsView
{
	Q_OBJECT

public:
	explicit View(Board* board, QWidget* parent = nullptr);

protected:
	void mouseReleaseEvent(QMouseEvent* event) override;

private slots:
	void gameStarted();

private:
	Board* m_board;
};

#endif
