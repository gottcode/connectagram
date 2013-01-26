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
#include <QSettings>
#include <QSpinBox>
#include <QVBoxLayout>

#include <ctime>

NewGameDialog::NewGameDialog(Board* board, QWidget* parent)
: QDialog(parent), m_board(board) {
	setWindowTitle(tr("New Game"));

	// Create languages box
	m_wordlist = new WordList(this);
	m_wordlist->setLanguage(WordList::defaultLanguage());
	m_languages_box = new QComboBox(this);
	QStringList languages = QDir("connectagram:").entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	foreach (const QString& language, languages) {
		m_languages_box->addItem(LocaleDialog::languageName(language), language);
	}
	connect(m_languages_box, SIGNAL(currentIndexChanged(int)), this, SLOT(languageSelected(int)));

	// Create patterns box
	m_patterns_box = new QComboBox(this);
	for (int i = 0; i < Pattern::types(); ++i) {
		Pattern* pattern = Pattern::create(m_wordlist, i);
		m_patterns.append(pattern);
		m_patterns_box->addItem(pattern->name(), i);
	}
	connect(m_patterns_box, SIGNAL(currentIndexChanged(int)), this, SLOT(patternSelected(int)));

	// Create word length box
	m_word_length_box = new QComboBox(this);

	// Create word count box
	m_word_count_box = new QComboBox(this);
	m_word_count_box->addItems(QStringList() << tr("Low") << tr("Medium") << tr("High") << tr("Very High"));

	// Create seed box
	m_seed_box = new QSpinBox(this);
	m_seed_box->setRange(0, 2147483647);
	m_seed_box->setSpecialValueText(tr("Random"));

	// Create buttons
	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
	connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

	// Lay out dialog
	QFormLayout* contents_layout = new QFormLayout;
	contents_layout->addRow(tr("Language:"), m_languages_box);
	contents_layout->addRow(tr("Pattern:"), m_patterns_box);
	contents_layout->addRow(tr("Word Length:"), m_word_length_box);
	contents_layout->addRow(tr("Amount of Words:"), m_word_count_box);
	contents_layout->addRow(tr("Game number:"), m_seed_box);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addLayout(contents_layout);
	layout->addSpacing(18);
	layout->addWidget(buttons);

	// Load settings
	QSettings settings;
	setLanguage(settings.value("NewGame/Language", WordList::defaultLanguage()).toString());
	setPattern(settings.value("NewGame/Pattern", 1).toInt());
	setCount(settings.value("NewGame/Count", 1).toInt());
	setLength(settings.value("NewGame/Length", 7).toInt());
}

//-----------------------------------------------------------------------------

NewGameDialog::~NewGameDialog() {
	qDeleteAll(m_patterns);
}

//-----------------------------------------------------------------------------

void NewGameDialog::accept() {
	QString language = m_languages_box->itemData(m_languages_box->currentIndex()).toString();
	int id = m_patterns_box->itemData(m_patterns_box->currentIndex()).toInt();
	int count = m_word_count_box->currentIndex();
	int length = m_word_length_box->itemData(m_word_length_box->currentIndex()).toInt();
	int seed = m_seed_box->value();
	if (seed == 0) {
		srand(time(0));
		seed = rand();
	}

	QSettings settings;
	settings.remove("Current");
	settings.setValue("NewGame/Language", language);
	settings.setValue("NewGame/Pattern", id);
	settings.setValue("NewGame/Count", count);
	settings.setValue("NewGame/Length", length);
	settings.setValue("Current/Version", 3);
	settings.setValue("Current/Language", language);
	settings.setValue("Current/Pattern", id);
	settings.setValue("Current/Count", count);
	settings.setValue("Current/Length", length);
	settings.setValue("Current/Seed", seed);
	settings.setValue("Current/Time", 0);

	m_board->openGame();

	QDialog::accept();
}

//-----------------------------------------------------------------------------

void NewGameDialog::languageSelected(int index) {
	m_wordlist->setLanguage(m_languages_box->itemData(index).toString());
	patternSelected(m_patterns_box->currentIndex());
}

//-----------------------------------------------------------------------------

void NewGameDialog::patternSelected(int index) {
	Pattern* pattern = m_patterns.at(m_patterns_box->itemData(index).toInt());

	// Set up word length combobox
	int length = m_word_length_box->itemData(m_word_length_box->currentIndex()).toInt();
	m_word_length_box->clear();
	for (int i = pattern->minimumLength(); i <= pattern->maximumLength(); ++i) {
		m_word_length_box->addItem(tr("%n letter(s)", "", i), i);
	}
	setLength(length);
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
}

//-----------------------------------------------------------------------------

void NewGameDialog::setPattern(int pattern) {
	int id = m_patterns_box->findData(pattern);
	if (id == -1) {
		id = 0;
	} else if (id == 0) {
		m_patterns_box->setCurrentIndex(1);
	}
	m_patterns_box->setCurrentIndex(id);
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
}
