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
      https://github.com/open-watcom/open-watcom-v2/releases/download/Current-build/open-watcom-2_0-c-win-x64.exe && \
    cd /tmp/owx && 7z x -y /tmp/ow-setup.exe >/dev/null && \
    cp -a /tmp/owx/binnt /tmp/owx/binw /tmp/owx/binl64 /tmp/owx/h /tmp/owx/lib286 /tmp/owx/lib386 /tmp/owx/eddat /opt/watcom/ 2>/dev/null; \
    test -f /opt/watcom/binnt/wpp386.exe && \
    test -f /opt/watcom/binnt/wmake.exe && \
    rm -rf /tmp/owx /tmp/ow-setup.exe

# --- обёртки: watcom-инструменты и angelmasm через wine ---
# --- jwlink (форк wlink) — линкер: у OW2-wlink из снапшота нет system nt_win ---
RUN curl -fsSL -o /tmp/jwlink.zip \
      "https://downloads.sourceforge.net/project/jwlink/preliminary%20versions/JWlink_v19b13_linux.zip" && \
    cd /tmp && unzip -o -q jwlink.zip && mv /tmp/jwlink /opt/jwlink && chmod +x /opt/jwlink && \
    rm -f /tmp/jwlink.zip

# --- линкер из CI-снапшота (у него есть system nt_win; в установщике Current-build он сломан) ---
RUN curl -fsSL -o /tmp/ow.tar.xz \
      https://github.com/open-watcom/open-watcom-v2/releases/download/Last-CI-build/ow-snapshot.tar.xz && \
    mkdir -p /opt/owsnap && tar -xJf /tmp/ow.tar.xz -C /opt/owsnap ./binl64 ./binnt ./lib386 && \
    chmod +x /opt/owsnap/binl64/* && rm -f /tmp/ow.tar.xz

COPY bin/ /usr/local/bin/
RUN chmod +x /usr/local/bin/*

COPY entrypoint.sh /usr/local/bin/nw-entrypoint
COPY nw-build /usr/local/bin/nw-build
RUN chmod +x /usr/local/bin/nw-entrypoint /usr/local/bin/nw-build

ENV WINEPREFIX=/opt/wineprefix \
    WINEDEBUG=-all \
    WINEDLLOVERRIDES="mscoree,mshtml=" \
    WINEARCH=win64 \
    WATCOM=C:\\WATCOM \
    EDPATH=C:\\WATCOM\\EDDAT \
    INCLUDE=C:\\WATCOM\\h;C:\\WATCOM\\h\\nt

WORKDIR /work
ENTRYPOINT ["nw-entrypoint"]
