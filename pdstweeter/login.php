<?php
define("DATA_ROOT", '../tweeter-data/');// folder containing all data
define("USERS_FILENAME", 'users');// filename containing all users
define("HOME_PAGE", 'home.php');
define("KEY_EMAIL", 'email');
define("KEY_PASSWORD", 'password');
define("KEY_FIRST_NAME", 'firstName');
define("KEY_LAST_NAME", 'lastName');

define("SERVER_PORT", 8888);
define("SERVER_ADDR", 'localhost');
define("BUFFER_SIZE", 2048);
define("SUCCESS", "yes");

$loginEmail = "";

session_start();

// Submit email and password for authentication
if(isset($_POST[KEY_EMAIL]) && isset($_POST[KEY_PASSWORD])){
	try{
		// Connect to the remote host
		$hostIp = gethostbyname(SERVER_ADDR);
		$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
		
		if(socket_connect($socket, $hostIp, SERVER_PORT)){
			// Submit email and password for authentication
			$message = 'LOGIN '.$_POST[KEY_EMAIL].' '.$_POST[KEY_PASSWORD];
			socket_write($socket, $message, strlen($message));
			
			// Analyse the returned results
			$results = socket_read($socket, BUFFER_SIZE, PHP_NORMAL_READ);
			echo "<br>Results (".$results.")";
			if($results != ""){
				if (strcmp(trim($results), SUCCESS) == 0) {
					// Start user session and redirects to the user's homepage
					$_SESSION[KEY_EMAIL] = $_POST[KEY_EMAIL];
					header('Location: ' . HOME_PAGE);
					exit();
				}
			}
			// Remain in login page
			echo "<br>Invalid username or password";
			$loginEmail = $_POST[KEY_EMAIL];
		}else{
			echo "Error Connecting: ".socket_strerror(socket_last_error());
		}
		socket_close($socket);
	}catch(exception $ex){
		echo "<br>Exception:".$ex;
	}
}

//************

if (isset($_POST[KEY_EMAIL]) && isset($_POST[KEY_PASSWORD]) && 1 == 3) {
	$fileUsers = file(DATA_ROOT . USERS_FILENAME);
	if (isValidUser($fileUsers, $_POST[KEY_EMAIL], $_POST[KEY_PASSWORD])) {
		// Start user session and redirects to the user's homepage
		$_SESSION[KEY_EMAIL] = $_POST[KEY_EMAIL];
		header('Location: ' . HOME_PAGE);
		exit();
	} else {
		// Remain in login page
		echo "Invalid username or password";
		$loginEmail = $_POST[KEY_EMAIL];
	}
}

// Check if the email and password match
function isValidUser($fileUsers, $email, $password) {
	foreach ($fileUsers as $line) {
		$user = json_decode($line, true);
		if ($user[KEY_EMAIL] == $email) {
			if ($user[KEY_PASSWORD] == $password){
				$_SESSION[KEY_FIRST_NAME] = $user[KEY_FIRST_NAME];
				$_SESSION[KEY_LAST_NAME] = $user[KEY_LAST_NAME];
				return true;	
			}else
				return false;
		}
	}
	return false;
}
?>
