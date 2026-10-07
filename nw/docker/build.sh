#!/bin/bash
# Сборка игры в docker-контейнере.
#   ./build.sh [release|debug] [--clean]
# TASM (проприетарный) в образ не входит: положите tasmx.exe (или tasm.exe)
# рядом с этим скриптом в каталог docker/tasm/ — он будет смонтирован в контейнер.
set -e
cd "$(dirname "$0")"

IMG=nw-build
[ "$(docker images -q $IMG 2>/dev/null)" ] || docker build -t $IMG .

EXTRA_MOUNTS=()
[ -d tasm ] && EXTRA_MOUNTS=(-v "$(pwd)/tasm":/opt/tasm-host:ro)

SRC="$(cd .. && pwd)"
[ -t 0 ] && TTY=-it
docker run --rm ${TTY:+-it} \
    -v "$SRC":/work/src \
    -v "$(pwd)/nw-build":/usr/local/bin/nw-build:ro \
    "${EXTRA_MOUNTS[@]}" \
    $IMG nw-build "$@"
