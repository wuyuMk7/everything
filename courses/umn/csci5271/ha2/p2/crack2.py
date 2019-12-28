#!/usr/bin/env python

import hashlib

# Implementation of digest authentication
# As description from https://en.wikipedia.org/wiki/Digest_access_authentication

# Algorithm: MD5
# HA1 = MD5(username:realm:password)
def HA1(username, realm, password):
  msg = "{}:{}:{}".format(username, realm, password)
  md5 = hashlib.md5()
  md5.update(msg)
  return md5.hexdigest()

# QOP: auth
# HA2 = MD5(method:digestURI)
def HA2(method, uri):
  msg = "{}:{}".format(method, uri)
  md5 = hashlib.md5()
  md5.update(msg)
  return md5.hexdigest()

# QOP: auth
# response = MD5(HA1:nonce:nonceCount:cnonce:qop:HA2)
def response(HA1, nonce, nc, cnonce, qop, HA2):
  msg = "{}:{}:{}:{}:{}:{}".format(
    HA1, nonce, nc, cnonce, qop, HA2
  )
  md5 = hashlib.md5()
  md5.update(msg)
  return md5.hexdigest()


if __name__ == "__main__":
  method = "HEAD"
  uri = "/secret/cheese"
  HA2_value = HA2(method, uri)
  
  username = "travis15"
  realm = "Cheese"
  target_response="4b9e38225d22ef3ceaa4b1f230ab1473"
  nonce="HtmwzmaXBQA=cfe89db021040491580c1340cc19d3ba22423d02"
  nc="00000001"
  cnonce="ZjFiMTJhNjJkN2RiOTUxN2UwZTc5YzBkNTQ5ODA3MzE="
  qop="auth"
  with open("passwords.txt", "r") as f:
    for line in f:
      HA1_value = HA1(username, realm, line.strip())
      corr_resp = response(HA1_value, nonce, nc, cnonce, qop, HA2_value)
      if corr_resp == target_response:
        print("password:" + line.strip()) 


