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

#include "definitions.h"

#include "dictionary.h"

#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QListWidget>
#include <QSettings>
#include <QSplitter>
#include <QTextBrowser>
#include <QVBoxLayout>

Definitions::Definitions(QWidget* parent)
: QDialog(parent) {
	QSettings settings;
	setWindowTitle(tr("Definitions"));
	setModal(true);

	m_dictionary = new Dictionary(this);
	connect(m_dictionary, SIGNAL(wordDefined(QString, QString)), this, SLOT(wordDefined(QString, QString)));

	m_contents = new QSplitter(this);

	m_words = new QListWidget(m_contents);
	connect(m_words, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this, SLOT(wordSelected(QListWidgetItem*)));
	m_contents->addWidget(m_words);
	m_contents->setStretchFactor(0, 0);
	m_contents->setSizes(QList<int>() << settings.value("Definitions/Splitter", m_words->fontMetrics().averageCharWidth() * 12).toInt());

	m_text = new QTextBrowser(m_contents);
	m_text->setReadOnly(true);
	m_text->setOpenLinks(false);
	connect(m_text, SIGNAL(anchorClicked(QUrl)), this, SLOT(anchorClicked(QUrl)));
	m_contents->addWidget(m_text);
	m_contents->setStretchFactor(1, 1);

	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
	connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(m_contents);
	layout->addWidget(buttons);

	resize(settings.value("Definitions/Size", QSize(500, 400)).toSize());
}

//-----------------------------------------------------------------------------

Definitions::~Definitions() {
	m_dictionary->wait();
}

//-----------------------------------------------------------------------------

void Definitions::clear() {
	m_text->clear();
	m_word_table.clear();
	m_words->clear();
}

//-----------------------------------------------------------------------------

void Definitions::addWord(const QString& word) {
	QListWidgetItem* item = new QListWidgetItem(QString(word.length(), QChar('?')), m_words);
	m_word_table.insert(word, item);
	m_words->clearSelection();
	m_words->setCurrentRow(0);
}

//-----------------------------------------------------------------------------

void Definitions::solveWord(const QString& original_word, const QString& current_word) {
	QListWidgetItem* item = m_word_table.value(original_word);
	if (item == 0) {
		return;
	}
	if (original_word != current_word) {
		m_word_table.remove(original_word);
		m_word_table.insert(current_word, item);
	}
	item->setText(current_word);
	m_words->sortItems();
	if (item == m_words->currentItem()) {
		wordSelected(item);
	}
}

//-----------------------------------------------------------------------------

void Definitions::selectWord(const QString& word) {
	QListWidgetItem* item = m_word_table.value(word);
	if (item == 0) {
		item = m_words->item(0);
	}
	m_words->setCurrentItem(item);
	show();
}

//-----------------------------------------------------------------------------

void Definitions::hideEvent(QHideEvent* event) {
	QSettings settings;
	settings.setValue("Definitions/Size", size());
	settings.setValue("Definitions/Splitter", m_contents->sizes().first());
	QDialog::hideEvent(event);
}

//-----------------------------------------------------------------------------

void Definitions::anchorClicked(const QUrl& link) {
	QDesktopServices::openUrl(m_dictionary->url().resolved(link));
}

//-----------------------------------------------------------------------------

void Definitions::wordSelected(QListWidgetItem* item) {
	if (item == 0) {
		m_text->clear();
		return;
	}

	if (item->text().at(0) != QChar('?')) {
		QString definition = item->data(Qt::UserRole).toString();
		if (definition.isEmpty()) {
			definition = QString("<font color=\"#555\">%1</font>").arg(tr("Downloading definition..."));
			item->setData(Qt::UserRole, definition);
			m_dictionary->lookup(item->text());
		}
		m_text->setHtml(definition);
	} else {
		m_text->setText(tr("Unsolved word"));
	}
}

//-----------------------------------------------------------------------------

void Definitions::wordDefined(const QString& word, const QString& definition) {
	QListWidgetItem* item = m_word_table.value(word);
	if (item == 0) {
		return;
	}
	item->setData(Qt::UserRole, definition);
	if (item == m_words->currentItem()) {
		m_text->setHtml(definition);
	}
}
