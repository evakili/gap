find_package(Threads REQUIRED)

add_executable(gap-server
    main.cpp
    volatile_authenticator.cpp
    persistent_authenticator.cpp)

target_include_directories(gap-server
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(gap-server PRIVATE Boost::system Threads::Threads)