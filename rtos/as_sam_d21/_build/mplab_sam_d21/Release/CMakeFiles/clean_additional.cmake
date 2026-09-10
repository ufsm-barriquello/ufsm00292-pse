# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "/home/thomas/prog/barco/ufsm00292-pse/rtos/as_sam_d21/out/mplab_sam_d21/Release.bin"
  "/home/thomas/prog/barco/ufsm00292-pse/rtos/as_sam_d21/out/mplab_sam_d21/Release.eep"
  "/home/thomas/prog/barco/ufsm00292-pse/rtos/as_sam_d21/out/mplab_sam_d21/Release.hex"
  "/home/thomas/prog/barco/ufsm00292-pse/rtos/as_sam_d21/out/mplab_sam_d21/Release.lss"
  "/home/thomas/prog/barco/ufsm00292-pse/rtos/as_sam_d21/out/mplab_sam_d21/Release.srec"
  )
endif()
