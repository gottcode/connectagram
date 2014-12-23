/***********************************************************************
 *
 * Copyright (C) 2009, 2012, 2014 Graeme Gott <graeme@gottcode.org>
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

#include "score_board.h"

#include <QDialogButtonBox>
#include <QHeaderView>
#include <QSettings>
#include <QTime>
#include <QTreeWidget>
#include <QVBoxLayout>

#include <cmath>

ScoreBoard::ScoreBoard(QWidget* parent)
: QDialog(parent) {
	setWindowTitle(tr("Scores"));

	// Create tree view
	m_scores = new QTreeWidget(this);
	m_scores->setHeaderLabels(QStringList() << tr("Score") << tr("Time") << tr("Words") << tr("Length"));
	m_scores->header()->setSectionsMovable(false);
	m_scores->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	m_scores->header()->setStretchLastSection(false);
	m_scores->setRootIsDecorated(false);

	// Load scores
	QStringList scores = QSettings().value("Scores/Values").toStringList();
	if (!scores.isEmpty() && scores.count() % 3 == 0) {
		for (int i = 0; i < scores.count(); i += 3) {
			createScoreItem(scores[i].toInt(), scores[i + 1].toInt(), scores[i + 2].toInt());
		}
	}

	// Create OK button
	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
	connect(buttons, &QDialogButtonBox::rejected, this, &ScoreBoard::reject);

	// Layout window
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(m_scores);
	layout->addWidget(buttons);

	// Resize window
	resize(QSettings().value("Scores/Size", sizeHint()).toSize());
}

//-----------------------------------------------------------------------------

void ScoreBoard::addScore(int secs, int count, int length) {
	// Add score to list
	QTreeWidgetItem* item = createScoreItem(secs, count, length);
	if (item == 0) {
		return;
	}
	m_scores->clearSelection();
	item->setSelected(true);

	// Save scores
	QStringList scores;
	for (int i = 0; i < m_scores->topLevelItemCount(); ++i) {
		item = m_scores->topLevelItem(i);
		scores.append(item->data(1, Qt::UserRole).toString());
		scores.append(item->text(2));
		scores.append(item->text(3));
	}
	QSettings().setValue("Scores/Values", scores);

	show();
}

//-----------------------------------------------------------------------------

void ScoreBoard::hideEvent(QHideEvent* event) {
	QSettings().setValue("Scores/Size", size());
	QDialog::hideEvent(event);
}

//-----------------------------------------------------------------------------

QTreeWidgetItem* ScoreBoard::createScoreItem(int secs, int count, int length) {
	// Calculate score
	if (secs == 0) {
		return 0;
	}
	int score = std::lround((count * length * 120.0) / secs);
	if (score == 0) {
		return 0;
	}

	// Find position
	int pos = 0;
	for (pos = 0; pos < m_scores->topLevelItemCount(); ++pos) {
		if (score > m_scores->topLevelItem(pos)->text(0).toInt()) {
			break;
		}
	}
	if (pos >= 10) {
		return 0;
	}

	// Create item
	QTime time = QTime(0, 0, 0).addSecs(secs);
	QTreeWidgetItem* item = new QTreeWidgetItem(QStringList() << QString::number(score) << time.toString("hh:mm:ss") << QString::number(count) << QString::number(length));
	item->setData(1, Qt::UserRole, secs);
	item->setTextAlignment(0, Qt::AlignRight | Qt::AlignVCenter);
	item->setTextAlignment(1, Qt::AlignRight | Qt::AlignVCenter);
	item->setTextAlignment(2, Qt::AlignRight | Qt::AlignVCenter);
	item->setTextAlignment(3, Qt::AlignRight | Qt::AlignVCenter);

	// Add item
	if (m_scores->topLevelItemCount() == 10) {
		delete m_scores->takeTopLevelItem(9);
	}
	m_scores->insertTopLevelItem(pos, item);

	return item;
}
