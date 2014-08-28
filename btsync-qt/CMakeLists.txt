project(btsyncqt)

find_package(Qt5Widgets REQUIRED)

if(UNIX)
	set(btsyncqt_PLATFORM_SOURCES
		src/sighandler.cpp
		src/sighandler.h
		)
elseif(WIN32)
	set(btsyncqt_PLATFORM_SOURCES
		res/icon.rc
		)
endif()

set(btsyncqt_SOURCES
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
	src/utils.cpp
	src/utils.h
	${btsyncqt_PLATFORM_SOURCES}
	)

set(btsyncqt_UI
	ui/mainwin.ui
	ui/devices.ui
	ui/sharedfolders.ui
	ui/transfers.ui
	ui/history.ui
	ui/preferences.ui
	ui/addfolderdialog.ui
	ui/folderinfo.ui
	)

set(btsyncqt_QRC
	res/res.qrc
	)

qt5_wrap_ui(btsyncqt_UI_SOURCES ${btsyncqt_UI})
qt5_add_resources(btsyncqt_RES_SOURCES ${btsyncqt_QRC})

add_executable(btsyncqt WIN32
	${btsyncqt_SOURCES}
	${btsyncqt_UI_SOURCES}
	${btsyncqt_RES_SOURCES})
target_link_libraries(btsyncqt
	libbtsync-qt
	qrencode
	Qt5::Widgets)
target_include_directories(btsyncqt
	PRIVATE
		src)

set(BIN_PREFIX "${CMAKE_INSTALL_PREFIX}/bin${INSTALL_BINDIR_SUFFIX}")
configure_file("btsyncqt.desktop.in" "${CMAKE_CURRENT_BINARY_DIR}/btsyncqt.desktop" @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/btsyncqt.desktop"
	DESTINATION "share/applications"
	COMPONENT gui)
install(FILES "res/btsync.png"
	DESTINATION "share/icons/hicolor/256x256/apps"
	COMPONENT gui
	RENAME "btsyncqt.png")
install(TARGETS btsyncqt
	LIBRARY DESTINATION "bin${INSTALL_BINDIR_SUFFIX}"
		COMPONENT gui
	ARCHIVE DESTINATION "bin${INSTALL_BINDIR_SUFFIX}"
		COMPONENT gui
	RUNTIME DESTINATION "bin${INSTALL_BINDIR_SUFFIX}"
		COMPONENT gui)
