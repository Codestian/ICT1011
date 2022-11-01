from unittest import result
from flask import Flask,render_template,request
import urllib.request, json
<<<<<<< HEAD
import requests
=======
from keys import func1
>>>>>>> 2c6c100dd685bbe9b64586b9304077b3a5eedbb8
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
<<<<<<< HEAD
        idList.append(hotel["id"])
=======
>>>>>>> 2c6c100dd685bbe9b64586b9304077b3a5eedbb8
        for key,value in hotel['availability'].items():
            if "status" in key:
                if "vacant" in value.lower():
                    if "number" in hotel:
                        hotelRoom.append((hotel["number"]))
                    for key,value in hotel["type"].items():
                        if "name" in key:
<<<<<<< HEAD
                            hotelType.append(value)
        
    
    hotelInformation  = {hotelRoom[i]: hotelType[i] for i in range(len(hotelRoom))}
    
    return render_template("index.html",data = hotelInformation,data2=idList)
=======
                            hotelType.append(value)  
    hotelInformation  = {hotelRoom[i]: hotelType[i] for i in range(len(hotelRoom))}
    print(hotelInformation)
    return render_template("index.html",data = hotelInformation)
>>>>>>> 2c6c100dd685bbe9b64586b9304077b3a5eedbb8

 
@app.route('/data/', methods = ['POST', 'GET'])
def data():
    url = "http://localhost:1337/api/guests?populate=*"
    url2 = "http://localhost:1337/api/rooms?populate=*"
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
<<<<<<< HEAD
        headers = {'Content-Type': 'application/json'}
        data = request.form.to_dict()
        Dict = {}
        Dict["data"] = {}
        for key,value in data.items():
            Dict["data"][key] = value
        
        print(json.dumps(Dict))
        headers = {'Content-Type': 'application/json'}
        r = requests.post(url, data=json.dumps(Dict), headers=headers)
        return render_template('booking.html',data=informationId)


@app.route('/booking/', methods = ['PUT','GET'])
def booking():    
    if request.method == 'GET':
        data = request.full_path
        
        hotelNumber = data[-3:]
        hotelID = data[-4]
        
        Dict = {}
        Dict["data"] = {}
        Dict["data"]["number"] = hotelNumber
        Dict["data"]["availability"] = {}
        Dict["data"]["availability"]["id"] = 6
        Dict["data"]["availability"]["status"] = "Occupied"
        print(Dict)
        url = "http://localhost:1337/api/rooms/"+hotelID+"?populate=*"
        print(url)
        headers = {'Content-Type': 'application/json'}
        r = requests.put(url, data=json.dumps(Dict), headers=headers)
        
        return(hello_world())
        
=======
        form_data = request.form
        print(form_data)
        return render_template('data.html',form_data = form_data)
 
>>>>>>> 2c6c100dd685bbe9b64586b9304077b3a5eedbb8
if __name__ == "__main__":
    app.run()