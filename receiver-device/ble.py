# -*- coding: utf-8 -*-
"""
The following code can only be run on a Linux device with Bluetooth, preferably a Raspberry Pi.
It prints out the RSSI distance between the specified BLE address and the device this code is running on.
This was tested on a linux computer.
Bluepy is required.
"""

import time
import json
from bluepy.btle import Scanner

list_of_addr  = ["c6:a5:27:1f:ce:dc","e5:de:83:e7:0f:9d"]

GRACE_PERIOD_DENIED = 5
GRACE_PERIOD_GRANTED = 20

def scanForDevices():

    access = {
        "c6:a5:27:1f:ce:dc": 1,
        "e5:de:83:e7:0f:9d": 0
    }
    addr_in_range = []

    print("Scanning for TinyKey...")

    scan_for_key = True

    while scan_for_key:
        try:
            # Scans every 500 milliseconds
            ble_list = Scanner().scan(0.25)

            for dev in ble_list:

                if(searchInList(dev.addr) and dev.rssi >= -67):
                    addr_in_range.append(access[dev.addr])
                
            if(addr_in_range):
                if(sum(addr_in_range) == 0):
                    outputJson(2)
                    scan_for_key = False
                    time.sleep(GRACE_PERIOD_DENIED)
                    scan_for_key = True
                elif(sum(addr_in_range) == 1):
                    outputJson(1)
                    scan_for_key = False
                    time.sleep(GRACE_PERIOD_GRANTED)
                    scan_for_key = True
            else:
                outputJson(0)

            addr_in_range = []

        except:
            raise Exception("Error occured")

def searchInList(addr):
        if addr in list_of_addr:
            return True
        else:
            return False

def outputJson(val):
    output = { "status": val }

    json_object = json.dumps(output, indent=4)

    with open("status.json", "w") as outfile:
        outfile.write(json_object)

scanForDevices()
