# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "/home/thomas/prog/barco/ufsm00292-pse/rtos/as_sam_d21/out/mplab_sam_d21/Debug.bin"
  "/home/thomas/prog/barco/ufsm00292-pse/rtos/as_sam_d21/out/mplab_sam_d21/Debug.eep"
  "/home/thomas/prog/barco/ufsm00292-pse/rtos/as_sam_d21/out/mplab_sam_d21/Debug.hex"
  "/home/thomas/prog/barco/ufsm00292-pse/rtos/as_sam_d21/out/mplab_sam_d21/Debug.lss"
  "/home/thomas/prog/barco/ufsm00292-pse/rtos/as_sam_d21/out/mplab_sam_d21/Debug.srec"
  )
endif()
