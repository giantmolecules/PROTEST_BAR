![alt text](https://raw.githubusercontent.com/giantmolecules/PROTEST_BAR/master/MEDIA/IMG_3084.jpg)

# PROTEST_BAR

## a tool for electronic disobedience in wireless infrastructure space

PROTEST_BAR enables the user to post single- or multi-line messages in the list of available networks present on mobile- and non-mobile devices. PROTEST_BAR transmits the messages in the SSIDs of non-existing wireless networks. PROTEST_BAR accomplishes this feat by crafting and [transmitting](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/wifi.html#preconditions-of-using-esp-wifi-80211-tx) custom beacon frames. The device responsible for this action is Espressif's ESP32 WiFi-enabled microcontroller running the [Arduino core](https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi/examples).

PROTEST_BAR is based on [ESP8266 Beacon Spam](https://github.com/spacehuhn/esp8266_beaconSpam) and its ESP32 version [esp32_beaconSpam](https://github.com/Tnze/esp32_beaconSpam). PROTEST_BAR repurposes this method of spamming lists of available networks to transmit useful messages making it a platform for activism and art.
