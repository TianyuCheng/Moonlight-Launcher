include(FetchContent)

# define external project
FetchContent_Declare(
  toml
  GIT_REPOSITORY https://github.com/marzer/tomlplusplus
  GIT_TAG        v3.4.0
)

# get properties
FetchContent_GetProperties(toml)

# populate toml sources

# build toml when needed
if(NOT TARGET toml)
  FetchContent_MakeAvailable(toml)
  add_library(toml ALIAS tomlplusplus_tomlplusplus)
endif()

# mark toml as found
set(toml_FOUND TRUE)

# move to different folder
set_target_properties(tomlplusplus_tomlplusplus PROPERTIES FOLDER "Vendors")
