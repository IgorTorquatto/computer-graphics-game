#!/usr/bin/env bash

cd "$( dirname "${BASH_SOURCE[0]}" )"/.. # force go to project root

function print_info() { printf "\033[34m[INFO] $1\033[m\n"; }
function print_error() { printf "\033[31m[ERROR] $1\033[m\n"; }
function print_success() { printf "\033[32m[SUCCESS] $1\033[m\n"; }

DEBUG=false
case "$1" in
    -d|--debug) DEBUG=true; shift ;;
    -r|--release) DEBUG=false; shift ;;
    --) shift; break ;;
    -*) print_error "Opção desconhecida: $1"; exit 1 ;;
    *) break ;;
esac

if $DEBUG; then
    print_info "Running binary with debug..."
    if [ ! -f "./bin/debug/Trabalho" ]; then
        ./bin/debug/Trabalho
    fi
    case $$ in
        0) print_success "Debug binary exited successfully." ;;
        *) print_error "Debug binary exited with error. ($$)"; exit 1 ;;
    esac
else
    print_info "Running release binary..."
    if [ ! -f "./bin/release/Trabalho" ]; then
        ./bin/release/Trabalho
    fi
    case $$ in
        0) print_success "Binary exited successfully." ;;
        *) print_error "Binary exited with error. ($$)"; exit 1 ;;
    esac
fi
