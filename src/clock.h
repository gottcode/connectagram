/***********************************************************************
 *
 * Copyright (C) 2009 Graeme Gott <graeme@gottcode.org>
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

#ifndef CLOCK_H
#define CLOCK_H

#include <QLabel>
#include <QTime>
class QTimeLine;

class Clock : public QLabel
{
	Q_OBJECT

public:
	Clock(QWidget* parent);

public slots:
	void start();
	void stop();
	void setLoading();
	void setPaused(bool paused);

signals:
	void togglePaused();

protected:
	virtual void mousePressEvent(QMouseEvent* event);

private slots:
	void tick();

private:
	void updateTime();
	void updateText();

private:
	QTime m_clock;
	QTimeLine* m_clock_timer;
	bool m_paused;
	bool m_stopped;
};

#endif
