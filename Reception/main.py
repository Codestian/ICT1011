from unittest import result
from flask import Flask,render_template,request
import urllib.request, json
from keys import func1
app = Flask(__name__)


@app.route("/")
def hello_world():
    url = "http://localhost:1337/api/rooms?populate=*"
    response = urllib.request.urlopen(url)
    hotels = response.read()
    dict = json.loads(hotels)
    hotelRoom,hotelType = [], []
    hotelInformation = {}
    
    for hotel in dict["data"]:
        for key,value in hotel['availability'].items():
            if "status" in key:
                if "vacant" in value.lower():
                    if "number" in hotel:
                        hotelRoom.append((hotel["number"]))
                    for key,value in hotel["type"].items():
                        if "name" in key:
                            hotelType.append(value)  
    hotelInformation  = {hotelRoom[i]: hotelType[i] for i in range(len(hotelRoom))}
    print(hotelInformation)
    return render_template("index.html",data = hotelInformation)

 
@app.route('/data/', methods = ['POST', 'GET'])
def data():
    if request.method == 'GET':
        return f"The URL /data is accessed directly. Try going to '/form' to submit form"
    if request.method == 'POST':
        form_data = request.form
        print(form_data)
        return render_template('data.html',form_data = form_data)
 
if __name__ == "__main__":
    app.run()