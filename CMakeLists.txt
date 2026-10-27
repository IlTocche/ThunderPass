cmake_minimum_required(VERSION 3.16)

project(ThunderPass VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Trova Qt6 (Widgets + Network per Firebase più avanti)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)

# File sorgenti
set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui
    resources/resources.qrc
)

# Crea l'eseguibile
qt_add_executable(ThunderPass
    ${PROJECT_SOURCES}
    src/registerdialog.h src/registerdialog.cpp
    src/registerdialog.ui
)

# Collega le librerie Qt
target_link_libraries(ThunderPass PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
)

# Impostazioni specifiche per macOS / Windows / Linux
set_target_properties(ThunderPass PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Installazione (opzionale)
include(GNUInstallDirs)
install(TARGETS ThunderPass
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
