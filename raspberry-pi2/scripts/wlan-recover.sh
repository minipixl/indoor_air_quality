#!/bin/bash

IFACE="wlan0"
ROUTER="192.168.1.1"     # anpassen
TAG="wlan-recover"

# Nur prüfen, wenn Interface existiert
ip link show "$IFACE" >/dev/null 2>&1 || exit 0

# Wenn Router erreichbar → WLAN OK
if ping -c1 -W2 "$ROUTER" >/dev/null 2>&1; then
    logger -t "$TAG" "WLAN OK"
    exit 0
fi

# Router NICHT erreichbar → möglicher Hänger
logger -t "$TAG" "WLAN stuck, reloading driver"

ip link set "$IFACE" down
modprobe -r rtl8192cu rtl_usb rtlwifi
sleep 3
modprobe rtl8192cu
ip link set "$IFACE" up
