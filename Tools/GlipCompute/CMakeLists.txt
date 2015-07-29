cmake_minimum_required(VERSION 2.8)

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
)

# Executable : 
add_executable(
		glip-compute
		${source_files}
)

# Link : 
target_link_libraries(glip-compute X11 GL freeimageplus glip)

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

