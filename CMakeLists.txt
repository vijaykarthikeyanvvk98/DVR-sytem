cmake_minimum_required(VERSION 3.19)
project(dvr_system LANGUAGES CXX)

# 👉 Point to your OpenCV install
set(OpenCV_DIR
    "C:/Users/vijay/Downloads/build2/install"
)

find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Quick)
find_package(Qt6 REQUIRED COMPONENTS Core)
pkg_check_modules(GST REQUIRED
    gstreamer-1.0
    gstreamer-video-1.0
    gstreamer-audio-1.0
    gstreamer-app-1.0
)

qt_standard_project_setup()

qt_add_executable(dvr_system
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
)
qt_add_qml_module(dvr_system
    URI dvr_system
    VERSION 1.0
    QML_FILES
    Main.qml
    RESOURCES images/0%.png images/100%.png images/25%.png images/50%.png images/75%.png images/add.png images/arrow.png images/brake.png images/button.png images/close.jpg images/close_2.png images/close_3.png images/CME-Logo.jpg images/CME-Logo.png images/compass-needle.png images/compass.png images/connect.png images/connected.png images/crossHair.png images/depth.png images/dummy_image_template.jpg images/dummy_template.jpg images/dummy_template3.jpg images/forward.png images/gps_off.png images/gps_on.png images/handbrake.png images/light.png images/location.png images/location_2.png images/location_3.png images/logo2.png images/mike_off.png images/mike_on.png images/open.jpg images/piston.png images/pressure.png images/reset.png images/reverse.png images/rover.png images/screenshot.png images/screenshot2.png images/screenshot3.png images/settings.png images/settings_2.png images/speed.png images/tare.png images/temperature.png images/video_hide.png images/video_show.png images/vikra.png images/vikra_2.jpeg images/vikra_2.png
    SOURCES videostreamer.h videostreamer.cpp
    SOURCES opencvimageprovider.h opencvimageprovider.cpp
    RESOURCES images/subtitle.svg
)
target_link_libraries(dvr_system
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Quick
        ${OpenCV_LIBS}
)
target_link_libraries(dvr_system PRIVATE Qt6::Core)
target_include_directories(dvr_system PRIVATE
    ${GST_INCLUDE_DIRS}
)

target_link_directories(dvr_system PRIVATE
    ${GST_LIBRARY_DIRS}
)

target_link_libraries(dvr_system PRIVATE
    ${GST_LIBRARIES}
)

include(GNUInstallDirs)

install(TARGETS dvr_system
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET dvr_system
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

install(
    FILES ${CMAKE_SOURCE_DIR}/audio.py
    DESTINATION ${CMAKE_INSTALL_BINDIR}
)

