cmake_minimum_required(VERSION 3.5)

project(kiran-calculator)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets LinguistTools  REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)

#翻译文件
set(TS_FILES "translations/kiran-calculator.zh_CN.ts")
qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

file(GLOB_RECURSE SRC "src/*.cpp" "src/*.c" "src/*.h" "src/*.ui" )
add_executable(kiran-calculator
    ${SRC}
    ${QM_FILES}
    resources/kiran-calculator-resources.qrc
    )

target_include_directories(kiran-calculator PRIVATE
    src
    resources
    data
    src/gui
    src/core
    src/math
    src/gui/mode-switcher-page
    src/gui/programmer-mode-page
    src/gui/science-mode-page
    src/gui/standard-mode-page
    ${CMAKE_BINARY_DIR}
    ${KIRAN_WIDGETS_QT5_INCLUDE_DIRS}
    ${KLOG_QT5_INCLUDE_DIRS}
    )
target_link_libraries(kiran-calculator PRIVATE
    Qt5::Widgets
    kiranwidgets-qt5
    ${KIRAN_WIDGETS_QT5_LIBRARIES}
    ${KLOG_QT5_LIBRARIES}
    )

include(GNUInstallDirs)
# 安装
set(CALCULATOR_ICON_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/24x24/apps)
set(INSTALL_DATADIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR})
set(TRANSLATION_DIR_INSTALL_PATH ${INSTALL_DATADIR}/${PROJECT_NAME}/translations)
configure_file(${CMAKE_SOURCE_DIR}/data/config.h.in ${CMAKE_BINARY_DIR}/config.h @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/data/kiran-calculator.desktop.in ${CMAKE_BINARY_DIR}/kiran-calculator.desktop @ONLY)

install(FILES ${CMAKE_BINARY_DIR}/kiran-calculator.desktop DESTINATION ${INSTALL_DATADIR}/applications )
install(FILES ${CMAKE_SOURCE_DIR}/resources/kiran-calculator-images/kc-calculator.png DESTINATION ${CALCULATOR_ICON_INSTALL_PATH})
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR} )
install (FILES ${QM_FILES} DESTINATION ${TRANSLATION_DIR_INSTALL_PATH})

