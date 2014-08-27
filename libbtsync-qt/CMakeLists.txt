project(libbtsync-qt)

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)

set(libbtsync-qt_SOURCES
	src/bts_global.cpp
	src/bts_client.cpp
	src/bts_spawnclient.cpp
	src/bts_remoteclient.cpp
	src/bts_api.cpp
	src/bts_apifolder.cpp
	)

set(libbtsync-qt_HEADERS
	include/bts_export_helper.h
	include/bts_global.h
	include/bts_client.h
	include/bts_spawnclient.h
	include/bts_remoteclient.h
	include/bts_api.h
	include/bts_apifolder.h
	)

add_library(libbtsync-qt SHARED
	${libbtsync-qt_SOURCES}
	${libbtsync-qt_HEADERS})
target_link_libraries(libbtsync-qt
	PUBLIC
		Qt5::Core
	PRIVATE
		Qt5::Network)
target_include_directories(libbtsync-qt
	PUBLIC 
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
		"$<INSTALL_INTERFACE:include/libbtsync-qt>")
set_target_properties(libbtsync-qt PROPERTIES
	OUTPUT_NAME btsync-qt
	VERSION 0
	SOVERSION 0)

set(LIBBTS_VERSION "0.1")

export(TARGETS libbtsync-qt FILE "${CMAKE_CURRENT_BINARY_DIR}/LibBtsyncQtTarget.cmake")
export(PACKAGE LibBtsyncQt)

configure_file("LibBtsyncQtConfig.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/LibBtsyncQtConfig.cmake" @ONLY)
configure_file("LibBtsyncQtConfigVersion.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/LibBtsyncQtConfigVersion.cmake" @ONLY)

install(FILES ${libbtsync-qt_HEADERS}
	DESTINATION "include/libbtsync-qt"
	COMPONENT dev)
install(FILES
	"${CMAKE_CURRENT_BINARY_DIR}/LibBtsyncQtConfig.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/LibBtsyncQtConfigVersion.cmake"
	DESTINATION "lib${INSTALL_LIBDIR_SUFFIX}/cmake/LibBtsyncQt"
	COMPONENT dev)
install(TARGETS libbtsync-qt
	EXPORT LibBtsyncQtTarget
	LIBRARY DESTINATION "lib${INSTALL_LIBDIR_SUFFIX}"
		COMPONENT lib
	ARCHIVE DESTINATION "lib${INSTALL_LIBDIR_SUFFIX}"
		COMPONENT lib
	RUNTIME DESTINATION "lib${INSTALL_LIBDIR_SUFFIX}"
		COMPONENT lib)
install(EXPORT LibBtsyncQtTarget
	DESTINATION "lib${INSTALL_LIBDIR_SUFFIX}/cmake/LibBtsyncQt"
	COMPONENT dev)
