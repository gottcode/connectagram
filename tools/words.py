#!/usr/bin/env python3
#
# Copyright (C) 2013 Graeme Gott <graeme@gottcode.org>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

"""words.py: Create Connectagram word list from a list of words."""


import argparse
from collections import defaultdict
import re


def replacement_map(language):
    """Return a dict of replacement strings for language."""
    if language == 'he':
        return {ord('ך'): 'כ',
                ord('ם'): 'מ',
                ord('ן'): 'נ',
                ord('ף'): 'פ',
                ord('ץ'): 'צ'}
    else:
        return {}


def skip_regex(language):
    """Return a regex that finds characters not allowed in words."""
    if language == 'en':
        return re.compile("[A-Z0-9'\-]")
    elif language == 'fr':
        return re.compile("[ÀÂÆÉÈÊËÎÏÔŒÙÛÜŸÇA-Z0-9'\-]")
    else:
        return re.compile('\d')


def main():
    parser = argparse.ArgumentParser(
            description='Create Connectagram word list from a list of words')
    parser.add_argument('FILE',
            help='file containing list of words')
    parser.add_argument('--language',
            help='specify language', type=str,
            choices=['en', 'fr', 'he', 'ro', 'ru', 'tr'])
    parser.add_argument('-o', '--out', type=str,
            help='place words in file OUT instead of default file')
    args = parser.parse_args()

    # Read words and replace letters
    words = defaultdict(list)
    with open(args.FILE, encoding='utf-8', mode='r') as f:
        replacements = replacement_map(args.language)
        skip = skip_regex(args.language)
        for word in f:
            word = word.strip()
            if len(word) < 5:
                continue
            if bool(skip.search(word)) == True:
                continue
            key = word.upper().translate(replacements)
            if not word in words[key]:
                words[key].append(word)

    # Join words into lines
    lines = []
    for solution, spellings in words.items():
        if (len(spellings) == 1) and (spellings[0] == solution.lower()):
            # Doesn't need to store proper spellings in this case;
            # Connectagram will fall back to all lowercase of solution.
            lines.append(solution + '\n')
        else:
            # Store solution and proper spellings.
            lines.append(solution + ' ' + ' '.join(spellings) + '\n')
    lines.sort()

    # Save lines to disk
    outf = 'words'
    if args.out:
        outf = args.out
    with open(outf, encoding='utf-8', mode='w') as f:
        for line in lines:
            f.write(line)
    print("Words generated and placed in file '{0}'".format(outf))


if __name__ == '__main__':
    main()
