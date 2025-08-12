# CMakeList.txt : CMake project for RasterEngine2, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("RasterEngine2")

# Add source to this project's executable.
add_executable (RasterEngine2 
	"src/RasterEngine2.cpp"
    "src/Texture.cpp"
    "src/Mesh.cpp"
    "src/DrawFuncs.cpp"
    "src/RasterEngine2.h"
)

target_include_directories(RasterEngine2 PRIVATE src External Inc Inc/buffer Inc/geometry Inc/ shading Inc/Vectors) 

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET RasterEngine2 PROPERTY CXX_STANDARD 20)
endif()

target_compile_definitions(RasterEngine2 PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
)

# TODO: Add tests and install targets if needed.
