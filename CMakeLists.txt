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
set(SOURCETRAILDB_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/src/SourcetrailDB-build/core)
ExternalProject_Add(SourcetrailDB
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}
    GIT_SUBMODULES external/SourcetrailDB
    SOURCE_DIR ${SOURCETRAILDB_SOURCE_DIR}
    INSTALL_COMMAND ""
)
LINK_DIRECTORIES(${SOURCETRAILDB_LIB_DIR})
list(APPEND EXTRA_LIBS sourcetraildb)
list(APPEND EXTRA_INCLUDES "${SOURCETRAILDB_LIB_INCLUDE}")

# Boost ########################################################################

FIND_PACKAGE(Boost 1.40
    REQUIRED COMPONENTS filesystem system
)
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

# Target #######################################################################

add_executable(stacktraceflow
    src/main.cpp
    src/StackTraceFlowReader.cpp
    src/StfToSdbTranslator.cpp
    src/SdbWriterProxy.cpp
)

list(APPEND EXTRA_LIBS -pthread)
list(APPEND EXTRA_LIBS -ldl)
target_link_libraries(stacktraceflow
    PRIVATE ${EXTRA_LIBS}
    PRIVATE ${Boost_LIBRARIES})
target_include_directories(stacktraceflow PRIVATE ${EXTRA_INCLUDES})
set_target_properties(stacktraceflow PROPERTIES
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF
)
