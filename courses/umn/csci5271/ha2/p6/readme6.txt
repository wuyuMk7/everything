Using the hint from the problem description: 

    Since the input to the hash function has a fixed length of 20 bytes, 
    even though we put a longer string to the hash function, 
    it will be truncated to 20 bytes. 
    Since the input is the combination of the cookie content and key,
    if we have 20 bytes for the cookie content then there will be no space for key,
    which means the input only contains the cookie content.
    If we have 19 bytes for the cookie content then the first byte of the key
    will be concatenated to the cookie content as the input 
    and the input will be 19 bytes cookie content followed by first byte of the key.
    We can iterate over all of possible content for the first byte of the key.
    Repeat this process we can get every byte of the key.
    

Steps:

(1) Initiate a 19 byte username and send it to the SERVER to get the MAC.
    Extract the MAC from the response.

(2) Construct a string to contain the username followed by one byte data.
    Iterate over all of 256 possible values for this byte data. 
    Now there are 256 combinations of username and the one-byte data.

(3) Compute the SHA1 value of each combination and compare the value with the MAC.
    If there is one value that is equal to the MAC, it means that this combination 
    is the one that used by the SERVER to produce the MAC.
    Hence, the one-byte data from this combination from the key.

(4) Now we have already got the first byte of the key.
    In the next round, we can send a 18 bytes username to the SERVER to get the MAC.
    SERVER will produce a combination of 18 bytes username and 2 bytes from the key. 
    Notice that we got the first byte of the key from step 3.
    Hence, in this round, on the local side, we need to compute the SHA1 value of
    the following string:

    {18 types constructed username}{first byte of the key}{one byte for test}

    The first 19 bytes of data are known by us so we only need to 
    test 256 different values for the last byte to find out which one can 
    give us a same SHA1 value as the MAC.
    Then this byte of data is the second byte of the key.

(5) Repeat this process similarly until we have disclosed every byte of the key.
    Eventually, we can get the following value:

    Key is (text): WKsC4A6UbXuC0Z3Ydz6u
    Key is (hex): 574b73433441365562587543305a3359647a3675
