cmake_minimum_required(VERSION 3.10)

# ------------------------------------------------------------
# Detect environment and check generator mismatch early
# ------------------------------------------------------------
# Detect MSYS/MinGW via env MSYSTEM or uname
if(DEFINED ENV{MSYSTEM})
    string(TOUPPER "$ENV{MSYSTEM}" _msys_env_upper)
else()
    execute_process(COMMAND ${CMAKE_COMMAND} -E environment
                    OUTPUT_VARIABLE _env_out)
    string(FIND "${CMAKE_SYSTEM_NAME}" "Windows" _is_windows)
    set(_msys_env_upper "")
endif()

# Helper: decide desired generator based on environment
set(_DESIRED_GENERATOR "")
if(DEFINED ENV{MSYSTEM})
    if("$ENV{MSYSTEM}" MATCHES "MINGW")
        set(_DESIRED_GENERATOR "MinGW Makefiles")
    elseif("$ENV{MSYSTEM}" MATCHES "MSYS")
        set(_DESIRED_GENERATOR "Unix Makefiles")
    endif()
endif()

if(_DESIRED_GENERATOR STREQUAL "" )
    # Fallback: on non-Windows prefer Unix Makefiles, on Windows prefer MinGW Makefiles
    if(WIN32)
        set(_DESIRED_GENERATOR "MinGW Makefiles")
    else()
        set(_DESIRED_GENERATOR "Unix Makefiles")
    endif()
endif()

# If there is an existing CMakeCache.txt, try to read the generator used previously.
if(EXISTS "${CMAKE_BINARY_DIR}/CMakeCache.txt")
    file(READ "${CMAKE_BINARY_DIR}/CMakeCache.txt" _cmake_cache_content)
    string(REGEX MATCH "CMAKE_GENERATOR:INTERNAL=([^\r\n]+)" _match "${_cmake_cache_content}")
    if(_match)
        # extract previous generator name
        string(REGEX REPLACE "CMAKE_GENERATOR:INTERNAL=([^\r\n]+).*" "\\1" _PREV_GENERATOR "${_match}")
        if(NOT _PREV_GENERATOR STREQUAL _DESIRED_GENERATOR)
            message(FATAL_ERROR
"Generator mismatch detectado.
O diretório de build atual (${CMAKE_BINARY_DIR}) foi gerado anteriormente com o generator:
  ${_PREV_GENERATOR}

Mas o ambiente atual sugere usar:
  ${_DESIRED_GENERATOR}

Soluções:
 - Remova o diretório de build atual (por ex.: rm -rf ${CMAKE_BINARY_DIR}/CMakeFiles ${CMAKE_BINARY_DIR}/CMakeCache.txt) e gere de novo, ou
 - Use um diretório de build diferente (ex.: mkdir build-mingw && cmake -S . -B build-mingw -G \"${_DESIRED_GENERATOR}\" ...).

Abortando para evitar corromper o cache existente.")
        endif()
    endif()
endif()

project(Trabalho LANGUAGES C CXX)

# padrões C/C++
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# raiz do projeto (um nível acima de scripts)
set(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")

# pega todos os .c e .cpp recursivamente
file(GLOB_RECURSE SOURCES
    "${PROJECT_ROOT}/src/*.c"
    "${PROJECT_ROOT}/src/*.cpp"
)

if(NOT SOURCES)
    message(FATAL_ERROR "Nenhum arquivo .c ou .cpp encontrado em src/")
    return()
endif()

include_directories("${PROJECT_ROOT}/src")
include_directories("${PROJECT_ROOT}/dependencies")

# Detectar arquitetura 32/64 bits
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCHITECTURE "64-bit")
    add_compile_definitions(IS_64BIT=1)
else()
    set(ARCHITECTURE "32-bit")
    add_compile_definitions(IS_32BIT=1)
endif()

message(STATUS "Plataforma: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Processador: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Arquitetura detectada: ${ARCHITECTURE}")

# Cria executável
add_executable(${PROJECT_NAME} ${SOURCES})

# Define diretórios de saída específicos por configuração
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${PROJECT_ROOT}/bin/debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_ROOT}/bin/release"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_ROOT}/bin/relwithdebinfo"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${PROJECT_ROOT}/bin/minsizerel"
)

# Copiar assets para o diretório de saída do executável
if(EXISTS "${PROJECT_ROOT}/assets")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${PROJECT_ROOT}/assets"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        COMMENT "Copiando assets para o diretório do executável"
    )
    if(EXISTS "${PROJECT_ROOT}/assets/audio/music")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${PROJECT_ROOT}/assets/audio/music"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
            COMMENT "Copiando assets de audio para o diretório do executável"
        )
    else()
        message(WARNING "Pasta assets/audio/music não encontrada em ${PROJECT_ROOT}/assets - pule a cópia")
    endif()
else()
    message(WARNING "Pasta assets não encontrada em ${PROJECT_ROOT}/assets - pule a cópia")
endif()

# --- Encontrar SDL2 e SDL2_mixer ---
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)

if(SDL2_FOUND AND SDL2_MIXER_FOUND)
    include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_MIXER_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES} ${SDL2_MIXER_LIBRARIES})
else()
    # fallback via pkg-config (Linux)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)

    include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_MIXER_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES} ${SDL2_MIXER_LIBRARIES})
endif()

# --- Link padrão por plataforma ---
if(WIN32)
    # Se você estiver no MinGW, algumas vezes é preciso linkar mingw32 antes de SDL2main
    if("$ENV{MSYSTEM}" MATCHES "MINGW")
        # Para evitar depender de nomes de libs estáticos, deixe o pacote SDL2 cuidar do link.
        # Mas caso precise, você pode adicionar:
        # target_link_libraries(${PROJECT_NAME} PRIVATE -lmingw32)
        message(STATUS "Ambiente MinGW detectado: ajustando flags de link conforme necessário.")
    endif()

    target_link_libraries(${PROJECT_NAME} PRIVATE glut GL GLU m)
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework OpenGL" "-framework GLUT" m ${COCOA_LIBRARY})
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE glut GL GLU m)
endif()

# --- Clean custom target ---
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E rm -rf
        "${PROJECT_ROOT}/bin/debug"
        "${PROJECT_ROOT}/bin/release"
        "${PROJECT_ROOT}/bin/relwithdebinfo"
        "${PROJECT_ROOT}/bin/minsizerel"
    COMMENT "Removendo todos os diretórios de build e binários"
)
