# Smart Prometheus Gasmeter
This Project uses an analog hall effect Sensor and a Software Schmittrigger to count the impulses of a gas meter.
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

## TODOs
- Move Schmittigger and Button implentation into own Class and only call update() on it in loop
