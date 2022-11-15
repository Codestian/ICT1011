import json

output = { "status": 2 }

json_object = json.dumps(output, indent=4)

with open("status.json", "w") as outfile:
    outfile.write(json_object)