# INF1011 Project

The goal of this project is to build a keyless BLE beacon hotel room management system with useful utilities packaged in a small portable device.

STEP BY STEP GUIDE ON HOW IT WORKS:
Setting Up.
Step 1: Go to Reception\main.py and change the COM Port on Line 120, to your individual COM Port number.

Starting Up.
Step 2: Run your backend Strapi Server!! ---> [ cd/hotel-backend npm run develop ]

Step 3: Make sure your Strapi Sever has rooms Vacant! --> [ Go to http://localhost:1337/admin/content-manager/collectionType/api::room.room?page=1&pageSize=10&sort=id:ASC ]

Step 4: Upload Smart watch base.ino onto your Tinycircuit!

Step 5: Run the frontend! ---> [ cd/reception-gui flask -A main --debug run ]

Step 6: Have a nice Hotel Stay! :D

## Pre-requisites

1.) Have Flask installed. [pip install flask] {Flask Version == 2.2.2}

2.) Have Node.js installed for npm. [ https://nodejs.org/en/download/ ] {Npm Version == 8.19.2}
    2.1) Download the latest LTS version from the website.
    2.2) Install it as per normal. Make sure you choose npm package manager , not the default of Node.js runtime . This way, we can install Node and NPM simultaneously.
    2.3) If any problems [ https://radixweb.com/blog/installing-npm-and-nodejs-on-windows-and-mac ]

3.) Have Request installed. [pip install request] {Requests Version == 2.28.1}

4.) Have Pyserial installed. [pip isntall pyserial] {Pyserial Version == 3.5}
