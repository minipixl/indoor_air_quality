\# Quick Setup



1\. Frisches Raspberry Pi OS Lite installieren

2\. SSH aktivieren

3\. Repository klonen:



&nbsp;  git clone https://github.com/USER/REPO.git

&nbsp;  cd REPO



4\. Setup ausführen:



&nbsp;  chmod +x setup.sh

&nbsp;  ./setup.sh



5\. Reboot:



&nbsp;  sudo reboot

Processes:\
joerg@pi2joerg:/ $ pstree
systemd─┬─ModemManager───2*[{ModemManager}]
        ├─NetworkManager───2*[{NetworkManager}]
        ├─2*[agetty]
        ├─avahi-daemon───avahi-daemon
        ├─chronograf───7*[{chronograf}]
        ├─cron
        ├─dbus-daemon
        ├─influxd───14*[{influxd}]
        ├─mosquitto
        ├─polkitd───2*[{polkitd}]
        ├─sshd─┬─sshd───sshd───sftp-server
        │      └─sshd───sshd───bash───pstree
        ├─systemd───(sd-pam)
        ├─systemd-journal
        ├─systemd-logind
        ├─systemd-timesyn───{systemd-timesyn}
        ├─systemd-udevd
        ├─telegraf───8*[{telegraf}]
        ├─thd
        ├─watchdog
        └─wpa_supplicant


<img width="3652" height="2544" alt="image" src="https://github.com/user-attachments/assets/737a75f8-07d4-47f0-a438-c0a3a9309afc" />



