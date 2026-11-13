add_library(libjson Object.hpp Boolean.cpp Boolean.hpp Data.hpp Null.cpp Null.hpp Number.cpp Number.hpp String.cpp String.hpp Array.hpp json.hpp
)
target_include_directories(libjson PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")