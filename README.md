# INF1011 Project

The goal of this project is to build a keyless BLE beacon hotel room management system with useful utilities packaged in a small portable device.

STEP BY STEP GUIDE ON HOW IT WORKS:
Setting Up.
Step 1: Go to Reception\main.py and change the COM Port on Line 120, to your individual COM Port number.

Starting Up.
Step 2: Run your backend Strapi Server!! ---> [ cd/hotel-backend npm run develop ]

Step 3: Make sure your Strapi Sever has rooms Vacant! --> [ Go to http://localhost:1337/admin/content-manager/collectionType/api::room.room?page=1&pageSize=10&sort=id:ASC ]

Step 4: Upload Smart watch base.ino onto your Tinycircuit!

Step 5: Run the frontend! ---> [ cd/Reception flask -A main --debug run ]

Step 6: Have a nice Hotel Stay! :D