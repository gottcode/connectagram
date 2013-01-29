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

#include "view.h"

#include "board.h"

#include <QMouseEvent>

View::View(Board* board, QWidget* parent)
: QGraphicsView(board, parent), m_board(board) {
	setDragMode(QGraphicsView::ScrollHandDrag);
	viewport()->setCursor(Qt::ArrowCursor);
	setFrameStyle(QFrame::NoFrame);
	setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	connect(m_board, SIGNAL(started()), this, SLOT(gameStarted()));

	// Handle default font being bigger than font of letters
	QFont f = font();
	f.setPixelSize(20);
	qreal h1 = QFontMetrics(f).height();
	qreal h2 = fontMetrics().height();
	if (h2 > h1) {
		qreal s = h2 / h1;
		scale(s, s);
	}
}

//-----------------------------------------------------------------------------

void View::mouseReleaseEvent(QMouseEvent* event) {
	QGraphicsView::mouseReleaseEvent(event);
	if (!itemAt(event->pos())) {
		viewport()->setCursor(Qt::ArrowCursor);
	}
}

//-----------------------------------------------------------------------------

void View::gameStarted() {
	centerOn(m_board->sceneRect().center());
}
