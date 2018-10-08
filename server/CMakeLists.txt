find_package(Threads REQUIRED)

find_package(ODB REQUIRED COMPONENTS sqlite)
include(${ODB_USE_FILE})

set(ODB_HDRS include/db/user.h)

odb_compile(ODB_SRCS FILES ${ODB_HDRS} DB sqlite STANDARD c++11 INCLUDE_PREFIX db GENERATE_QUERY GENERATE_SCHEMA)

add_executable(gap-server
    main.cpp
    volatile_authenticator.cpp
    persistent_authenticator.cpp
    db_authenticator.cpp
    actions.cpp
    server.cpp
    ${ODB_SRCS})

target_include_directories(gap-server
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${ODB_INCLUDE_DIRS}
        ${ODB_COMPILE_OUTPUT_DIR})

target_link_libraries(gap-server PRIVATE Boost::system Threads::Threads ${ODB_LIBRARIES})
