#!/usr/bin/env bash

set -e

cd "$( dirname "${BASH_SOURCE[0]}" )"/..

function print_info() { printf "\033[34m[INFO] $1\033[m\n"; }
function print_error() { printf "\033[31m[ERROR] $1\033[m\n"; }
function print_success() { printf "\033[32m[SUCCESS] $1\033[m\n"; }

mkdir -p bin

PROJECT_ROOT="$(pwd)"
SRC_DIR="src"
BIN="bin"
SRC_DEBUG="$BIN/debug"
SRC_RELEASE="$BIN/release"
OUT="Trabalho"

DEBUG=false
while [[ $# -gt 0 ]]; do
  case "$1" in
    -d|--debug) DEBUG=true; shift ;;
    --) shift; break ;;
    -*) print_error "Opção desconhecida: $1"; exit 1 ;;
    *) break ;;
  esac
done

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

# Usando CMake do ./scripts/CMakeLists.txt
if command -v cmake &> /dev/null; then
    print_info "CMake encontrado. Iniciando build via CMake..."
    mkdir -p "$BIN"
    cd "$BIN"

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

# fallback gcc
if command -v g++ &> /dev/null; then
    print_info "Buscando arquivos .cpp e .c recursivamente em '$SRC_DIR'..."
	readarray -d '' -t SOURCES < <(find "$SRC_DIR" \( -name '*.cpp' -o -name '*.c' \) -print0)

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
    print_info "Compilando com gcc..."
    gcc "${SOURCES[@]}" -o "$OUT" "${CXXFLAGS[@]}" "${LINK_LIBS[@]}"
    print_success "Compilado com sucesso: $OUT"
    exit 0
fi

print_error "Nenhum compilador encontrado."
exit 1
