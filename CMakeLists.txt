cmake_minimum_required(VERSION 3.16)

#set(CMAKE_TOOLCHAIN_FILE cmake/toolchain.cmake)

project(ProtosServer_ParamService C CXX ASM)
add_definitions(-D_VER="V1.4")
add_definitions(-D_BUILD_TYPE_="${CMAKE_BUILD_TYPE}")

message("SysName: ${CMAKE_SYSTEM_NAME}")
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_PREFIX_PATH $ENV{HOME}/rpi/qt5.15)
    #    set(TARGET_USER sa)
    #    set(CMAKE_INSTALL_RPATH "/home/${TARGET_USER}/${PROJECT_NAME}")
endif()

set(CMAKE_CXX_STANDARD 17)
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
find_package(${qt_ver}Sql         REQUIRED)

add_compile_options(
        -finput-charset=CP1251
        -fexec-charset=CP866
)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/Monitor/*.*
        ${CMAKE_CURRENT_SOURCE_DIR}/DB_Driver/*.*
        ${CMAKE_CURRENT_SOURCE_DIR}/view_TableModel/*.*
        ${CMAKE_CURRENT_SOURCE_DIR}/Param_Item/*.*
        ${CMAKE_CURRENT_SOURCE_DIR}/view_Settings/*.*
        ${CMAKE_CURRENT_SOURCE_DIR}/view_ParamTabs/*.*
        )

set(SOURCE_FILES
        main.cpp
        main_window.cpp
        ParamService.cpp
        ${WIN32_RESOURCES}
        ${SOURCES}
        )

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    enable_language("RC")
    set (WIN32_RESOURCES Resource/resource.rc)
endif()

add_executable(${PROJECT_NAME}
        ${SOURCE_FILES}
        ${WIN32_RESOURCES}
        )

IF(CMAKE_BUILD_TYPE MATCHES Release)
    message("Release build!")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/saved_params.json ${CMAKE_CURRENT_BINARY_DIR}/saved_params.json COPYONLY)
    add_custom_command(
            TARGET ${PROJECT_NAME} PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/qss/ ${CMAKE_CURRENT_BINARY_DIR}/qss/
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Resource/ ${CMAKE_CURRENT_BINARY_DIR}/Resource/
    )
    add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/CMakeCache.txt
            COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake
            COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/Makefile
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_autogen
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/.cmake
    )
    if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)
        add_custom_command(
                COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/logs/ ${CMAKE_CURRENT_BINARY_DIR}/logs/
                COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/appLogs/ ${CMAKE_CURRENT_BINARY_DIR}/appLogs/
                COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/psql_libs/lib ${CMAKE_CURRENT_BINARY_DIR}/
                TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND windeployqt "$<TARGET_FILE:${PROJECT_NAME}>"
                VERBATIM
        )
    endif()
ENDIF()

#target_compile_definitions(${PROJECT_NAME}
#        PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Monitor
        ${CMAKE_CURRENT_SOURCE_DIR}/DB_Driver
        ${CMAKE_CURRENT_SOURCE_DIR}/view_TableModel
        ${CMAKE_CURRENT_SOURCE_DIR}/Param_Item
        ${CMAKE_CURRENT_SOURCE_DIR}/view_Settings/
        ${CMAKE_CURRENT_SOURCE_DIR}/view_ParamTabs/
        )

target_sources(${PROJECT_NAME}
        PRIVATE
        ${SOURCE_FILES}
        )

#add_library(psql SHARED IMPORTED)
#set_target_properties(psql PROPERTIES
#        IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/psql_libs/lib"
#        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/psql_libs/inc"
#        )
#target_link_libraries(${PROJECT_NAME} psql)

target_link_libraries(${PROJECT_NAME} ${qt_ver}::Core)
target_link_libraries(${PROJECT_NAME} ${qt_ver}::Gui)
target_link_libraries(${PROJECT_NAME} ${qt_ver}::Widgets)
target_link_libraries(${PROJECT_NAME} ${qt_ver}::Network)
target_link_libraries(${PROJECT_NAME} ${qt_ver}::SerialPort)
target_link_libraries(${PROJECT_NAME} ${qt_ver}::Sql)