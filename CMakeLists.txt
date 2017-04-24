
cmake_minimum_required(VERSION 2.8)


PROJECT(StructuredGridReader)
set(VTK_DIR "C:/VTK/VTK-bin") 

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_executable(StructuredGridReader MACOSX_BUNDLE StructuredGridReader)

if(VTK_LIBRARIES)
  target_link_libraries(StructuredGridReader ${VTK_LIBRARIES})
else()
  target_link_libraries(StructuredGridReader vtkHybrid vtkWidgets)
endif()
