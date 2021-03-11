/*
	SPDX-FileCopyrightText: 2013 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef CONNECTAGRAM_WORD_LIST_H
#define CONNECTAGRAM_WORD_LIST_H

#include <QHash>
#include <QSharedPointer>
#include <QStringList>
#include <QObject>

class WordList : public QObject
{
	Q_OBJECT

public:
	explicit WordList(QObject* parent = nullptr);

	bool isEmpty() const
	{
		return m_data->isEmpty();
	}

	QString language() const
	{
		return m_langcode;
	}

	int maximumLength() const
	{
		return m_data->maximumLength();
	}

	QStringList filter(const QString& known_letters) const;

	QStringList spellings(const QString& word) const
	{
		return m_data->spellings(word);
	}

	void addAnagramFilter(const QString& word);
	void resetAnagramFilters();
	void setLanguage(const QString& langcode);
	void setLength(int length);

	static QString defaultLanguage();

signals:
	void languageChanged(const QString& language);

private:
	void resetWords();

private:
	class WordListData
	{
	public:
		explicit WordListData(const QString& language = QString());

		bool isEmpty() const
		{
			return m_all_words.isEmpty();
		}

		int maximumLength() const
		{
			return m_maximum_length;
		}

		QStringList spellings(const QString& word) const
		{
			return m_spellings.value(word, QStringList(word.toLower()));
		}

		QStringList words(int length) const
		{
			return m_all_words.value(length);
		}

	private:
		QHash<int, QStringList> m_all_words;
		QHash<QString, QStringList> m_spellings;
		int m_maximum_length;
	};

private:
	QString m_langcode;
	QSharedPointer<WordListData> m_data;
	QStringList m_words;
	QStringList m_anagram_filters;
	int m_length;
};

#endif // CONNECTAGRAM_WORD_LIST_H
