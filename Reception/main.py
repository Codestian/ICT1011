from unittest import result
from flask import Flask,render_template,request
import urllib.request, json
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
        for key,value in hotel['attributes'].items():
            if "number" in key:
                hotelRoom.append(value)
            if "attributes" in key:
                hotelType.append(value="text")
            else:
                hotelType.append("dead")
    hotelInformation  = {hotelRoom[i]: hotelType[i] for i in range(len(hotelRoom))}
    return render_template("index.html",data = hotelInformation)

 
@app.route('/data/', methods = ['POST', 'GET'])
def data():
    if request.method == 'GET':
        return f"The URL /data is accessed directly. Try going to '/form' to submit form"
    if request.method == 'POST':
        form_data = request.form
        return render_template('data.html',form_data = form_data)

@app.route("/test/")
def test():
    url = "http://localhost:1337/api/rooms?populate=*"
    response = urllib.request.urlopen(url)
    hotels = response.read()
    dict = json.loads(hotels)
    return render_template('test.html', title="page", jsonfile=json.dumps(dict))

@app.route("/test2/")
def test2():
    url = "http://localhost:1337/api/rooms?populate=*"
    response = urllib.request.urlopen(url)
    hotels = response.read()
    dict = json.loads(hotels)
    hotelRoom,hotelType = [], []
    hotelInformation = {}
    
    for hotel in dict["data"]:
        for key,value in hotel['attributes'].items():
            if "number" in key:
                hotelRoom.append(value)
            if "attributes" in key:
                hotelType.append(value="text")
            else:
                hotelType.append("dead")
    hotelInformation  = {hotelRoom[i]: hotelType[i] for i in range(len(hotelRoom))}
    return render_template("dicttest.html",result = hotelInformation)
 
if __name__ == "__main__":
    app.run()