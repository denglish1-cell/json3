
add_executable (todo "marshal.cpp" "main.cpp" "todo.cpp")

target_include_directories(todo PUBLIC include)

target_link_libraries(todo PUBLIC libjson)

set_property(TARGET todo PROPERTY CXX_STANDARD 20)
