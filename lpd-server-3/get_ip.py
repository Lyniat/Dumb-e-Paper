import json

def change_ip(ip):

    with open("conf.json", "r") as file:
            content = json.load(file)

    content["client"]["ip"] = ip

    with open("conf.json", "w") as file:
        json.dump(content, file)