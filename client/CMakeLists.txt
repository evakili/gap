add_executable(gap-client main.cpp)

target_include_directories(gap-client PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(gap-client PRIVATE Boost::system Threads::Threads)