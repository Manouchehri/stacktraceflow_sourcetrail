cmake_minimum_required(VERSION 2.8)
project(stacktraceflow)

include(ExternalProject)
ExternalProject_Add(valgrind
    GIT_SUBMODULES external/valgrind
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/valgrind
    CONFIGURE_COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/external/valgrind && ./autogen.sh && ./configure --prefix=<INSTALL_DIR>
    BUILD_COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/external/valgrind && make
    INSTALL_COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/external/valgrind && make install
)
ExternalProject_Add(SourcetrailDB
    GIT_SUBMODULES external/SourcetrailDB
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/SourcetrailDB
    INSTALL_COMMAND ""
)
