# This file configures the compiler to use with CMake.

set(CMAKE_C_COMPILER_WORKS YES CACHE BOOL "Tell CMake that the compiler works, but cannot be run during the configuration stage")
set(MP_CC "/home/thomas/.mplab/app-finder/apps/arm-gcc/v14.2.1/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-gcc" CACHE PATH "Legacy variable from MPLAB X pointing to the compiler")
set(MP_CC_DIR "/home/thomas/.mplab/app-finder/apps/arm-gcc/v14.2.1/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi/bin" CACHE PATH "Legacy variable from MPLAB X pointing to the compiler base directory")
set(CMAKE_C_COMPILER "/home/thomas/.mplab/app-finder/apps/arm-gcc/v14.2.1/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-gcc" CACHE FILEPATH "Path to the compiler binary")

set(CMAKE_CXX_COMPILER_WORKS YES CACHE BOOL "Tell CMake that the compiler works, but cannot be run during the configuration stage")
set(MP_CPPC "/home/thomas/.mplab/app-finder/apps/arm-gcc/v14.2.1/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-g++" CACHE PATH "Legacy variable from MPLAB X pointing to the c++ compiler")
set(MP_CPPC_DIR "/home/thomas/.mplab/app-finder/apps/arm-gcc/v14.2.1/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi/bin" CACHE PATH "Legacy variable from MPLAB X pointing to the c++ compiler base directory")
set(CMAKE_CXX_COMPILER "/home/thomas/.mplab/app-finder/apps/arm-gcc/v14.2.1/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-g++" CACHE FILEPATH "Path to the compiler binary.")
set(MP_CPPC_DIR "${CMAKE_CXX_COMPILER}" CACHE PATH "Legacy variable from MPLAB X pointing to the compiler base directory.")

set(CMAKE_ASM_COMPILER_WORKS YES CACHE BOOL "Tell CMake that the assembler works, but cannot be run during the configuration stage")
set(MP_AS "/home/thomas/.mplab/app-finder/apps/arm-gcc/v14.2.1/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-gcc" CACHE PATH "Legacy variable from MPLAB X pointing to the assembler")
set(MP_AS_DIR "/home/thomas/.mplab/app-finder/apps/arm-gcc/v14.2.1/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi/bin" CACHE PATH "Legacy variable from MPLAB X pointing to the assembler base directory")
set(CMAKE_ASM_COMPILER "/home/thomas/.mplab/app-finder/apps/arm-gcc/v14.2.1/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-gcc" CACHE FILEPATH "Path to the compiler binary.")
set(MP_AS "${CMAKE_ASM_COMPILER}" CACHE FILEPATH "Legacy variable from MPLAB X pointing to the assembler binary.")

set(MP_LD "/home/thomas/.mplab/app-finder/apps/arm-gcc/v14.2.1/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-ld" CACHE FILEPATH "Legacy variable from MPLAB X pointing to the linker binary.")
set(MP_LD_DIR "/home/thomas/.mplab/app-finder/apps/arm-gcc/v14.2.1/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi/bin" CACHE PATH "Legacy variable from MPLAB X pointing to the linker base directory")

set(MP_AR "/home/thomas/.mplab/app-finder/apps/arm-gcc/v14.2.1/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-ar" CACHE FILEPATH "Legacy variable from MPLAB X pointing to the archiver binary.")
set(MP_AR_DIR "/home/thomas/.mplab/app-finder/apps/arm-gcc/v14.2.1/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi/bin" CACHE PATH "Legacy variable from MPLAB X pointing to the archiver base directory")

set(CMAKE_AR "/home/thomas/.mplab/app-finder/apps/arm-gcc/v14.2.1/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-ar" CACHE FILEPATH "Path to the archiver binary.")

set(OBJCOPY "/home/thomas/.mplab/app-finder/apps/arm-gcc/v14.2.1/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-objcopy" CACHE FILEPATH "Path to objcopy executable")
set(OBJDUMP "/home/thomas/.mplab/app-finder/apps/arm-gcc/v14.2.1/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-objdump" CACHE FILEPATH "Path to objdump executable")

# Extend the object path max if the OS is capable and it looks like the toolchain supports it
# See https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation
if(WIN32 AND NOT COMPILER_IS_LONG_PATH_AWARE)
    cmake_host_system_information(
        RESULT LONG_PATHS_ENABLED
        QUERY WINDOWS_REGISTRY "HKEY_LOCAL_MACHINE/SYSTEM/CurrentControlSet/Control/FileSystem"
        VALUE "LongPathsEnabled")
    if(${CMAKE_MAKE_PROGRAM} MATCHES ".*(make|make.exe)")
        message(STATUS "Make does not work well with long paths, leaving it low")
    elseif(${LONG_PATHS_ENABLED} STREQUAL "1")
        set(CMAKE_C_COMPILER_EXE_MANIFEST_FILE "${CMAKE_C_COMPILER}.manifest")
        if(EXISTS "${CMAKE_C_COMPILER_EXE_MANIFEST_FILE}")
            file( READ "${CMAKE_C_COMPILER_EXE_MANIFEST_FILE}" CMAKE_C_COMPILER_EXE_MANIFEST)
            if(${CMAKE_C_COMPILER_EXE_MANIFEST} MATCHES ".*longPathAware>\\s*true</.*")
                message(STATUS "Windows is configured with LongPathsEnabled, and the compiler has a .manifest with longPathAware=true")
                set(CMAKE_OBJECT_PATH_MAX 32767)
            endif()
        else()
            message(STATUS "Windows is configured with LongPathsEnabled, however the compiler is not")
        endif()
    else()
        message(STATUS "Windows is not configured with LongPathsEnabled, see https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation")
    endif()
endif()

# Do not detect ABI for C, see https://gitlab.kitware.com/cmake/cmake/-/issues/25936
set(CMAKE_C_ABI_COMPILED YES)
# Do not detect ABI for CXX, see https://gitlab.kitware.com/cmake/cmake/-/issues/25936
set(CMAKE_CXX_ABI_COMPILED YES)
# Do not detect ABI for ASM, see https://gitlab.kitware.com/cmake/cmake/-/issues/25936
set(CMAKE_ASM_ABI_COMPILED YES)

# For makefiles, change the search order of make executable so we do not pick up gmake
if("${CMAKE_GENERATOR}" STREQUAL "Unix Makefiles")
    find_program(CMAKE_MAKE_PROGRAM
        NAMES make
        DOC "Find a suitable make, avoid gmake")
endif()

