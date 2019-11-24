cmake_minimum_required(VERSION 2.8)
project(stacktraceflow)

include(ExternalProject)
ExternalProject_Add(valgrind
    GIT_SUBMODULES valgrind
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/valgrind
    CONFIGURE_COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/valgrind && ./autogen.sh && ./configure --prefix=<INSTALL_DIR>
    BUILD_COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/valgrind && make
    INSTALL_COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/valgrind && make install
    )
