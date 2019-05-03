#!/usr/bin/env python

plain_yes=bytearray(open('plain6.3yes', 'rb').read())
plain_no=bytearray(open('plain6.3no', 'rb').read())
iv1=bytearray(open('iv6.3.1', 'rb').read())
iv2=bytearray(open('iv6.3.2', 'rb').read())

result=""
for i in range(len(plain_yes)):
  ch = plain_yes[i] ^ iv1[i] ^ iv2[i]
  result += chr(ch)

with open("attack_plain", "wb") as f:
  f.write(bytes(result))

#print(result)


