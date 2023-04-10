# ESP32-CAM-WebServer

### Ideas:
- Only launch with MQTT, announce raw (ADC) battery reading.
  Also announce version number or hash, if it is different server will announce UPDATE.
  Then OTA client will start and fetch the firmware.bin from specified url.
  This would reduce time awake and processing needed when nothing is being accessed, increasing battery time.
  
