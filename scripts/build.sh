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
  Linux*|MINGW*|MSYS*|CYGWIN*) LINK_LIBS=( -lglut -lGL -lGLU -lm ) ;;
  Darwin*) LINK_LIBS=( -framework OpenGL -framework GLUT -lm ) ;;
  *) LINK_LIBS=( -lglut -lGL -lGLU -lm ) ;;
esac

# ==== Build com CMake ====
if command -v cmake &> /dev/null; then
    print_info "CMake encontrado. Iniciando build via CMake..."
    mkdir -p "$BIN"

    BUILD_DIR="$BIN"
    cd "$BUILD_DIR"

    if $DEBUG; then
      cmake ../scripts -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug
    else
      cmake ../scripts -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
    fi

    jobs=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 1)
    make -j"$jobs"

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
