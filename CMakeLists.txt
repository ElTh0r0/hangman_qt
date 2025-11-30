# SPDX-FileCopyrightText: 2008-2025 Thorsten Roth
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.16)
project(Hangman
    VERSION 4.2.0
    DESCRIPTION "Hangman game framework written in C++ and Qt 6."
    LANGUAGES C CXX
)
set(PROJECT_COPYRIGHT "(C) 2008-present Thorsten Roth")

set(QT_MAIN_VERSION Qt6)
set(QT_MINIMUM_VERSION 6.0)

find_package(${QT_MAIN_VERSION} ${QT_MINIMUM_VERSION}
    REQUIRED Core Gui Widgets LinguistTools
)

option(GENERATE_TS "Regenerate translation source files" OFF)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(LINUX ON)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(WINDOWS ON)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(MACOS ON)
endif()
    
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(WINDOWS)
    set(app_icon_resource_windows "${CMAKE_CURRENT_SOURCE_DIR}/data/win/hangman_win.rc")
    qt_add_executable(hangman ${app_icon_resource_windows})
    set_target_properties(hangman PROPERTIES
        OUTPUT_NAME Hangman
        WIN32_EXECUTABLE TRUE
    )
elseif(MACOS)
    set(app_icon_macos "${CMAKE_CURRENT_SOURCE_DIR}/data/mac/icon.icns")
    set_source_files_properties(${app_icon_macos} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    target_sources(Hangman PRIVATE data/mac/icon.icns)
    set_target_properties(Hangman PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_ICON_FILE "icon.icns"
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/data/mac/Info.plist"
    )
    qt_add_executable(hangman MACOSX_BUNDLE ${app_icon_macos})
else()
    qt_add_executable(hangman)
endif()

target_compile_definitions(hangman
    PRIVATE
        APP_NAME="${PROJECT_NAME}"
        APP_VERSION="${PROJECT_VERSION}"
        APP_COPY="${PROJECT_COPYRIGHT}"
        APP_DESC="${PROJECT_DESCRIPTION}"
        QT_NO_FOREACH
        $<$<CONFIG:DEBUG>:QT_DISABLE_DEPRECATED_BEFORE=0x061000>
)

if(MSVC)
    target_compile_options(hangman
        PRIVATE
            $<$<CONFIG:DEBUG>:/W4 /permissive->
    )
else()
    target_compile_options(hangman
        PRIVATE
            $<$<CONFIG:DEBUG>:-Wall -Wextra -pedantic>
    )
endif()

target_link_libraries(hangman PRIVATE
    ${QT_MAIN_VERSION}::Core
    ${QT_MAIN_VERSION}::Gui
    ${QT_MAIN_VERSION}::Widgets
)

set(HEADERS
    hangman.h
    game.h
)

set(SOURCES
    main.cpp
    hangman.cpp
    game.cpp
)

set(FORMS
    hangman.ui
)

set(RESOURCES
    data/hangman_resources.qrc
)

set(TRANSLATIONS
    lang/hangman_de.ts
    lang/hangman_en.ts
    lang/hangman_it.ts
    lang/hangman_nl.ts
)

if (GENERATE_TS)
    qt6_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TRANSLATIONS} OPTIONS -no-obsolete)
else ()
    qt6_add_translation(QM_FILES ${TRANSLATIONS})
endif ()
add_custom_target(translations_${PROJECT_NAME} ALL DEPENDS ${QM_FILES})
foreach(qm_file ${QM_FILES})
    get_filename_component(qm_filename "${qm_file}" NAME)
    set_source_files_properties(
        "${qm_file}"
        PROPERTIES QT_RESOURCE_ALIAS "${qm_filename}"
    )
endforeach()
qt_add_resources(hangman translations_hang
    PREFIX "/"
    FILES ${QM_FILES}
)

target_sources(hangman PRIVATE ${HEADERS} ${SOURCES} ${FORMS} ${RESOURCES})

if(LINUX)
    include(GNUInstallDirs)
    install(TARGETS hangman DESTINATION ${CMAKE_INSTALL_BINDIR})
    install(FILES data/unix/com.github.elth0r0.hangman.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
    install(FILES data/unix/com.github.elth0r0.hangman.metainfo.xml DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)
    install(FILES data/hangman.png DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps)
    install(FILES data/unix/hangman.xpm DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps)
endif()
