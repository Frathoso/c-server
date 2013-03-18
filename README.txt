NB: Only #1 "FOLDERS SETUP" is essential for setting up the system.

1. FOLDERS SETUP:
	(a) For the SITE:
		> "mytweeter/"	 --> is the public root folder for the site
	
	(b) For the SERVER:
		> "server/"		 --> contains the C files for the server
		> "server/data/" --> contains data for the site
		
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
