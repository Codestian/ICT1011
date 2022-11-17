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
    idx = 0
    check = [0,0,0,0,0,0,0,0,0,0]
    print("Scanning for TinyKey...")
    while True:
        try:
            # Scans every 500 milliseconds
            ble_list = Scanner().scan(0.25)
            for dev in ble_list:
                if dev.addr == tiny_key_addr:
                    if(idx == 10):
                        if(sum(check) >= 6):
                            print("DEVICE NEAR " + str(check))
                            outputJson(1)
                        else:
                            print("DEVICE FAR " + str(check))
                            outputJson(0)
                        idx = 0
                    else:
                        if(dev.rssi >= -70):
                            check[idx] = 1
                        else:
                            check[idx] = 0
                        print(str(idx) + ": DISTANCE: " + str(dev.rssi))
                        idx = idx + 1
        except:
            raise Exception("Error occured")

def outputJson(val):
    output = { "status": val }

    json_object = json.dumps(output, indent=4)

    with open("status.json", "w") as outfile:
        outfile.write(json_object)

scanForDevices()