include(FetchContent)

# define external project
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.14.1
)

# get properties
FetchContent_GetProperties(spdlog)

# build spdlog when needed
set(SPDLOG_BUILD_ALL    OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "" FORCE)
if(NOT TARGET spdlog)
  FetchContent_MakeAvailable(spdlog)
endif()

# mark spdlog as found
set(spdlog_FOUND TRUE)

# move to different folder
set_target_properties(spdlog PROPERTIES FOLDER "Vendors")
