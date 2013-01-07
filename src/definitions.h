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

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <QDialog>
#include <QHash>
class QListWidget;
class QListWidgetItem;
class QSplitter;
class QTextBrowser;
class QUrl;
class Dictionary;

class Definitions : public QDialog {
	Q_OBJECT

	public:
		Definitions(QWidget* parent = 0);
		~Definitions();

	public slots:
		void clear();
		void addWord(const QString& word);
		void solveWord(const QString& original_word, const QString& current_word);
		void selectWord(const QString& word = QString());

	protected:
		virtual void hideEvent(QHideEvent* event);

	private slots:
		void anchorClicked(const QUrl& link);
		void wordSelected(QListWidgetItem* item);
		void wordDefined(const QString& word, const QString& definition);

	private:
		QSplitter* m_contents;
		QListWidget* m_words;
		QTextBrowser* m_text;
		Dictionary* m_dictionary;
		QHash<QString, QListWidgetItem*> m_word_table;
};

#endif
