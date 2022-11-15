# -*- coding: utf-8 -*-
"""
The following code can only be run on a Linux device with Bluetooth, preferably a Raspberry Pi.
It prints out the RSSI distance between the specified BLE address and the device this code is running on.
This was tested on a Pi Zero W.
Bluepy is required.
"""

import json
from bluepy.btle import Scanner

tiny_key_addr = "c6:a5:27:1f:ce:dc"

def scanForDevices():
    global result
    print("Scanning for TinyKey...")
    while True:
        try:
            # Scans every 500 milliseconds
            ble_list = Scanner().scan(0.5)
            for dev in ble_list:
                if dev.addr == tiny_key_addr:
                    if(dev.rssi >= -70):
                        print("Device near:" + str(dev.rssi))
                        outputJson(1)
                    else:
                        print("Device far:" + str(dev.rssi))
                        outputJson(0)
        except:
            raise Exception("Error occured")

def outputJson(val):
    output = { "status": val }

    json_object = json.dumps(output, indent=4)

    with open("status.json", "w") as outfile:
        outfile.write(json_object)

scanForDevices()