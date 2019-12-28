(1) After check the Authorization header in the packets, 
    I found that in this case, 
    the algorithm directive's value is "MD5" and qop directive's value is "auth".
    Hence, the "response" value in the Authorization header is computed as follows:

    HA1 = MD5(username:realm:password)
    HA2 = MD5(method:digestURI)
    response = MD5(HA1:nonce:nonceCount:cnonce:qop:HA2) 

(2) There are 3 functions in crack2.py to compute HA1, HA2, and response value
    using the equation in the previous part.
    Parameter values can be extracted from Authorization header except password.
    Possible password value from a dictionary file which I downloaded from Internet.

(3) The crack2.py can test every word in the dictionary file and 
    use these words to produce response. 
    If there is one computed response that is the same as response value 
    from the Authorization header, then the word used to produce this response 
    is the password we are looking for.
