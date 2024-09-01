include(FetchContent)

# define external project
FetchContent_Declare(
  eigen
  GIT_REPOSITORY https://github.com/libigl/eigen
  GIT_TAG        master
)

# get properties
FetchContent_GetProperties(eigen)

# populate eigen sources
if(NOT eigen_POPULATED)
  FetchContent_Populate(eigen)
endif()

# build eigen when needed
if(NOT TARGET eigen)
  add_library(eigen INTERFACE)
  target_include_directories(eigen INTERFACE ${eigen_SOURCE_DIR})
endif()

# mark eigen as found
set(eigen_FOUND TRUE)

# move to different folder
set_target_properties(eigen PROPERTIES FOLDER "Vendors")
