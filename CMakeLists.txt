cmake_minimum_required(VERSION 3.16)

project(PotosServer_ParamService C CXX ASM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
#    enable_language("RC")
#    set (WIN32_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/windows/MyApp.rc)
#endif()

set(qt_ver      Qt5)
find_package(${qt_ver}Widgets     REQUIRED)
find_package(${qt_ver}Gui         REQUIRED)
find_package(${qt_ver}Core        REQUIRED)
find_package(${qt_ver}Network     REQUIRED)
find_package(${qt_ver}SerialPort  REQUIRED)

add_compile_options(
        -finput-charset=CP1251
        -fexec-charset=CP866
)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/Monitor/*.*
        )

set(SOURCE_FILES
        main.cpp
        main_window.cpp
        ${WIN32_RESOURCES}
        ${SOURCES}
        )

add_executable(${PROJECT_NAME}
        ${SOURCE_FILES}
        )

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Monitor
        )

target_sources(${PROJECT_NAME}
        PRIVATE
        ${SOURCE_FILES}
        )

target_link_libraries(${PROJECT_NAME} ${qt_ver}::Core)
target_link_libraries(${PROJECT_NAME} ${qt_ver}::Gui)
target_link_libraries(${PROJECT_NAME} ${qt_ver}::Widgets)
target_link_libraries(${PROJECT_NAME} ${qt_ver}::Network)
target_link_libraries(${PROJECT_NAME} ${qt_ver}::SerialPort)

#add_custom_command(
#        TARGET ${PROJECT_NAME} POST_BUILD
#        COMMAND windeployqt "$<TARGET_FILE:${PROJECT_NAME}>"
#        VERBATIM)

#target_compile_definitions(${PROJECT_NAME}
#        PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
