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
ifeq "$(wildcard nbproject/Makefile-local-Debug.mk)" "nbproject/Makefile-local-Debug.mk"
include nbproject/Makefile-local-Debug.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=Debug
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=a
DEBUGGABLE_SUFFIX=
FINAL_IMAGE=${DISTDIR}/mplab_sam_r21.X.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=a
DEBUGGABLE_SUFFIX=
FINAL_IMAGE=${DISTDIR}/mplab_sam_r21.X.${OUTPUT_SUFFIX}
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
	${MAKE}  -f nbproject/Makefile-Debug.mk ${DISTDIR}/mplab_sam_r21.X.${OUTPUT_SUFFIX}

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
${OBJECTDIR}/_ext/1939168694/interrupt_sam_nvic.o: ../src/ASF/common/utils/interrupt/interrupt_sam_nvic.c  .generated_files/flags/Debug/10dc7f94382880da58e1416fb606b441fc63ce85 .generated_files/flags/Debug/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1939168694" 
	@${RM} ${OBJECTDIR}/_ext/1939168694/interrupt_sam_nvic.o.d 
	@${RM} ${OBJECTDIR}/_ext/1939168694/interrupt_sam_nvic.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1939168694/interrupt_sam_nvic.o.d" -o ${OBJECTDIR}/_ext/1939168694/interrupt_sam_nvic.o ../src/ASF/common/utils/interrupt/interrupt_sam_nvic.c  -DXPRJ_Debug=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1350218445/board_init.o: ../src/ASF/sam0/boards/samr21_xplained_pro/board_init.c  .generated_files/flags/Debug/b8d50f714182ac31e3eb098f5aa17693ac393928 .generated_files/flags/Debug/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1350218445" 
	@${RM} ${OBJECTDIR}/_ext/1350218445/board_init.o.d 
	@${RM} ${OBJECTDIR}/_ext/1350218445/board_init.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1350218445/board_init.o.d" -o ${OBJECTDIR}/_ext/1350218445/board_init.o ../src/ASF/sam0/boards/samr21_xplained_pro/board_init.c  -DXPRJ_Debug=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/876312722/port.o: ../src/ASF/sam0/drivers/port/port.c  .generated_files/flags/Debug/f34d6a23c102d616a4ff6b8cf410303c8056f39c .generated_files/flags/Debug/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/876312722" 
	@${RM} ${OBJECTDIR}/_ext/876312722/port.o.d 
	@${RM} ${OBJECTDIR}/_ext/876312722/port.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/876312722/port.o.d" -o ${OBJECTDIR}/_ext/876312722/port.o ../src/ASF/sam0/drivers/port/port.c  -DXPRJ_Debug=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1900123450/clock.o: ../src/ASF/sam0/drivers/system/clock/clock_samd21_r21_da_ha1/clock.c  .generated_files/flags/Debug/9f03c3dfea75f13f9cc38fd4f71d1883aacabb8c .generated_files/flags/Debug/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1900123450" 
	@${RM} ${OBJECTDIR}/_ext/1900123450/clock.o.d 
	@${RM} ${OBJECTDIR}/_ext/1900123450/clock.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1900123450/clock.o.d" -o ${OBJECTDIR}/_ext/1900123450/clock.o ../src/ASF/sam0/drivers/system/clock/clock_samd21_r21_da_ha1/clock.c  -DXPRJ_Debug=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1900123450/gclk.o: ../src/ASF/sam0/drivers/system/clock/clock_samd21_r21_da_ha1/gclk.c  .generated_files/flags/Debug/2b116684e555c0315bed78110db96be179585bbe .generated_files/flags/Debug/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1900123450" 
	@${RM} ${OBJECTDIR}/_ext/1900123450/gclk.o.d 
	@${RM} ${OBJECTDIR}/_ext/1900123450/gclk.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1900123450/gclk.o.d" -o ${OBJECTDIR}/_ext/1900123450/gclk.o ../src/ASF/sam0/drivers/system/clock/clock_samd21_r21_da_ha1/gclk.c  -DXPRJ_Debug=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1234282992/system_interrupt.o: ../src/ASF/sam0/drivers/system/interrupt/system_interrupt.c  .generated_files/flags/Debug/869aca788ee693da630c6756a203b0c5e08a6b88 .generated_files/flags/Debug/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1234282992" 
	@${RM} ${OBJECTDIR}/_ext/1234282992/system_interrupt.o.d 
	@${RM} ${OBJECTDIR}/_ext/1234282992/system_interrupt.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1234282992/system_interrupt.o.d" -o ${OBJECTDIR}/_ext/1234282992/system_interrupt.o ../src/ASF/sam0/drivers/system/interrupt/system_interrupt.c  -DXPRJ_Debug=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/980481618/pinmux.o: ../src/ASF/sam0/drivers/system/pinmux/pinmux.c  .generated_files/flags/Debug/5e5126132d5345342c3acc836b2b1a95ba13c105 .generated_files/flags/Debug/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/980481618" 
	@${RM} ${OBJECTDIR}/_ext/980481618/pinmux.o.d 
	@${RM} ${OBJECTDIR}/_ext/980481618/pinmux.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/980481618/pinmux.o.d" -o ${OBJECTDIR}/_ext/980481618/pinmux.o ../src/ASF/sam0/drivers/system/pinmux/pinmux.c  -DXPRJ_Debug=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/227780132/system.o: ../src/ASF/sam0/drivers/system/system.c  .generated_files/flags/Debug/c004b71a10d84909a7179903ecabce97415b5e8b .generated_files/flags/Debug/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/227780132" 
	@${RM} ${OBJECTDIR}/_ext/227780132/system.o.d 
	@${RM} ${OBJECTDIR}/_ext/227780132/system.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/227780132/system.o.d" -o ${OBJECTDIR}/_ext/227780132/system.o ../src/ASF/sam0/drivers/system/system.c  -DXPRJ_Debug=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/386045837/startup_samr21.o: ../src/ASF/sam0/utils/cmsis/samr21/source/gcc/startup_samr21.c  .generated_files/flags/Debug/8f616fa00c6efa209241d1d24964f776f942966c .generated_files/flags/Debug/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/386045837" 
	@${RM} ${OBJECTDIR}/_ext/386045837/startup_samr21.o.d 
	@${RM} ${OBJECTDIR}/_ext/386045837/startup_samr21.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/386045837/startup_samr21.o.d" -o ${OBJECTDIR}/_ext/386045837/startup_samr21.o ../src/ASF/sam0/utils/cmsis/samr21/source/gcc/startup_samr21.c  -DXPRJ_Debug=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/2084470165/system_samr21.o: ../src/ASF/sam0/utils/cmsis/samr21/source/system_samr21.c  .generated_files/flags/Debug/2f3681b802f1c9b23d67585cc28900bacdc9efe3 .generated_files/flags/Debug/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/2084470165" 
	@${RM} ${OBJECTDIR}/_ext/2084470165/system_samr21.o.d 
	@${RM} ${OBJECTDIR}/_ext/2084470165/system_samr21.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/2084470165/system_samr21.o.d" -o ${OBJECTDIR}/_ext/2084470165/system_samr21.o ../src/ASF/sam0/utils/cmsis/samr21/source/system_samr21.c  -DXPRJ_Debug=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1284275751/syscalls.o: ../src/ASF/sam0/utils/syscalls/gcc/syscalls.c  .generated_files/flags/Debug/d97d29d8d455a89067b3e3417d637a4029c63e42 .generated_files/flags/Debug/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1284275751" 
	@${RM} ${OBJECTDIR}/_ext/1284275751/syscalls.o.d 
	@${RM} ${OBJECTDIR}/_ext/1284275751/syscalls.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1284275751/syscalls.o.d" -o ${OBJECTDIR}/_ext/1284275751/syscalls.o ../src/ASF/sam0/utils/syscalls/gcc/syscalls.c  -DXPRJ_Debug=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1360937237/cpu-port.o: ../src/cpu-port.c  .generated_files/flags/Debug/74d5481b9eae272be0cc77715bae6eb411f1727c .generated_files/flags/Debug/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/cpu-port.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/cpu-port.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/cpu-port.o.d" -o ${OBJECTDIR}/_ext/1360937237/cpu-port.o ../src/cpu-port.c  -DXPRJ_Debug=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1360937237/rtos.o: ../src/rtos.c  .generated_files/flags/Debug/7a647263655b35e72b0499cb4471d815aa48b82c .generated_files/flags/Debug/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/rtos.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/rtos.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/rtos.o.d" -o ${OBJECTDIR}/_ext/1360937237/rtos.o ../src/rtos.c  -DXPRJ_Debug=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1360937237/main.o: ../src/main.c  .generated_files/flags/Debug/e66bf613db8fb521eac55bd8191503ac3454eb30 .generated_files/flags/Debug/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/main.o.d" -o ${OBJECTDIR}/_ext/1360937237/main.o ../src/main.c  -DXPRJ_Debug=$(CND_CONF)  $(COMPARISON_BUILD) 
	
else
${OBJECTDIR}/_ext/1939168694/interrupt_sam_nvic.o: ../src/ASF/common/utils/interrupt/interrupt_sam_nvic.c  .generated_files/flags/Debug/58a558f71130176fcda9dcbcb161b03da55f75ad .generated_files/flags/Debug/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1939168694" 
	@${RM} ${OBJECTDIR}/_ext/1939168694/interrupt_sam_nvic.o.d 
	@${RM} ${OBJECTDIR}/_ext/1939168694/interrupt_sam_nvic.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1939168694/interrupt_sam_nvic.o.d" -o ${OBJECTDIR}/_ext/1939168694/interrupt_sam_nvic.o ../src/ASF/common/utils/interrupt/interrupt_sam_nvic.c  -DXPRJ_Debug=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1350218445/board_init.o: ../src/ASF/sam0/boards/samr21_xplained_pro/board_init.c  .generated_files/flags/Debug/bc46e51d66fa1717ea90a2e904fdd2d881691a1a .generated_files/flags/Debug/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1350218445" 
	@${RM} ${OBJECTDIR}/_ext/1350218445/board_init.o.d 
	@${RM} ${OBJECTDIR}/_ext/1350218445/board_init.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1350218445/board_init.o.d" -o ${OBJECTDIR}/_ext/1350218445/board_init.o ../src/ASF/sam0/boards/samr21_xplained_pro/board_init.c  -DXPRJ_Debug=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/876312722/port.o: ../src/ASF/sam0/drivers/port/port.c  .generated_files/flags/Debug/1b2af0b2440faea9a16f0bed4055cb5ab37fefc1 .generated_files/flags/Debug/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/876312722" 
	@${RM} ${OBJECTDIR}/_ext/876312722/port.o.d 
	@${RM} ${OBJECTDIR}/_ext/876312722/port.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/876312722/port.o.d" -o ${OBJECTDIR}/_ext/876312722/port.o ../src/ASF/sam0/drivers/port/port.c  -DXPRJ_Debug=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1900123450/clock.o: ../src/ASF/sam0/drivers/system/clock/clock_samd21_r21_da_ha1/clock.c  .generated_files/flags/Debug/e36429f88fe9844d357c6ddfe30d10a7179763a1 .generated_files/flags/Debug/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1900123450" 
	@${RM} ${OBJECTDIR}/_ext/1900123450/clock.o.d 
	@${RM} ${OBJECTDIR}/_ext/1900123450/clock.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1900123450/clock.o.d" -o ${OBJECTDIR}/_ext/1900123450/clock.o ../src/ASF/sam0/drivers/system/clock/clock_samd21_r21_da_ha1/clock.c  -DXPRJ_Debug=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1900123450/gclk.o: ../src/ASF/sam0/drivers/system/clock/clock_samd21_r21_da_ha1/gclk.c  .generated_files/flags/Debug/aecbede9f6422ec5835f2fb4999d6f0526a81efd .generated_files/flags/Debug/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1900123450" 
	@${RM} ${OBJECTDIR}/_ext/1900123450/gclk.o.d 
	@${RM} ${OBJECTDIR}/_ext/1900123450/gclk.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1900123450/gclk.o.d" -o ${OBJECTDIR}/_ext/1900123450/gclk.o ../src/ASF/sam0/drivers/system/clock/clock_samd21_r21_da_ha1/gclk.c  -DXPRJ_Debug=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1234282992/system_interrupt.o: ../src/ASF/sam0/drivers/system/interrupt/system_interrupt.c  .generated_files/flags/Debug/6719382963eed237e3b0bf570c4f0a42675260dd .generated_files/flags/Debug/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1234282992" 
	@${RM} ${OBJECTDIR}/_ext/1234282992/system_interrupt.o.d 
	@${RM} ${OBJECTDIR}/_ext/1234282992/system_interrupt.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1234282992/system_interrupt.o.d" -o ${OBJECTDIR}/_ext/1234282992/system_interrupt.o ../src/ASF/sam0/drivers/system/interrupt/system_interrupt.c  -DXPRJ_Debug=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/980481618/pinmux.o: ../src/ASF/sam0/drivers/system/pinmux/pinmux.c  .generated_files/flags/Debug/f778f1175771264d825a52726601fbaf10c3a6d8 .generated_files/flags/Debug/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/980481618" 
	@${RM} ${OBJECTDIR}/_ext/980481618/pinmux.o.d 
	@${RM} ${OBJECTDIR}/_ext/980481618/pinmux.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/980481618/pinmux.o.d" -o ${OBJECTDIR}/_ext/980481618/pinmux.o ../src/ASF/sam0/drivers/system/pinmux/pinmux.c  -DXPRJ_Debug=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/227780132/system.o: ../src/ASF/sam0/drivers/system/system.c  .generated_files/flags/Debug/172e92062d0e71393d8d8c85667c246ea61eb085 .generated_files/flags/Debug/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/227780132" 
	@${RM} ${OBJECTDIR}/_ext/227780132/system.o.d 
	@${RM} ${OBJECTDIR}/_ext/227780132/system.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/227780132/system.o.d" -o ${OBJECTDIR}/_ext/227780132/system.o ../src/ASF/sam0/drivers/system/system.c  -DXPRJ_Debug=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/386045837/startup_samr21.o: ../src/ASF/sam0/utils/cmsis/samr21/source/gcc/startup_samr21.c  .generated_files/flags/Debug/b2f86df9e3e281aecad348b1cd862d83607562a8 .generated_files/flags/Debug/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/386045837" 
	@${RM} ${OBJECTDIR}/_ext/386045837/startup_samr21.o.d 
	@${RM} ${OBJECTDIR}/_ext/386045837/startup_samr21.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/386045837/startup_samr21.o.d" -o ${OBJECTDIR}/_ext/386045837/startup_samr21.o ../src/ASF/sam0/utils/cmsis/samr21/source/gcc/startup_samr21.c  -DXPRJ_Debug=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/2084470165/system_samr21.o: ../src/ASF/sam0/utils/cmsis/samr21/source/system_samr21.c  .generated_files/flags/Debug/e8d21c753e0a7a00b4531a42b5e35382b844c5f5 .generated_files/flags/Debug/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/2084470165" 
	@${RM} ${OBJECTDIR}/_ext/2084470165/system_samr21.o.d 
	@${RM} ${OBJECTDIR}/_ext/2084470165/system_samr21.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/2084470165/system_samr21.o.d" -o ${OBJECTDIR}/_ext/2084470165/system_samr21.o ../src/ASF/sam0/utils/cmsis/samr21/source/system_samr21.c  -DXPRJ_Debug=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1284275751/syscalls.o: ../src/ASF/sam0/utils/syscalls/gcc/syscalls.c  .generated_files/flags/Debug/87891aa1e56821226def4e4b361c8e0ff94a0f70 .generated_files/flags/Debug/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1284275751" 
	@${RM} ${OBJECTDIR}/_ext/1284275751/syscalls.o.d 
	@${RM} ${OBJECTDIR}/_ext/1284275751/syscalls.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1284275751/syscalls.o.d" -o ${OBJECTDIR}/_ext/1284275751/syscalls.o ../src/ASF/sam0/utils/syscalls/gcc/syscalls.c  -DXPRJ_Debug=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1360937237/cpu-port.o: ../src/cpu-port.c  .generated_files/flags/Debug/1ca43db003a55b69664a5fde85b96ef1f4952d8d .generated_files/flags/Debug/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/cpu-port.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/cpu-port.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/cpu-port.o.d" -o ${OBJECTDIR}/_ext/1360937237/cpu-port.o ../src/cpu-port.c  -DXPRJ_Debug=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1360937237/rtos.o: ../src/rtos.c  .generated_files/flags/Debug/53213ed385b689024618594a367464af0ae90e7e .generated_files/flags/Debug/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/rtos.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/rtos.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/rtos.o.d" -o ${OBJECTDIR}/_ext/1360937237/rtos.o ../src/rtos.c  -DXPRJ_Debug=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1360937237/main.o: ../src/main.c  .generated_files/flags/Debug/d70d50702eaa48bf98b626c086375dfffaeb99eb .generated_files/flags/Debug/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -mcpu=cortex-m0plus  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -mthumb ${PACK_COMMON_OPTIONS}  -Os -ffunction-sections -mlong-calls -Wall -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/main.o.d" -o ${OBJECTDIR}/_ext/1360937237/main.o ../src/main.c  -DXPRJ_Debug=$(CND_CONF)  $(COMPARISON_BUILD) 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: archive
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${DISTDIR}/mplab_sam_r21.X.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} ${DISTDIR} 
	${MP_AR} $(MP_EXTRA_AR_PRE)  -r ${DISTDIR}/mplab_sam_r21.X.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}    
else
${DISTDIR}/mplab_sam_r21.X.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} ${DISTDIR} 
	${MP_AR} $(MP_EXTRA_AR_PRE)  -r ${DISTDIR}/mplab_sam_r21.X.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}    
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
