cmake_minimum_required(VERSION 2.6)

#Project name
project(GLIP)

set(CMAKE_CXX_FLAGS -I/usr/include/unicap)

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
