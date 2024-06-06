cmake_minimum_required(VERSION 3.10)
project(nooks VERSION 1.2.0)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

add_executable(nooks nooks.c handle_arguments.c actions.c)

# Set the default build output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add nooks version from header of this file to function that print --version
target_compile_definitions(nooks PUBLIC NOOKS_VERSION="${PROJECT_VERSION}")


# Add an option to install the binary to a target directory
option(INSTALL_TO_TARGET_DIR "Install the binary to a target directory" OFF)

if(INSTALL_TO_TARGET_DIR)
    # Find the install directory for executables
    include(GNUInstallDirs)

    # Set the install target for the executable
    install(TARGETS nooks
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

