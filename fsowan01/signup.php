<?php
include_once ('protocol.php');

// Start a session if not yet started
if (!session_id())
	session_start();

$firstName = $lastName = $signEmail = "";

// Add User
if (isset($_POST[KEY_FIRST_NAME]) && isset($_POST[KEY_LAST_NAME]) && isset($_POST[KEY_EMAIL]) && isset($_POST[KEY_PASSWORD])) {
	$serversCount = count($SERVERS);
	$currServer = -1;

	while (++$currServer < $serversCount) {
		try {
			// Connect to the remote host
			$address = $SERVERS[$currServer][0];
			$port = intval($SERVERS[$currServer][1]);
			$errNo = $errStr = "";
			$fp = stream_socket_client($address . ":" . $port, $errNo, $errStr, 0);
			if ($fp) {
				// Send request to add a user
				$request = ADD_USER . DELIMITER . $_POST[KEY_EMAIL] . DELIMITER . $_POST[KEY_PASSWORD] . DELIMITER;
				$request .= $_POST[KEY_FIRST_NAME] . DELIMITER . $_POST[KEY_LAST_NAME];
				fputs($fp, $request, strlen($request));

				$response = trim(fgets($fp, BUFFER_SIZE));
				fclose($fp);

				// Analyse response
				if (strcmp($response, ADD_USER . DELIMITER . FAILURE) == 0) {
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
				break;
			} else {
				//echo "Connection Error: " . $errStr;
			}
		} catch(exception $ex) {
			//echo "Error: " . $ex;
		}
	}
}
?>
