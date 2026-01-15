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

/**
 * @brief The WordList class contains the list of words used by the game board.
 */
class WordList : public QObject
{
	Q_OBJECT

public:
	/**
	 * Constructs a word list instance.
	 * @param parent the QObject that manages the word list
	 */
	explicit WordList(QObject* parent = nullptr);

	/**
	 * @return are there words in the word list
	 */
	bool isEmpty() const
	{
		return m_data->isEmpty();
	}

	/**
	 * @return the language the word list is for
	 */
	QString language() const
	{
		return m_langcode;
	}

	/**
	 * @return the longest length of a word in the word list
	 */
	int maximumLength() const
	{
		return m_data->maximumLength();
	}

	/**
	 * Fetches a subset of the word list that matches the letters.
	 * @param known_letters the letters to filter by as a regex
	 * @return the subset of the word list matching the letters
	 */
	QStringList filter(const QString& known_letters) const;

	/**
	 * Fetches all of the spelling variations of a word.
	 * @param word the word to find spellings of
	 * @return the list of spellings for a word
	 */
	QStringList spellings(const QString& word) const
	{
		return m_data->spellings(word);
	}

	/**
	 * Adds a word and its anagrams to ignore in #filter() to prevent finding mulitple instances
	 * of the same anagram when filtering the list.
	 * @param word the letters to ignore when filtering the list
	 */
	void addAnagramFilter(const QString& word);

	/**
	 * Clears the list of ignored words.
	 */
	void resetAnagramFilters();

	/**
	 * Sets the language of the word list. This will load the list from disk.
	 * @param langcode the language to use
	 */
	void setLanguage(const QString& langcode);

	/**
	 * Sets the word list to be a subset of the full list of words of a language where only words
	 * of the specified length are included.
	 * @param length the length of words in the word list
	 */
	void setLength(int length);

	/**
	 * @return the language matching the player's locale if possible, falls back to English
	 */
	static QString defaultLanguage();

	/**
	 * Fetches the translated name of a language.
	 * @param language the language to look up
	 * @return the translated name of the language
	 */
	static QString languageName(const QString& language);

Q_SIGNALS:
	/**
	 * Emitted after the word list has loaded a new set of words.
	 * @param language the language used by the word list
	 */
	void languageChanged(const QString& language);

private:
	/**
	 * Load the subset of the full word list matching the current length.
	 */
	void resetWords();

private:
	/**
	 * @brief The WordList::WordListData class contains the full set of words for a language.
	 */
	class WordListData
	{
	public:
		/**
		 * Constructs a word list data instance.
		 * @param language the language the word list is for
		 */
		explicit WordListData(const QString& language = QString());

		/**
		 * @return is the word list empty
		 */
		bool isEmpty() const
		{
			return m_all_words.isEmpty();
		}

		/**
		 * @return the longest length of a word in the word list
		 */
		int maximumLength() const
		{
			return m_maximum_length;
		}

		/**
		 * Fetches all of the spelling variations of a word.
		 * @param word the word to find spellings of
		 * @return the list of spellings for a word
		 */
		QStringList spellings(const QString& word) const
		{
			return m_spellings.value(word, QStringList(word.toLower()));
		}

		/**
		 * Fetches a subset of the word list.
		 * @param length the length of the words in the subset
		 * @return a subset of the word list
		 */
		QStringList words(int length) const
		{
			return m_all_words.value(length);
		}

	private:
		QHash<int, QStringList> m_all_words; /**< the words in the word list grouped by length */
		QHash<QString, QStringList> m_spellings; /**< the proper spellings of each word */
		int m_maximum_length; /**< the length of the longest word */
	};

private:
	QString m_langcode; /**< the language of the word list */
	QSharedPointer<WordListData> m_data; /**< the full word list */
	QStringList m_words; /**< the current subset of the full word list */
	QStringList m_anagram_filters; /**< list of words to ignore when filtering */
	int m_length; /**< length of words in current subset */
};

#endif // CONNECTAGRAM_WORD_LIST_H
