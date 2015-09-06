if(OPENGLES_INCLUDE_DIR)
	# Already in the cache, be silent :
	set(OPENGLES_FIND_QUIETLY TRUE)
else()
	find_path(OPENGLES_INCLUDE_DIR GLES/gl.h)
	find_library(OPENGLES_LIBRARY NAMES GLESv1_CM)

	include(FindPackageHandleStandardArgs)
	find_package_handle_standard_args(OPENGLES DEFAULT_MSG OPENGLES_INCLUDE_DIR OPENGLES_LIBRARY)
	
	set(OPENGLES_LIBRARIES ${OPENGLES_LIBRARY})

	mark_as_advanced(OPENGLES_INCLUDE_DIR)
	mark_as_advanced(OPENGLES_LIBRARY)
endif()

