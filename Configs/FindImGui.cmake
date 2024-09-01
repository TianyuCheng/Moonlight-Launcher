include(FetchContent)

# define imgui project
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        v1.89.3
)

# define imguizmo project
FetchContent_Declare(
  imguizmo
  GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo.git
  GIT_TAG        1.83
)

# define imnodes project
FetchContent_Declare(
  imnodes
  GIT_REPOSITORY https://github.com/Nelarius/imnodes.git
  GIT_TAG        v0.5
)

# get properties
FetchContent_GetProperties(imgui)
FetchContent_GetProperties(imguizmo)
FetchContent_GetProperties(imnodes)

# populate imgui
if(NOT imgui_POPULATED)
  FetchContent_Populate(imgui)
endif()

# populate imguizmo
if(NOT imguizmo_POPULATED)
  FetchContent_Populate(imguizmo)
endif()

# populate imnodes
if(NOT imnodes_POPULATED)
  FetchContent_Populate(imnodes)
endif()

# add imgui target (if not done so)
if(NOT TARGET imgui)
  add_library(imgui
    # imgui core sources
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp

    # imguizmo sources
    ${imguizmo_SOURCE_DIR}/GraphEditor.cpp
    ${imguizmo_SOURCE_DIR}/ImCurveEdit.cpp
    ${imguizmo_SOURCE_DIR}/ImGradient.cpp
    ${imguizmo_SOURCE_DIR}/ImGuizmo.cpp
    ${imguizmo_SOURCE_DIR}/ImSequencer.cpp

    # imnodes sources
    ${imnodes_SOURCE_DIR}/imnodes.cpp
  )

  # imgui headers
  target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imguizmo_SOURCE_DIR}
    ${imnodes_SOURCE_DIR}
  )

  # OpenGL + GLFW backends
  find_package(glfw REQUIRED)
  find_package(OpenGL REQUIRED)
  target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR}/backends)
  target_include_directories(imgui PUBLIC ${OPENGL_INCLUDE_DIRS})
  target_link_libraries(imgui PUBLIC glfw ${OPENGL_LIBRARIES})
  target_sources(imgui PRIVATE
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
  )
endif()

# mark imgui as found
set(imgui_FOUND TRUE)

# put imgui under folder
set_target_properties(imgui PROPERTIES PREFIX "")
set_target_properties(imgui PROPERTIES FOLDER "Vendors")
