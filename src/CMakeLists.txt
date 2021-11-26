cmake_minimum_required (VERSION 3.8)


set (DEMO_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Demo")

set (SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/DX12Handle.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ImGuiHandle.cpp"
    "${DEMO_SRC_DIR}/DemoRayCPU.cpp"
    "${DEMO_SRC_DIR}/DemoTriangle.cpp")


target_sources(DX12Learning PUBLIC ${SRC_FILES})