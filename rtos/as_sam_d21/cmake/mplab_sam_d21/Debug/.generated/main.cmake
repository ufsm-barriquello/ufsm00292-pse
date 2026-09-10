include("${CMAKE_CURRENT_LIST_DIR}/rule.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/file.cmake")

set(mplab_sam_d21_Debug_library_list )

# Handle files with suffix s, for group Debug-ARM-GCC
if(mplab_sam_d21_Debug_Debug_ARM_GCC_FILE_TYPE_assemble)
add_library(mplab_sam_d21_Debug_Debug_ARM_GCC_assemble OBJECT ${mplab_sam_d21_Debug_Debug_ARM_GCC_FILE_TYPE_assemble})
    mplab_sam_d21_Debug_Debug_ARM_GCC_assemble_rule(mplab_sam_d21_Debug_Debug_ARM_GCC_assemble)
    list(APPEND mplab_sam_d21_Debug_library_list "$<TARGET_OBJECTS:mplab_sam_d21_Debug_Debug_ARM_GCC_assemble>")

endif()

# Handle files with suffix S, for group Debug-ARM-GCC
if(mplab_sam_d21_Debug_Debug_ARM_GCC_FILE_TYPE_assembleWithPreprocess)
add_library(mplab_sam_d21_Debug_Debug_ARM_GCC_assembleWithPreprocess OBJECT ${mplab_sam_d21_Debug_Debug_ARM_GCC_FILE_TYPE_assembleWithPreprocess})
    mplab_sam_d21_Debug_Debug_ARM_GCC_assembleWithPreprocess_rule(mplab_sam_d21_Debug_Debug_ARM_GCC_assembleWithPreprocess)
    list(APPEND mplab_sam_d21_Debug_library_list "$<TARGET_OBJECTS:mplab_sam_d21_Debug_Debug_ARM_GCC_assembleWithPreprocess>")

endif()

# Handle files with suffix [cC], for group Debug-ARM-GCC
if(mplab_sam_d21_Debug_Debug_ARM_GCC_FILE_TYPE_compile)
add_library(mplab_sam_d21_Debug_Debug_ARM_GCC_compile OBJECT ${mplab_sam_d21_Debug_Debug_ARM_GCC_FILE_TYPE_compile})
    mplab_sam_d21_Debug_Debug_ARM_GCC_compile_rule(mplab_sam_d21_Debug_Debug_ARM_GCC_compile)
    list(APPEND mplab_sam_d21_Debug_library_list "$<TARGET_OBJECTS:mplab_sam_d21_Debug_Debug_ARM_GCC_compile>")

endif()

# Handle files with suffix cpp, for group Debug-ARM-GCC
if(mplab_sam_d21_Debug_Debug_ARM_GCC_FILE_TYPE_compile_cpp)
add_library(mplab_sam_d21_Debug_Debug_ARM_GCC_compile_cpp OBJECT ${mplab_sam_d21_Debug_Debug_ARM_GCC_FILE_TYPE_compile_cpp})
    mplab_sam_d21_Debug_Debug_ARM_GCC_compile_cpp_rule(mplab_sam_d21_Debug_Debug_ARM_GCC_compile_cpp)
    list(APPEND mplab_sam_d21_Debug_library_list "$<TARGET_OBJECTS:mplab_sam_d21_Debug_Debug_ARM_GCC_compile_cpp>")

endif()

# Handle files with suffix elf, for group Debug-ARM-GCC
if(mplab_sam_d21_Debug_Debug_ARM_GCC_FILE_TYPE_objcopy_ihex)
add_library(mplab_sam_d21_Debug_Debug_ARM_GCC_objcopy_ihex OBJECT ${mplab_sam_d21_Debug_Debug_ARM_GCC_FILE_TYPE_objcopy_ihex})
    mplab_sam_d21_Debug_Debug_ARM_GCC_objcopy_ihex_rule(mplab_sam_d21_Debug_Debug_ARM_GCC_objcopy_ihex)
    list(APPEND mplab_sam_d21_Debug_library_list "$<TARGET_OBJECTS:mplab_sam_d21_Debug_Debug_ARM_GCC_objcopy_ihex>")

endif()

# Handle files with suffix elf, for group Debug-ARM-GCC
if(mplab_sam_d21_Debug_Debug_ARM_GCC_FILE_TYPE_objcopy_eep)
add_library(mplab_sam_d21_Debug_Debug_ARM_GCC_objcopy_eep OBJECT ${mplab_sam_d21_Debug_Debug_ARM_GCC_FILE_TYPE_objcopy_eep})
    mplab_sam_d21_Debug_Debug_ARM_GCC_objcopy_eep_rule(mplab_sam_d21_Debug_Debug_ARM_GCC_objcopy_eep)
    list(APPEND mplab_sam_d21_Debug_library_list "$<TARGET_OBJECTS:mplab_sam_d21_Debug_Debug_ARM_GCC_objcopy_eep>")

endif()

# Handle files with suffix elf, for group Debug-ARM-GCC
if(mplab_sam_d21_Debug_Debug_ARM_GCC_FILE_TYPE_objcopy_lss)
add_library(mplab_sam_d21_Debug_Debug_ARM_GCC_objcopy_lss OBJECT ${mplab_sam_d21_Debug_Debug_ARM_GCC_FILE_TYPE_objcopy_lss})
    mplab_sam_d21_Debug_Debug_ARM_GCC_objcopy_lss_rule(mplab_sam_d21_Debug_Debug_ARM_GCC_objcopy_lss)
    list(APPEND mplab_sam_d21_Debug_library_list "$<TARGET_OBJECTS:mplab_sam_d21_Debug_Debug_ARM_GCC_objcopy_lss>")

endif()

# Handle files with suffix elf, for group Debug-ARM-GCC
if(mplab_sam_d21_Debug_Debug_ARM_GCC_FILE_TYPE_objcopy_srec)
add_library(mplab_sam_d21_Debug_Debug_ARM_GCC_objcopy_srec OBJECT ${mplab_sam_d21_Debug_Debug_ARM_GCC_FILE_TYPE_objcopy_srec})
    mplab_sam_d21_Debug_Debug_ARM_GCC_objcopy_srec_rule(mplab_sam_d21_Debug_Debug_ARM_GCC_objcopy_srec)
    list(APPEND mplab_sam_d21_Debug_library_list "$<TARGET_OBJECTS:mplab_sam_d21_Debug_Debug_ARM_GCC_objcopy_srec>")

endif()


# Main target for this project
add_executable(mplab_sam_d21_Debug_image_vh_EUdiN ${mplab_sam_d21_Debug_library_list})

set_target_properties(mplab_sam_d21_Debug_image_vh_EUdiN PROPERTIES
    OUTPUT_NAME "Debug"
    SUFFIX ".elf"
    ADDITIONAL_CLEAN_FILES "${output_extensions}"
    RUNTIME_OUTPUT_DIRECTORY "${mplab_sam_d21_Debug_output_dir}")
target_link_libraries(mplab_sam_d21_Debug_image_vh_EUdiN PRIVATE ${mplab_sam_d21_Debug_Debug_ARM_GCC_FILE_TYPE_link})
# Add the link options from the rule file.
mplab_sam_d21_Debug_link_rule( mplab_sam_d21_Debug_image_vh_EUdiN)


#Add objcopy steps
mplab_sam_d21_Debug_objcopy_ihex_rule(mplab_sam_d21_Debug_image_vh_EUdiN)
mplab_sam_d21_Debug_objcopy_eep_rule(mplab_sam_d21_Debug_image_vh_EUdiN)
mplab_sam_d21_Debug_objcopy_lss_rule(mplab_sam_d21_Debug_image_vh_EUdiN)
mplab_sam_d21_Debug_objcopy_srec_rule(mplab_sam_d21_Debug_image_vh_EUdiN)

