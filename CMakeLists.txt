# SPDX-FileCopyrightText: 2021-2025 Graeme Gott <graeme@gottcode.org>
#
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.16)

# Configure project
project(connectagram VERSION 1.3.8 LANGUAGES CXX)

set(project_copyright "2009-2025 Graeme Gott")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui LinguistTools Widgets)
include(GNUInstallDirs)

add_compile_definitions(
	QT_NO_KEYWORDS
	$<$<CONFIG:DEBUG>:QT_STRICT_ITERATORS>
	$<$<CONFIG:DEBUG>:QT_NO_NARROWING_CONVERSIONS_IN_CONNECT>
	$<$<CONFIG:DEBUG>:QT_DISABLE_DEPRECATED_BEFORE=0x060900>
)

# Version number
include(cmake/AddVersionCompileDefinition.cmake)
add_version_compile_definition(src/main.cpp VERSIONSTR)

# Create program
qt_add_executable(connectagram
	# Headers
	src/board.h
	src/cell.h
	src/clock.h
	src/definitions.h
	src/letter.h
	src/locale_dialog.h
	src/new_game_dialog.h
	src/pattern.h
	src/scores_dialog.h
	src/view.h
	src/window.h
	src/word.h
	src/wordlist.h
	# Sources
	src/board.cpp
	src/cell.cpp
	src/clock.cpp
	src/definitions.cpp
	src/letter.cpp
	src/locale_dialog.cpp
	src/main.cpp
	src/new_game_dialog.cpp
	src/pattern.cpp
	src/scores_dialog.cpp
	src/view.cpp
	src/window.cpp
	src/word.cpp
	src/wordlist.cpp
	# Resources
	icons/icons.qrc
	${translations_QM}
)

target_link_libraries(connectagram PRIVATE
	Qt6::Core
	Qt6::Gui
	Qt6::Widgets
)

# Create translations
file(GLOB translations_SRCS translations/*.ts)
qt_add_translations(connectagram
	TS_FILES ${translations_SRCS}
	QM_FILES_OUTPUT_VARIABLE translations_QM
	LUPDATE_OPTIONS -no-obsolete -locations none
)

# Create word lists
add_subdirectory(tools)

file(GLOB languages wordlists/*)
foreach(lang_file ${languages})
	get_filename_component(lang ${lang_file} NAME)
	set(lang_source ${CMAKE_SOURCE_DIR}/gamedata/${lang})

	# Create wordlist for ${lang}
	set(wordlist_file ${lang_source}/words)
	add_custom_command(
		OUTPUT ${wordlist_file}
		COMMAND "$<SHELL_PATH:$<TARGET_FILE:wordlist>>" -l ${lang} -o "$<SHELL_PATH:${wordlist_file}>" "$<SHELL_PATH:${lang_file}>"
		DEPENDS ${lang_file} ${CMAKE_SOURCE_DIR}/tools/wordlist.cpp
		WORKING_DIRECTORY ${CMAKE_PREFIX_PATH}/bin # needed for Windows to find Qt
		VERBATIM
	)

	# Add ${lang} as dependency of target
	set(data_files ${wordlist_file} ${lang_source}/language.ini)
	if(EXISTS ${lang_source}/README)
		list(APPEND data_files ${lang_source}/README)
	endif()
	target_sources(connectagram PRIVATE ${data_files})

	# Install data for ${lang}
	if(APPLE)
		set_source_files_properties(
			${data_files}
			PROPERTIES
			MACOSX_PACKAGE_LOCATION Resources/gamedata/${lang}
		)
	elseif(NOT WIN32)
		install(FILES ${data_files} DESTINATION ${CMAKE_INSTALL_DATADIR}/connectagram/gamedata/${lang})
	endif()
endforeach()

# Optimize build
option(ENABLE_LINK_TIME_OPTIMIZATION "Enable link time optimization" OFF)
if(ENABLE_LINK_TIME_OPTIMIZATION)
	include(CheckIPOSupported)
	check_ipo_supported(RESULT result)
	if(result)
		set_target_properties(connectagram PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
	endif()
endif()

option(ENABLE_STRIP "Enable automatic stripping of builds" OFF)
if(ENABLE_STRIP)
	add_custom_command(TARGET connectagram
		POST_BUILD
		COMMAND ${CMAKE_STRIP} $<TARGET_FILE:connectagram>
	)
endif()

# Install
if(APPLE)
	set(datadir "../Resources")

	set_target_properties(connectagram PROPERTIES
		OUTPUT_NAME Connectagram
		MACOSX_BUNDLE TRUE
		MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/mac/Info.plist.in
	)

	include(cmake/BundleResources.cmake)
	bundle_data(connectagram ${CMAKE_SOURCE_DIR}/icons/connectagram.icns Resources)
	bundle_translations(connectagram "${translations_QM}")
elseif(WIN32)
	set(datadir ".")

	# Use Qt6 macro until CMake provides something
	# https://bugreports.qt.io/browse/QTBUG-87618
	set_target_properties(connectagram PROPERTIES
		OUTPUT_NAME Connectagram
		WIN32_EXECUTABLE TRUE
		QT_TARGET_VERSION "${PROJECT_VERSION}"
		QT_TARGET_COMPANY_NAME "Graeme Gott"
		QT_TARGET_DESCRIPTION "Anagram game"
		QT_TARGET_COPYRIGHT "\\xA9 ${project_copyright}"
		QT_TARGET_PRODUCT_NAME "Connectagram"
		QT_TARGET_RC_ICONS ${CMAKE_SOURCE_DIR}/icons/connectagram.ico
	)
	_qt_internal_generate_win32_rc_file(connectagram)
else()
	file(RELATIVE_PATH datadir ${CMAKE_INSTALL_FULL_BINDIR} ${CMAKE_INSTALL_FULL_DATADIR}/connectagram)

	target_sources(connectagram PRIVATE icons/icon.qrc)

	install(TARGETS connectagram RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
	install(FILES ${translations_QM} DESTINATION ${CMAKE_INSTALL_DATADIR}/connectagram/translations)
	install(FILES doc/connectagram.6 DESTINATION ${CMAKE_INSTALL_MANDIR}/man6 COMPONENT doc)
	install(DIRECTORY icons/hicolor DESTINATION ${CMAKE_INSTALL_DATADIR}/icons)

	include(cmake/TranslateMetainfo.cmake)
	process_and_install_metainfo(PO_DIR ${CMAKE_SOURCE_DIR}/icons/po)
endif()

set_property(SOURCE src/main.cpp APPEND PROPERTY COMPILE_DEFINITIONS CONNECTAGRAM_DATADIR="${datadir}")
