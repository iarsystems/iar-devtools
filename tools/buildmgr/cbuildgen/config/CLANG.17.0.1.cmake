# This file maps the CMSIS project options to toolchain settings.
#
#   - Applies to toolchain: LLVM/Clang Bare Metal Toolchain for the Arm Architecture 16.0.0 and greater

set(AS "clang")
set(CC "clang")
set(CXX "clang++")
set(CPP "clang")
set(OC "llvm-objcopy")

set(TOOLCHAIN_ROOT "${REGISTERED_TOOLCHAIN_ROOT}")
set(TOOLCHAIN_VERSION "${REGISTERED_TOOLCHAIN_VERSION}")

if(DEFINED TOOLCHAIN_ROOT)
  set(PREFIX)
  set(EXT)
  
  set(AS ${TOOLCHAIN_ROOT}/${PREFIX}${AS}${EXT})
  set(CC ${TOOLCHAIN_ROOT}/${PREFIX}${CC}${EXT})
  set(CXX ${TOOLCHAIN_ROOT}/${PREFIX}${CXX}${EXT})
  set(CPP ${TOOLCHAIN_ROOT}/${PREFIX}${CPP}${EXT})
  set(OC ${TOOLCHAIN_ROOT}/${PREFIX}${OC}${EXT})
endif()

# Helpers

function(cbuild_set_defines lang defines)
  set(TMP_DEFINES)
  foreach(DEFINE ${${defines}})
    string(REPLACE "\"" "\\\"" ENTRY ${DEFINE})
    string(REGEX REPLACE "=.*" "" KEY ${ENTRY})
    if (KEY STREQUAL ENTRY)
      set(VALUE "1")
    else()
      string(REGEX REPLACE ".*=" "" VALUE ${ENTRY})
    endif()
    string(APPEND TMP_DEFINES "-D${ENTRY} ")
  endforeach()
  set(${defines} ${TMP_DEFINES} PARENT_SCOPE)
endfunction()

set(OPTIMIZE_VALUES       "debug" "none" "balanced" "size" "speed")
set(OPTIMIZE_CC_FLAGS     "-Og"   "-O0"  "-O2"      "-Os" "-O3")
set(OPTIMIZE_ASM_FLAGS    ${OPTIMIZE_CC_FLAGS})
set(OPTIMIZE_CXX_FLAGS    ${OPTIMIZE_CC_FLAGS})
set(OPTIMIZE_LD_FLAGS     ${OPTIMIZE_CC_FLAGS})

set(DEBUG_VALUES          "on"      "off")
set(DEBUG_CC_FLAGS        "-g3"     "-g0")
set(DEBUG_CXX_FLAGS       ${DEBUG_CC_FLAGS})
set(DEBUG_LD_FLAGS        ${DEBUG_CC_FLAGS})
set(DEBUG_ASM_FLAGS       ${DEBUG_CC_FLAGS})

set(WARNINGS_VALUES       "on" "off" "all")
set(WARNINGS_CC_FLAGS     ""   "-w"  "-Wall")
set(WARNINGS_ASM_FLAGS    ""   "-w"  "-Wall")
set(WARNINGS_CXX_FLAGS    ""   "-w"  "-Wall")
set(WARNINGS_LD_FLAGS     ""   "-w"  "-Wall")

set(LANGUAGE_VALUES       "c90"      "gnu90"      "c99"      "gnu99"      "c11"      "gnu11"      "c17"      "gnu17"      "c23"      "gnu23"      "c++98"      "gnu++98"      "c++03"      "gnu++03"      "c++11"      "gnu++11"      "c++14"      "gnu++14"      "c++17"      "gnu++17"      "c++20"      "gnu++20"      "c++23"      "gnu++23")
set(LANGUAGE_CC_FLAGS     "-std=c90" "-std=gnu90" "-std=c99" "-std=gnu99" "-std=c11" "-std=gnu11" "-std=c17" "-std=gnu17" "-std=c23" "-std=gnu23" ""           ""             ""           ""             ""           ""             ""           ""             ""           ""             ""           ""             ""           "")
set(LANGUAGE_CXX_FLAGS    ""         ""           ""         ""           ""         ""           ""         ""           ""         ""           "-std=c++98" "-std=gnu++98" "-std=c++03" "-std=gnu++03" "-std=c++11" "-std=gnu++11" "-std=c++14" "-std=gnu++14" "-std=c++17" "-std=gnu++17" "-std=c++20" "-std=gnu++20" "-std=c++23" "-std=gnu++23")

function(cbuild_set_option_flags lang option value flags)
  if(NOT DEFINED ${option}_${lang}_FLAGS)
    return()
  endif()
  list(FIND ${option}_VALUES "${value}" _index)
  if (${_index} GREATER -1)
    list(GET ${option}_${lang}_FLAGS ${_index} flag)
    set(${flags} "${flag} ${${flags}}" PARENT_SCOPE)
  elseif(NOT value STREQUAL "")
    string(TOLOWER "${option}" _option)
    message(FATAL_ERROR "unkown '${_option}' value '${value}' !")
  endif()
endfunction()

function(cbuild_set_options_flags lang optimize debug warnings language flags)
  set(opt_flags)
  cbuild_set_option_flags(${lang} OPTIMIZE "${optimize}" opt_flags)
  cbuild_set_option_flags(${lang} DEBUG    "${debug}"    opt_flags)
  cbuild_set_option_flags(${lang} WARNINGS "${warnings}" opt_flags)
  cbuild_set_option_flags(${lang} LANGUAGE "${language}" opt_flags)
  set(${flags} "${opt_flags} ${${flags}}" PARENT_SCOPE)
endfunction()

set(CLANG_MFPU "none")
set(CLANG_MFLOATABI "soft")

if(CPU STREQUAL "Cortex-M0")
  set(CLANG_ARCH "armv6m")
  set(CLANG_ARCH_SUFFIX "_soft_nofp")
  set(CLANG_MCPU "cortex-m0")  
elseif(CPU STREQUAL "Cortex-M0+")
  set(CLANG_ARCH "armv6m")
  set(CLANG_ARCH_SUFFIX "_soft_nofp")
  set(CLANG_MCPU "cortex-m0plus")
elseif(CPU STREQUAL "Cortex-M1")
  set(CLANG_ARCH "armv6m")
  set(CLANG_ARCH_SUFFIX "_soft_nofp")
  set(CLANG_MCPU "cortex-m1")
elseif(CPU STREQUAL "Cortex-M3")
  set(CLANG_ARCH "armv7m")
  set(CLANG_ARCH_SUFFIX "_soft_nofp")
  set(CLANG_MCPU "cortex-m3")
elseif(CPU STREQUAL "Cortex-M4")
  set(CLANG_ARCH "armv7em")  
  if(FPU STREQUAL "SP_FPU")
    set(CLANG_ARCH_SUFFIX "_hard_fpv4_sp_d16")
    set(CLANG_MCPU "cortex-m4")
    set(CLANG_MFPU "fpv4-sp-d16")
    set(CLANG_MFLOATABI "hard")
  else()
    set(CLANG_ARCH_SUFFIX "_soft_nofp")
    set(CLANG_MCPU "cortex-m4")
  endif()
elseif(CPU STREQUAL "Cortex-M7")
  set(CLANG_ARCH "armv7em")
  if(FPU STREQUAL "DP_FPU")
    set(CLANG_ARCH_SUFFIX "_hard_fpv5_d16")
    set(CLANG_MCPU "cortex-m7")
    set(CLANG_MFPU "fpv5-d16")
    set(CLANG_MFLOATABI "hard")
  elseif(FPU STREQUAL "SP_FPU")
    set(CLANG_ARCH_SUFFIX "_hard_fpv4_sp_d16")
    set(CLANG_MCPU "cortex-m7")
    set(CLANG_MFPU "fpv5-sp-d16")
    set(CLANG_MFLOATABI "hard")
  else()
    set(CLANG_ARCH_SUFFIX "_soft_nofp")
    set(CLANG_MCPU "cortex-m7")
  endif()
elseif(CPU STREQUAL "Cortex-M23")
  set(CLANG_ARCH "armv6m")
  set(CLANG_ARCH_SUFFIX "_soft_nofp")
  set(CLANG_MCPU "cortex-m23")
elseif(CPU STREQUAL "Cortex-M33")
  set(CLANG_ARCH "armv8m.main")
  if(FPU STREQUAL "SP_FPU")
    set(CLANG_ARCH_SUFFIX "_hard_fp")
    if(DSP STREQUAL "DSP")
      set(CLANG_MCPU "cortex-m33")
    else()
      set(CLANG_MCPU "cortex-m33+nodsp")      
    endif()
    set(CLANG_MFPU "fpv5-sp-d16")
    set(CLANG_MFLOATABI "hard")
  else()
    set(CLANG_ARCH_SUFFIX "_soft_nofp")
    if(DSP STREQUAL "DSP")
      set(CLANG_MCPU "cortex-m33")
    else()
      set(CLANG_MCPU "cortex-m33+nodsp")
    endif()
  endif()
elseif(CPU STREQUAL "Cortex-M35P")
  set(CLANG_ARCH "armv8m.main")
  if(FPU STREQUAL "SP_FPU")
    set(CLANG_ARCH_SUFFIX "_hard_fp")
    if(DSP STREQUAL "DSP")
      set(CLANG_MCPU "cortex-m35p")
    else()
      set(CLANG_MCPU "cortex-m35p+nodsp")
    endif()
    set(CLANG_MFPU "fpv5-sp-d16")
    set(CLANG_MFLOATABI "hard")
  else()
    set(CLANG_ARCH_SUFFIX "_soft_nofp")
    if(DSP STREQUAL "DSP")
      set(CLANG_MCPU "cortex-m35p")
    else()
      set(CLANG_MCPU "cortex-m35p+nodsp")
    endif()
  endif()
elseif(CPU STREQUAL "Cortex-M52")
  set(CLANG_ARCH "armv8.1m.main")
  if(FPU STREQUAL "NO_FPU")
    if(MVE STREQUAL "NO_MVE")
      set(CLANG_ARCH_SUFFIX "_soft_nofp_nomve")
      set(CLANG_MCPU "cortex-m52+nofp+nomve")
    else()
      set(CLANG_ARCH_SUFFIX "_hard_nofp_mve") 
      set(CLANG_MCPU "cortex-m52+nofp")
      set(CLANG_MFLOATABI "hard")
    endif()
  else()
    set(CLANG_ARCH_SUFFIX "_hard_fp") 
    if(MVE STREQUAL "NO_MVE")
      set(CLANG_MCPU "cortex-m52+nomve")
    elseif(MVE STREQUAL "MVE")
      set(CLANG_MCPU "cortex-m52+nomve.fp")
    else()
      set(CLANG_MCPU "cortex-m52")
    endif()
    set(CLANG_MFPU "fpv5-sp-d16")
    set(CLANG_MFLOATABI "hard")
  endif()
elseif(CPU STREQUAL "Cortex-M55")
  set(CLANG_ARCH "armv8.1m.main")
  if(FPU STREQUAL "NO_FPU")
    if(MVE STREQUAL "NO_MVE")
      set(CLANG_ARCH_SUFFIX "_soft_nofp_nomve")
      set(CLANG_MCPU "cortex-m55+nofp+nomve")
    else()
      set(CLANG_ARCH_SUFFIX "_hard_nofp_mve") 
      set(CLANG_MCPU "cortex-m55+nofp")
      set(CLANG_MFLOATABI "hard")
    endif()
  else()
    set(CLANG_ARCH_SUFFIX "_hard_fp") 
    if(MVE STREQUAL "NO_MVE")
      set(CLANG_MCPU "cortex-m55+nomve")
    elseif(MVE STREQUAL "MVE")
      set(CLANG_MCPU "cortex-m55+nomve.fp")
    else()
      set(CLANG_MCPU "cortex-m55")
    endif()
    set(CLANG_MFPU "fpv5-sp-d16")
    set(CLANG_MFLOATABI "hard")
  endif()
elseif(CPU STREQUAL "Cortex-M85")
  set(CLANG_ARCH "armv8.1m.main")
  if(FPU STREQUAL "NO_FPU")
    if(MVE STREQUAL "NO_MVE")
      set(CLANG_ARCH_SUFFIX "_soft_nofp_nomve")
      set(CLANG_MCPU "cortex-m85+nofp+nomve")
    else()
      set(CLANG_ARCH_SUFFIX "_hard_nofp_mve") 
      set(CLANG_MCPU "cortex-m85+nofp")
      set(CLANG_MFLOATABI "hard")
    endif()
  else()
    set(CLANG_ARCH_SUFFIX "_hard_fp") 
    if(MVE STREQUAL "NO_MVE")
      set(CLANG_MCPU "cortex-m85+nomve")
    elseif(MVE STREQUAL "MVE")
      set(CLANG_MCPU "cortex-m85+nomve.fp")
    else()
      set(CLANG_MCPU "cortex-m85")
    endif()
    set(CLANG_MFPU "fpv5-sp-d16")
    set(CLANG_MFLOATABI "hard")
  endif()
elseif(CPU STREQUAL "Cortex-A5")
  set(CLANG_ARCH "armv7")
  if(FPU STREQUAL "DP_FPU")
    set(CLANG_MCPU "cortex-a5+nosimd")
    set(CLANG_MFPU "vfpv4-d16")
    set(CLANG_MFLOATABI "hard")
  else()
    set(CLANG_MCPU "cortex-a5+nosimd+nofp")
  endif()
elseif(CPU STREQUAL "Cortex-A7")
  set(CLANG_ARCH "armv7")
  if(FPU STREQUAL "DP_FPU")
    set(CLANG_MCPU "cortex-a7+nosimd")
    set(CLANG_MFPU "vfpv4-d16")
    set(CLANG_MFLOATABI "hard")
  else()
    set(CLANG_MCPU "cortex-a7+nosimd+nofp")
  endif()
elseif(CPU STREQUAL "Cortex-A9")
  set(CLANG_ARCH "armv7")
  if(FPU STREQUAL "DP_FPU")
    set(CLANG_MCPU "cortex-a9+nosimd")
    set(CLANG_MFPU "vfpv3-d16")
    set(CLANG_MFLOATABI "hard")
  else()
    set(CLANG_MCPU "cortex-a9+nosimd+nofp")
  endif()
elseif(CPU STREQUAL "Cortex-A35")
  set(CLANG_ARCH "armv8")
  if(FPU STREQUAL "DP_FPU")
    set(CLANG_MCPU "cortex-a35")
  else()
    set(CLANG_MCPU "cortex-a35+nofp")
  endif()
elseif(CPU STREQUAL "Cortex-A53")
  set(CLANG_ARCH "armv8")
  if(FPU STREQUAL "DP_FPU")
    set(CLANG_MCPU "cortex-a53")
  else()
    set(CLANG_MCPU "cortex-a53+nofp")
  endif()
elseif(CPU STREQUAL "Cortex-A55")
  set(CLANG_ARCH "armv8")
  if(FPU STREQUAL "DP_FPU")
    set(CLANG_MCPU "cortex-a55")
  else()
    set(CLANG_MCPU "cortex-a55+nofp")
  endif()
elseif(CPU STREQUAL "Cortex-A57")
  set(CLANG_ARCH "armv8")
  if(FPU STREQUAL "DP_FPU")
    set(CLANG_MCPU "cortex-a57")
  else()
    set(CLANG_MCPU "cortex-a57+nofp")
  endif()
elseif(CPU STREQUAL "Cortex-R4")
  set(CLANG_ARCH "armv7")
  if(FPU STREQUAL "DP_FPU")
    set(CLANG_MCPU "cortex-r4f")
    set(CLANG_MFPU "vfpv3-d16")
    set(CLANG_MFLOATABI "hard")
  else()
    set(CLANG_MCPU "cortex-r4+nofp")
  endif()
elseif(CPU STREQUAL "Cortex-R5")
  set(CLANG_ARCH "armv7")
  if(FPU STREQUAL "DP_FPU")
    set(CLANG_MCPU "cortex-r5")
    set(CLANG_MFPU "vfpv3-d16")
    set(CLANG_MFLOATABI "hard")
  else()
    set(CLANG_MCPU "cortex-r5+nofp")
  endif()
elseif(CPU STREQUAL "Cortex-R7")
  set(CLANG_ARCH "armv7")
  if(FPU STREQUAL "DP_FPU")
    set(CLANG_MCPU "cortex-r7")
    set(CLANG_MFPU "vfpv3-d16")
    set(CLANG_MFLOATABI "hard")
  else()
    set(CLANG_MCPU "-mcpu=cortex-r7+nofp")
  endif()
elseif(CPU STREQUAL "Cortex-R8")
  set(CLANG_ARCH "armv7")
  if(FPU STREQUAL "DP_FPU")
    set(CLANG_MCPU "cortex-r8")
    set(CLANG_MFPU "vfpv3-d16")
    set(CLANG_MFLOATABI "hard")
  else()
    set(CLANG_MCPU "-mcpu=cortex-r8+nofp")
  endif()
endif()
if(NOT DEFINED CLANG_MCPU)
  message(FATAL_ERROR "Error: CPU is not supported!")
endif()

if(CLANG_ARCH STREQUAL "armv8")
  set(CLANG_TARGET "aarch64-none-elf")
  set(CLANG_CPU "-mcpu=${CLANG_MCPU}")
else()
  if(CLANG_MFLOATABI STREQUAL "hard")
    set(CLANG_TARGET "${CLANG_ARCH}-none-eabihf")
  else()
    set(CLANG_TARGET "${CLANG_ARCH}-none-eabi")
  endif()
  set(CLANG_CPU "--target=${CLANG_TARGET} -mcpu=${CLANG_MCPU} -mfloat-abi=${CLANG_MFLOATABI}")
endif()

set(CMAKE_CXX_COMPILER_TARGET "${CLANG_TARGET}")
set(CMAKE_C_COMPILER_TARGET   "${CLANG_TARGET}")

# Assembler

set(ASM_CPU "${CLANG_CPU}")
set(ASM_DEFINES ${DEFINES})
cbuild_set_defines(ASM ASM_DEFINES)

set(ASM_OPTIONS_FLAGS)
cbuild_set_options_flags(ASM "${OPTIMIZE}" "${DEBUG}" "${WARNINGS}" "" ASM_OPTIONS_FLAGS)

if(BYTE_ORDER STREQUAL "Little-endian")
  set(ASM_BYTE_ORDER "-mlittle-endian")
elseif(BYTE_ORDER STREQUAL "Big-endian")
  set(ASM_BYTE_ORDER "-mbig-endian")
endif()

# C Pre-Processor

if(SECURE STREQUAL "Secure" OR SECURE STREQUAL "Secure-only")
  set(CC_SECURE "-mcmse")
endif()

set(CPP_FLAGS "-E -P ${CLANG_CPU} -xc ${CC_SECURE}")
set(CPP_DEFINES ${LD_SCRIPT_PP_DEFINES})
cbuild_set_defines(CC CPP_DEFINES)
if(DEFINED LD_REGIONS AND NOT LD_REGIONS STREQUAL "")
  set(CPP_INCLUDES "-include \"${LD_REGIONS}\"")
endif()
set(CPP_ARGS_LD_SCRIPT "${CPP_FLAGS} ${CPP_DEFINES} ${CPP_INCLUDES} \"${LD_SCRIPT}\" -o \"${LD_SCRIPT_PP}\"")
separate_arguments(CPP_ARGS_LD_SCRIPT NATIVE_COMMAND ${CPP_ARGS_LD_SCRIPT})

# C Compiler

set(CC_CPU "${CLANG_CPU}")
set(CC_DEFINES ${ASM_DEFINES})
set(CC_BYTE_ORDER ${ASM_BYTE_ORDER})
set(CC_FLAGS "")
set(CC_LTO "-flto")
set(_PI "-include ")
set(_ISYS "-isystem ")
set(CC_OPTIONS_FLAGS)
cbuild_set_options_flags(CC "${OPTIMIZE}" "${DEBUG}" "${WARNINGS}" "${LANGUAGE_CC}" CC_OPTIONS_FLAGS)

if(BRANCHPROT STREQUAL "NO_BRANCHPROT")
  set(CC_BRANCHPROT "-mbranch-protection=none")
elseif(BRANCHPROT STREQUAL "BTI")
  set(CC_BRANCHPROT "-mbranch-protection=bti")
elseif(BRANCHPROT STREQUAL "BTI_SIGNRET")
  set(CC_BRANCHPROT "-mbranch-protection=bti+pac-ret")
endif()

# C++ Compiler

set(CXX_CPU "${CC_CPU}")
set(CXX_DEFINES "${CC_DEFINES}")
set(CXX_BYTE_ORDER "${CC_BYTE_ORDER}")
set(CXX_SECURE "${CC_SECURE}")
set(CXX_BRANCHPROT "${CC_BRANCHPROT}")
set(CXX_FLAGS "${CC_FLAGS}")
set(CXX_LTO "${CC_LTO}")
set(CXX_OPTIONS_FLAGS)
cbuild_set_options_flags(CXX "${OPTIMIZE}" "${DEBUG}" "${WARNINGS}" "${LANGUAGE_CXX}" CXX_OPTIONS_FLAGS)

# Linker

set(LD_CPU ${CLANG_CPU})
set(_LS "-T ")

if(SECURE STREQUAL "Secure")
  set(LD_SECURE "-Wl,--cmse-implib -Wl,--out-implib=\"${OUT_DIR}/${CMSE_LIB}\"")
endif()

set(LD_LTO "-flto")
set(LD_FLAGS)
set(LD_OPTIONS_FLAGS)
cbuild_set_options_flags(LD "${OPTIMIZE}" "${DEBUG}" "${WARNINGS}" "" LD_OPTIONS_FLAGS)

# Group libraries for rescanning
set(LIB_FILES -Wl,--start-group ${LIB_FILES} -Wl,--end-group)

# ELF to HEX conversion
set (ELF2HEX -O ihex "${OUT_DIR}/$<TARGET_PROPERTY:${TARGET},OUTPUT_NAME>$<TARGET_PROPERTY:${TARGET},SUFFIX>" "${OUT_DIR}/${HEX_FILE}")

# ELF to BIN conversion
set (ELF2BIN -O binary "${OUT_DIR}/$<TARGET_PROPERTY:${TARGET},OUTPUT_NAME>$<TARGET_PROPERTY:${TARGET},SUFFIX>" "${OUT_DIR}/${BIN_FILE}")

# Linker Map file generation
set (LD_MAP -Wl,-Map="${OUT_DIR}/${LD_MAP_FILE}")

# Set CMake variables for toolchain initialization
set(CMAKE_C_FLAGS_INIT "${CC_CPU}")
set(CMAKE_CXX_FLAGS_INIT "${CXX_CPU}")
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_CROSSCOMPILING TRUE)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_ASM_COMPILER "${AS}")
set(CMAKE_C_COMPILER "${CC}")
set(CMAKE_CXX_COMPILER "${CXX}")
set(CMAKE_OBJCOPY "${OC}")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMakeASM")
