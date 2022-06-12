/*
	SPDX-FileCopyrightText: 2009-2014 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "clock.h"

#include <QBrush>
#include <QCursor>
#include <QFont>
#include <QPen>
#include <QSettings>
#include <QTimeLine>

//-----------------------------------------------------------------------------

Clock::Clock(QWidget* parent)
	: QLabel(parent)
	, m_clock(0, 0, 0)
	, m_paused(true)
	, m_stopped(true)
{
	QFont f = font();
	f.setPixelSize(20);
	setFont(f);
	setCursor(Qt::PointingHandCursor);

	QPalette p = palette();
	p.setColor(foregroundRole(), Qt::white);
	setPalette(p);

	m_clock_timer = new QTimeLine(1000, this);
	connect(m_clock_timer, &QTimeLine::finished, this, &Clock::tick);
}

//-----------------------------------------------------------------------------

void Clock::start()
{
	m_paused = false;
	m_stopped = false;
	m_clock_timer->stop();
	m_clock = QTime(0, 0, 0);
	m_clock = m_clock.addMSecs(QSettings().value("Current/Time").toInt());
	updateText();
	m_clock_timer->start();
}

//-----------------------------------------------------------------------------

void Clock::stop()
{
	m_stopped = true;
	m_clock_timer->stop();
	QSettings().remove("Current/Time");
	updateText();
}

//-----------------------------------------------------------------------------

void Clock::setLoading()
{
	m_stopped = true;
	m_clock_timer->stop();
	setText(tr("Loading"));
}

//-----------------------------------------------------------------------------

void Clock::setPaused(bool paused)
{
	m_paused = paused;
	if (m_clock_timer->state() != QTimeLine::NotRunning) {
		m_clock_timer->setPaused(paused);
	}
	if (!m_stopped) {
		updateText();
	}
}

//-----------------------------------------------------------------------------

void Clock::mousePressEvent(QMouseEvent* event)
{
	QLabel::mousePressEvent(event);
	Q_EMIT togglePaused();
}

//-----------------------------------------------------------------------------

void Clock::tick()
{
	updateTime();
	if (!m_stopped) {
		m_clock_timer->start();
	}
}

//-----------------------------------------------------------------------------

void Clock::updateTime()
{
	m_clock = m_clock.addMSecs(m_clock_timer->currentTime());
	QSettings().setValue("Current/Time", QTime(0, 0, 0).msecsTo(m_clock));
	updateText();
}

//-----------------------------------------------------------------------------

void Clock::updateText()
{
	setText(!m_paused ? m_clock.toString("hh:mm:ss") : tr("Paused"));
}

//-----------------------------------------------------------------------------
