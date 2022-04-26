cmake_minimum_required (VERSION 3.8)

set (DEPS_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")
set (IMGUI_SRC "${DEPS_SRC}/imgui")
set (DEPS_INC "${CMAKE_CURRENT_SOURCE_DIR}/include")
set (DEPS_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib")

set (DEPS_SRC_FILES
    "${IMGUI_SRC}/imgui.cpp"
    "${IMGUI_SRC}/imgui_demo.cpp"
    "${IMGUI_SRC}/imgui_draw.cpp"
    "${IMGUI_SRC}/imgui_impl_dx12.cpp"
    "${IMGUI_SRC}/imgui_impl_glfw.cpp"
    "${IMGUI_SRC}/imgui_tables.cpp"
    "${IMGUI_SRC}/imgui_widgets.cpp"
    "${IMGUI_SRC}/imgui_stdlib.cpp"
    "${DEPS_SRC}/Intersection.cpp"
    "${DEPS_SRC}/Plane.cpp"
    "${DEPS_SRC}/SegmentPlane.cpp"
    "${DEPS_SRC}/SpherePlane.cpp"
    "${DEPS_SRC}/DDSTextureLoader12.cpp")

target_include_directories(DX12Learning PUBLIC "${DEPS_INC}/")
target_include_directories(DX12Learning PUBLIC "${DEPS_INC}/imgui/")


target_sources(DX12Learning PUBLIC ${DEPS_SRC_FILES})