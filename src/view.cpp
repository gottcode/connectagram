/*
	SPDX-FileCopyrightText: 2009-2014 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "view.h"

#include "board.h"

#include <QMouseEvent>

//-----------------------------------------------------------------------------

View::View(Board* board, QWidget* parent)
	: QGraphicsView(board, parent)
	, m_board(board)
{
	setDragMode(QGraphicsView::ScrollHandDrag);
	viewport()->setCursor(Qt::ArrowCursor);
	setFrameStyle(QFrame::NoFrame);
	setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	connect(m_board, &Board::started, this, &View::gameStarted);

	// Scale board to be 50% larger than default font
	QFont f = font();
	f.setPixelSize(20);
	qreal h1 = QFontMetrics(f).height();
	qreal h2 = fontMetrics().height() * 1.5;
	qreal s = h2 / h1;
	scale(s, s);
}

//-----------------------------------------------------------------------------

void View::mouseReleaseEvent(QMouseEvent* event)
{
	QGraphicsView::mouseReleaseEvent(event);
	if (!itemAt(event->pos())) {
		viewport()->setCursor(Qt::ArrowCursor);
	}
}

//-----------------------------------------------------------------------------

void View::gameStarted()
{
	centerOn(m_board->sceneRect().center());
}

//-----------------------------------------------------------------------------
