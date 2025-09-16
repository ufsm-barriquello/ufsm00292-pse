#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-Release.mk)" "nbproject/Makefile-local-Release.mk"
include nbproject/Makefile-local-Release.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=Release
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/mplab_sam_r21.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/mplab_sam_r21.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=
else
COMPARISON_BUILD=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../src/ASF/common/utils/interrupt/interrupt_sam_nvic.c ../src/ASF/sam0/boards/samr21_xplained_pro/board_init.c ../src/ASF/sam0/drivers/port/port.c ../src/ASF/sam0/drivers/system/clock/clock_samd21_r21_da_ha1/clock.c ../src/ASF/sam0/drivers/system/clock/clock_samd21_r21_da_ha1/gclk.c ../src/ASF/sam0/drivers/system/interrupt/system_interrupt.c ../src/ASF/sam0/drivers/system/pinmux/pinmux.c ../src/ASF/sam0/drivers/system/system.c ../src/ASF/sam0/utils/cmsis/samr21/source/gcc/startup_samr21.c ../src/ASF/sam0/utils/cmsis/samr21/source/system_samr21.c ../src/ASF/sam0/utils/syscalls/gcc/syscalls.c ../src/cpu-port.c ../src/rtos.c ../src/main.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1939168694/interrupt_sam_nvic.o ${OBJECTDIR}/_ext/1350218445/board_init.o ${OBJECTDIR}/_ext/876312722/port.o ${OBJECTDIR}/_ext/1900123450/clock.o ${OBJECTDIR}/_ext/1900123450/gclk.o ${OBJECTDIR}/_ext/1234282992/system_interrupt.o ${OBJECTDIR}/_ext/980481618/pinmux.o ${OBJECTDIR}/_ext/227780132/system.o ${OBJECTDIR}/_ext/386045837/startup_samr21.o ${OBJECTDIR}/_ext/2084470165/system_samr21.o ${OBJECTDIR}/_ext/1284275751/syscalls.o ${OBJECTDIR}/_ext/1360937237/cpu-port.o ${OBJECTDIR}/_ext/1360937237/rtos.o ${OBJECTDIR}/_ext/1360937237/main.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1939168694/interrupt_sam_nvic.o.d ${OBJECTDIR}/_ext/1350218445/board_init.o.d ${OBJECTDIR}/_ext/876312722/port.o.d ${OBJECTDIR}/_ext/1900123450/clock.o.d ${OBJECTDIR}/_ext/1900123450/gclk.o.d ${OBJECTDIR}/_ext/1234282992/system_interrupt.o.d ${OBJECTDIR}/_ext/980481618/pinmux.o.d ${OBJECTDIR}/_ext/227780132/system.o.d ${OBJECTDIR}/_ext/386045837/startup_samr21.o.d ${OBJECTDIR}/_ext/2084470165/system_samr21.o.d ${OBJECTDIR}/_ext/1284275751/syscalls.o.d ${OBJECTDIR}/_ext/1360937237/cpu-port.o.d ${OBJECTDIR}/_ext/1360937237/rtos.o.d ${OBJECTDIR}/_ext/1360937237/main.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1939168694/interrupt_sam_nvic.o ${OBJECTDIR}/_ext/1350218445/board_init.o ${OBJECTDIR}/_ext/876312722/port.o ${OBJECTDIR}/_ext/1900123450/clock.o ${OBJECTDIR}/_ext/1900123450/gclk.o ${OBJECTDIR}/_ext/1234282992/system_interrupt.o ${OBJECTDIR}/_ext/980481618/pinmux.o ${OBJECTDIR}/_ext/227780132/system.o ${OBJECTDIR}/_ext/386045837/startup_samr21.o ${OBJECTDIR}/_ext/2084470165/system_samr21.o ${OBJECTDIR}/_ext/1284275751/syscalls.o ${OBJECTDIR}/_ext/1360937237/cpu-port.o ${OBJECTDIR}/_ext/1360937237/rtos.o ${OBJECTDIR}/_ext/1360937237/main.o

# Source Files
SOURCEFILES=../src/ASF/common/utils/interrupt/interrupt_sam_nvic.c ../src/ASF/sam0/boards/samr21_xplained_pro/board_init.c ../src/ASF/sam0/drivers/port/port.c ../src/ASF/sam0/drivers/system/clock/clock_samd21_r21_da_ha1/clock.c ../src/ASF/sam0/drivers/system/clock/clock_samd21_r21_da_ha1/gclk.c ../src/ASF/sam0/drivers/system/interrupt/system_interrupt.c ../src/ASF/sam0/drivers/system/pinmux/pinmux.c ../src/ASF/sam0/drivers/system/system.c ../src/ASF/sam0/utils/cmsis/samr21/source/gcc/startup_samr21.c ../src/ASF/sam0/utils/cmsis/samr21/source/system_samr21.c ../src/ASF/sam0/utils/syscalls/gcc/syscalls.c ../src/cpu-port.c ../src/rtos.c ../src/main.c

# Pack Options 
PACK_COMMON_OPTIONS=-I "${DFP_DIR}/include"  -I "${CMSIS_DIR}/CMSIS/Core/Include"



CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-Release.mk ${DISTDIR}/mplab_sam_r21.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=SAMR21G18A
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1939168694/interrupt_sam_nvic.o: ../src/ASF/common/utils/interrupt/interrupt_sam_nvic.c  .generated_files/flags/Release/2078efb76b64462fd21119c5123ce275cfc4c472 .generated_files/flags/Release/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1939168694" 
	@${RM} ${OBJECTDIR}/_ext/1939168694/interrupt_sam_nvic.o.d 
	@${RM} ${OBJECTDIR}/_ext/1939168694/interrupt_sam_nvic.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1939168694/interrupt_sam_nvic.o.d" -o ${OBJECTDIR}/_ext/1939168694/interrupt_sam_nvic.o ../src/ASF/common/utils/interrupt/interrupt_sam_nvic.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1350218445/board_init.o: ../src/ASF/sam0/boards/samr21_xplained_pro/board_init.c  .generated_files/flags/Release/d5518946b542093e1a1b64f97bbd8a186add14ab .generated_files/flags/Release/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1350218445" 
	@${RM} ${OBJECTDIR}/_ext/1350218445/board_init.o.d 
	@${RM} ${OBJECTDIR}/_ext/1350218445/board_init.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1350218445/board_init.o.d" -o ${OBJECTDIR}/_ext/1350218445/board_init.o ../src/ASF/sam0/boards/samr21_xplained_pro/board_init.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/876312722/port.o: ../src/ASF/sam0/drivers/port/port.c  .generated_files/flags/Release/871360fdd9596623b3b5fb4faf338d69a174d551 .generated_files/flags/Release/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/876312722" 
	@${RM} ${OBJECTDIR}/_ext/876312722/port.o.d 
	@${RM} ${OBJECTDIR}/_ext/876312722/port.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/876312722/port.o.d" -o ${OBJECTDIR}/_ext/876312722/port.o ../src/ASF/sam0/drivers/port/port.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1900123450/clock.o: ../src/ASF/sam0/drivers/system/clock/clock_samd21_r21_da_ha1/clock.c  .generated_files/flags/Release/4c513fbfe3d90ecaf49b5d73300366b2c7c5f9f2 .generated_files/flags/Release/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1900123450" 
	@${RM} ${OBJECTDIR}/_ext/1900123450/clock.o.d 
	@${RM} ${OBJECTDIR}/_ext/1900123450/clock.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1900123450/clock.o.d" -o ${OBJECTDIR}/_ext/1900123450/clock.o ../src/ASF/sam0/drivers/system/clock/clock_samd21_r21_da_ha1/clock.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1900123450/gclk.o: ../src/ASF/sam0/drivers/system/clock/clock_samd21_r21_da_ha1/gclk.c  .generated_files/flags/Release/a316aae7beea5ee840601e1cee24bdad7d9cac1c .generated_files/flags/Release/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1900123450" 
	@${RM} ${OBJECTDIR}/_ext/1900123450/gclk.o.d 
	@${RM} ${OBJECTDIR}/_ext/1900123450/gclk.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1900123450/gclk.o.d" -o ${OBJECTDIR}/_ext/1900123450/gclk.o ../src/ASF/sam0/drivers/system/clock/clock_samd21_r21_da_ha1/gclk.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1234282992/system_interrupt.o: ../src/ASF/sam0/drivers/system/interrupt/system_interrupt.c  .generated_files/flags/Release/8f9c74c0ad07db99af33ed7a3fdd1822a10396ae .generated_files/flags/Release/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1234282992" 
	@${RM} ${OBJECTDIR}/_ext/1234282992/system_interrupt.o.d 
	@${RM} ${OBJECTDIR}/_ext/1234282992/system_interrupt.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1234282992/system_interrupt.o.d" -o ${OBJECTDIR}/_ext/1234282992/system_interrupt.o ../src/ASF/sam0/drivers/system/interrupt/system_interrupt.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/980481618/pinmux.o: ../src/ASF/sam0/drivers/system/pinmux/pinmux.c  .generated_files/flags/Release/f1ef772d4a7d22b162d5d8fc2eec1f7be64d5d0 .generated_files/flags/Release/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/980481618" 
	@${RM} ${OBJECTDIR}/_ext/980481618/pinmux.o.d 
	@${RM} ${OBJECTDIR}/_ext/980481618/pinmux.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/980481618/pinmux.o.d" -o ${OBJECTDIR}/_ext/980481618/pinmux.o ../src/ASF/sam0/drivers/system/pinmux/pinmux.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/227780132/system.o: ../src/ASF/sam0/drivers/system/system.c  .generated_files/flags/Release/33c9929e3ad53ba8be5836f1e2837ab214fe6fa6 .generated_files/flags/Release/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/227780132" 
	@${RM} ${OBJECTDIR}/_ext/227780132/system.o.d 
	@${RM} ${OBJECTDIR}/_ext/227780132/system.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/227780132/system.o.d" -o ${OBJECTDIR}/_ext/227780132/system.o ../src/ASF/sam0/drivers/system/system.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/386045837/startup_samr21.o: ../src/ASF/sam0/utils/cmsis/samr21/source/gcc/startup_samr21.c  .generated_files/flags/Release/bde0fc15c9fa0066dbae359ff3586be160d774bf .generated_files/flags/Release/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/386045837" 
	@${RM} ${OBJECTDIR}/_ext/386045837/startup_samr21.o.d 
	@${RM} ${OBJECTDIR}/_ext/386045837/startup_samr21.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/386045837/startup_samr21.o.d" -o ${OBJECTDIR}/_ext/386045837/startup_samr21.o ../src/ASF/sam0/utils/cmsis/samr21/source/gcc/startup_samr21.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/2084470165/system_samr21.o: ../src/ASF/sam0/utils/cmsis/samr21/source/system_samr21.c  .generated_files/flags/Release/a88daf7fe5b7057f4543eb27d59353e9f349bfef .generated_files/flags/Release/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/2084470165" 
	@${RM} ${OBJECTDIR}/_ext/2084470165/system_samr21.o.d 
	@${RM} ${OBJECTDIR}/_ext/2084470165/system_samr21.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/2084470165/system_samr21.o.d" -o ${OBJECTDIR}/_ext/2084470165/system_samr21.o ../src/ASF/sam0/utils/cmsis/samr21/source/system_samr21.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1284275751/syscalls.o: ../src/ASF/sam0/utils/syscalls/gcc/syscalls.c  .generated_files/flags/Release/a5bab57904676ac51ead2bd14bd2c0bea7d37ffb .generated_files/flags/Release/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1284275751" 
	@${RM} ${OBJECTDIR}/_ext/1284275751/syscalls.o.d 
	@${RM} ${OBJECTDIR}/_ext/1284275751/syscalls.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1284275751/syscalls.o.d" -o ${OBJECTDIR}/_ext/1284275751/syscalls.o ../src/ASF/sam0/utils/syscalls/gcc/syscalls.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1360937237/cpu-port.o: ../src/cpu-port.c  .generated_files/flags/Release/6987e7f7121c8d8185e3eae669625693931b7c67 .generated_files/flags/Release/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/cpu-port.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/cpu-port.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/cpu-port.o.d" -o ${OBJECTDIR}/_ext/1360937237/cpu-port.o ../src/cpu-port.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1360937237/rtos.o: ../src/rtos.c  .generated_files/flags/Release/38228ba183e764104ccf8f11885236aa301085d5 .generated_files/flags/Release/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/rtos.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/rtos.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/rtos.o.d" -o ${OBJECTDIR}/_ext/1360937237/rtos.o ../src/rtos.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1360937237/main.o: ../src/main.c  .generated_files/flags/Release/9cc27b6ab072fb96e5756da5b1a44cd53ac29dae .generated_files/flags/Release/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/main.o.d" -o ${OBJECTDIR}/_ext/1360937237/main.o ../src/main.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
else
${OBJECTDIR}/_ext/1939168694/interrupt_sam_nvic.o: ../src/ASF/common/utils/interrupt/interrupt_sam_nvic.c  .generated_files/flags/Release/117a94794982a0a2cd12bd23af5e8a15281e6cf .generated_files/flags/Release/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1939168694" 
	@${RM} ${OBJECTDIR}/_ext/1939168694/interrupt_sam_nvic.o.d 
	@${RM} ${OBJECTDIR}/_ext/1939168694/interrupt_sam_nvic.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1939168694/interrupt_sam_nvic.o.d" -o ${OBJECTDIR}/_ext/1939168694/interrupt_sam_nvic.o ../src/ASF/common/utils/interrupt/interrupt_sam_nvic.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1350218445/board_init.o: ../src/ASF/sam0/boards/samr21_xplained_pro/board_init.c  .generated_files/flags/Release/e46604941d8120db13097b5633cf4696d29610d .generated_files/flags/Release/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1350218445" 
	@${RM} ${OBJECTDIR}/_ext/1350218445/board_init.o.d 
	@${RM} ${OBJECTDIR}/_ext/1350218445/board_init.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1350218445/board_init.o.d" -o ${OBJECTDIR}/_ext/1350218445/board_init.o ../src/ASF/sam0/boards/samr21_xplained_pro/board_init.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/876312722/port.o: ../src/ASF/sam0/drivers/port/port.c  .generated_files/flags/Release/d92b2e69a46b36f078ce5ec151ef580cd9b7ed50 .generated_files/flags/Release/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/876312722" 
	@${RM} ${OBJECTDIR}/_ext/876312722/port.o.d 
	@${RM} ${OBJECTDIR}/_ext/876312722/port.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/876312722/port.o.d" -o ${OBJECTDIR}/_ext/876312722/port.o ../src/ASF/sam0/drivers/port/port.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1900123450/clock.o: ../src/ASF/sam0/drivers/system/clock/clock_samd21_r21_da_ha1/clock.c  .generated_files/flags/Release/ecd47bb00b3b25cb5c79c5aa12490ada0ad5bea2 .generated_files/flags/Release/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1900123450" 
	@${RM} ${OBJECTDIR}/_ext/1900123450/clock.o.d 
	@${RM} ${OBJECTDIR}/_ext/1900123450/clock.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1900123450/clock.o.d" -o ${OBJECTDIR}/_ext/1900123450/clock.o ../src/ASF/sam0/drivers/system/clock/clock_samd21_r21_da_ha1/clock.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1900123450/gclk.o: ../src/ASF/sam0/drivers/system/clock/clock_samd21_r21_da_ha1/gclk.c  .generated_files/flags/Release/2bd1af40da21ac7b4c69df736696e61ddeb5b854 .generated_files/flags/Release/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1900123450" 
	@${RM} ${OBJECTDIR}/_ext/1900123450/gclk.o.d 
	@${RM} ${OBJECTDIR}/_ext/1900123450/gclk.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1900123450/gclk.o.d" -o ${OBJECTDIR}/_ext/1900123450/gclk.o ../src/ASF/sam0/drivers/system/clock/clock_samd21_r21_da_ha1/gclk.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1234282992/system_interrupt.o: ../src/ASF/sam0/drivers/system/interrupt/system_interrupt.c  .generated_files/flags/Release/e4b8681963971a9cdf76d757ff9c77d027cdb392 .generated_files/flags/Release/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1234282992" 
	@${RM} ${OBJECTDIR}/_ext/1234282992/system_interrupt.o.d 
	@${RM} ${OBJECTDIR}/_ext/1234282992/system_interrupt.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1234282992/system_interrupt.o.d" -o ${OBJECTDIR}/_ext/1234282992/system_interrupt.o ../src/ASF/sam0/drivers/system/interrupt/system_interrupt.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/980481618/pinmux.o: ../src/ASF/sam0/drivers/system/pinmux/pinmux.c  .generated_files/flags/Release/83638b59e122a7fc81625d3c74d98285c2043ea3 .generated_files/flags/Release/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/980481618" 
	@${RM} ${OBJECTDIR}/_ext/980481618/pinmux.o.d 
	@${RM} ${OBJECTDIR}/_ext/980481618/pinmux.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/980481618/pinmux.o.d" -o ${OBJECTDIR}/_ext/980481618/pinmux.o ../src/ASF/sam0/drivers/system/pinmux/pinmux.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/227780132/system.o: ../src/ASF/sam0/drivers/system/system.c  .generated_files/flags/Release/210a5118770b3718fd78a3230ae588b30f81dd62 .generated_files/flags/Release/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/227780132" 
	@${RM} ${OBJECTDIR}/_ext/227780132/system.o.d 
	@${RM} ${OBJECTDIR}/_ext/227780132/system.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/227780132/system.o.d" -o ${OBJECTDIR}/_ext/227780132/system.o ../src/ASF/sam0/drivers/system/system.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/386045837/startup_samr21.o: ../src/ASF/sam0/utils/cmsis/samr21/source/gcc/startup_samr21.c  .generated_files/flags/Release/5e31b59d8f8026ad81d7a0b9cd67bbaad4e9fa14 .generated_files/flags/Release/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/386045837" 
	@${RM} ${OBJECTDIR}/_ext/386045837/startup_samr21.o.d 
	@${RM} ${OBJECTDIR}/_ext/386045837/startup_samr21.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/386045837/startup_samr21.o.d" -o ${OBJECTDIR}/_ext/386045837/startup_samr21.o ../src/ASF/sam0/utils/cmsis/samr21/source/gcc/startup_samr21.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/2084470165/system_samr21.o: ../src/ASF/sam0/utils/cmsis/samr21/source/system_samr21.c  .generated_files/flags/Release/8bc1d6bb815aa9f6ec3061c4baddb200adfc4f36 .generated_files/flags/Release/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/2084470165" 
	@${RM} ${OBJECTDIR}/_ext/2084470165/system_samr21.o.d 
	@${RM} ${OBJECTDIR}/_ext/2084470165/system_samr21.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/2084470165/system_samr21.o.d" -o ${OBJECTDIR}/_ext/2084470165/system_samr21.o ../src/ASF/sam0/utils/cmsis/samr21/source/system_samr21.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1284275751/syscalls.o: ../src/ASF/sam0/utils/syscalls/gcc/syscalls.c  .generated_files/flags/Release/10999ea819c16e7b26b7dbc3216a371b0bcde4ad .generated_files/flags/Release/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1284275751" 
	@${RM} ${OBJECTDIR}/_ext/1284275751/syscalls.o.d 
	@${RM} ${OBJECTDIR}/_ext/1284275751/syscalls.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1284275751/syscalls.o.d" -o ${OBJECTDIR}/_ext/1284275751/syscalls.o ../src/ASF/sam0/utils/syscalls/gcc/syscalls.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1360937237/cpu-port.o: ../src/cpu-port.c  .generated_files/flags/Release/b1b5dacdbcf83838ca19d84a4198d026331aea82 .generated_files/flags/Release/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/cpu-port.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/cpu-port.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/cpu-port.o.d" -o ${OBJECTDIR}/_ext/1360937237/cpu-port.o ../src/cpu-port.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1360937237/rtos.o: ../src/rtos.c  .generated_files/flags/Release/170ced699c4b589121f38fb9f6106c3df5847a69 .generated_files/flags/Release/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/rtos.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/rtos.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/rtos.o.d" -o ${OBJECTDIR}/_ext/1360937237/rtos.o ../src/rtos.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1360937237/main.o: ../src/main.c  .generated_files/flags/Release/aaf0384be8cc20c2269febb4779be031d7c360f1 .generated_files/flags/Release/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/main.o.d" -o ${OBJECTDIR}/_ext/1360937237/main.o ../src/main.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${DISTDIR}/mplab_sam_r21.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    ../src/ASF/sam0/utils/linker_scripts/samr21/gcc/samr21g18a_flash.ld
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE) -mcpu=cortex-m0plus  -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -D__$(MP_PROCESSOR_OPTION)__   -T"..\src\ASF\sam0\utils\linker_scripts\samr21\gcc\samr21g18a_flash.ld"  -mthumb --specs=nosys.specs -Wl,-Map="${DISTDIR}\mplab_sam_r21.X.${IMAGE_TYPE}.map"  -o ${DISTDIR}/mplab_sam_r21.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}      -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_SIMULATOR=1 -Wl,--gc-sections  
	
	
	
	
	
	
else
${DISTDIR}/mplab_sam_r21.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   ../src/ASF/sam0/utils/linker_scripts/samr21/gcc/samr21g18a_flash.ld
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE) -mcpu=cortex-m0plus  -D__$(MP_PROCESSOR_OPTION)__   -T"..\src\ASF\sam0\utils\linker_scripts\samr21\gcc\samr21g18a_flash.ld"  -mthumb --specs=nosys.specs -Wl,-Map="${DISTDIR}\mplab_sam_r21.X.${IMAGE_TYPE}.map"  -o ${DISTDIR}/mplab_sam_r21.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}      -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION) -Wl,--gc-sections  
	
	${MP_CC_DIR}\\arm-none-eabi-objcopy -O ihex -R .eeprom -R .fuse -R .lock -R .signature "${DISTDIR}/mplab_sam_r21.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}" "${DISTDIR}/mplab_sam_r21.X.${IMAGE_TYPE}.hex"
	
	
	
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${OBJECTDIR}
	${RM} -r ${DISTDIR}

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(wildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
