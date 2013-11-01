<?php
include_once ('protocol.php');

$loginEmail = "";

session_start();

// Submit email and password for authentication
if (isset($_POST[KEY_EMAIL]) && isset($_POST[KEY_PASSWORD])) {
	$serversCount = count($SERVERS);
	$currServer = -1;

	while (++$currServer < $serversCount) {
		try {
			// Connect to the remote host
			$address = $SERVERS[$currServer][0];
			$port    = intval($SERVERS[$currServer][1]);
			$errNo   = $errStr = "";
			$fp = stream_socket_client($address . ":" . $port, $errNo, $errStr, 0);

			if ($fp) {
				// Submit email and password for authentication
				$request = AUTHENTICATE_USER . DELIMITER . $_POST[KEY_EMAIL] . DELIMITER . $_POST[KEY_PASSWORD];
				fputs($fp, $request, strlen($request));

				// Analyse the returned results
				$results = trim(fgets($fp, BUFFER_SIZE));
				fclose($fp);
				if ($results) {
					if (strcmp(trim($results), AUTHENTICATE_USER . DELIMITER . SUCCESS) == 0) {
						// Start user session and redirects to the user's homepage
						$_SESSION[KEY_EMAIL] = $_POST[KEY_EMAIL];
						header('Location: ' . HOME_PAGE);
						exit();
					}
				}
				// Remain in login page
				echo "<br>Invalid username or password";
				$loginEmail = $_POST[KEY_EMAIL];
				break;
			} else {
				//echo "Error Connecting: " . $errNo . ": " . $errStr;
			}
		} catch(exception $ex) {
			//echo "<br>Error:" . $ex;
		}
	}
}
?>
