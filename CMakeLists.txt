CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

PROJECT(
    BIRDDOG_NDI_OVER_WEBRTC_PROCESSOR
    VERSION 1.0.0
    DESCRIPTION "BirdDog WebRTC support(NDI over WEBRTC)."
    LANGUAGES C CXX
)

EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCH )
message ("-- ARCH : ${ARCH}")

string(COMPARE EQUAL "${ARCH}" armv7l _cmparm32lo)
string(COMPARE EQUAL "${ARCH}" aarch64 _cmparm64lo)
string(COMPARE EQUAL "${ARCH}" x86_64 _cmpx64lo)
string(COMPARE EQUAL "${ARCH}" x86 _cmpx86lo)

if((NOT _cmparm32lo) AND (NOT _cmparm64lo) AND (NOT _cmpx64lo) AND (NOT _cmpx86lo))
    message ("Unsupported archetecture. Exiting")
endif()

SET(LIBS_3RD_PARTY      ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/libs)

if(MSVC)
    add_compile_definitions( BIRDDOG_SRT_PROCESSOR PRIVATE WIN32_APP=1)
else()
    add_compile_definitions( BIRDDOG_SRT_PROCESSOR PRIVATE LINUX_APP=1)
endif()

add_subdirectory(app/src)