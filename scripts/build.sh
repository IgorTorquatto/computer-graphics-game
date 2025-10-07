#!/usr/bin/env bash

set -e

cd "$( dirname "${BASH_SOURCE[0]}" )"/..

function print_info()    { printf "\033[34m[INFO] %s\033[m\n" "$1"; }
function print_error()   { printf "\033[31m[ERROR] %s\033[m\n" "$1"; }
function print_success() { printf "\033[32m[SUCCESS] %s\033[m\n" "$1"; }

mkdir -p bin

PROJECT_ROOT="$(pwd)"
SRC_DIR="src"
BIN="bin"
SRC_DEBUG="$BIN/debug"
SRC_RELEASE="$BIN/release"
OUT="Trabalho"

DEBUG=false
CLEAN=false
while [[ $# -gt 0 ]]; do
  case "$1" in
    -d|--debug) DEBUG=true; shift ;;
    -c|--clean) CLEAN=true; shift ;;
    --) shift; break ;;
    -*) print_error "Opção desconhecida: $1"; exit 1 ;;
    *) break ;;
  esac
done

if $CLEAN; then
  print_info "Limpando os arquivos de build."
  rm -rf bin/*
  # if command -v cmake &> /dev/null; then
  #   cmake --build "$BIN" --target clean-all
  # else
  #   rm -rf "$SRC_DEBUG" "$SRC_RELEASE"
  # fi
  exit 0
fi

if $DEBUG; then
  print_info "Modo DEBUG ativado."
else
  print_info "Modo Release (padrão)."
fi

UNAME="$(uname -s)"
case "$UNAME" in
  Linux*|Darwin*) LINK_LIBS=( -lglut -lGL -lGLU -lm ) ;;
  MINGW*|MSYS*|CYGWIN*) LINK_LIBS=( -lglut -lGL -lGLU -lm ) ;;
  *) LINK_LIBS=( -lglut -lGL -lGLU -lm ) ;;
esac

# ==== Build com CMake ====
if command -v cmake &> /dev/null; then
    print_info "CMake encontrado. Iniciando build via CMake..."
    mkdir -p "$BIN"
    BUILD_DIR="$BIN"
    cd "$BUILD_DIR"

    # Detectar ambiente MSYS2/MINGW via MSYSTEM ou uname
    MSYS_ENV="${MSYSTEM:-}"
    GENERATOR=""
    CMAKE_MAKE_PROG=""
    CMAKE_EXTRA_ARGS=()

    if [[ "$MSYS_ENV" == MSYS* ]] || [[ "$UNAME" == MSYS* ]]; then
        # Usuário está no MSYS (ambiente POSIX puro) — não é o recomendado para compilar via MinGW
        print_error "Parece que você está em um shell MSYS puro. Abra o 'mingw64.exe' (MinGW64) em vez de 'msys2.exe' e rode este script novamente."
        exit 1
    fi

    if [[ "$MSYS_ENV" == MINGW* ]] || [[ "$UNAME" == MINGW* ]]; then
        # MinGW environment (mingw32/mingw64)
        GENERATOR="MinGW Makefiles"
        # Tenta encontrar mingw32-make no PATH
        if command -v mingw32-make &> /dev/null; then
            CMAKE_MAKE_PROG="$(command -v mingw32-make)"
        elif [[ -x "/mingw64/bin/mingw32-make.exe" ]]; then
            CMAKE_MAKE_PROG="/mingw64/bin/mingw32-make.exe"
        elif [[ -x "/mingw32/bin/mingw32-make.exe" ]]; then
            CMAKE_MAKE_PROG="/mingw32/bin/mingw32-make.exe"
        else
            print_error "mingw32-make não encontrado. Instale o toolchain do MSYS2: pacman -S mingw-w64-x86_64-toolchain"
            exit 1
        fi
        print_info "Ambiente MinGW detectado. Usando generator: ${GENERATOR}"
        CMAKE_EXTRA_ARGS+=( "-DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROG}" )
    else
        # Linux / macOS or other POSIX
        GENERATOR="Unix Makefiles"
        print_info "Ambiente POSIX detectado. Usando generator: ${GENERATOR}"
    fi

    # Configurar tipo de build
    if $DEBUG; then
      CONFIG_TYPE="Debug"
    else
      CONFIG_TYPE="Release"
    fi

    # Invoca o CMake (coloca o source em ../scripts como no seu fluxo)
    print_info "Executando cmake (source: ../scripts) ..."
    cmake ../scripts -G "${GENERATOR}" -DCMAKE_BUILD_TYPE="${CONFIG_TYPE}" "${CMAKE_EXTRA_ARGS[@]}"

    # Número de jobs
    jobs=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 1)
    # Use cmake --build para ser portável; passa -j para o backend
    print_info "Compilando com ${jobs} jobs..."
    cmake --build . --config "${CONFIG_TYPE}" -- -j"${jobs}"

    print_success "Build via CMake finalizado."
    exit 0
fi

# ==== Fallback manual com GCC ====
if command -v g++ &> /dev/null || command -v gcc &> /dev/null; then
    print_info "Buscando arquivos .c e .cpp recursivamente em '$SRC_DIR'..."
    readarray -d '' -t SOURCES < <(find "$SRC_DIR" \( -name '*.c' -o -name '*.cpp' \) -print0)

    if [ ${#SOURCES[@]} -eq 0 ]; then
        print_error "Nenhum arquivo .c ou .cpp encontrado em $SRC_DIR"
        exit 1
    fi

    print_info "Arquivos encontrados:"
    for s in "${SOURCES[@]}"; do echo "  $s"; done

    if $DEBUG; then
      CXXFLAGS=( -g -O0 -DDEBUG )
      OUT="$SRC_DEBUG/$OUT"
    else
      CXXFLAGS=( -O2 -DNDEBUG )
      OUT="$SRC_RELEASE/$OUT"
    fi

    INCLUDE_DIRS=( "$PROJECT_ROOT/src" )
    for inc in "${INCLUDE_DIRS[@]}"; do
      CXXFLAGS+=( -I"$inc" )
    done

    mkdir -p "$(dirname "$OUT")"
    print_info "Compilando com GCC/G++..."

    # Usa g++ se disponível (linka C++ por padrão), senão gcc
    if command -v g++ &> /dev/null; then
      g++ "${SOURCES[@]}" -o "$OUT" "${CXXFLAGS[@]}" "${LINK_LIBS[@]}"
    else
      gcc "${SOURCES[@]}" -o "$OUT" "${CXXFLAGS[@]}" "${LINK_LIBS[@]}"
    fi

    print_success "Compilado com sucesso: $OUT"
    exit 0
fi

print_error "Nenhum compilador encontrado."
exit 1
