cmake_minimum_required(VERSION 2.6)

#Project name
project(GLIP-LIB)

set(CMAKE_BUILD_TYPE Release)

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
	#set(CMAKE_CXX_FLAGS "")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

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

# Executable : 
add_library(
		glip
		SHARED
		${source_files}
)

# Install :
install(
	TARGETS glip
 	DESTINATION lib
)
install(
	FILES ${header_files_base}
	DESTINATION include/glip-lib
)
install(
	FILES ${header_files_core}
	DESTINATION include/glip-lib/Core
)
install(
	FILES ${header_files_modules}
	DESTINATION include/glip-lib/Modules
)

# Packaging :
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "GLIP-Lib, An OpenGL Image Processing Library.")
set(CPACK_PACKAGE_VENDOR "Ronan Kerviche")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")

# Debian package :
execute_process(COMMAND dpkg --print-architecture COMMAND tr -d '\n' OUTPUT_VARIABLE DEB_ARCHITECTURE )#Get the architecture.
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Ronan Kerviche")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${DEB_ARCHITECTURE}") #To be improved.

include(CPack)

