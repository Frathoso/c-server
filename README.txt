A parallel and distributed server written in C using pthreads for the "Parralel and Distributed Systems" course taken in NYU-Polytechnic Institude, New York, USA.

NB: Only (1) "SETUP" is essential for setting up the and running system.

1. SETUP:
	(a) Directories for the SITE:
		> "fsowan01/"	 --> is the public root folder for the site
	
	(b) Directories for the SERVER:
		> "server/"	 --> contains the C files for the server
		> "server/data/" --> contains data for the site
		
	(c) If the server is started with a custom port (8888), the port should also be 
		changed in the "fsowan01/protocol.php" file.
	
	(d) By default the server location is "localhost", if changed the "SERVER_ADDR" 
		in "fsowan01/protocol.php" should also be modified.
	
2. SAMPLE USERS:
	I have created three sample users with the following details:
	(a) Email:		sample1@gmail.com
		Password:	sample1
		First Name:	Sample1
		Last Name:	User
	
	(b) Email:		sample2@gmail.com
		Password:	sample2
		First Name:	Sample2
		Last Name:	User
		
	(c) Email:		sample3@gmail.com
		Password:	sample3
		First Name:	Sample3
		Last Name:	User
		
3. PROTOCOL:
	(a) Requests formats:
		> Authenticate User :   
			AUTUSR <email> <password>
		> Add User            
			ADDUSR <email> <pasword> <first name> <last name>
		> Remove User        
			REMUSR <email>
		> Follow User         
			FOLUSR <email>
		> Unfollow User      
			UNFUSR <emails>
		> Get User Details      
			GETUSR <email>
		> Get User Tweets    
			GETTWT <email>
		> Put User Tweet     
			PUTTWT <email> <tweet>
		> Get Followed Users  
			GETFOL <email>
		> Unknown Request  
			UNKNOW

	(b) Protocol Constants:	
		> SUCCESS       "YES"
		> FAILURE       "NO"
		> DELIMINATER   "~"
