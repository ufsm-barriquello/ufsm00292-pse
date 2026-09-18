# The following functions contains all the flags passed to the different build stages.

set(PACK_REPO_PATH "/home/thomas/.mchp_packs" CACHE PATH "Path to the root of a pack repository.")

function(mplab_sam_d21_Release_Release_ARM_GCC_assemble_rule target)
    set(options
        "-g"
        "${ASSEMBLER_PRE}"
        "-gdwarf-2"
        "-mcpu=cortex-m0plus"
        "-mthumb"
        "-Wa,--defsym=__MPLAB_BUILD=1${MP_EXTRA_AS_POST},--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--gdwarf-2")
    list(REMOVE_ITEM options "")
    target_compile_options(${target} PRIVATE "${options}")
    target_compile_definitions(${target}
        PRIVATE "__DEBUG=1"
        PRIVATE "__SAMD21J18A__")
    target_include_directories(${target}
        PRIVATE "../src/ASF/sam0/utils/header_files"
        PRIVATE "../src/ASF/sam0/drivers/system/power/power_sam_d_r"
        PRIVATE "../src/ASF/common/utils"
        PRIVATE "../src/ASF/sam0/drivers/system/pinmux"
        PRIVATE "../src/ASF/sam0/drivers/system/power"
        PRIVATE "../src/ASF/sam0/drivers/system/reset/reset_sam_d_r"
        PRIVATE "../src/ASF/common/boards"
        PRIVATE "../src/ASF/sam0/drivers/port"
        PRIVATE "../src/ASF/sam0/boards"
        PRIVATE "../src/ASF/sam0/utils"
        PRIVATE "../src/ASF/thirdparty/CMSIS/Include"
        PRIVATE "../src/config"
        PRIVATE "../src/ASF/thirdparty/CMSIS/Lib/GCC"
        PRIVATE "../src/ASF/sam0/drivers/system/reset"
        PRIVATE "../src/ASF/sam0/drivers/system/interrupt/system_interrupt_samd21"
        PRIVATE "../src/ASF/sam0/boards/samd21_xplained_pro"
        PRIVATE "../src"
        PRIVATE "../src/ASF/sam0/utils/preprocessor"
        PRIVATE "../src/ASF/sam0/utils/cmsis/samd21/include"
        PRIVATE "../src/ASF/sam0/drivers/system"
        PRIVATE "../src/ASF/sam0/drivers/system/clock/clock_samd21_r21_da"
        PRIVATE "../src/ASF/sam0/utils/cmsis/samd21/source"
        PRIVATE "../src/ASF/sam0/drivers/system/clock"
        PRIVATE "../src/ASF/sam0/drivers/system/interrupt")
endfunction()
function(mplab_sam_d21_Release_Release_ARM_GCC_assembleWithPreprocess_rule target)
    set(options
        "-x"
        "assembler-with-cpp"
        "-g"
        "-gdwarf-2"
        "-mcpu=cortex-m0plus"
        "-mthumb"
        "-Wa,--defsym=__MPLAB_BUILD=1${MP_EXTRA_AS_POST},--defsym=__MPLAB_DEBUG=1,--gdwarf-2,-g,--defsym=__DEBUG=1")
    list(REMOVE_ITEM options "")
    target_compile_options(${target} PRIVATE "${options}")
    target_compile_definitions(${target}
        PRIVATE "__DEBUG=1"
        PRIVATE "__SAMD21J18A__"
        PRIVATE "Release=Release")
    target_include_directories(${target}
        PRIVATE "../src/ASF/sam0/utils/header_files"
        PRIVATE "../src/ASF/sam0/drivers/system/power/power_sam_d_r"
        PRIVATE "../src/ASF/common/utils"
        PRIVATE "../src/ASF/sam0/drivers/system/pinmux"
        PRIVATE "../src/ASF/sam0/drivers/system/power"
        PRIVATE "../src/ASF/sam0/drivers/system/reset/reset_sam_d_r"
        PRIVATE "../src/ASF/common/boards"
        PRIVATE "../src/ASF/sam0/drivers/port"
        PRIVATE "../src/ASF/sam0/boards"
        PRIVATE "../src/ASF/sam0/utils"
        PRIVATE "../src/ASF/thirdparty/CMSIS/Include"
        PRIVATE "../src/config"
        PRIVATE "../src/ASF/thirdparty/CMSIS/Lib/GCC"
        PRIVATE "../src/ASF/sam0/drivers/system/reset"
        PRIVATE "../src/ASF/sam0/drivers/system/interrupt/system_interrupt_samd21"
        PRIVATE "../src/ASF/sam0/boards/samd21_xplained_pro"
        PRIVATE "../src"
        PRIVATE "../src/ASF/sam0/utils/preprocessor"
        PRIVATE "../src/ASF/sam0/utils/cmsis/samd21/include"
        PRIVATE "../src/ASF/sam0/drivers/system"
        PRIVATE "../src/ASF/sam0/drivers/system/clock/clock_samd21_r21_da"
        PRIVATE "../src/ASF/sam0/utils/cmsis/samd21/source"
        PRIVATE "../src/ASF/sam0/drivers/system/clock"
        PRIVATE "../src/ASF/sam0/drivers/system/interrupt")
endfunction()
function(mplab_sam_d21_Release_Release_ARM_GCC_compile_rule target)
    set(options
        "-g"
        "-gdwarf-2"
        "-mcpu=cortex-m0plus"
        "-x"
        "c"
        "-mthumb"
        "-Os"
        "-ffunction-sections"
        "-mlong-calls"
        "-Wall"
        "-pipe"
        "-fno-strict-aliasing"
        "-Wall"
        "-Wstrict-prototypes"
        "-Wmissing-prototypes"
        "-Werror-implicit-function-declaration"
        "-Wpointer-arith"
        "-std=gnu99"
        "-ffunction-sections"
        "-fdata-sections"
        "-Wchar-subscripts"
        "-Wcomment"
        "-Wformat=2"
        "-Wimplicit-int"
        "-Wmain"
        "-Wparentheses"
        "-Wsequence-point"
        "-Wreturn-type"
        "-Wswitch"
        "-Wtrigraphs"
        "-Wunused"
        "-Wuninitialized"
        "-Wunknown-pragmas"
        "-Wfloat-equal"
        "-Wundef"
        "-Wshadow"
        "-Wbad-function-cast"
        "-Wwrite-strings"
        "-Wsign-compare"
        "-Waggregate-return"
        "-Wmissing-declarations"
        "-Wformat"
        "-Wmissing-format-attribute"
        "-Wno-deprecated-declarations"
        "-Wpacked"
        "-Wredundant-decls"
        "-Wnested-externs"
        "-Wlong-long"
        "-Wunreachable-code"
        "-Wcast-align"
        "SHELL:--param max-inline-insns-single=500"
        "-fdata-sections")
    list(REMOVE_ITEM options "")
    target_compile_options(${target} PRIVATE "${options}")
    target_compile_definitions(${target}
        PRIVATE "__DEBUG"
        PRIVATE "__SAMD21J18A__"
        PRIVATE "NDEBUG"
        PRIVATE "BOARD=SAMD21_XPLAINED_PRO"
        PRIVATE "__SAMD21J18A__"
        PRIVATE "ARM_MATH_CM0PLUS=true"
        PRIVATE "Release=Release")
    target_include_directories(${target}
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ASF/sam0/utils/header_files"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ASF/sam0/drivers/system/power/power_sam_d_r"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ASF/common/utils"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ASF/sam0/drivers/system/pinmux"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ASF/sam0/drivers/system/power"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ASF/sam0/drivers/system/reset/reset_sam_d_r"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ASF/common/boards"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ASF/sam0/drivers/port"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ASF/sam0/boards"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ASF/sam0/utils"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ASF/thirdparty/CMSIS/Include"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/config"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ASF/thirdparty/CMSIS/Lib/GCC"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ASF/sam0/drivers/system/reset"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ASF/sam0/drivers/system/interrupt/system_interrupt_samd21"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ASF/sam0/boards/samd21_xplained_pro"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../src"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ASF/sam0/utils/preprocessor"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ASF/sam0/utils/cmsis/samd21/include"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ASF/sam0/drivers/system"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ASF/sam0/drivers/system/clock/clock_samd21_r21_da"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ASF/sam0/utils/cmsis/samd21/source"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ASF/sam0/drivers/system/clock"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ASF/sam0/drivers/system/interrupt"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../mplab_sam_d21.X")
endfunction()
function(mplab_sam_d21_Release_Release_ARM_GCC_compile_cpp_rule target)
    set(options
        "-g"
        "-gdwarf-2"
        "-x"
        "c++"
        "-mcpu=cortex-m0plus"
        "-mthumb"
        "-Os"
        "-fno-rtti"
        "-fno-exceptions"
        "-mlong-calls"
        "-Wall")
    list(REMOVE_ITEM options "")
    target_compile_options(${target} PRIVATE "${options}")
    target_compile_definitions(${target}
        PRIVATE "__DEBUG"
        PRIVATE "__SAMD21J18A__"
        PRIVATE "Release=Release")
    target_include_directories(${target} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../mplab_sam_d21.X")
endfunction()
function(mplab_sam_d21_Release_link_rule target)
    set(options
        "-gdwarf-2"
        "${MP_EXTRA_LD_PRE}"
        "-mthumb"
        "-Wl,-Map=mem.map"
        "--specs=nano.specs"
        "--specs=nosys.specs"
        "-Wl,--defsym=__MPLAB_BUILD=1${MP_EXTRA_LD_POST},--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1"
        "-L${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ASF/thirdparty/CMSIS/Lib/GCC"
        "-L${CMAKE_CURRENT_SOURCE_DIR}/../../../mplab_sam_d21.X"
        "-Wl,--gc-sections"
        "-Wl,--entry=Reset_Handler"
        "-Wl,--cref"
        "-mthumb"
        "-T../src/ASF/sam0/utils/linker_scripts/samd21/gcc/samd21j18a_flash.ld")
    list(REMOVE_ITEM options "")
    target_link_options(${target} PRIVATE "${options}")
    target_compile_definitions(${target}
        PRIVATE "__SAMD21J18A__"
        PRIVATE "Release=Release")
    target_link_libraries(${target}
        PRIVATE "arm_cortexM0l_math"
        PRIVATE "m")
endfunction()
function(mplab_sam_d21_Release_objcopy_ihex_rule target)
    add_custom_command(
        TARGET ${target}
        POST_BUILD
        COMMAND ${OBJCOPY}
        ARGS --output-target=ihex --remove-section=.eeprom --remove-section=.fuse --remove-section=.lock --remove-section=.signature ${mplab_sam_d21_Release_image_name} ${mplab_sam_d21_Release_image_base_name}.hex
        WORKING_DIRECTORY ${mplab_sam_d21_Release_output_dir})
endfunction()
function(mplab_sam_d21_Release_objcopy_eep_rule target)
    add_custom_command(
        TARGET ${target}
        POST_BUILD
        COMMAND ${OBJCOPY}
        ARGS --only-section=.eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 --no-change-warnings --output-target=binary ${mplab_sam_d21_Release_image_name} ${mplab_sam_d21_Release_image_base_name}.eep
        WORKING_DIRECTORY ${mplab_sam_d21_Release_output_dir})
endfunction()
function(mplab_sam_d21_Release_objcopy_lss_rule target)
    add_custom_command(
        TARGET ${target}
        POST_BUILD
        COMMAND ${OBJDUMP}
        ARGS --disassemble --wide --demangle --line-numbers --section-headers --source ${mplab_sam_d21_Release_image_name} > ${mplab_sam_d21_Release_image_base_name}.lss
        WORKING_DIRECTORY ${mplab_sam_d21_Release_output_dir})
endfunction()
function(mplab_sam_d21_Release_objcopy_srec_rule target)
    add_custom_command(
        TARGET ${target}
        POST_BUILD
        COMMAND ${OBJCOPY}
        ARGS --output-target=srec --remove-section=.eeprom --remove-section=.fuse --remove-section=.lock --remove-section=.signature ${mplab_sam_d21_Release_image_name} ${mplab_sam_d21_Release_image_base_name}.srec
        WORKING_DIRECTORY ${mplab_sam_d21_Release_output_dir})
endfunction()
