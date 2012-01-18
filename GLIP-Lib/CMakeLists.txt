cmake_minimum_required(VERSION 2.6)

#Project name
project(GLIP)

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
else() # Linux psecifics
endif()

set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

include_directories(
		./include/
		./include/Core/
		./include/Modules/
		)

#Génération de la liste des fichiers sources
file(
		GLOB_RECURSE
		source_files
		src/*
		include/*
)

#Déclaration de l'exécutable
add_library(
		glip
		STATIC
		${source_files}
)
