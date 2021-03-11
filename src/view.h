/***********************************************************************
 *
 * Copyright (C) 2009, 2013 Graeme Gott <graeme@gottcode.org>
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

#ifndef VIEW_H
#define VIEW_H

#include <QGraphicsView>
#include <QTime>
class Board;

class View : public QGraphicsView
{
	Q_OBJECT

public:
	View(Board* board, QWidget* parent = nullptr);

protected:
	void mouseReleaseEvent(QMouseEvent* event);

private slots:
	void gameStarted();

private:
	Board* m_board;
};

#endif
