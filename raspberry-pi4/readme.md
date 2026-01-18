## Indoor air quality measurement and logging

I chose Ubuntu server LTS 22.04. for my application.\
Previously I tried PI OS lite and failed to connect to 5 GHz WLAN. On some forums I found the info that this is a HW-bug but with Ubuntu server I have no problems at all to get connected. So no HW-bug..

After ssh-login I see plenty of reserves:\
<img width="611" height="235" alt="image" src="https://github.com/user-attachments/assets/b8d133af-ac5e-4788-8f0c-820987d4ec85" />

I only use half of the enclosure to support heat dissipation. Maybe I'll drill holes into the case and then use both halves.\
The UBS-stick is the backup medium, mounted before backup, then unmounted.
<img width="620" height="871" alt="raspi4" src="https://github.com/user-attachments/assets/f04a8e0c-6a01-40af-97fa-56b05d1e5f81" />

pstree shows the processes:\
<img width="379" height="426" alt="image" src="https://github.com/user-attachments/assets/ee06d08e-740e-4a35-8c7c-c998585983b4" />

# Installations\
see the files as example.\

mosquitto ist the MQTT broker\
setup & configs: ...\
\
telegraf is a bridge between mosquitto and InfluxDB\
setup & configs: ...\
\
InfluxDB v2.8.0\
Took me some time to get the configuration running and to create and manage tokens\
setup & configs: ...\
\
Grafana v13.12.1\
for visualization of the influx database\
setup & configs: ...\
