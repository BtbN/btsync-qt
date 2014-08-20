project(qrencode)

set(qrencode_SOURCES
	include/bitstream.h
	include/mask.h
	include/mmask.h
	include/mqrspec.h
	include/qrencode.h
	include/qrencode_inner.h
	include/qrinput.h
	include/qrspec.h
	include/rsecc.h
	include/split.h
	src/bitstream.c
	src/mask.c
	src/mmask.c
	src/mqrspec.c
	src/qrencode.c
	src/qrinput.c
	src/qrspec.c
	src/rsecc.c
	src/split.c
	src/config.h)

add_library(qrencode STATIC
	${qrencode_SOURCES})
target_link_libraries(qrencode)
target_include_directories(qrencode
	PUBLIC
		include)
target_compile_definitions(qrencode
	PRIVATE
		-DHAVE_CONFIG_H=1)

