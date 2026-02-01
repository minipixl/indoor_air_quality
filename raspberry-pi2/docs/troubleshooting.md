\## WLAN hängt nach langer Offline-Zeit



Manuell testen:



sudo modprobe -r rtl8192cu rtl\_usb rtlwifi

sudo modprobe rtl8192cu



Wenn WLAN danach sofort wieder funktioniert, ist der Treiber-Reset korrekt konfiguriert.



