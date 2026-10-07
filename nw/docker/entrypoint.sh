#!/bin/bash
# Точка входа контейнера.
# Исходники монтируются в /work/src; drive_c/NW -> /work/src, чтобы исходники
# были видны как C:\NW (ожидание make.inc, PROJ = C:\NW).
#
# Инструментарий: Open Watcom 2.0 (C:\OW2 = /opt/watcom) для всего.
# Ассемблерные .obj собираются отдельно (docker/nw-asm-wine, Borland TASMX)
# и складываются в docker/asm-obj; nw-build копирует их в OBJ/WR, OBJ/WD.
set -e

SRC=${SRC_DIR:-/work/src}
DC="$WINEPREFIX/drive_c"
mkdir -p "$DC"
ln -sfn "$SRC"      "$DC/NW"
ln -sfn /opt/watcom "$DC/OW2"
ln -sfn /opt/watcom "$DC/WATCOM"

export WATCOM='C:\OW2' EDPATH='C:\OW2\EDDAT' INCLUDE='C:\OW2\h;C:\OW2\h\nt'
export PATH=/usr/local/bin:$PATH

cd "$SRC"

if [ $# -eq 0 ]; then exec bash; else exec "$@"; fi
