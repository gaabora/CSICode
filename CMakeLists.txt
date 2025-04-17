cmake_minimum_required(VERSION 3.5)
project(CSI VERSION 1.0.0)

set(SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/reaper_csurf_integrator)
set(WDL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/WDL)
set(RES_PATH ${CMAKE_CURRENT_SOURCE_DIR}/reaper_csurf_integrator)

if (UNIX)
    find_package(Perl REQUIRED)
    execute_process(COMMAND ${PERL_EXECUTABLE} ${WDL_PATH}/swell/swell_resgen.pl --quiet ${RES_PATH}/res.rc)
endif ()

set(TARGET reaper_csurf_integrator)
set(CMAKE_CXX_STANDARD 17)

add_library(${TARGET} SHARED
    ${SRC_PATH}/control_surface_action_contexts.h
    ${SRC_PATH}/control_surface_integrator.cpp
    ${SRC_PATH}/control_surface_integrator.h
    ${SRC_PATH}/control_surface_integrator_Reaper.h
    ${SRC_PATH}/control_surface_integrator_ui.cpp
    ${SRC_PATH}/control_surface_manager_actions.h
    ${SRC_PATH}/control_surface_midi_widgets.h
    ${SRC_PATH}/control_surface_OSC_widgets.h
    ${SRC_PATH}/control_surface_Reaper_actions.h
    ${SRC_PATH}/handy_functions.h
    ${SRC_PATH}/main.cpp
    ${SRC_PATH}/oscpkt.hh
    ${SRC_PATH}/reaper_plugin.h
    ${SRC_PATH}/reaper_plugin_functions.h
    ${SRC_PATH}/resource.h
    ${SRC_PATH}/udp.hh
    ${WDL_PATH}/win32_utf8.c
)

install(TARGETS ${TARGET}
    RUNTIME
    DESTINATION $ENV{REAPER_PATH}/UserPlugins
)
