cmake_minimum_required (VERSION 3.8)
project (shadow)

find_package(Vulkan)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(FreeImagePlus)
find_package(FreeImage)
find_package(TinyOBJLoader)

# get rid of annoying MSVC warnings.
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

set (CMAKE_CXX_STANDARD 11)

include_directories(${Vulkan_INCLUDE_DIR} ${FREEIMAGEPLUS_INCLUDE_DIR} ${FREEIMAGE_INCLUDE_DIR} ${TINYOBJLOADER_INCLUDE_DIR})

set(ALL_LIBS  ${Vulkan_LIBRARY} ${FREEIMAGEPLUS_LIBRARY} ${FREEIMAGE_LIBRARY} ${TINYOBJLOADER_LIBRARY})

if(WIN32)
  link_directories(${ADDITIONAL_LIBRARY_DIRS})
else()
  find_package(glfw3 REQUIRED)
  include_directories(${GLFW_INCLUDE_DIRS})
  set(ALL_LIBS ${ALL_LIBS} ${GLFW_LIBRARIES} )
endif()

#uncomment this to detect broken memory problems via gcc sanitizers
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize-address-use-after-scope -fno-omit-frame-pointer -fsanitize=leak -fsanitize=undefined -fsanitize=bounds-strict")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

add_executable(scene main.cpp src/vk_utils.h src/vk_utils.cpp src/image.h src/image.cpp src/application.h src/application.cpp src/render.h src/render.cpp src/camera.h src/camera.cpp src/object.h src/object.cpp src/texture.h src/texture.cpp src/light.h src/light.cpp)

set_target_properties(scene PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

file(COPY shaders/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/shaders)
file(COPY textures/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/textures)
file(COPY models/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/models)
target_link_libraries(scene ${ALL_LIBS} ${GLFW_LIBRARIES} glfw)

