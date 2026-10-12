FROM debian:bookworm-slim

# wine - запуск Win32-хостовых инструментов Open Watcom.
# Ассемблер (TASM) в образ не входит: см. docker/nw-asm-wine — .obj собираются
# отдельным скриптом на хосте один раз и кладутся в obj/wr, obj/wd.
ENV DEBIAN_FRONTEND=noninteractive
RUN dpkg --add-architecture i386 && apt-get update && \
    apt-get install -y --no-install-recommends \
        wine wine32 wine64 \
        ca-certificates curl xz-utils p7zip-full unzip && \
    rm -rf /var/lib/apt/lists/*

# --- Open Watcom 2.0 (open source, замена Watcom 10.6) ---
# Распаковываем win-хостовую сборку (binnt) — make-файлы будут работать
# с C:\NW без правок. Инструменты (32-бит PE) запускаются через wine.
RUN mkdir -p /tmp/owx /opt/watcom && \
    curl -fsSL -o /tmp/ow-setup.exe \
	  https://github.com/open-watcom/open-watcom-v2/releases/download/2024-01-01-Build/open-watcom-2_0-c-win-x86.exe && \
    cd /tmp/owx && 7z x -y /tmp/ow-setup.exe >/dev/null && \
    cp -a /tmp/owx/binnt /tmp/owx/binw /tmp/owx/binl64 /tmp/owx/h /tmp/owx/lib286 /tmp/owx/lib386 /tmp/owx/eddat /opt/watcom/ 2>/dev/null; \
    test -f /opt/watcom/binnt/wpp386.exe && \
    test -f /opt/watcom/binnt/wmake.exe && \
	test -f /opt/watcom/binnt/wlink.exe && \
    rm -rf /tmp/owx /tmp/ow-setup.exe

# --- cv2pdb: конвертация CodeView-отладки в PDB (для winedbg) ---
#RUN curl -fsSL -o /tmp/cv2pdb.zip \
#      https://github.com/rainers/cv2pdb/releases/download/v0.54/cv2pdb-0.54.zip && \
#    mkdir -p /opt/cv2pdb && cd /tmp && unzip -o -q cv2pdb.zip -d /opt/cv2pdb && \
#    rm -f /tmp/cv2pdb.zip && \
#    printf '#!/bin/bash\nexec /usr/bin/wine /opt/cv2pdb/cv2pdb.exe "$@"\n' > /usr/local/bin/cv2pdb && \
#    chmod +x /usr/local/bin/cv2pdb

COPY entrypoint.sh /usr/local/bin/nw-entrypoint
COPY nw-build /usr/local/bin/nw-build
RUN chmod +x /usr/local/bin/nw-entrypoint /usr/local/bin/nw-build

ENV WINEPREFIX=/opt/wineprefix \
    WINEDEBUG=-all \
    WINEDLLOVERRIDES="mscoree,mshtml=" \
    WINEARCH=win64 

WORKDIR /work
ENTRYPOINT ["nw-entrypoint"]
