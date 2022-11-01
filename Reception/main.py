from unittest import result
from datetime import datetime
from flask import Flask,render_template,request
import urllib.request, json
import requests
import re
app = Flask(__name__)


@app.route("/")
def hello_world():
    url = "http://localhost:1337/api/rooms?populate=*"
    response = urllib.request.urlopen(url)
    hotels = response.read()
    dict = json.loads(hotels)
    hotelRoom,hotelType = [], []
    hotelInformation = {}
    idList = []
    for hotel in dict["data"]:
        idList.append(hotel["id"])
        for key,value in hotel['availability'].items():
            if "status" in key:
                if "vacant" in value.lower():
                    if "number" in hotel:
                        hotelRoom.append((hotel["number"]))
                    for key,value in hotel["type"].items():
                        if "name" in key:
                            hotelType.append(value)
        
    
    hotelInformation  = {hotelRoom[i]: hotelType[i] for i in range(len(hotelRoom))}
    
    return render_template("index.html",data = hotelInformation,data2=idList)

 
@app.route('/data/', methods = ['POST', 'GET'])
def data():
    url = "http://localhost:1337/api/guests?populate=*"
    url2 = "http://localhost:1337/api/rooms?populate=*"
    response2 = urllib.request.urlopen(url)
    guests =response2.read()
    dict = json.loads(guests)
    guestInformationId = {}
    guestList,guestID = [],[]
    for guest in dict["data"]:
        guestList.append(guest["fullName"])
        guestID.append(guest["id"])
    guestInformationId = {guestID[i]: guestList[i] for i in range(len(guestID))}
    response = urllib.request.urlopen(url2)
    informationId = {}
    hotelId, hotelRoom= [],[]
    hotels = response.read()
    dict = json.loads(hotels)
    for hotel in dict["data"]:
        
        for key,value in hotel['availability'].items():
            
            if "status" in key:
                if "vacant" in value.lower():
                    if "number" in hotel:
                        hotelRoom.append((hotel["number"]))
                        hotelId.append(hotel["id"])
    informationId = {hotelId[i]: hotelRoom[i] for i in range(len(hotelId))}
    if request.method == 'GET':
        return f"The URL /data is accessed directly. Try going to '/form' to submit form"
    if request.method == 'POST':
        headers = {'Content-Type': 'application/json'}
        data = request.form.to_dict()
        Dict = {}
        Dict["data"] = {}
        for key,value in data.items():
            Dict["data"][key] = value
        
        # print(json.dumps(Dict))
        headers = {'Content-Type': 'application/json'}
        r = requests.post(url, data=json.dumps(Dict), headers=headers)
        return render_template('booking.html',data=informationId,data2=guestInformationId)

    
@app.route('/booking/', methods = ['PUT','GET'])
def booking():    
    if request.method == 'GET':
        data = request.full_path
        hotelNumber = data[-3:]
        hotelID = data[-4]
        data2 = request.full_path.split('&')
        data2 = data2[0].split("=")
        guestID = int(re.search(r'\d+', data2[1]).group())
        fullName = data2[1].replace(str(guestID), "")
        print("guest",guestID)
        print("name",fullName)
        Dict = {}
        Dict["data"] = {}
        Dict["data"]["number"] = hotelNumber
        Dict["data"]["availability"] = {}
        Dict["data"]["availability"]["id"] = 6
        Dict["data"]["availability"]["status"] = "Occupied"
        print(Dict)
        url = "http://localhost:1337/api/rooms/"+hotelID+"?populate=*"
        url2 = "http://localhost:1337/api/bookings/?populate=*"
        Dict2 = {}
        Dict2["data"] = {}
        Dict2["data"]["room"] ={}
        Dict2["data"]["room"]["number"] =hotelNumber
        Dict2["data"]["room"]["id"] = hotelID
        date = datetime.today().strftime('%Y-%m-%d')
        Dict2["data"]["guest"] = {}
        Dict2["data"]["guest"]["id"] = guestID
        Dict2["data"]["guest"]["fullName"] = fullName
        Dict2["data"]["dateStart"] = date
        Dict2["data"]["dateEnd"] = date
        print(Dict2)
        headers = {'Content-Type': 'application/json'}
        r = requests.put(url, data=json.dumps(Dict), headers=headers)
        r2 = requests.post(url2,data=json.dumps(Dict2),headers=headers)
        return(hello_world())
        
if __name__ == "__main__":
    app.run()