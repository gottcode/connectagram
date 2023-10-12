/*
	SPDX-FileCopyrightText: 2009-2021 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "scores_dialog.h"

#include "new_game_dialog.h"

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QLocale>
#include <QPushButton>
#include <QSettings>
#include <QStyle>
#include <QTime>

#if defined(Q_OS_UNIX)
#include <pwd.h>
#include <unistd.h>
#elif defined(Q_OS_WIN)
#include <lmcons.h>
#include <windows.h>
#endif

#include <cmath>

//-----------------------------------------------------------------------------

ScoresDialog::ScoresDialog(QWidget* parent)
	: QDialog(parent)
{
	setWindowTitle(tr("High Scores"));

	QSettings settings;

	// Load default name
	m_default_name = settings.value("Scores/DefaultName").toString();
	if (m_default_name.isEmpty()) {
#if defined(Q_OS_UNIX)
		passwd* pws = getpwuid(geteuid());
		if (pws) {
			m_default_name = QString::fromLocal8Bit(pws->pw_gecos).section(',', 0, 0);
			if (m_default_name.isEmpty()) {
				m_default_name = QString::fromLocal8Bit(pws->pw_name);
			}
		}
#elif defined(Q_OS_WIN)
		TCHAR buffer[UNLEN + 1];
		DWORD count = UNLEN + 1;
		if (GetUserName(buffer, &count)) {
			m_default_name = QString::fromWCharArray(buffer, count);
		}
#endif
	}

	m_username = new QLineEdit(this);
	m_username->hide();
	connect(m_username, &QLineEdit::editingFinished, this, &ScoresDialog::editingFinished);

	// Create score widgets
	m_scores_layout = new QGridLayout(this);
	m_scores_layout->setSizeConstraint(QLayout::SetFixedSize);
	m_scores_layout->setHorizontalSpacing(18);
	m_scores_layout->setVerticalSpacing(6);
	m_scores_layout->setColumnStretch(NameColumn, 1);
	m_scores_layout->addWidget(new QLabel("<b>" + tr("Rank") + "</b>", this), 1, RankColumn, Qt::AlignCenter);
	m_scores_layout->addWidget(new QLabel("<b>" + tr("Name") + "</b>", this), 1, NameColumn, Qt::AlignCenter);
	m_scores_layout->addWidget(new QLabel("<b>" + tr("Score") + "</b>", this), 1, ScoreColumn, Qt::AlignCenter);
	m_scores_layout->addWidget(new QLabel("<b>" + tr("Time") + "</b>", this), 1, TimeColumn, Qt::AlignCenter);
	m_scores_layout->addWidget(new QLabel("<b>" + tr("Words") + "</b>", this), 1, WordsColumn, Qt::AlignCenter);
	m_scores_layout->addWidget(new QLabel("<b>" + tr("Length") + "</b>", this), 1, LengthColumn, Qt::AlignCenter);

	QFrame* divider = new QFrame(this);
	divider->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	m_scores_layout->addWidget(divider, 2, 0, 1, TotalColumns);

	QList<Qt::Alignment> alignments(TotalColumns, Qt::AlignTrailing);
	alignments[NameColumn] = Qt::AlignLeading;
	alignments[WordsColumn] = Qt::AlignHCenter;
	for (int r = 0; r < 10; ++r) {
		m_score_labels[r][0] = new QLabel(tr("#%1").arg(r + 1), this);
		m_scores_layout->addWidget(m_score_labels[r][0], r + 3, 0, alignments[RankColumn] | Qt::AlignVCenter);
		for (int c = RankColumn + 1; c < TotalColumns; ++c) {
			m_score_labels[r][c] = new QLabel("-", this);
			m_scores_layout->addWidget(m_score_labels[r][c], r + 3, c, alignments[c] | Qt::AlignVCenter);
		}
	}

	// Populate scores widgets with values
	load(settings);

	// Lay out dialog
	m_buttons = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
	m_buttons->setCenterButtons(style()->styleHint(QStyle::SH_MessageBox_CenterButtons));
	m_buttons->button(QDialogButtonBox::Close)->setDefault(true);
	m_buttons->button(QDialogButtonBox::Close)->setFocus();
	connect(m_buttons, &QDialogButtonBox::rejected, this, &ScoresDialog::reject);
	m_scores_layout->addWidget(m_buttons, 13, 0, 1, TotalColumns);
}

//-----------------------------------------------------------------------------

bool ScoresDialog::addScore(int secs, int count, int length)
{
	// Add score
	if (secs == 0) {
		m_row = -1;
		return false;
	}
	if (!addScore(m_default_name, secs, count, length)) {
		return false;
	}

	// Inform player of success
	QLabel* label = new QLabel(this);
	label->setAlignment(Qt::AlignCenter);
	if (m_row == 0) {
		label->setText(QString("<big>🎉</big> %1<br>%2").arg(tr("Congratulations!"), tr("You beat your top score!")));
	} else {
		label->setText(QString("<big>🙌</big> %1<br>%2").arg(tr("Well done!"), tr("You have a new high score!")));
	}
	m_scores_layout->addWidget(label, 0, 0, 1, TotalColumns);

	// Add score to display
	updateItems();

	// Show lineedit
	m_scores_layout->addWidget(m_username, m_row + 3, 1);
	m_score_labels[m_row][1]->hide();
	m_username->setText(m_scores[m_row].name);
	m_username->show();
	m_username->setFocus();

	m_buttons->button(QDialogButtonBox::Close)->setDefault(false);

	return true;
}

//-----------------------------------------------------------------------------

void ScoresDialog::migrate()
{
	QSettings settings;
	if (!settings.contains("Scores/Values")) {
		return;
	}

	const QStringList scores = settings.value("Scores/Values").toStringList();
	settings.remove("Scores/Size");
	settings.remove("Scores/Values");

	if (scores.isEmpty() || (scores.size() % 3 != 0)) {
		return;
	}

	int index = 0;
	for (int i = 0; i < scores.size(); i += 3) {
		const int secs = scores[i].toInt();
		int count = scores[i + 1].toInt();
		if (count >= 16) {
			count = 3;
		} else if (count >= 12) {
			count = 2;
		} else if (count >= 7) {
			count = 1;
		} else if (count >= 4) {
			count = 0;
		}
		const int length = scores[i + 2].toInt();

		settings.beginWriteArray("Scores_Connectagram");
		settings.setArrayIndex(index);
		settings.setValue("Time", secs);
		settings.setValue("Count", count);
		settings.setValue("Length", length);
		settings.endArray();
		++index;
	}
}

//-----------------------------------------------------------------------------

void ScoresDialog::hideEvent(QHideEvent* event)
{
	editingFinished();
	QDialog::hideEvent(event);
}

//-----------------------------------------------------------------------------

void ScoresDialog::keyPressEvent(QKeyEvent* event)
{
	if ((event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return)) {
		event->ignore();
		return;
	}
	QDialog::keyPressEvent(event);
}

//-----------------------------------------------------------------------------

void ScoresDialog::editingFinished()
{
	if (!m_username->isVisible()) {
		return;
	}

	Q_ASSERT(m_row != -1);

	// Set player name
	m_scores[m_row].name = m_username->text();
	m_score_labels[m_row][1]->setText("<b>" + m_scores[m_row].name + "</b>");

	// Hide lineedit
	m_username->hide();
	m_scores_layout->removeWidget(m_username);
	m_score_labels[m_row][1]->show();

	m_buttons->button(QDialogButtonBox::Close)->setDefault(true);
	m_buttons->button(QDialogButtonBox::Close)->setFocus();

	// Save scores
	QSettings settings;
	settings.setValue("Scores/DefaultName", m_username->text());
	settings.beginWriteArray("Scores_Connectagram");
	for (int r = 0, size = m_scores.size(); r < size; ++r) {
		const Score& score = m_scores[r];
		settings.setArrayIndex(r);
		settings.setValue("Name", score.name);
		settings.setValue("Time", score.secs);
		settings.setValue("Count", score.count);
		settings.setValue("Length", score.length);
	}
	settings.endArray();
}

//-----------------------------------------------------------------------------

bool ScoresDialog::addScore(const QString& name, int secs, int count, int length)
{
	m_row = -1;

	const int score = std::lround(((count + 1) * length * 1200.0) / secs);
	if (score == 0) {
		return false;
	}

	m_row = 0;
	for (const Score& s : std::as_const(m_scores)) {
		if (score >= s.score) {
			break;
		}
		++m_row;
	}
	if (m_row == 10) {
		m_row = -1;
		return false;
	}

	Score s = { name, score, secs, count, length };
	m_scores.insert(m_row, s);
	if (m_scores.size() == 11) {
		m_scores.removeLast();
	}

	return true;
}

//-----------------------------------------------------------------------------

void ScoresDialog::load(QSettings& settings)
{
	const int size = std::min(settings.beginReadArray("Scores_Connectagram"), 10);
	for (int r = 0; r < size; ++r) {
		settings.setArrayIndex(r);
		const QString name = settings.value("Name", m_default_name).toString();
		const int secs = settings.value("Time").toInt();
		const int count = settings.value("Count").toInt();
		const int length = settings.value("Length").toInt();
		addScore(name, secs, count, length);
	}
	settings.endArray();

	m_row = -1;
	updateItems();
}

//-----------------------------------------------------------------------------

void ScoresDialog::updateItems()
{
	const int size = m_scores.size();

	// Add scores
	for (int r = 0; r < size; ++r) {
		const Score& score = m_scores[r];
		m_score_labels[r][NameColumn]->setText(score.name);
		m_score_labels[r][ScoreColumn]->setNum(score.score);
		m_score_labels[r][TimeColumn]->setText(QTime(0, 0, 0).addSecs(score.secs).toString("hh:mm:ss"));
		m_score_labels[r][WordsColumn]->setText(NewGameDialog::densityString(score.count));
		m_score_labels[r][LengthColumn]->setNum(score.length);
	}

	// Fill remainder of scores with dashes
	for (int r = size; r < 10; ++r) {
		for (int c = RankColumn + 1; c < TotalColumns; ++c) {
			m_score_labels[r][c]->setText("-");
		}
	}

	// Use bold text for new score
	if (m_row != -1) {
		for (int c = 0; c < TotalColumns; ++c) {
			m_score_labels[m_row][c]->setText("<b>" + m_score_labels[m_row][c]->text() + "</b>");
		}
	}
}

//-----------------------------------------------------------------------------
