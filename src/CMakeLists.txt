cmake_minimum_required (VERSION 3.8)


set (SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/D3D12Handle.cpp")


target_sources(DX12Learning PUBLIC ${SRC_FILES})