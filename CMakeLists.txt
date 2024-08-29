cmake_minimum_required(VERSION 3.10)
project(nooks VERSION 1.3.5)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# Add executable
add_executable(nooks nooks.c handle_arguments.c actions.c)

# Set the default build output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add nooks version from header of this file to function that print --version
target_compile_definitions(nooks PUBLIC NOOKS_VERSION="${PROJECT_VERSION}")

# Find the install directory for executables
include(GNUInstallDirs)

# Set the install target for the executable
install(TARGETS nooks
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Install Zsh completion script
install(FILES _nooks
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/zsh/site-functions
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

# Option for custom installation
option(INSTALL_TO_TARGET_DIR "Install the binary to a target directory" ON)
