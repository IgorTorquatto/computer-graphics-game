cmake_minimum_required(VERSION 3.10)

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
endif()

include_directories("${PROJECT_ROOT}/src")

# cria executável
add_executable(${PROJECT_NAME} ${SOURCES})

# define diretórios de saída específicos por configuração
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${PROJECT_ROOT}/bin/debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_ROOT}/bin/release"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_ROOT}/bin/relwithdebinfo"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${PROJECT_ROOT}/bin/minsizerel"
)

# bibliotecas por plataforma
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE glut GL GLU m)
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework OpenGL" "-framework GLUT" m ${COCOA_LIBRARY})
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE glut GL GLU m)
endif()
