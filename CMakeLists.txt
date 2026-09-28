cmake_minimum_required(VERSION 3.24)
project(ShelbyPapyrusAE VERSION 1.3.0 LANGUAGES CXX C RC)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(/utf-8)

set(ENABLE_SKYRIM_AE ON CACHE BOOL "" FORCE)
set(ENABLE_SKYRIM_SE OFF CACHE BOOL "" FORCE)
set(ENABLE_SKYRIM_VR OFF CACHE BOOL "" FORCE)
set(COMMONLIB_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

string(TIMESTAMP COPYRIGHT_YEAR "%Y")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/DLLVersion.rc.in" "${CMAKE_CURRENT_BINARY_DIR}/version.rc" @ONLY)
configure_file("ModVersion.h.in" "${CMAKE_CURRENT_BINARY_DIR}/include/Version.h" @ONLY)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp src/*.h include/*.hpp include/*.h)
add_library(${PROJECT_NAME} SHARED Main.cpp ${SOURCES} "${CMAKE_CURRENT_BINARY_DIR}/version.rc")

add_subdirectory(lib/CommonLibSSE-NG)

target_include_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/CommonLibSSE-NG/include"
        "${CMAKE_CURRENT_BINARY_DIR}/include"
)

target_compile_definitions(${PROJECT_NAME} PRIVATE SKYRIM_SUPPORT_AE=1)

target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog fmt::fmt nlohmann_json::nlohmann_json)
target_link_libraries(${PROJECT_NAME} PUBLIC CommonLibSSE::CommonLibSSE)
target_precompile_headers(${PROJECT_NAME} PRIVATE "src/PCH.h")

set(MOD_PLUGINS_DIR "C:/Program Files (x86)/Steam/steamapps/common/Skyrim Special Edition/Data/SKSE/Plugins")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${MOD_PLUGINS_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:${PROJECT_NAME}>" "${MOD_PLUGINS_DIR}/"
)

set(SKYRIM_DIR          "C:/Program Files (x86)/Steam/steamapps/common/Skyrim Special Edition")
set(PAPYRUS_COMPILER    "${SKYRIM_DIR}/Papyrus Compiler/PapyrusCompiler.exe")
set(PAPYRUS_FLAGS       "TESV_Papyrus_Flags.flg")
set(PAPYRUS_VANILLA_SRC "${SKYRIM_DIR}/Data/Source/Scripts")
set(PAPYRUS_PEX_DIR     "${SKYRIM_DIR}/Data/Scripts")
set(PAPYRUS_PSC_DIR     "${SKYRIM_DIR}/Data/Source/Scripts")

file(GLOB PAPYRUS_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Scripts/*.psc")
set(PEX_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/pex")

if(NOT EXISTS "${PAPYRUS_COMPILER}")
    message(WARNING "PapyrusCompiler.exe not found at ${PAPYRUS_COMPILER} - .psc files will NOT be compiled")
elseif(NOT EXISTS "${PAPYRUS_VANILLA_SRC}/${PAPYRUS_FLAGS}")
    message(WARNING "${PAPYRUS_FLAGS} not found in ${PAPYRUS_VANILLA_SRC} - extract the CK's Scripts.zip there")
else()
    set(PEX_OUTPUTS "")
    foreach(PSC ${PAPYRUS_SOURCES})
        get_filename_component(SCRIPT_NAME "${PSC}" NAME_WE)
        set(PEX "${PEX_BUILD_DIR}/${SCRIPT_NAME}.pex")
        add_custom_command(
                OUTPUT "${PEX}"
                COMMAND ${CMAKE_COMMAND} -E make_directory "${PEX_BUILD_DIR}"
                COMMAND "${PAPYRUS_COMPILER}" "${PSC}"
                        "-f=${PAPYRUS_FLAGS}"
                        "-i=${CMAKE_CURRENT_SOURCE_DIR}/Scripts$<SEMICOLON>${PAPYRUS_VANILLA_SRC}"
                        "-o=${PEX_BUILD_DIR}"
                COMMAND ${CMAKE_COMMAND} -E make_directory "${PAPYRUS_PEX_DIR}" "${PAPYRUS_PSC_DIR}" "${SKYRIM_DIR}/Data/Scripts/Source"
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PEX}" "${PAPYRUS_PEX_DIR}/"
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PSC}" "${PAPYRUS_PSC_DIR}/"
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PSC}" "${SKYRIM_DIR}/Data/Scripts/Source/"
                DEPENDS "${PSC}"
                COMMENT "Papyrus: compiling ${SCRIPT_NAME}.psc"
                VERBATIM
        )
        list(APPEND PEX_OUTPUTS "${PEX}")
    endforeach()

    add_custom_target(PapyrusScripts ALL DEPENDS ${PEX_OUTPUTS})
    add_dependencies(${PROJECT_NAME} PapyrusScripts)
endif()
