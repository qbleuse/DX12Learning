cmake_minimum_required (VERSION 3.8)

project ("DX12Learning")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(DEPS_DIR "${CMAKE_SOURCE_DIR}/deps")
set(LIB_DIR "${DEPS_DIR}/lib")
set(INC_DIR "${CMAKE_SOURCE_DIR}/include")

include_directories("${INC_DIR}/")

# Record SUB_SYS configuration (WIN32 for release Windows to remove console)
IF(CMAKE_BUILD_TYPE MATCHES Release)
    message(STATUS "Generate CMake cache for Release")
    IF(WIN32)
        set(SUB_SYS WIN32)
    ENDIF(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
ELSE()
    message(STATUS "Generate CMake cache for Debug")
    add_definitions(-DDEBUG)
ENDIF(CMAKE_BUILD_TYPE MATCHES Release)
 
# List source files
set (MAIN_FILE "${SRC_DIR}/main.cpp" "src/DemoRayCPU.cpp")

# Add source to project executable
add_executable (DX12Learning ${SUB_SYS} ${MAIN_FILE})

# Add libraries
target_link_libraries(DX12Learning "d3d12.lib" "dxgi.lib" "d3dcompiler.lib")
target_link_libraries(DX12Learning "${LIB_DIR}/glfw3.lib")

# Add sub projects.
add_subdirectory(${SRC_DIR})
add_subdirectory(${DEPS_DIR})