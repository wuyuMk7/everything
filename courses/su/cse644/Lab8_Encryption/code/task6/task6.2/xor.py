#!/usr/bin/env python

plain1=bytearray(open('plain6.2', 'rb').read())
cipher1=bytearray(open('cipher6.2.1', 'rb').read())
cipher2=bytearray(open('cipher6.2.2', 'rb').read())

result=""
for i in range(len(plain1)):
  ch = plain1[i] ^ cipher1[i] ^ cipher2[i]
  result += chr(ch)

print(result)


