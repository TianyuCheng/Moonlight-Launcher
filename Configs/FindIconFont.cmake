include(FetchContent)

# define IconFontCppHeaders project
FetchContent_Declare(
  iconfont
  GIT_REPOSITORY https://github.com/juliettef/IconFontCppHeaders
  GIT_TAG        main
)

# get properties
FetchContent_GetProperties(iconfont)

# populate iconfont
if(NOT iconfont_POPULATED)
  FetchContent_Populate(iconfont)
endif()

# add iconfont target (if not done so)
if(NOT TARGET iconfont)
  add_library(iconfont INTERFACE)

  # iconfont headers
  target_include_directories(iconfont INTERFACE ${iconfont_SOURCE_DIR})
endif()

# mark iconfont as found
set(iconfont_FOUND TRUE)

# put iconfont under folder
set_target_properties(iconfont PROPERTIES PREFIX "")
set_target_properties(iconfont PROPERTIES FOLDER "Vendors")
