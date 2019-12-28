#!/usr/bin/env python

import hashlib
import urllib2
import ssl
import re

def mac(username, key):
  msg = "{}{}".format(username, key)
  sha1 = hashlib.sha1()
  sha1.update(msg)
  return sha1.hexdigest()

if __name__ == "__main__":
  username = "12345678901234567890"
  key = ""

  for i in range(1, 21):
    tmp_username = username[i:]
    url = "https://192.168.15.1/mac-cookie?username=" + tmp_username
    resp = urllib2.urlopen(url, context = ssl._create_unverified_context()).read()
    mac_from_resp = re.search("[a-f0-9]{40}", resp).group()
    for count in range(0, 256):
      tmp_key = "{}{}".format(key, chr(count))
      mac_from_local = mac(tmp_username, tmp_key)
      if mac_from_resp == mac_from_local:
        key = tmp_key
        break

  print("Key is (text): " + key)
  print("Key is (hex): " + "".join([c.encode("hex") for c in key]))
