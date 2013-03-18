<?php
include_once ('protocol.php');

// Start a session if not yet started
if (!session_id()) session_start();

$firstName = $lastName = $signEmail = "";

// Add User
if (isset($_POST[KEY_FIRST_NAME]) && isset($_POST[KEY_LAST_NAME]) && isset($_POST[KEY_EMAIL]) && isset($_POST[KEY_PASSWORD])) {
	try {

		$errNo = $errStr = "";
		$fp = stream_socket_client(SERVER_ADDR . ":" . SERVER_PORT, $errNo, $errStr, 0);
		if ($fp) {
			// Send request to add a user
			$request = ADD_USER . DELIMINATER . $_POST[KEY_EMAIL] . DELIMINATER . $_POST[KEY_PASSWORD] . DELIMINATER;
			$request .= $_POST[KEY_FIRST_NAME] . DELIMINATER . $_POST[KEY_LAST_NAME];
			fputs($fp, $request, strlen($request));

			$response = trim(fgets($fp, BUFFER_SIZE));
			fclose($fp);

			// Analyse response
			if (strcmp($response, ADD_USER . DELIMINATER . FAILURE) == 0) {
				// User exists
				echo "Email exists";
				$firstName = $_POST[KEY_FIRST_NAME];
				$lastName = $_POST[KEY_LAST_NAME];
				$signEmail = $_POST[KEY_EMAIL];
			} else {
				// user added...Start user session and redirects to the user's homepage
				$_SESSION[KEY_EMAIL] = $_POST[KEY_EMAIL];
				$_SESSION[KEY_FIRST_NAME] = $_POST[KEY_FIRST_NAME];
				$_SESSION[KEY_LAST_NAME] = $_POST[KEY_LAST_NAME];
				header('Location: ' . HOME_PAGE);
				exit();
			}
		} else {
			echo "Connection Error: " . $errStr;
		}
	} catch(exception $ex) {
		echo "Error: " . $ex;
	}
}
?>
