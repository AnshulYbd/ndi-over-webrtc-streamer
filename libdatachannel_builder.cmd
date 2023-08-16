echo off
set root_dir=%cd%
path|find "%root_dir%\BuildTools\7z-win"    >nul || set path=%path%;%root_dir%\BuildTools\7z-win
set EXRACT_BIN="7z.exe"

set TARGET_ARCH=arm-linux-gnueabihf
@REM set GCC_NAME=gcc-linaro-7.5.0-2019.12-i686-mingw32_%TARGET_ARCH%
set GCC_NAME=gcc-arm-10.3-2021.07-mingw-w64-i686-arm-none-linux-gnueabihf
set GCC_PREFIX=arm-none-linux-gnueabihf

set GCC_REGULAR_PATH=%root_dir%\toolchains\%GCC_NAME%
if exist "%GCC_REGULAR_PATH%" (
    echo "tool chain path gcc present"
) else (
    echo "Downloading tar ball now"
	mkdir %root_dir%\toolchains
	cd %root_dir%\toolchains
    IF "%TARGET_ARCH%"=="aarch64-linux-gnu" (
        wget https://releases.linaro.org/components/toolchain/binaries/latest-7/aarch64-linux-gnu/gcc-linaro-7.5.0-2019.12-i686-mingw32_aarch64-linux-gnu.tar.xz
    )
    IF "%TARGET_ARCH%"=="arm-linux-gnueabihf" (
        @REM wget https://releases.linaro.org/components/toolchain/binaries/latest-7/arm-linux-gnueabihf/gcc-linaro-7.5.0-2019.12-i686-mingw32_arm-linux-gnueabihf.tar.xz
		@REM wget https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-a/10.3-2021.07/binrel/gcc-arm-10.3-2021.07-mingw-w64-i686-arm-none-linux-gnueabihf.tar.xz
    )

	%EXRACT_BIN% x %GCC_NAME%.tar.xz -so | %EXRACT_BIN% x -si -y -ttar
)

SUBST T: /D
SUBST T: %GCC_REGULAR_PATH%
set GCC_PATH=T:
rm -rf %root_dir%/build
rm -rf %root_dir%/deps
cd %root_dir%
git submodule update --init --recursive --depth 1
copy /Y .\BuildTools\libsrtp_CMakeLists.txt .\deps\libsrtp\CMakeLists.txt
copy /Y .\BuildTools\usrsctp_CMakeLists.txt .\deps\usrsctp\CMakeLists.txt

@rem cmake -B build -DUSE_GNUTLS=0 -DUSE_NICE=0 -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=automate.cmake
cmake -B build -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=automate.cmake
cd ./build
cmake --build .
cd ..

SUBST T: /D
