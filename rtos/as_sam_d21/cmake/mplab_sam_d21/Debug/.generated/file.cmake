# The following variables contains the files used by the different stages of the build process.
set(mplab_sam_d21_Debug_Debug_ARM_GCC_FILE_TYPE_assemble)
set_source_files_properties(${mplab_sam_d21_Debug_Debug_ARM_GCC_FILE_TYPE_assemble} PROPERTIES LANGUAGE ASM)

# For assembly files, add "." to the include path for each file so that .include with a relative path works
foreach(source_file ${mplab_sam_d21_Debug_Debug_ARM_GCC_FILE_TYPE_assemble})
        set_source_files_properties(${source_file} PROPERTIES INCLUDE_DIRECTORIES "$<PATH:NORMAL_PATH,$<PATH:REMOVE_FILENAME,${source_file}>>")
endforeach()

set(mplab_sam_d21_Debug_Debug_ARM_GCC_FILE_TYPE_assembleWithPreprocess)
set_source_files_properties(${mplab_sam_d21_Debug_Debug_ARM_GCC_FILE_TYPE_assembleWithPreprocess} PROPERTIES LANGUAGE ASM)

# For assembly files, add "." to the include path for each file so that .include with a relative path works
foreach(source_file ${mplab_sam_d21_Debug_Debug_ARM_GCC_FILE_TYPE_assembleWithPreprocess})
        set_source_files_properties(${source_file} PROPERTIES INCLUDE_DIRECTORIES "$<PATH:NORMAL_PATH,$<PATH:REMOVE_FILENAME,${source_file}>>")
endforeach()

set(mplab_sam_d21_Debug_Debug_ARM_GCC_FILE_TYPE_compile
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ASF/common/utils/interrupt/interrupt_sam_nvic.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ASF/sam0/boards/samd21_xplained_pro/board_init.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ASF/sam0/drivers/port/port.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ASF/sam0/drivers/system/clock/clock_samd21_r21_da/clock.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ASF/sam0/drivers/system/clock/clock_samd21_r21_da/gclk.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ASF/sam0/drivers/system/interrupt/system_interrupt.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ASF/sam0/drivers/system/pinmux/pinmux.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ASF/sam0/drivers/system/system.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ASF/sam0/utils/cmsis/samd21/source/gcc/startup_samd21.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ASF/sam0/utils/cmsis/samd21/source/system_samd21.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ASF/sam0/utils/syscalls/gcc/syscalls.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/cpu-port.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/main.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/rtos.c")
set_source_files_properties(${mplab_sam_d21_Debug_Debug_ARM_GCC_FILE_TYPE_compile} PROPERTIES LANGUAGE C)
set(mplab_sam_d21_Debug_Debug_ARM_GCC_FILE_TYPE_compile_cpp)
set_source_files_properties(${mplab_sam_d21_Debug_Debug_ARM_GCC_FILE_TYPE_compile_cpp} PROPERTIES LANGUAGE CXX)
set(mplab_sam_d21_Debug_Debug_ARM_GCC_FILE_TYPE_link)
set(mplab_sam_d21_Debug_Debug_ARM_GCC_FILE_TYPE_objcopy_ihex)
set(mplab_sam_d21_Debug_Debug_ARM_GCC_FILE_TYPE_objcopy_eep)
set(mplab_sam_d21_Debug_Debug_ARM_GCC_FILE_TYPE_objcopy_lss)
set(mplab_sam_d21_Debug_Debug_ARM_GCC_FILE_TYPE_objcopy_srec)
set(mplab_sam_d21_Debug_image_name "Debug.elf")
set(mplab_sam_d21_Debug_image_base_name "Debug")

# The output directory of the final image.
set(mplab_sam_d21_Debug_output_dir "${CMAKE_CURRENT_SOURCE_DIR}/../../../out/mplab_sam_d21")

# The full path to the final image.
set(mplab_sam_d21_Debug_full_path_to_image ${mplab_sam_d21_Debug_output_dir}/${mplab_sam_d21_Debug_image_name})

# Potential output file extensions
set(output_extensions
    .hex
    .lss
    .eep
    .bin
    .srec)
list(TRANSFORM output_extensions PREPEND "${mplab_sam_d21_Debug_output_dir}/${mplab_sam_d21_Debug_image_base_name}")
