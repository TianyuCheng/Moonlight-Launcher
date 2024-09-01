include(FetchContent)

# define SetDPI project
FetchContent_Declare(
  SetDPI
  GIT_REPOSITORY https://github.com/imniko/SetDPI.git
  GIT_TAG        v1.0
)

# get properties
FetchContent_GetProperties(SetDPI)

# populate SetDPI
if(NOT SetDPI_POPULATED)
  FetchContent_Populate(SetDPI)
endif()

# add SetDPI target (if not done so)
if(NOT TARGET SetDPI)
  add_library(SetDPI
    ${setdpi_SOURCE_DIR}/DpiHelper.h
    ${setdpi_SOURCE_DIR}/DpiHelper.cpp
  )

  # SetDPI headers
  target_include_directories(SetDPI PUBLIC ${setdpi_SOURCE_DIR})
endif()

# mark SetDPI as found
set(SetDPI_FOUND TRUE)

# put SetDPI under folder
set_target_properties(SetDPI PROPERTIES PREFIX "")
set_target_properties(SetDPI PROPERTIES FOLDER "Vendors")
