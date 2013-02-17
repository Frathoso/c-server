<?php
	define("DATA_ROOT", '../tweeter-data/');  // folder containing all data
	define("USERS_DATA_FILENAME", 'users');	  // filename containing all users
	define("HOME_PAGE", 'home.php');
	
	define("KEY_EMAIL", 'email');	
	define("KEY_PASSWORD", 'password');
	
	$loginEmail = "";
	
		
	if(isset($_POST[KEY_EMAIL]) && isset($_POST[KEY_PASSWORD]))
	{
		$fileUsers = file(DATA_ROOT.USERS_DATA_FILENAME);
		if(isValidUser($fileUsers, $_POST[KEY_EMAIL], $_POST[KEY_PASSWORD])){
			// Redirects to the user's homepage
			header('Location: '.HOME_PAGE);
		}else{
			echo "Invalid username or password";
			$loginEmail = $_POST[KEY_EMAIL];
		}
	}
	
	// Check if the email and password match
	function isValidUser($fileUsers, $email, $password){		
		foreach($fileUsers as $line){
			$user = json_decode($line, true);
			if( $user[KEY_EMAIL] == $email){
				if($user[KEY_PASSWORD] == $password) return true;
				else return false;
			}
		}
		return false;
	}

?>
