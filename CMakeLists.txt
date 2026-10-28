cmake_minimum_required(VERSION 3.16)

# ============================================================
# Configurazione vcpkg per MinGW
# ============================================================
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
set(VCPKG_TARGET_TRIPLET "x64-mingw-static" CACHE STRING "Vcpkg target triplet")

project(ThunderPass VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)

# Trova libsodium installato con vcpkg
find_package(unofficial-sodium CONFIG REQUIRED)

set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui
    src/registerdialog.cpp
    src/registerdialog.h
    src/registerdialog.ui
    src/crypto.cpp
    src/crypto.h
    src/authmanager.cpp
    src/authmanager.h
    resources/resources.qrc
)

qt_add_executable(ThunderPass
    ${PROJECT_SOURCES}
    src/crypto.h src/crypto.cpp
    src/authmanager.h src/authmanager.cpp
    src/vaultwindow.h src/vaultwindow.cpp
    src/vaultwindow.ui

)

target_link_libraries(ThunderPass PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    unofficial-sodium::sodium
)

set_target_properties(ThunderPass PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)
