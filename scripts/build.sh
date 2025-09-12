#!/usr/bin/env bash

set -e # Exit if any error

# garante que estamos na raiz do projeto
cd "$( dirname "${BASH_SOURCE[0]}" )"/..

function print_info() {
  printf "\033[34m[INFO] $1\033[m\n"
}

function print_error() {
  printf "\033[31m[ERROR] $1\033[m\n"
}

function print_success() {
  printf "\033[32m[SUCCESS] $1\033[m\n"
}

if [ ! -d bin ]; then
    mkdir bin
fi

PROJECT_ROOT="$(pwd)"
SRC_DIR="src"
BIN="bin"
SRC_DEBUG="$BIN/debug"
SRC_RELEASE="$BIN/release"
OUT="Trabalho"

# parse simples de opções (aceita -d e --debug)
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

# bibliotecas padrão por plataforma (pode ajustar se necessário)
UNAME="$(uname -s)"
case "$UNAME" in
  Linux*|MINGW*|MSYS*|CYGWIN*)
    # Linux / WSL / MSYS
    LINK_LIBS=( -lglut -lGL -lGLU -lm )
    ;;
  Darwin*)
    # macOS (usuário pode ajustar)
    LINK_LIBS=( -framework OpenGL -framework GLUT -lm )
    ;;
  *)
    LINK_LIBS=( -lglut -lGL -lGLU -lm )
    ;;
esac

# Verifica se o cmake está disponível
if command -v cmake &> /dev/null; then
    print_info "CMake encontrado. Iniciando build via CMake..."

    mkdir -p "$BIN"
    cd "$BIN"

    if $DEBUG; then
      cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug
    else
      cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
    fi

    jobs=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 1)
    make -j"$jobs"

    print_success "Build via CMake finalizado."
    exit 0
fi

# Se chegou aqui, não há CMake — tenta usar g++
if command -v g++ &> /dev/null; then
    print_warning "CMake não encontrado. Usando gcc diretamente..."

    print_info "Buscando arquivos .cpp recursivamente em '$SRC_DIR'..."
    # lê nomes com segurança (suporta espaços) usando -print0
    readarray -d '' -t SOURCES < <(find "$SRC_DIR" -name '*.c' -print0)

    if [ ${#SOURCES[@]} -eq 0 ]; then
        print_error "Nenhum arquivo .c encontrado em $SRC_DIR"
        exit 1
    fi

    print_success "Arquivos encontrados:"
    for s in "${SOURCES[@]}"; do
      echo "  $s"
    done

    # FLAGS dependendo do modo
    if $DEBUG; then
      CXXFLAGS=( -g -O0 -DDEBUG )
    else
      CXXFLAGS=( -O2 -DNDEBUG )
    fi

    # Include dirs (como no CMakeLists: src)
    INCLUDE_DIRS=( "$PROJECT_ROOT/src" )
    for inc in "${INCLUDE_DIRS[@]}"; do
      CXXFLAGS+=( -I"$inc" )
    done

    # garante que o diretório de saída existe
    mkdir -p "$(dirname "$OUT")"

    # Compila: usa array para lidar com muitos arquivos/nomes com espaços
    print_info "Compilando com gcc..."
    gcc "${SOURCES[@]}" -o "$OUT" "${CXXFLAGS[@]}" "${LINK_LIBS[@]}"

    print_success "Compilado com sucesso: $OUT"
    exit 0
fi

# Nenhum compilador encontrado
print_error "Nenhum compilador encontrado."
echo "Instale o CMake ou o gcc para compilar este projeto."
exit 1
