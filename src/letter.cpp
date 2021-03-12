/*
	SPDX-FileCopyrightText: 2009-2021 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "letter.h"

#include "board.h"
#include "cell.h"
#include "word.h"

#include <QCursor>
#include <QFont>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSimpleTextItem>
#include <QPainterPath>
#include <QPen>

//-----------------------------------------------------------------------------

Letter::Letter(const QChar& character, Board* board)
	: m_board(board)
	, m_character(character)
	, m_cell(nullptr)
	, m_correct(false)
	, m_movable(true)
	, m_dragged(false)
	, m_shadow(nullptr)
{
	QPainterPath path;
	path.addRoundedRect(0, 0, 32, 32, 5, 5);
	setPath(path);
	setCursor(Qt::OpenHandCursor);
	setPen(Qt::NoPen);
	setBrush(QColor(0x90, 0x90, 0x90, 0x9d));
	setFlag(QGraphicsItem::ItemIsMovable);
	setZValue(1);

	m_text = new QGraphicsSimpleTextItem(this);

	QFont f = m_text->font();
	f.setPixelSize(20);
	f.setWeight(QFont::Bold);
	m_text->setFont(f);
	m_text->setBrush(Qt::white);
	setText(m_character);
}

//-----------------------------------------------------------------------------

void Letter::setCell(Cell* cell)
{
	m_cell = cell;
	QGraphicsItem* item = m_shadow ? m_shadow : this;
	item->setPos(m_cell->position().x() * 34 + 2, m_cell->position().y() * 34 + 34);
}

//-----------------------------------------------------------------------------

void Letter::setCorrect()
{
	setFlag(QGraphicsItem::ItemIsMovable, false);
	setBrush(QColor(0, 0x8c, 0));
	setCursor(Qt::PointingHandCursor);
	m_correct = true;
	m_movable = false;
}

//-----------------------------------------------------------------------------

void Letter::setHighlight(bool highlight)
{
	if (m_movable) {
		setBrush(highlight ? QColor(0, 0x57, 0xae) : QColor(0x90, 0x90, 0x90, 0x9d));
	}
}

//-----------------------------------------------------------------------------

void Letter::setJoin()
{
	setFlag(QGraphicsItem::ItemIsMovable, false);
	setBrush(QColor(0, 0, 0, 0xaa));
	setCursor(Qt::ArrowCursor);
	m_movable = false;
}

//-----------------------------------------------------------------------------

void Letter::setPaused(bool paused)
{
	if (m_movable) {
		setText(!paused ? m_character : '?');
	}
}

//-----------------------------------------------------------------------------

void Letter::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	if (m_dragged == false) {
		event->accept();
		return;
	}
	QGraphicsPathItem::mouseMoveEvent(event);

	if (m_board->isPaused()) {
		m_board->setPaused(false);
		m_board->setCurrentWord(m_cell->word());
	}

	QPoint pos = (((mapToScene(boundingRect().center()) - QPoint(2.f, 34.f)) / 34.f) - QPointF(0.5f, 0.5f)).toPoint();
	Cell* cell = nullptr;
	if (m_cell->word()->orientation() == Qt::Horizontal) {
		cell = m_board->cell(pos.x(), m_cell->position().y());
	} else {
		cell = m_board->cell(m_cell->position().x(), pos.y());
	}
	if (!cell || cell == m_cell || cell->word() != m_cell->word()) {
		return;
	}

	Letter* letter = cell->letter();
	if (!letter || letter->m_movable == false) {
		return;
	}
	m_cell->setLetter(letter);
	cell->setLetter(this);
}

//-----------------------------------------------------------------------------

void Letter::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	if (event->button() != Qt::LeftButton) {
		event->ignore();
		return;
	}
	m_dragged = true;

	if (!m_movable) {
		if (m_correct) {
			Word* word = m_cell->word();
			if (word) {
				word->click();
			} else {
				m_board->click("");
			}
		}
		m_dragged = false;
		return;
	}
	setCursor(Qt::ClosedHandCursor);
	setZValue(10);
	m_board->setPaused(false);
	m_board->setCurrentWord(m_cell->word());

	m_shadow = scene()->addPath(path(), Qt::NoPen, QColor(0xa5, 0xc1, 0xe4));
	m_shadow->setPos(pos());

	QGraphicsPathItem::mousePressEvent(event);
}

//-----------------------------------------------------------------------------

void Letter::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	if (event->button() != Qt::LeftButton) {
		event->ignore();
		return;
	}

	if (m_dragged == false) {
		event->accept();
		return;
	}

	m_dragged = false;

	setCursor(Qt::OpenHandCursor);
	setZValue(1);

	setPos(m_shadow->pos());
	delete m_shadow;
	m_shadow = nullptr;

	m_cell->word()->check();

	QGraphicsPathItem::mouseReleaseEvent(event);
}

//-----------------------------------------------------------------------------

void Letter::setText(const QChar& character)
{
	m_text->setText(character);
	m_text->setPos(boundingRect().center() - m_text->boundingRect().center());
}

//-----------------------------------------------------------------------------
