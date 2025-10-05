cmake_minimum_required(VERSION 3.12)
project(ProteinWaterAnalyzer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Incluir directorios
include_directories(include)

# Archivos fuente
set(SOURCES
    src/main.cpp
    src/proteinWaterAnalyzer.cpp
    src/threadPool.cpp
    src/histogram1D.cpp
)

# Ejecutable principal
add_executable(bop-rdf ${SOURCES})

# Habilitar pthread
find_package(Threads REQUIRED)
target_link_libraries(bop-rdf Threads::Threads)

# Optimizaciones
target_compile_options(bop-rdf PRIVATE -O2 -march=native)
