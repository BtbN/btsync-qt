project(btsync-qt)

find_package(Qt5Widgets REQUIRED)

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
	src/deviceswidget.cpp
	src/deviceswidget.h
	src/historywidget.cpp
	src/historywidget.h
	src/preferenceswidget.cpp
	src/preferenceswidget.h
	src/sharedfolderswidget.cpp
	src/sharedfolderswidget.h
	src/transferswidget.cpp
	src/transferswidget.h
	src/addfolderdialog.cpp
	src/addfolderdialog.h
	src/folderinfodialog.cpp
	src/folderinfodialog.h
	src/qrcodewidget.cpp
	src/qrcodewidget.h
	${btsync-qt_UNIX_SOURCES}
	)

set(btsync-qt_UI
	ui/mainwin.ui
	ui/devices.ui
	ui/sharedfolders.ui
	ui/transfers.ui
	ui/history.ui
	ui/preferences.ui
	ui/addfolderdialog.ui
	ui/folderinfo.ui
	)

qt5_wrap_ui(btsync-qt_UI_HEADERS ${btsync-qt_UI})

add_executable(btsync-qt WIN32
	${btsync-qt_SOURCES}
	${btsync-qt_UI_HEADERS})
target_link_libraries(btsync-qt
	libbtsync-qt
	qrencode
	Qt5::Widgets)
target_include_directories(btsync-qt
	PRIVATE
		src)
