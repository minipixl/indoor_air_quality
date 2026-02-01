#!/bin/bash
set -e

echo "=== Raspberry Pi Watchdog + WLAN Recovery Setup ==="

# 1. Watchdog aktivieren
if ! grep -q "dtparam=watchdog=on" /boot/config.txt; then
    echo "dtparam=watchdog=on" | sudo tee -a /boot/config.txt
fi

sudo apt update
sudo apt install -y watchdog

# watchdog.conf
sudo tee /etc/watchdog.conf > /dev/null <<EOF
watchdog-device = /dev/watchdog
watchdog-timeout = 15
interval = 5
max-load-1 = 20
max-load-5 = 15
max-load-15 = 10
EOF

# default watchdog
sudo tee /etc/default/watchdog > /dev/null <<EOF
run_watchdog=1
run_wd_keepalive=0
watchdog_module="bcm2835_wdt"
watchdog_options=""
EOF

sudo systemctl enable watchdog

# 2. WLAN Recovery Script
sudo tee /usr/local/sbin/wlan-recover.sh > /dev/null <<'EOF'
#!/bin/bash

IFACE="wlan0"
ROUTER="192.168.1.1"
TAG="wlan-recover"

ip link show "$IFACE" >/dev/null 2>&1 || exit 0

if ping -c1 -W2 "$ROUTER" >/dev/null 2>&1; then
    exit 0
fi

logger -t "$TAG" "WLAN stuck, reloading driver"

ip link set "$IFACE" down
modprobe -r rtl8192cu rtl_usb rtlwifi
sleep 3
modprobe rtl8192cu
ip link set "$IFACE" up
EOF

sudo chmod +x /usr/local/sbin/wlan-recover.sh

# systemd service
sudo tee /etc/systemd/system/wlan-recover.service > /dev/null <<EOF
[Unit]
Description=WLAN driver recovery

[Service]
Type=oneshot
ExecStart=/usr/local/sbin/wlan-recover.sh
EOF

# systemd timer
sudo tee /etc/systemd/system/wlan-recover.timer > /dev/null <<EOF
[Unit]
Description=Check WLAN driver every 10 minutes

[Timer]
OnBootSec=5min
OnUnitActiveSec=10min
AccuracySec=1min

[Install]
WantedBy=timers.target
EOF

sudo systemctl daemon-reload
sudo systemctl enable wlan-recover.timer
sudo systemctl start wlan-recover.timer

# 3. journald Limits
sudo tee /etc/systemd/journald.conf > /dev/null <<EOF
[Journal]
Storage=volatile
RuntimeMaxUse=20M
SystemMaxUse=50M
MaxRetentionSec=7day
Compress=yes
EOF

sudo systemctl restart systemd-journald

echo "Setup complete."
echo "Reboot recommended."
