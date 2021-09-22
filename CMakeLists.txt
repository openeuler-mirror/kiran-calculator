cmake_minimum_required(VERSION 3.5)

project(kiran-calculator)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)

  add_executable(kiran-calculator
    main.cpp
    gui/window.cpp
    gui/window.h
    gui/window.ui
    gui/history-recoder.cpp
    gui/history-recoder.h
    gui/stage-page.cpp
    gui/stage-page.h
    gui/keys-page.h
    gui/utils.cpp
    gui/utils.h
    gui/general-enum.h
    gui/sselection.h

    gui/mode-switcher-page/mode-switcher-page.cpp
    gui/mode-switcher-page/mode-switcher-page.h
    gui/mode-switcher-page/mode-switcher-page.ui
    gui/mode-switcher-page/mode-list.cpp
    gui/mode-switcher-page/mode-list.h

    gui/standard-mode-page/expr-calculator.cpp
    gui/standard-mode-page/expr-calculator.h
    gui/standard-mode-page/standard-keys-page.cpp
    gui/standard-mode-page/standard-keys-page.h
    gui/standard-mode-page/standard-mode-page.ui
    gui/standard-mode-page/standard-mode-page.cpp
    gui/standard-mode-page/standard-mode-page.h

    gui/science-mode-page/science-expr-calculator.cpp
    gui/science-mode-page/science-expr-calculator.h
    gui/science-mode-page/science-keys-page.cpp
    gui/science-mode-page/science-keys-page.h
    gui/science-mode-page/science-mode-page.cpp
    gui/science-mode-page/science-mode-page.h
    gui/science-mode-page/science-mode-page.ui


    gui/programmer-mode-page/programmer-expr-calculator.cpp
    gui/programmer-mode-page/programmer-expr-calculator.h
    gui/programmer-mode-page/num-conversion.cpp
    gui/programmer-mode-page/num-conversion.h
    gui/programmer-mode-page/programmer-mode-page.cpp
    gui/programmer-mode-page/programmer-mode-page.h
    gui/programmer-mode-page/programmer-mode-page.ui
    gui/programmer-mode-page/programmer-keys-page.cpp
    gui/programmer-mode-page/programmer-keys-page.h



    core/book.cpp
    core/book.h
    core/constants.cpp
    core/constants.h
    core/evaluator.cpp
    core/evaluator.h
    core/functions.cpp
    core/functions.h
    core/numberformatter.cpp
    core/numberformatter.h
    core/opcode.cpp
    core/opcode.h
    core/pageserver.cpp
    core/pageserver.h
    core/session.cpp
    core/session.h
    core/sessionhistory.cpp
    core/sessionhistory.h
    core/settings.cpp
    core/settings.h
    core/userfunction.cpp
    core/userfunction.h
    core/variable.cpp
    core/variable.h

    math/cmath.cpp
    math/cmath.h
    math/cnumberparser.cpp
    math/cnumberparser.h
    math/floatcommon.c
    math/floatcommon.h
    math/floatconfig.h
    math/floatconst.c
    math/floatconst.h
    math/floatconvert.c
    math/floatconvert.h
    math/floaterf.c
    math/floaterf.h
    math/floatexp.c
    math/floatexp.h
    math/floatgamma.c
    math/floatgamma.h
    math/floathmath.c
    math/floathmath.h
    math/floatincgamma.c
    math/floatincgamma.h
    math/floatio.c
    math/floatio.h
    math/floatipower.c
    math/floatipower.h
    math/floatlog.c
    math/floatlog.h
    math/floatlogic.c
    math/floatlogic.h
    math/floatlong.c
    math/floatlong.h
    math/floatnum.c
    math/floatnum.h
    math/floatpower.c
    math/floatpower.h
    math/floatseries.c
    math/floatseries.h
    math/floattrig.c
    math/floattrig.h
    math/hmath.cpp
    math/hmath.h
    math/number.c
    math/number.h
    math/quantity.cpp
    math/quantity.h
    math/rational.cpp
    math/rational.h
    math/units.cpp
    math/units.h
  )

target_include_directories(kiran-calculator PRIVATE
    gui
    core
    math
    ${CMAKE_BINARY_DIR}
    gui/mode-switcher-page
    gui/programmer-mode-page
    gui/science-mode-page
    gui/standard-mode-page)
target_link_libraries(kiran-calculator PRIVATE Qt5::Widgets)

