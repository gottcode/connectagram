/***********************************************************************
 *
 * Copyright (C) 2009, 2013, 2014, 2015, 2019 Graeme Gott <graeme@gottcode.org>
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

#include "new_game_dialog.h"

#include "board.h"
#include "locale_dialog.h"
#include "pattern.h"
#include "wordlist.h"

#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDir>
#include <QFormLayout>
#include <QFrame>
#include <QGridLayout>
#include <QKeyEvent>
#include <QSettings>
#include <QSpinBox>
#include <QToolButton>
#include <QVBoxLayout>

#include <ctime>
#include <random>

NewGameDialog::NewGameDialog(Board* board, QWidget* parent)
: QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint), m_board(board) {
	setWindowTitle(tr("New Game"));

	// Create languages box
	m_wordlist = new WordList(this);
	m_wordlist->setLanguage(WordList::defaultLanguage());
	m_languages_box = new QComboBox(this);
	QStringList languages = QDir("connectagram:").entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	for (const QString& language : languages) {
		m_languages_box->addItem(LocaleDialog::languageName(language), language);
	}

	// Create word count box
	m_word_count_box = new QComboBox(this);
	m_word_count_box->addItems(QStringList() << tr("Low") << tr("Medium") << tr("High") << tr("Very High"));

	// Create word length box
	m_word_length_box = new QComboBox(this);
	connect(m_word_length_box, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &NewGameDialog::lengthSelected);

	// Create pattern buttons
	QFrame* patterns = new QFrame(this);
	QGridLayout* patterns_layout = new QGridLayout(patterns);
	patterns->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	for (int i = 0; i < Pattern::types(); ++i) {
		Pattern* pattern = Pattern::create(m_wordlist, i);
		m_patterns.append(pattern);

		QToolButton* pattern_button = new QToolButton(patterns);
		pattern_button->setAutoRaise(false);
		pattern_button->setText(pattern->name());
		pattern_button->setIconSize(QSize(80,80));
		pattern_button->setIcon(QIcon(QString(":/patterns/%1.png").arg(i)));
		pattern_button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		pattern_button->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
		connect(pattern_button, &QToolButton::clicked, this, &NewGameDialog::patternSelected);

		patterns_layout->addWidget(pattern_button, i / 3, i % 3);
		m_pattern_buttons.append(pattern_button);
	}

	// Create cancel button
	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Cancel, Qt::Horizontal, this);
	connect(buttons, &QDialogButtonBox::rejected, this, &NewGameDialog::reject);

	// Lay out dialog
	QFormLayout* contents_layout = new QFormLayout;
	contents_layout->setFormAlignment(Qt::AlignHCenter | Qt::AlignTop);
	contents_layout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
	contents_layout->addRow(tr("Language:"), m_languages_box);
	contents_layout->addRow(tr("Amount of Words:"), m_word_count_box);
	contents_layout->addRow(tr("Word Length:"), m_word_length_box);

	QVBoxLayout* layout = new QVBoxLayout(this);
	const int margin = layout->contentsMargins().top();
	layout->addLayout(contents_layout);
	layout->addSpacing(margin);
	layout->addWidget(patterns);
	layout->addSpacing(margin);
	layout->addWidget(buttons);

	// Load settings
	QSettings settings;
	setLanguage(settings.value("NewGame/Language", WordList::defaultLanguage()).toString());
	setCount(settings.value("NewGame/Count", 1).toInt());
	setLength(settings.value("NewGame/Length", 7).toInt());
	m_pattern_buttons.at(settings.value("NewGame/Pattern").toInt())->setFocus();

	connect(m_languages_box, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &NewGameDialog::languageSelected);
}

//-----------------------------------------------------------------------------

NewGameDialog::~NewGameDialog() {
	qDeleteAll(m_patterns);
}

//-----------------------------------------------------------------------------

void NewGameDialog::keyPressEvent(QKeyEvent* event) {
	if ((event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return)) {
		for (QToolButton* button : m_pattern_buttons) {
			if (button->hasFocus() && button->isEnabled()) {
				button->click();
				break;
			}
		}
	} else {
		QDialog::keyPressEvent(event);
	}
}

//-----------------------------------------------------------------------------

void NewGameDialog::languageSelected(int index) {
	m_wordlist->setLanguage(m_languages_box->itemData(index).toString());

	int length = m_word_length_box->itemData(m_word_length_box->currentIndex()).toInt();
	m_word_length_box->clear();
	for (int i = 5; i <= m_wordlist->maximumLength(); ++i) {
		m_word_length_box->addItem(tr("%n letter(s)", "", i), i);
	}
	setLength(length);
}

//-----------------------------------------------------------------------------

void NewGameDialog::lengthSelected(int index) {
	int length = m_word_length_box->itemData(index).toInt();
	for (int i = 0; i < m_pattern_buttons.count(); ++i) {
		m_pattern_buttons.at(i)->setEnabled(length >= m_patterns.at(i)->minimumLength());
	}
}

//-----------------------------------------------------------------------------

void NewGameDialog::patternSelected() {
	int pattern = -1;
	for (int i = 0; i < m_pattern_buttons.count(); ++i) {
		if (m_pattern_buttons.at(i) == sender()) {
			pattern = i;
			break;
		}
	}
	if (pattern == -1) {
		return;
	}

	QString language = m_languages_box->itemData(m_languages_box->currentIndex()).toString();
	int count = m_word_count_box->currentIndex();
	int length = m_word_length_box->itemData(m_word_length_box->currentIndex()).toInt();
#ifndef Q_OS_WIN
	std::random_device rd;
	unsigned int seed = rd();
#else
	std::mt19937 gen(time(0));
	std::uniform_int_distribution<unsigned int> dist;
	unsigned int seed = dist(gen);
#endif

	QSettings settings;
	settings.remove("Current");
	settings.setValue("NewGame/Language", language);
	settings.setValue("NewGame/Pattern", pattern);
	settings.setValue("NewGame/Count", count);
	settings.setValue("NewGame/Length", length);
	settings.setValue("Current/Version", 4);
	settings.setValue("Current/Language", language);
	settings.setValue("Current/Pattern", pattern);
	settings.setValue("Current/Count", count);
	settings.setValue("Current/Length", length);
	settings.setValue("Current/Seed", seed);
	settings.setValue("Current/Time", 0);

	m_board->openGame();

	accept();
}

//-----------------------------------------------------------------------------

void NewGameDialog::setLanguage(const QString& language) {
	int id = m_languages_box->findData(language);
	if (id == -1) {
		id = 0;
	} else if (id == 0) {
		m_languages_box->setCurrentIndex(1);
	}
	m_languages_box->setCurrentIndex(id);
	languageSelected(id);
}

//-----------------------------------------------------------------------------

void NewGameDialog::setCount(int count) {
	count = qBound(0, count, 3);
	m_word_count_box->setCurrentIndex(count);
}

//-----------------------------------------------------------------------------

void NewGameDialog::setLength(int length) {
	int min = m_word_length_box->itemData(0).toInt();
	int max = m_word_length_box->itemData(m_word_length_box->count() - 1).toInt();
	m_word_length_box->setCurrentIndex(qBound(min, length, max) - min);
	lengthSelected(m_word_length_box->currentIndex());
}
