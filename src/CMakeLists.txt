cmake_minimum_required (VERSION 3.8)


set (DEMO_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Demo")

set (SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/DX12Handle.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/DX12Helper.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ImGuiHandle.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Camera.cpp"
    "${DEMO_SRC_DIR}/DemoRayCPUGradiant.cpp"
    "${DEMO_SRC_DIR}/DemoRayCPUSphere.cpp"
    "${DEMO_SRC_DIR}/DemoTriangle.cpp"
    "${DEMO_SRC_DIR}/DemoQuad.cpp"
	"${DEMO_SRC_DIR}/DemoModel.cpp"
	"${DEMO_SRC_DIR}/DemoScene.cpp")


target_sources(DX12Learning PUBLIC ${SRC_FILES})