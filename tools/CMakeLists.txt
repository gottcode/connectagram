# SPDX-FileCopyrightText: 2021 Graeme Gott <graeme@gottcode.org>
#
# SPDX-License-Identifier: GPL-3.0-or-later

# Program to generate wordlist
qt_add_executable(wordlist wordlist.cpp)
target_link_libraries(wordlist PRIVATE Qt6::Core)
