<?php
define("DATA_ROOT", '../tweeter-data/'); // folder containing all data
define("OBJECTS_ROOT", '../tweeter-objects/'); // folder containing all objects classes
define("USERS_FILENAME", 'users'); // filename containing all users
define("USERS_OBJECT_FILENAME", 'user.php'); // filename containing the user class
define("HOME_PAGE", 'home.php');
define("FOLLOW_EXT", '.follow');
define("TWEETS_EXT", '.tweets');
define("KEY_FIRST_NAME", 'firstName');
define("KEY_LAST_NAME", 'lastName');
define("KEY_EMAIL", 'email');
define("KEY_PASSWORD", 'password');

// Start a session if not yet started
if (!session_id()) session_start();

include_once (OBJECTS_ROOT . USERS_OBJECT_FILENAME);	// Contains "User" class

$firstName = $lastName = $signEmail = "";

if (isset($_POST[KEY_FIRST_NAME]) && isset($_POST[KEY_LAST_NAME]) && isset($_POST[KEY_EMAIL]) && isset($_POST[KEY_PASSWORD])) {
	// Make a user object from the data
	$newUser = new User($_POST[KEY_FIRST_NAME], $_POST[KEY_LAST_NAME], $_POST[KEY_EMAIL], $_POST[KEY_PASSWORD]);

	// Check if the email is already in use
	$fileUsers = file(DATA_ROOT . USERS_FILENAME);
	if (emailExists($fileUsers, $_POST[KEY_EMAIL])) {
		echo "Email exists";
		$firstName = $_POST[KEY_FIRST_NAME];
		$lastName = $_POST[KEY_LAST_NAME];
		$signEmail = $_POST[KEY_EMAIL];
	} else {
		// Add the user into the users file
		$fileUsers = fopen(DATA_ROOT . USERS_FILENAME, 'a');
		fwrite($fileUsers, json_encode($newUser));
		fwrite($fileUsers, "\n");
		fclose($fileUsers);

		// Create data files for the user
		$fileName = $_POST[KEY_EMAIL]; // user email will be used as his/her tweets filename
		$fileTweets = fopen(DATA_ROOT . $fileName . TWEETS_EXT, 'w');
		fclose($fileTweets);
		chmod(DATA_ROOT . $fileName . TWEETS_EXT, 0777);

		$fileFollow = fopen(DATA_ROOT . $fileName . FOLLOW_EXT, 'w');
		fclose($fileFollow);
		chmod(DATA_ROOT . $fileName . FOLLOW_EXT, 0777);

		// Start user session and redirects to the user's homepage
		$_SESSION[KEY_EMAIL] = $_POST[KEY_EMAIL];
		$_SESSION[KEY_FIRST_NAME] = $_POST[KEY_FIRST_NAME];
		$_SESSION[KEY_LAST_NAME] = $_POST[KEY_LAST_NAME];
		header('Location: ' . HOME_PAGE);
		exit();
	}
}

// Check if the email is already in use
function emailExists($fileUsers, $email) {
	foreach ($fileUsers as $line) {
		$user = json_decode($line, true);
		if ($user[KEY_EMAIL] == $email)
			return true;
	}
	return false;
}
?>
