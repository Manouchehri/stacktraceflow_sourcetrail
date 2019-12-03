cmake_minimum_required(VERSION 3.0.1)
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
set(SOURCETRAILDB_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/src/SourcetrailDB-build/core)
ExternalProject_Add(SourcetrailDB
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}
    GIT_SUBMODULES external/SourcetrailDB
    SOURCE_DIR ${SOURCETRAILDB_SOURCE_DIR}
    INSTALL_COMMAND ""
)
include_directories( ${SOURCETRAILDB_LIB_INCLUDE} )
add_library(sourcetraildb SHARED IMPORTED)
set_target_properties(sourcetraildb PROPERTIES
    IMPORTED_LOCATION ${SOURCETRAILDB_LIB_DIR}/libsourcetraildb.a)

# Boost ########################################################################

find_package(Boost 1.40
    REQUIRED COMPONENTS filesystem system
)
include_directories( ${Boost_INCLUDE_DIR} )

# stacktraceflow ###############################################################

add_executable(stacktraceflow
    src/main.cpp
    src/StackTraceFlowReader.cpp
    src/StfToSdbTranslator.cpp
    src/SdbWriterProxy.cpp
)

target_link_libraries(stacktraceflow
    PRIVATE sourcetraildb
    PRIVATE ${Boost_LIBRARIES}
    -pthread
    -ldl)

set_target_properties(stacktraceflow PROPERTIES
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF
)
