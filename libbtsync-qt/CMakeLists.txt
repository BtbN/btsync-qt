project(libbtsync-qt)

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)

set(libbtsync-qt_SOURCES
	src/bts_global.cpp
	src/bts_client.cpp
	src/bts_spawnclient.cpp
	src/bts_remoteclient.cpp
	src/bts_api.cpp
	)

set(libbtsync-qt_HEADERS
	include/bts_export_helper.h
	include/bts_global.h
	include/bts_client.h
	include/bts_spawnclient.h
	include/bts_remoteclient.h
	include/bts_api.h
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
	PUBLIC include)
set_target_properties(libbtsync-qt PROPERTIES
	OUTPUT_NAME btsync-qt
	VERSION 0
	SOVERSION 0)
