project(libbtsync-qt)

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)

set(libbtsync-qt_SOURCES
	)

set(libbtsync-qt_HEADERS
	)

add_library(libbtsync-qt SHARED
	${libbtsync-qt_SOURCES}
	&{libbtsync-qt_HEADERS})
target_link_libraries(libbtsync-qt
	Qt5::Core
	Qt5::Network)
