include(FetchContent)

# define promptfont project
FetchContent_Declare(
  promptfont
  GIT_REPOSITORY https://github.com/TianyuCheng/Prompt-Font-Header.git
  GIT_TAG        master
)

# get properties
FetchContent_GetProperties(promptfont)

# populate promptfont
if(NOT promptfont_POPULATED)
  FetchContent_Populate(promptfont)
endif()

# add promptfont target (if not done so)
if(NOT TARGET promptfont)
  add_library(promptfont INTERFACE)

  # promptfont headers
  target_include_directories(promptfont INTERFACE ${promptfont_SOURCE_DIR}/include)
endif()

# mark promptfont as found
set(promptfont_FOUND TRUE)

# put promptfont under folder
set_target_properties(promptfont PROPERTIES PREFIX "")
set_target_properties(promptfont PROPERTIES FOLDER "Vendors")
