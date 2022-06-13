# SPDX-FileCopyrightText: 2021 Graeme Gott <graeme@gottcode.org>
#
# SPDX-License-Identifier: GPL-3.0-or-later

# Program to generate wordlist
set(wordlist_SRCS ../tools/wordlist/main.cpp)
qt6_add_executable(wordlist ${wordlist_SRCS})
target_link_libraries(wordlist PRIVATE Qt6::Core)

# Create wordlists and dice
foreach(lang ca cs de el es en fr he hu id nl pl pt pt_BR ru)
	set(lang_file ${CMAKE_CURRENT_SOURCE_DIR}/${lang})
	set(lang_source ${CMAKE_SOURCE_DIR}/data/${lang})
	set(lang_dest ${CMAKE_INSTALL_DATADIR}/connectagram/data/${lang}/)

	# Create wordlist for ${lang}
	set(wordlist_file ${lang_source}/words)
	list(APPEND wordlist_files ${wordlist_file})
	add_custom_command(
		OUTPUT ${wordlist_file}
		COMMAND wordlist -l ${lang} -o "$<SHELL_PATH:${wordlist_file}>" "$<SHELL_PATH:${lang_file}>"
		DEPENDS ${lang_file} ${wordlist_SRCS}
		WORKING_DIRECTORY ${CMAKE_PREFIX_PATH}/bin # needed for Windows to find Qt
		VERBATIM
	)

	# Install data for ${lang}
	if(UNIX AND NOT APPLE)
		install(FILES ${wordlist_file} ${lang_source}/language.ini DESTINATION ${lang_dest})
		if(EXISTS ${lang_source}/README)
			install(FILES ${lang_source}/README DESTINATION ${lang_dest})
		endif()
	endif()
endforeach()

add_custom_target(data DEPENDS ${wordlist_files})
add_dependencies(connectagram data)
