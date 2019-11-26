cmake_minimum_required(VERSION 2.8)
project(stacktraceflow)

include(ExternalProject)

# Valgrind #####################################################################

ExternalProject_Add(valgrind
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}
    GIT_SUBMODULES external/valgrind
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/valgrind
    CONFIGURE_COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/external/valgrind && ./autogen.sh && ./configure --prefix=<INSTALL_DIR>
    BUILD_COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/external/valgrind && make
    INSTALL_COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/external/valgrind && make install
)

# SourcetrailDB ################################################################

set(SOURCETRAILDB_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/SourcetrailDB)
set(SOURCETRAILDB_LIB_INCLUDE ${SOURCETRAILDB_SOURCE_DIR}/core/include)
set(SOURCETRAILDB_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/SourcetrailDB-prefix/src/SourcetrailDB-build/core)
ExternalProject_Add(SourcetrailDB
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}
    GIT_SUBMODULES external/SourcetrailDB
    SOURCE_DIR ${SOURCETRAILDB_SOURCE_DIR}
    INSTALL_COMMAND ""
)
list(APPEND EXTRA_LIBS ${SOURCETRAILDB_LIB_DIR}/libsourcetraildb.a)
list(APPEND EXTRA_INCLUDES "${SOURCETRAILDB_LIB_INCLUDE}")

# Target #######################################################################

add_executable(stacktraceflow
    src/main.cpp
    src/funccall.cpp
    src/functiondirectory.cpp
    src/datafiles.cpp
)
add_dependencies(stacktraceflow SourcetrailDB)

list(APPEND EXTRA_LIBS -pthread)
list(APPEND EXTRA_LIBS -ldl)
target_link_libraries(stacktraceflow PRIVATE ${EXTRA_LIBS})
target_include_directories(stacktraceflow PRIVATE ${EXTRA_INCLUDES})
