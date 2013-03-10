<?php
define("DATA_ROOT", '../tweeter-data/');// folder containing all data
define("USERS_FILENAME", 'users');// filename containing all users
define("HOME_PAGE", 'home.php');
define("KEY_EMAIL", 'email');
define("KEY_PASSWORD", 'password');
define("KEY_FIRST_NAME", 'firstName');
define("KEY_LAST_NAME", 'lastName');

$loginEmail = "";

session_start();

if (isset($_POST[KEY_EMAIL]) && isset($_POST[KEY_PASSWORD])) {
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
