include(DumpCMakeProjectVersion)
include(ProjectVersionFromGitTag)
get_version_from_git_tag("tools/projmgr/")

project(ProjMgr VERSION "${PROJECT_VERSION}")
dump_cmake_project_version()

configure_file(src/ProductInfo.h.in ProductInfo.h)

file(DOWNLOAD
  https://raw.githubusercontent.com/Open-CMSIS-Pack/debug-adapter-registry/refs/heads/main/registry/debug-adapters.yml
  ${CMAKE_CURRENT_SOURCE_DIR}/templates/debug-adapters.yml
)

# rpc interface
include(FetchContent)
FetchContent_Declare(
  rpc-interface
  DOWNLOAD_EXTRACT_TIMESTAMP ON
  URL https://github.com/Open-CMSIS-Pack/csolution-rpc/releases/download/v0.0.3/csolution-rpc.zip
  URL_HASH SHA256=edc762373b3b7dad5b966ecb271f03866b12841675e0967a809c10c9883f29f4
)
FetchContent_MakeAvailable(rpc-interface)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT projmgr)

# projmgr library
SET(PROJMGR_SOURCE_FILES ProjMgr.cpp ProjMgrKernel.cpp ProjMgrCallback.cpp
  ProjMgrParser.cpp ProjMgrWorker.cpp ProjMgrGenerator.cpp ProjMgrXmlParser.cpp
  ProjMgrYamlParser.cpp ProjMgrLogger.cpp ProjMgrYamlSchemaChecker.cpp
  ProjMgrYamlEmitter.cpp ProjMgrUtils.cpp ProjMgrExtGenerator.cpp
  ProjMgrCbuildBase.cpp ProjMgrCbuild.cpp ProjMgrCbuildIdx.cpp
  ProjMgrCbuildGenIdx.cpp ProjMgrCbuildPack.cpp ProjMgrCbuildSet.cpp
  ProjMgrCbuildRun.cpp ProjMgrRunDebug.cpp
  ProjMgrRpcServer.cpp ProjMgrRpcServerData.cpp
)
SET(PROJMGR_HEADER_FILES ProjMgr.h ProjMgrKernel.h ProjMgrCallback.h
  ProjMgrParser.h ProjMgrWorker.h ProjMgrGenerator.h ProjMgrXmlParser.h
  ProjMgrYamlParser.h ProjMgrLogger.h ProjMgrYamlSchemaChecker.h
  ProjMgrYamlEmitter.h ProjMgrUtils.h ProjMgrExtGenerator.h
  ProjMgrCbuildBase.h ProjMgrRunDebug.h
  ProjMgrRpcServer.h ProjMgrRpcServerData.h
)

list(TRANSFORM PROJMGR_SOURCE_FILES PREPEND src/)
list(TRANSFORM PROJMGR_HEADER_FILES PREPEND include/)

add_library(projmgrlib OBJECT
  ${PROJMGR_SOURCE_FILES}
  ${PROJMGR_HEADER_FILES}
  ${rpc-interface_SOURCE_DIR}/RpcInterface.h
)
target_link_libraries(projmgrlib
  PUBLIC
  CrossPlatform RteFsUtils RteUtils XmlTree XmlTreeSlim XmlReader
  RteModel cxxopts yaml-cpp YmlSchemaChecker
)
target_include_directories(projmgrlib PUBLIC include ${PROJECT_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/external/json
  ${CMAKE_SOURCE_DIR}/external/json-rpc-cxx/include
  ${rpc-interface_SOURCE_DIR}
)

if(SWIG_LIBS)
  # projmgr swig
  add_subdirectory(swig)
else()
  # projmgr target
  add_executable(projmgr src/ProjMgrMain.cpp)
  set_target_properties(projmgr PROPERTIES OUTPUT_NAME "csolution")
  if(MSVC)
    target_link_options(projmgr PUBLIC "$<$<CONFIG:Debug>:/SAFESEH:NO>")
  endif(MSVC)
  set_property(TARGET projmgr PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
     "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_link_options(projmgr PUBLIC "-static")
  endif()
  target_link_libraries(projmgr projmgrlib)
  target_include_directories(projmgr PRIVATE include)
  if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    target_sources(projmgr PRIVATE src/csolution.manifest)
  endif()
endif()

# projmgr test
enable_testing()
add_subdirectory(test)
