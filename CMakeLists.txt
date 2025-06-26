cmake_minimum_required(VERSION 3.20)
project(rebuildTUI VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add compiler-specific options
if(MSVC)
    add_compile_options(/W4)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

option(BUILD_EXAMPLES "Build examples" ON)
option(BUILD_LIBRARY "Build static library" ON)
option(BUILD_EXECUTABLE "Build main executable" OFF)
option(INSTALL_REBUILDTUI "Generate install targets" ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/rebuildTUI)

set(LIB_SOURCES
    src/terminal_utils.cpp
    src/navigation_tui.cpp
)

set(HEADERS
    include/rebuildTUI/navigation_tui.hpp
    include/rebuildTUI/section.hpp
    include/rebuildTUI/section_builder.hpp
    include/rebuildTUI/selectable_item.hpp
    include/rebuildTUI/terminal_utils.hpp
)

if(BUILD_LIBRARY)
    add_library(rebuildTUI STATIC ${LIB_SOURCES} ${HEADERS})
    add_library(rebuildTUI::rebuildTUI ALIAS rebuildTUI)

    target_include_directories(rebuildTUI PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )

    set_target_properties(rebuildTUI PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        PUBLIC_HEADER "${HEADERS}"
    )

    message(STATUS "Building static library: rebuildTUI")
endif()


if(BUILD_EXECUTABLE AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
    add_executable(custom_tui src/main.cpp)

    if(BUILD_LIBRARY)
        target_link_libraries(custom_tui PRIVATE rebuildTUI)
    else()
        target_sources(custom_tui PRIVATE ${LIB_SOURCES})
    endif()

    set_target_properties(custom_tUI PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
    message(STATUS "Building main executable: custom_tui")
elseif(BUILD_EXECUTABLE)
    message(WARNING "BUILD_EXECUTABLE is ON but src/main.cpp not found. Skipping executable build.")
endif()

if(BUILD_EXAMPLES)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples")
        file(GLOB EXAMPLE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/examples/*.cpp")

        foreach(EXAMPLE_FILE ${EXAMPLE_FILES})
            get_filename_component(EXAMPLE_NAME ${EXAMPLE_FILE} NAME_WE)

            add_executable(${EXAMPLE_NAME} ${EXAMPLE_FILE})

            if(BUILD_LIBRARY)
                target_link_libraries(${EXAMPLE_NAME} PRIVATE rebuildTUI)
            else()
                target_sources(${EXAMPLE_NAME} PRIVATE ${LIB_SOURCES})
            endif()

            set_target_properties(${EXAMPLE_NAME} PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
            )

            message(STATUS "Building example: ${EXAMPLE_NAME}")
        endforeach()
    else()
        message(WARNING "BUILD_EXAMPLES is ON but examples directory not found. Skipping examples build.")
    endif()
endif()

if(INSTALL_REBUILDTUI)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    install(TARGETS rebuildTUI
        EXPORT rebuildTUITargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rebuildTUI
    )

    install(DIRECTORY include/rebuildTUI
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rebuildTUI
        FILES_MATCHING PATTERN "*.hpp"
    )

    if(BUILD_EXAMPLES)
        install(DIRECTORY ${CMAKE_BINARY_DIR}/bin/
            DESTINATION ${CMAKE_INSTALL_BINDIR}
            USE_SOURCE_PERMISSIONS
            PATTERN "*.exe" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        )
    endif()

    configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/rebuildTUIConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/rebuildTUIConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/rebuildTUI
    )

    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/rebuildTUIConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
    )

    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/rebuildTUIConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/rebuildTUIConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/rebuildTUI
    )

    install(EXPORT rebuildTUITargets
        FILE rebuildTUITargets.cmake
        NAMESPACE rebuildTUI::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/rebuildTUI
    )
endif()

if(INSTALL_REBUILDTUI)
    export(EXPORT rebuildTUITargets
        FILE ${CMAKE_CURRENT_BINARY_DIR}/rebuildTUITargets.cmake
        NAMESPACE rebuildTUI::
    )
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG_BUILD)
    if(NOT MSVC)
        add_compile_options(-g -O0)
    endif()
else()
    add_compile_definitions(NDEBUG)
    if(NOT MSVC)
        add_compile_options(-O3)
    endif()
endif()

message(STATUS "=== rebuildTUI Build Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "BUILD_LIBRARY: ${BUILD_LIBRARY}")
message(STATUS "BUILD_EXECUTABLE: ${BUILD_EXECUTABLE}")
message(STATUS "BUILD_EXAMPLES: ${BUILD_EXAMPLES}")
message(STATUS "INSTALL_REBUILDTUI: ${INSTALL_REBUILDTUI}")
message(STATUS "=============================")