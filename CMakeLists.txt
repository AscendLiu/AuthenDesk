cmake_minimum_required(VERSION 3.16)
project(AuthenDesk VERSION 1.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Quick QuickControls2 Network Sql DBus)

find_package(OpenSSL REQUIRED)

qt_add_executable(AuthenDesk
    src/main.cpp
    src/totp.cpp
    src/totp.h
    src/tokendata.cpp
    src/tokendata.h
    src/tokenmanager.cpp
    src/tokenmanager.h
    src/result.h
    src/localemanager.cpp
    src/localemanager.h
    src/nativefiledialog.cpp
    src/nativefiledialog.h
    src/importparser.cpp
    src/importparser.h
    src/serviceiconprovider.cpp
    src/serviceiconprovider.h
    src/qrdecoder.cpp
    src/qrdecoder.h
    src/models/tokenlistmodel.cpp
    src/models/tokenlistmodel.h
    qml.qrc
)

file(GLOB_RECURSE ICON_PNGS "${CMAKE_SOURCE_DIR}/assets/icons/*.png")
foreach(ICON IN LISTS ICON_PNGS)
    get_filename_component(ICON_NAME "${ICON}" NAME)
    set_source_files_properties("${ICON}" PROPERTIES QT_RESOURCE_ALIAS "icons/${ICON_NAME}")
endforeach()
qt_add_resources(AuthenDesk "icons_qrc"
    PREFIX "/"
    FILES ${ICON_PNGS}
)

target_include_directories(AuthenDesk PRIVATE src)

target_link_libraries(AuthenDesk PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Network
    Qt6::Sql
    Qt6::DBus
    OpenSSL::SSL
    OpenSSL::Crypto
)

set_target_properties(AuthenDesk PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
