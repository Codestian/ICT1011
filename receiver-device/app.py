from flask import Flask, render_template
import json

app = Flask(__name__)

@app.route('/')
def hello():
    return render_template('index.html')

@app.route("/status")
def get_status():
    with open('status.json', 'r') as openfile:
        json_object = json.load(openfile)
    return {"status": json_object["status"]}

