#!/usr/bin/env python

import requests, os, json
from sign import sign

from datetime import datetime

#siteurl = 'https://us-central1-dofinal.cloudfunctions.net/report'
siteurl = 'http://45.56.103.75:5000/dofinal/us-central1/report'

def send(isSitting, start_time):
    email = ""
    key = ""
    config_file = os.path.dirname(os.path.realpath(__file__)) + '/../config.json'

    with open(config_file, "r") as f:
        data = json.load(f)
        email = data['email']
        key = data['key']

    send_data = {}
    send_data['isSitting'] = isSitting
    send_data['datetime'] = start_time.isoformat()
    send_data['email'] = email

    plaintext = str(email + start_time.strftime('%Y%m%d%H%M'))
    send_data['sign'] = sign(plaintext, key)
    print(send_data)

    resp = requests.post(siteurl, json=send_data)
    return resp

if __name__ == '__main__':
    test_datetime = datetime.now()
    resp = send(0, test_datetime)
    print(resp.text)


