#!/bin/bash
# build.sh - Script para compilar el bop-rdf
# Uso: ./build.sh [opciones]

set -e # Salir en error

# Configuración por defecto
BUILD_TYPE="Release"
BUILD_DIR="build"
CLEAN_BUILD=false
SHOW_HELP=false
VERBOSE=false
JOBS=$(nproc)

# Detectar si soporta colores
if [ -t 1 ]; then
    HAS_COLOR=true
else
    HAS_COLOR=false
fi

# Función para mostrar ayuda
show_help() {
    cat <<'EOF'
Cálculo de la RDF para BOP - Script de Compilación

Uso:
    ./build.sh [opciones]

Opciones:
    -d, --debug          Compilar en modo Debug (por defecto: Release)
    -c, --clean          Limpiar directorio de build antes de compilar
    -j, --jobs N         Usar N jobs paralelos (por defecto: todos los cores)
    -v, --verbose        Mostrar output detallado
    -h, --help           Mostrar este mensaje de ayuda

Ejemplos:
    ./build.sh                    # Compilación normal Release
    ./build.sh --debug            # Compilación Debug
    ./build.sh --clean --debug    # Limpiar y compilar en Debug
    ./build.sh --jobs 4           # Usar 4 cores para compilar
    ./build.sh --verbose          # Output detallado

Descripción:
    Este script automatiza la compilación del programa que calcula la 
    función de distribución radial (RDF) de un parámetro de orden 
    (bond order parameter - bop) de las aguas que rodean una proteína.
    mediante el patrón de diseño de analizadores base.

Uso del programa después de compilar:
    ./bop-rdf <directorio> [opciones]
Opciones:
  -p <número>    Parámetro de orden a usar (1-4:Q4,Q6,W4,W6, por defecto: 2-Q6)
  -t <número>    Número de hilos (por defecto: CPUs disponibles)
  -o <archivo>   Archivo de salida (por defecto: histograma.dat)
  -min <valor>   Distancia mínima (por defecto: 0.0)
  -max <valor>   Distancia máxima (por defecto: 20.0)
  -bins <número> Número de bins (por defecto: 100)

Ejemplo:
  ./bop-rdf /ruta/a/mis/datos -p 2 -t 8 -o resultado.csv

Repositorio: https://github.com/manuxch/bop-pdf 
EOF
}

# Funciones de logging mejoradas (compatibles)
log_info() {
    if [ "$HAS_COLOR" = true ]; then
        printf "\033[34m[INFO]\033[0m %s\n" "$1"
    else
        printf "[INFO] %s\n" "$1"
    fi
}

log_success() {
    if [ "$HAS_COLOR" = true ]; then
        printf "\033[32m[SUCCESS]\033[0m %s\n" "$1"
    else
        printf "[SUCCESS] %s\n" "$1"
    fi
}

log_warning() {
    if [ "$HAS_COLOR" = true ]; then
        printf "\033[33m[WARNING]\033[0m %s\n" "$1"
    else
        printf "[WARNING] %s\n" "$1"
    fi
}

log_error() {
    if [ "$HAS_COLOR" = true ]; then
        printf "\033[31m[ERROR]\033[0m %s\n" "$1"
    else
        printf "[ERROR] %s\n" "$1"
    fi
}

# Parsear argumentos
while [[ $# -gt 0 ]]; do
    case $1 in
    -d | --debug)
        BUILD_TYPE="Debug"
        shift
        ;;
    -c | --clean)
        CLEAN_BUILD=true
        shift
        ;;
    -j | --jobs)
        if [[ $2 =~ ^[0-9]+$ ]]; then
            JOBS="$2"
            shift 2
        else
            log_error "Número de jobs inválido: $2"
            exit 1
        fi
        ;;
    -v | --verbose)
        VERBOSE=true
        shift
        ;;
    -h | --help)
        SHOW_HELP=true
        shift
        ;;
    *)
        log_error "Argumento desconocido: $1"
        echo "Usa './build.sh --help' para ver las opciones disponibles."
        exit 1
        ;;
    esac
done

# Mostrar ayuda si se solicitó
if [ "$SHOW_HELP" = true ]; then
    show_help
    exit 0
fi

echo ""

log_info "Iniciando compilación..."
log_info "Modo: $BUILD_TYPE"
log_info "Jobs: $JOBS"
log_info "Directorio de build: $BUILD_DIR"

# Limpiar build si se solicitó
if [ "$CLEAN_BUILD" = true ]; then
    log_info "Limpiando directorio de build..."
    rm -rf "$BUILD_DIR"
fi

# Crear directorio de build si no existe
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configurar CMake
log_info "Configurando CMake..."
if [ "$VERBOSE" = true ]; then
    cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" ..
else
    cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" .. >/dev/null 2>&1
fi

# Compilar
log_info "Compilando proyecto..."
if [ "$VERBOSE" = true ]; then
    cmake --build . --config "$BUILD_TYPE" --parallel "$JOBS"
else
    if cmake --build . --config "$BUILD_TYPE" --parallel "$JOBS" >/dev/null 2>&1; then
        log_success "Compilación completada exitosamente!"
    else
        log_error "Error en la compilación. Ejecuta con --verbose para más detalles."
        exit 1
    fi
fi

# Verificar que el ejecutable se creó
if [ -f "bop-rdf" ]; then
    log_success "Ejecutable generado correctamente"
    echo ""
    log_info "Ejecutable: $BUILD_DIR/bop-rdf"
    log_info "Tamaño: $(du -h bop-rdf | cut -f1)"

    # Mostrar información del ejecutable
    echo ""
    ./bop-rdf
    echo ""
    exit 0
else
    log_error "El ejecutable no se generó correctamente"
    exit 1
fi

# Mensaje final
log_success "¡El proyecto está listo para usar!"
