cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};../../CMakeTools/")

# Remove relative path warnings :
cmake_policy(SET CMP0015 NEW)

#Project name
project(glip-compute)
set(EXECUTABLE_OUTPUT_PATH .)

# Requirements: 
find_package(OpenGL REQUIRED)

# Includes paths :
include_directories(
		./src/
		../../GLIP-Lib/include/
		../ExternalSrc/X/
		../ExternalSrc/FreeImagePlus/
		../ExternalSrc/LibRaw/
		)

# Link paths : 
link_directories(
		../../GLIP-Lib/lib/
		)

# Generate the list of source files : 
file(
		GLOB_RECURSE
		source_files
		src/*
		../ExternalSrc/X/*
		../ExternalSrc/FreeImagePlus/*
		../ExternalSrc/LibRaw/*
)

# Executable : 
add_executable(
		glip-compute
		${source_files}
)

#if(GLIP_USE_GLES)
#	message(STATUS "Building against GLES")
#	find_package(OpenGLES REQUIRED)
#	target_link_libraries(glip-compute X11 ${OPENGLES_LIBRARIES} freeimageplus glip)
#	add_definitions(-DGLIP_USE_GLES)
#elseif(GLIP_USE_GLES2)
#	message(STATUS "Building against GLES2")
#	find_package(OpenGLES2 REQUIRED)
#	target_link_libraries(glip-compute X11 ${OPENGLES2_LIBRARIES} freeimageplus glip)
#	add_definitions(-DGLIP_USE_GLES2)
#else
if(GLIP_USE_GLES3)
	message(STATUS "Building against GLES3")
	find_package(OpenGLES3 REQUIRED)
	target_link_libraries(glip-compute X11 ${OPENGLES3_LIBRARIES} freeimageplus raw glip)
	add_definitions(-DGLIP_USE_GLES3)
else()
	message(STATUS "Building against GL")
	find_package(OpenGL REQUIRED)
	target_link_libraries(glip-compute X11 ${OPENGL_LIBRARIES} freeimageplus raw glip)
	add_definitions(-DGLIP_USE_GL)
endif()

# Link : 
#target_link_libraries(glip-compute X11 GL freeimageplus glip)
#target_link_libraries(glip-compute X11 GLESv1_CM freeimageplus glip)

if(WIN32) # Windows specifics
	# None	
else() # *NIX specifics
	# We are silencing the long long warning here.
	# To be improved in a future release.
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra -Wno-long-long")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
endif()

# Packaging :
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Glip-Compute, a command-line tool for GLIP-Lib.")
set(CPACK_PACKAGE_VENDOR "Ronan Kerviche")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")

# Source package :
set(CPACK_SOURCE_IGNORE_FILES 	"/\\\\.gitignore"
				"/CMakeCache\\\\.txt"
				"/install_manifest\\\\.txt"
				"cmake_install\\\\.cmake"
				"CPackConfig\\\\.cmake"
				"CPackSourceConfig\\\\.cmake"
				"/glip-compute-${CPACK_PACKAGE_VERSION_MAJOR}\\\\.${CPACK_PACKAGE_VERSION_MINOR}\\\\.${CPACK_PACKAGE_VERSION_PATCH}-Source\\\\.tar\\\\.gz"
				"/glip-compute-${CPACK_PACKAGE_VERSION_MAJOR}\\\\.${CPACK_PACKAGE_VERSION_MINOR}\\\\.${CPACK_PACKAGE_VERSION_PATCH}-Source\\\\.tar\\\\.Z"
				"/glip-compute-${CPACK_PACKAGE_VERSION_MAJOR}\\\\.${CPACK_PACKAGE_VERSION_MINOR}\\\\.${CPACK_PACKAGE_VERSION_PATCH}-Source\\\\.tar\\\\.bz2"
				"/glip-compute-${CPACK_PACKAGE_VERSION_MAJOR}\\\\.${CPACK_PACKAGE_VERSION_MINOR}\\\\.${CPACK_PACKAGE_VERSION_PATCH}-Linux\\\\.deb"
				"/glip-compute-${CPACK_PACKAGE_VERSION_MAJOR}\\\\.${CPACK_PACKAGE_VERSION_MINOR}\\\\.${CPACK_PACKAGE_VERSION_PATCH}-Linux\\\\.rpm"
				"/glip-compute"
				"/Makefile"
				"/CMakeFiles/"
				"/_CPack_Packages/")

# Debian package :
find_program(testDPKG dpkg)
if(testDPKG)
	execute_process(COMMAND dpkg --print-architecture COMMAND tr -d '\n' OUTPUT_VARIABLE DEB_ARCHITECTURE)#Get the architecture.
	set(CPACK_GENERATOR "DEB")
	set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Ronan Kerviche")
	set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${DEB_ARCHITECTURE}")
	set(CPACK_DEBIAN_PACKAGE_DEPENDS "glip-lib (>=1.0.0)")
endif()

find_program(testRPM rpm)
if(testRPM)
	set(CPACK_RPM_COMPONENT_INSTALL ON) #Generate multiple packages.
	set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
	set(CPACK_GENERATOR "RPM")
endif()

# Install :
install(
	TARGETS glip-compute
	DESTINATION bin
)

include(CPack)

