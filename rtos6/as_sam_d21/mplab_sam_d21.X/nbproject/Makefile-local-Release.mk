#
# Generated Makefile - do not edit!
#
#
# This file contains information about the location of compilers and other tools.
# If you commmit this file into your revision control server, you will be able to 
# to checkout the project and build it from the command line with make. However,
# if more than one person works on the same project, then this file might show
# conflicts since different users are bound to have compilers in different places.
# In that case you might choose to not commit this file and let MPLAB X recreate this file
# for each user. The disadvantage of not commiting this file is that you must run MPLAB X at
# least once so the file gets created and the project can be built. Finally, you can also
# avoid using this file at all if you are only building from the command line with make.
# You can invoke make with the values of the macros:
# $ makeMP_CC="/opt/microchip/mplabc30/v3.30c/bin/pic30-gcc" ...  
#
SHELL=cmd.exe
PATH_TO_IDE_BIN=C:/Program Files/Microchip/MPLABX/v6.20/mplab_platform/platform/../mplab_ide/modules/../../bin/
# Adding MPLAB X bin directory to path.
PATH:=C:/Program Files/Microchip/MPLABX/v6.20/mplab_platform/platform/../mplab_ide/modules/../../bin/:$(PATH)
# Path to java used to run MPLAB X when this makefile was created
MP_JAVA_PATH="C:\Program Files\Microchip\MPLABX\v6.20\sys\java\zulu8.64.0.19-ca-fx-jre8.0.345-win_x64/bin/"
OS_CURRENT="$(shell uname -s)"
MP_CC="C:\GNU_Arm_Embedded_Toolchain\bin\arm-none-eabi-gcc.exe"
MP_CPPC="C:\GNU_Arm_Embedded_Toolchain\bin\arm-none-eabi-g++.exe"
# MP_BC is not defined
MP_AS="C:\GNU_Arm_Embedded_Toolchain\bin\arm-none-eabi-as.exe"
MP_LD="C:\GNU_Arm_Embedded_Toolchain\bin\arm-none-eabi-ld.exe"
MP_AR="C:\GNU_Arm_Embedded_Toolchain\bin\arm-none-eabi-ar.exe"
DEP_GEN=${MP_JAVA_PATH}java -jar "C:/Program Files/Microchip/MPLABX/v6.20/mplab_platform/platform/../mplab_ide/modules/../../bin/extractobjectdependencies.jar"
MP_CC_DIR="C:\GNU_Arm_Embedded_Toolchain\bin"
MP_CPPC_DIR="C:\GNU_Arm_Embedded_Toolchain\bin"
# MP_BC_DIR is not defined
MP_AS_DIR="C:\GNU_Arm_Embedded_Toolchain\bin"
MP_LD_DIR="C:\GNU_Arm_Embedded_Toolchain\bin"
MP_AR_DIR="C:\GNU_Arm_Embedded_Toolchain\bin"
DFP_DIR=C:/Program Files/Microchip/MPLABX/v6.20/packs/Microchip/SAMD21_DFP/3.6.144
CMSIS_DIR=C:/Program Files/Microchip/MPLABX/v6.20/packs/arm/CMSIS/5.4.0
