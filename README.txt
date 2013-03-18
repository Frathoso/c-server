NB: Only #1 "FOLDERS SETUP" is essential for setting up the system.

1. SETUP:
	(a) Directories for the SITE:
		> "fsowan01/"	 --> is the public root folder for the site
	
	(b) Directories for the SERVER:
		> "server/"		 --> contains the C files for the server
		> "server/data/" --> contains data for the site
		
	(c) If the server is started with a custom port (8888), the port should also be 
		changed in the "fsowan01/protocol.php" file.
	
	(d) By default the server location is "localhost", if changed the "SERVER_ADDR" 
		in "fsowan01/protocol.php" should also be modified.
	
	
		
2. PROTOCOL:
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
