#!/usr/bin/env python

import base64
from Crypto import Random
from Crypto.Cipher import AES

def sign(data, key):
    iv = Random.new().read(AES.block_size)
    key = base64.b64decode(key)
    cipher = AES.new(key, AES.MODE_CFB, iv, segment_size=128)
    return base64.b64encode(iv + cipher.encrypt(data))

# To test the module, please set the iv the the following value manually:
#   iv = ("f729d49e57128de3004f0fc39a7d2d50").decode('hex')
# Expected ciphertext: 9ynUnlcSjeMATw/Dmn0tUMnAVpkbxqpW85ZrfW9m0zPA0lFfOCA=
if __name__ == '__main__':
    ciphertext = sign('test@a.edu201904260912', 'RlHd5A00sJPXqlNlP/6QjyxedOkCLYnLzo9vJ0szMw4=')
    print(ciphertext)
