# export compilation database
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# enable C++ 17
set(CMAKE_CXX_STANDARD 17)

# For Visual Studio
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# For Visual Studio (export .lib for .dll)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Enable ccache
set(CMAKE_CXX_COMPILER_LAUNCHER ccache)

# Compiler specific settings
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_link_options(-fsanitize=address)
    add_compile_options(-fsanitize=address)
  endif()
  add_compile_options(-Wall)
  add_compile_options(-Wextra)
  add_compile_options(-Wpedantic)
  add_compile_options(-Werror)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  add_compile_options(/wd4996) # ignore the stupid warning from MSVC
  add_compile_options(/wd4100) # ignore the unused parameter
  add_compile_options(/wd4458) # declaration of xxx hides class member
  add_compile_options(/W1)
  add_compile_options(/WX)
  # add_compile_options(/fsanitize=address)
endif()
