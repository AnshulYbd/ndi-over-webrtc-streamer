cmake_minimum_required(VERSION 3.7)


SET(3RD_PARTY           ${CMAKE_CURRENT_SOURCE_DIR}/../../3rdParty)
SET(LIBS_3RD_PARTY_NDI_PATH    	${3RD_PARTY}/libs/ndi)
SET(LIBS_3RD_PARTY_DC_PATH    	${3RD_PARTY}/libs/libdc)

SET(INCLUDE_3RD_PARTY_NDI   	${3RD_PARTY}/headers/ndi)
SET(INCLUDE_3RD_PARTY_JSON   	${3RD_PARTY}/headers/json/include)


INCLUDE_DIRECTORIES(${INCLUDE_3RD_PARTY_NDI}
					${CMAKE_CURRENT_SOURCE_DIR}/include
					${INCLUDE_3RD_PARTY_JSON}
					)
SET(LIBS_3RD_PARTY_NDI_WIN_LIB      ${LIBS_3RD_PARTY_NDI_PATH}/Processing.NDI.Lib.Advanced.x64.lib)
SET(LIBS_3RD_PARTY_NDI_LINUX        ${LIBS_3RD_PARTY_NDI_PATH}/libndi-x86_64.so.5)
SET(LIBS_3RD_PARTY_DC_WIN      		${LIBS_3RD_PARTY_DC_PATH}/datachannel.lib)
SET(LIBS_3RD_PARTY_DC_LINUX        ${LIBS_3RD_PARTY_DC_PATH}/libdatachannel.so)

SET(DLL_PATH_DC ${LIBS_3RD_PARTY_DC_PATH}/datachannel.dll)
SET(DLL_PATH_NDI ${LIBS_3RD_PARTY_NDI_PATH}/Processing.NDI.Lib.Advanced.x64.dll)

if (MSVC)
	SET(LIB_NDI_SHARED	${LIBS_3RD_PARTY_NDI_WIN_LIB})
	SET(LIB_DC_SHARED	${LIBS_3RD_PARTY_DC_WIN})
else()
	SET(LIB_NDI_SHARED	${LIBS_3RD_PARTY_NDI_LINUX})
	SET(LIB_DC_SHARED	${LIBS_3RD_PARTY_DC_LINUX})

endif()

#supplied libraries
add_library(libndi STATIC IMPORTED)
set_property(TARGET libndi PROPERTY IMPORTED_LOCATION ${LIB_NDI_SHARED})

#supplied libraries
add_library(libdc STATIC IMPORTED)
set_property(TARGET libdc PROPERTY IMPORTED_LOCATION ${LIB_DC_SHARED})

set(STREAMER_SOURCES
	main.cpp
	dispatchqueue.cpp
	dispatchqueue.hpp
	h264fileparser.cpp
	h264fileparser.hpp
	helpers.cpp
	helpers.hpp
	opusfileparser.cpp
	opusfileparser.hpp
	fileparser.cpp
	fileparser.hpp
	stream.cpp
	stream.hpp
	ArgParser.cpp
	ArgParser.hpp
)

set(STREAMER_UWP_RESOURCES
	uwp/Logo.png
	uwp/package.appxManifest
	uwp/SmallLogo.png
	uwp/SmallLogo44x44.png
	uwp/SplashScreen.png
	uwp/StoreLogo.png
	uwp/Windows_TemporaryKey.pfx
)

if(CMAKE_SYSTEM_NAME STREQUAL "WindowsStore")
	add_executable(BDWebrtcStreamer ${STREAMER_SOURCES} ${STREAMER_UWP_RESOURCES})
else()
	add_executable(BDWebrtcStreamer ${STREAMER_SOURCES})
endif()
set_target_properties(BDWebrtcStreamer PROPERTIES
	CXX_STANDARD 17
	CMAKE_CXX_STANDARD 17
	OUTPUT_NAME BDWebrtcStreamer)

if(MSVC)
    target_compile_definitions( BDWebrtcStreamer PRIVATE WIN32;_WINDOWS;WIN32_LEAN_AND_MEAN;NOMINMAX;_CRT_SECURE_NO_WARNINGS;_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING;RTC_ENABLE_WEBSOCKET=1;RTC_ENABLE_MEDIA=1;JSON_USE_IMPLICIT_CONVERSIONS=1;JSON_DIAGNOSTICS=0;CMAKE_INTDIR="Debug";PRINT_NALU=0 )
	SET(WIN_LIBS ws2_32.lib)
else()
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

find_package(Threads REQUIRED)
target_link_libraries(BDWebrtcStreamer ${WINLIBS} libndi libdc)

if(MSVC)
	add_custom_command(TARGET BDWebrtcStreamer POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${DLL_PATH_DC}
		$<TARGET_FILE_DIR:BDWebrtcStreamer>
	)
	add_custom_command(TARGET BDWebrtcStreamer POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	${DLL_PATH_NDI}
	$<TARGET_FILE_DIR:BDWebrtcStreamer>
)
endif()