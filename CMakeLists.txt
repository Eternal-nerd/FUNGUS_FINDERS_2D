cmake_minimum_required(VERSION 3.5)
project(FUNGUS_FINDERS_2D)

# set standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Vulkan
find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIRS})

# SDL3 
find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
include_directories(${SDL3_INCLUDE_DIRS})

# add source files:
set(SOURCES
	src/main.cpp
    src/engine.cpp
	src/util.cpp
	src/asset_manager.cpp
	src/texture.cpp

	src/renderables/renderable_manager.cpp
	src/renderables/rectangle.cpp
	src/renderables/player.cpp
)

# add headers
set(HEADERS
    src/engine.h
	src/util.h
	src/asset_manager.h
	src/texture.h

	src/renderables/renderable_manager.h
	src/renderables/rectangle.h
	src/renderables/player.h

	lib/stb_image.h
	#lib/tiny_obj_loader.h
)

# add executable:
add_executable(FUNGUS_FINDERS_2D ${SOURCES} ${HEADERS})

# add libs
target_link_libraries(FUNGUS_FINDERS_2D PRIVATE Vulkan::Vulkan SDL3::SDL3)

# Specify the output directory for the binary
set_target_properties(FUNGUS_FINDERS_2D PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

