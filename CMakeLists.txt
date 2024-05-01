cmake_minimum_required (VERSION 3.2...3.5)
project (NukedSC55 VERSION 0.2.0 LANGUAGES CXX)

include(FindPkgConfig)
include(GNUInstallDirs)
include(CheckCXXCompilerFlag)
include(CheckFunctionExists)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Prefer C++11 standard
set(CMAKE_CXX_STANDARD 11)

if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()
if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()
if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

if(NOT MSVC AND NOT MSDOS)
# Global optimization flags
#    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fno-omit-frame-pointer")
#    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fno-omit-frame-pointer")
# Turn on all warnings
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
# Deny undefined symbols
    if(NOT APPLE)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-undefined" )
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined" )
        set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--no-undefined" )
    endif()
endif()

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    if(NINTENDO_3DS OR NINTENDO_WII OR NINTENDO_WIIU OR NINTENDO_SWITCH)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu90")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c89")
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++98")
endif()

if(DEFINED CMAKE_BUILD_TYPE)
    string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)
endif()
if(CMAKE_BUILD_TYPE_LOWER EQUAL "release")
    add_definitions(-DNDEBUG)
ENDIF()

if(CMAKE_BUILD_TYPE_LOWER STREQUAL "debug")
    add_definitions(-DDEBUG_BUILD)
    if(CMAKE_COMPILER_IS_GNUCXX)
        add_definitions(-D_GLIBCXX_DEBUG=1 -D_GLIBCXX_ASSERTIONS=1)
    endif()
endif()

# Disable bogus MSVC warnings
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

set(SC55_SRC
    src/sc55sound.cpp src/sc55sound.h src/sc55sound.def

    src/mcu.cpp src/mcu.h
    src/lcd.cpp src/lcd.h src/lcd_font.h
    src/mcu_interrupt.cpp src/mcu_interrupt.h
    src/mcu_opcodes.cpp src/mcu_opcodes.h
    src/mcu_timer.cpp src/mcu_timer.h
    src/midi.h
    src/pcm.cpp src/pcm.h
    src/submcu.cpp src/submcu.h
)

#add_executable(nuked-sc55 ${SC55_SRC} ${UTF8MAIN_SRCS})
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static -Wl,--enable-stdcall-fixup")
add_library(sc55sound SHARED ${SC55_SRC})

set(SC55_INSTALL_FILES)

# Copy ROM files into runtime directory
macro(copy_rom_target TargetName TargetFile)
    add_custom_target(sc55_copy_${TargetName} ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${TargetFile}")
    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${TargetFile}"
        COMMAND ${CMAKE_COMMAND} -E copy
                "${CMAKE_CURRENT_SOURCE_DIR}/data/${TargetFile}"
                "${CMAKE_CURRENT_BINARY_DIR}/${TargetFile}"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/data/${TargetFile}"
    )
    list(APPEND SC55_INSTALL_FILES ${CMAKE_CURRENT_BINARY_DIR}/${TargetFile})
endmacro()

copy_rom_target(back_data back.data)

install(TARGETS sc55sound
        EXPORT NukedSC55StaticTargets
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")

install(FILES ${SC55_INSTALL_FILES}
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/nuked-sc55")
