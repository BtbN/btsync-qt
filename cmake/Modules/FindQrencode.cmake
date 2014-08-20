# Once done these will be defined:
#
#  QRENCODE_FOUND
#  QRENCODE_INCLUDE_DIRS
#  QRENCODE_LIBRARIES
#

if(QRENCODE_INCLUDE_DIRS AND QRENCODE_LIBRARIES)
	set(QRENCODE_FOUND TRUE)
else()
	find_package(PkgConfig QUIET)
	if (PKG_CONFIG_FOUND)
		pkg_check_modules(_QRENCODE QUIET libqrencode)
	endif()

	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(_lib_suffix 64)
	else()
		set(_lib_suffix 32)
	endif()

	find_path(Qrencode_INCLUDE_DIR
		NAMES qrencode.h
		HINTS
			${_QRENCODE_INCLUDE_DIRS}
			ENV QrencodePath
			ENV QrencodePath${_lib_suffix}
		PATHS
			/usr/include /usr/local/include /opt/local/include /sw/include)

	find_library(Qrencode_LIB
		NAMES ${_QRENCODE_LIBRARIES} qrencode
		HINTS
			${_QRENCODE_LIBRARY_DIRS}
			"${Qrencode_INCLUDE_DIR}/../lib"
			"${Qrencode_INCLUDE_DIR}/../lib${_lib_suffix}"
			"${Qrencode_INCLUDE_DIR}/lib"
			"${Qrencode_INCLUDE_DIR}/lib${_lib_suffix}"
		PATHS
			/usr/lib /usr/local/lib /opt/local/lib /sw/lib)

	set(QRENCODE_INCLUDE_DIRS ${Qrencode_INCLUDE_DIR} CACHE PATH "Qrencode include dir")
	set(QRENCODE_LIBRARIES ${Qrencode_LIB} CACHE STRING "Qrencode libraries")

	find_package_handle_standard_args(Qrencode DEFAULT_MSG Qrencode_LIB Qrencode_INCLUDE_DIR)
	mark_as_advanced(Qrencode_INCLUDE_DIR Qrencode_LIB)
endif()

