<?php
include_once ('protocol.php');

session_start();

// Search if a user exists with "query" details
if (isset($_POST[KEY_QUERY])) {
	$serversCount = count($SERVERS);
	$currServer = -1;

	while (++$currServer < $serversCount) {
		try {
			// Connect to the remote host
			$address = $SERVERS[$currServer][0];
			$port = intval($SERVERS[$currServer][1]);
			$errNo = $errStr = "";
			$fp = stream_socket_client($address . ":" . $port, $errNo, $errStr);
			if ($fp) {
				// Send request to retrieve user details (email)
				$request = GET_USER . DELIMITER . strtolower(trim($_POST[KEY_QUERY]));
				fputs($fp, $request, strlen($request));

				// Analyse server's response
				$results = trim(fgets($fp));
				fclose($fp);
				if ($results) {
					if (strcmp($results, GET_USER . DELIMITER . FAILURE) != 0) {
						$reqArray = explode(DELIMITER, $results);
						$_SESSION[KEY_USER_TO_FOLLOW] = $reqArray[1];
					}
				}
				break;
			} else {
				//echo "Error Connecting: " . $errNo . ": " . $errStr;
			}
		} catch(exception $ex) {
			//echo "<br> Error: " . $ex;
		}
	}
}

// Follow user
if (isset($_POST[KEY_FOLLOW_USER]) && isset($_SESSION[KEY_EMAIL]) && isset($_SESSION[KEY_USER_TO_FOLLOW])) {
	// TODO: check if the user is already followed

	$serversCount = count($SERVERS);
	$currServer = -1;

	while (++$currServer < $serversCount) {
		try {
			// Connect to the remote host
			$address = $SERVERS[$currServer][0];
			$port = intval($SERVERS[$currServer][1]);
			$errNo = $errStr = "";
			$fp = stream_socket_client($address . ":" . $port, $errNo, $errStr);
			if ($fp) {
				// Send request to retrieve user details (email)
				$request = FOLLOW_USER . DELIMITER . $_SESSION[KEY_EMAIL] . DELIMITER . $_SESSION[KEY_USER_TO_FOLLOW];
				fputs($fp, $request, strlen($request));

				// Analyse server's response
				$response = trim(fgets($fp));
				if (strcmp($response, FOLLOW_USER . DELIMITER . SUCCESS) == 0) {
					$_SESSION[KEY_MESSAGE] = $_SESSION[KEY_USER_TO_FOLLOW];
				}
				fclose($fp);
				break;
			} else {
				//echo "Error Connecting: " . $errNo . ": " . $errStr;
			}
		} catch(exception $ex) {
			//echo "<br> Error: " . $ex;
		}
	}

	// Remove the user-to-be-followed tag
	unset($_SESSION[KEY_USER_TO_FOLLOW]);
}
?>
<!DOCTYPE html>
<html>
	<head>
		<meta charset="UTF-8" />
		<title>My Home</title>
		<link rel="stylesheet" href="css/layout.css" type="text/css" />
	</head>
	<body>
		<div id="cont-page">
			<div id="cont-home">
				<div id="home-body">
					<div id="home-new-tweet">
						<span id='follow'><a href='home.php'>Home</a></span>
					</div>
					<div id="home-new-tweet">
						<form method="post" action="follow.php">
							<input type="text" id="new-tweet" name="query" min="1" max="50">
							<input type="submit" value="Search" class="button">
						</form>
					</div>
					<div>
						<?php
						if (isset($_SESSION[KEY_USER_TO_FOLLOW])) {
							$form = '<form method="post" action="follow.php">' . $_SESSION[KEY_USER_TO_FOLLOW] . '<input type="checkbox" name="followuser">' . '<input type="submit" value="Follow" class="button">' . '</form>';
							echo $form;
						} else if (isset($_POST[KEY_QUERY])) {
							echo "User not available";
						} else if (isset($_SESSION[KEY_MESSAGE])) {
							echo "You're now following <b>" . $_SESSION[KEY_MESSAGE] . "<b>";
							unset($_SESSION[KEY_MESSAGE]);
						}
						?>
					</div>
				</div>
			</div>
		</div>
	</body>
</html>