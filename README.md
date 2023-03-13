# Smart Prometheus Gasmeter
This Project uses an analog hall effect Sensor and a ESP8266 together with a software Schmittrigger to count the impulses of a gas meter.
The Count is then exposed as a Prometheus metric.

## Project State
Working and maintained. Used by myself.

## Features
- Prometheus Counter metrics
- live csv output of raw values via "Telnet"
- builtin OTA
- 

### Potential Future features
- Auto ajustment of trigger values

## Howto setup WIFI
1) To setup your WIFI's copy the data/wifi-sample.txt to data/wifi.txt 
2) Put SSID and Password in Lines following each other
3) Build and Upload the Sketch
4) Build and upload the SPI-FS

## TODOs
- Move Schmittigger and Button implentation into own Class and only call update() on it in loop
