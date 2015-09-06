cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};../CMakeTools/")

#Project name :
project(glip)

set(LIBRARY_OUTPUT_PATH lib/)
include_directories(
	./include/
	./include/Core/
	./include/Modules/
)

# Generate the list of source files : 
file(
	GLOB_RECURSE
	source_files
	src/*
	include/*
)
file(
	GLOB
	header_files_base
	include/*.hpp
)
file(
	GLOB_RECURSE
	header_files_core
	include/Core/*
)
file(
	GLOB_RECURSE
	header_files_modules
	include/Modules/*
)

# Binary to be generated (shared library) : 
add_library(
		glip
		SHARED
		${source_files}
)

#if(GLIP_USE_GLES)
#	message(STATUS "Building against GLES")
#	find_package(OpenGLES REQUIRED)
#	target_link_libraries(glip ${OPENGLES_LIBRARIES})
#	add_definitions(-DGLIP_USE_GLES)
#	list(REMOVE_ITEM source_files ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/glew.c)
#	list(REMOVE_ITEM source_files ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/wglew.h)
#	list(REMOVE_ITEM source_files ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/glxew.h)
#	list(REMOVE_ITEM source_files ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/glew.h)
#	list(REMOVE_ITEM header_files_core ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/wglew.h)
#	list(REMOVE_ITEM header_files_core ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/glxew.h)
#	list(REMOVE_ITEM header_files_core ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/glew.h)
#	message(STATUS "Files : ${source_files}")
#elseif(GLIP_USE_GLES2)
#	message(STATUS "Building against GLES2")
#	find_package(OpenGLES2 REQUIRED)
#	target_link_libraries(glip ${OPENGLES2_LIBRARIES})
#	add_definitions(-DGLIP_USE_GLES2)
#	list(REMOVE_ITEM source_files ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/glew.c)
#	list(REMOVE_ITEM source_files ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/wglew.h)
#	list(REMOVE_ITEM source_files ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/glxew.h)
#	list(REMOVE_ITEM source_files ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/glew.h)
#	list(REMOVE_ITEM header_files_core ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/wglew.h)
#	list(REMOVE_ITEM header_files_core ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/glxew.h)
#	list(REMOVE_ITEM header_files_core ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/glew.h)
#else
if(GLIP_USE_GLES3)
	message(STATUS "Building against GLES3.1")
	find_package(OpenGLES3 REQUIRED)
	target_link_libraries(glip ${OPENGLES3_LIBRARIES})
	add_definitions(-DGLIP_USE_GLES3)
	list(REMOVE_ITEM source_files ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/glew.c)
	list(REMOVE_ITEM source_files ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/wglew.h)
	list(REMOVE_ITEM source_files ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/glxew.h)
	list(REMOVE_ITEM source_files ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/glew.h)
	list(REMOVE_ITEM header_files_core ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/wglew.h)
	list(REMOVE_ITEM header_files_core ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/glxew.h)
	list(REMOVE_ITEM header_files_core ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/glew.h)
else()
	message(STATUS "Building against GL")
	find_package(OpenGL REQUIRED)
	target_link_libraries(glip ${OPENGL_LIBRARIES})
	add_definitions(-DGLIP_USE_GL)
endif()

# Options :
if(WIN32) # Windows specifics :
	# None
else() # *NIX specifics
	# We are silencing the long long warning here.
	# To be improved in a future release.
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra -Wno-long-long")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
endif()


# Packaging :
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "GLIP-Lib, An OpenGL Image Processing Library.")
set(CPACK_PACKAGE_VENDOR "Ronan Kerviche")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
set(CPACK_COMPONENTS_IGNORE_GROUPS ON)

# Source package :
set(CPACK_SOURCE_IGNORE_FILES 	"/\\\\.gitignore"
				"/CMakeCache\\\\.txt"
				"/install_manifest_lib\\\\.txt"
				"/install_manifest_lib-dev\\\\.txt"
				"cmake_install\\\\.cmake"
				"CPackConfig\\\\.cmake"
				"CPackSourceConfig\\\\.cmake"
				"/glip-${CPACK_PACKAGE_VERSION_MAJOR}\\\\.${CPACK_PACKAGE_VERSION_MINOR}\\\\.${CPACK_PACKAGE_VERSION_PATCH}-Source\\\\.tar\\\\.gz"
				"/glip-${CPACK_PACKAGE_VERSION_MAJOR}\\\\.${CPACK_PACKAGE_VERSION_MINOR}\\\\.${CPACK_PACKAGE_VERSION_PATCH}-Source\\\\.tar\\\\.Z"
				"/glip-${CPACK_PACKAGE_VERSION_MAJOR}\\\\.${CPACK_PACKAGE_VERSION_MINOR}\\\\.${CPACK_PACKAGE_VERSION_PATCH}-Source\\\\.tar\\\\.bz2"
				"/glip-${CPACK_PACKAGE_VERSION_MAJOR}\\\\.${CPACK_PACKAGE_VERSION_MINOR}\\\\.${CPACK_PACKAGE_VERSION_PATCH}-Linux-lib\\\\.deb"
				"/glip-${CPACK_PACKAGE_VERSION_MAJOR}\\\\.${CPACK_PACKAGE_VERSION_MINOR}\\\\.${CPACK_PACKAGE_VERSION_PATCH}-Linux-lib-dev\\\\.deb"
				"/glip-${CPACK_PACKAGE_VERSION_MAJOR}\\\\.${CPACK_PACKAGE_VERSION_MINOR}\\\\.${CPACK_PACKAGE_VERSION_PATCH}-Linux-lib\\\\.rpm"
				"/glip-${CPACK_PACKAGE_VERSION_MAJOR}\\\\.${CPACK_PACKAGE_VERSION_MINOR}\\\\.${CPACK_PACKAGE_VERSION_PATCH}-Linux-lib-dev\\\\.rpm"
				"/lib/"
				"/Makefile"
				"/CMakeFiles/"
				"/_CPack_Packages/")

# DEB package :
find_program(testDPKG dpkg)
if(testDPKG)
	execute_process(COMMAND dpkg --print-architecture COMMAND tr -d '\n' OUTPUT_VARIABLE DEB_ARCHITECTURE) #Get the architecture.
	set(CPACK_DEB_COMPONENT_INSTALL ON) #Generate multiple packages.
	set(CPACK_GENERATOR "DEB")
	set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Ronan Kerviche")
	set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${DEB_ARCHITECTURE}")
	set(CPACK_DEBIAN_PACKAGE_DEPENDS "glip-lib (>=1.0.0)")
endif()

# RPM package :
find_program(testRPM rpm)
if(testRPM)
	set(CPACK_RPM_COMPONENT_INSTALL ON) #Generate multiple packages.
	set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
	set(CPACK_GENERATOR "RPM")
endif()

# Install :
install(
	TARGETS glip
	DESTINATION lib
	COMPONENT lib
)
install(
	FILES ${header_files_base}
	DESTINATION include/glip-lib
	COMPONENT lib-dev
)
install(
	FILES ${header_files_core}
	DESTINATION include/glip-lib/Core
	COMPONENT lib-dev
)
install(
	FILES ${header_files_modules}
	DESTINATION include/glip-lib/Modules
	COMPONENT lib-dev
)

include(CPack)

