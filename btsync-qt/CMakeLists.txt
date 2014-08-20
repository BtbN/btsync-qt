project(btsync-qt)

find_package(Qt5Widgets REQUIRED)
find_package(Qrencode REQUIRED)

if(UNIX)
	set(btsync-qt_UNIX_SOURCES
		src/sighandler.cpp
		src/sighandler.h
		)
endif()

set(btsync-qt_SOURCES
	src/main.cpp
	src/mainwin.cpp
	src/mainwin.h
	${btsync-qt_UNIX_SOURCES}
	)

set(btsync-qt_UI
	ui/mainwin.ui
	ui/devices.ui
	ui/sharedFolders.ui
	ui/transfers.ui
	ui/history.ui
	ui/preferences.ui
	)

qt5_wrap_ui(btsync-qt_UI_HEADERS ${btsync-qt_UI})

add_executable(btsync-qt WIN32
	${btsync-qt_SOURCES}
	${btsync-qt_UI_HEADERS})
target_link_libraries(btsync-qt
	libbtsync-qt
	Qt5::Widgets
	${QRENCODE_LIBRARIES})
