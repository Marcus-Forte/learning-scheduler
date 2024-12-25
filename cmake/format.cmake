# Format
find_program(formatter clang-format)
if(formatter)
  file(GLOB_RECURSE sourcefiles
    "src/**/*.cc"
    "src/*.cc"
    "src/**/*.hh"
    "src/*.hh")
  string (REPLACE ";" " " sourcefiles "${sourcefiles}")
  add_custom_target(format ALL
  COMMAND sh -c "clang-format -i ${sourcefiles}"
  VERBATIM)
endif()