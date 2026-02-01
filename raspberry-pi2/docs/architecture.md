&nbsp;                        ┌─────────────────────────┐

&nbsp;                        │     Raspberry Pi 2      │

&nbsp;                        │     Raspberry Pi OS     │

&nbsp;                        └─────────────┬───────────┘

&nbsp;                                      │

&nbsp;       ┌──────────────────────────────┼──────────────────────────────┐

&nbsp;       │                              │                              │

&nbsp;       ▼                              ▼                              ▼

┌────────────────┐         ┌────────────────────┐         ┌──────────────────┐

│ Hardware       │         │ WLAN Recovery      │         │ Logging Control  │

│ Watchdog       │         │ (Driver Reset)     │         │ journald Limits  │

├────────────────┤         ├────────────────────┤         ├──────────────────┤

│ bcm2835\_wdt    │         │ wlan-recover.timer │         │ Storage=volatile │

│ watchdogd      │         │ wlan-recover.sh    │         │ Size limits      │

│ 15s Timeout    │         │ modprobe reload    │         │ Log rotation     │

└────────────────┘         └────────────────────┘         └──────────────────┘

&nbsp;       │                              │                              │

&nbsp;       ▼                              ▼                              ▼

&nbsp;  Reboot bei                  Reset RTL8192CU                 SD-Kartenschonung

&nbsp; Kernel-Hänger                bei Firmware-Hänger



