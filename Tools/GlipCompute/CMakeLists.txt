# When using QT, make sure it includes the library before OpenGL!

cmake_minimum_required(VERSION 2.8)

# Remove relative path warnings :
cmake_policy(SET CMP0015 NEW)

#Project name
project(GLIPCOMPUTE)

set(CMAKE_BUILD_TYPE Release)

# To track the compilation process
#set(CMAKE_VERBOSE_MAKEFILE on )

# We are silencing the long long warning here.
# To be improved in a future release.
set(CMAKE_CXX_FLAGS "-pedantic -Wall -Wextra -Wno-long-long")

if(WIN32) # Windows specifics
	# MAKE SURE THAT YOUR PATH CONTAINS THE DIRECTORY TO ALL MINGW BINARIES

	# Set the default pathes, should be changed to match your setup.
	set(WIN32_BASE C:/Windows/System32)

	# Naming : (The prefix that appears before each of the programs name in your Mingw32 intallation directory)
	set(MINGW_PREFIX "mingw32-")

	# Set mingw defaults
	set(CMAKE_C_COMPILER   ${MINGW_PREFIX}gcc)
	set(CMAKE_CXX_COMPILER ${MINGW_PREFIX}g++)
	set(CMAKE_AR           ar)
	set(CMAKE_RANLIB       ranlib)
	set(CMAKE_LINKER       ld)
	set(CMAKE_STRIP        strip)
	set(CMAKE_MAKE_PROGRAM ${MINGW_PREFIX}make)

	set(CMAKE_CXX_FLAGS "-lgcc -lmingw32 -lglu32 -lopengl32")

else() # Linux specifics
	set(CMAKE_CXX_FLAGS "")
endif()

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

