<?php
include_once ('protocol.php');

session_start();

// Remove user
if (isset($_GET["key"]) && isset($_SESSION[KEY_EMAIL])) {
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
				// Send request to remove user
				$request = REMOVE_USER . DELIMITER . $_SESSION[KEY_EMAIL];
				fputs($fp, $request, strlen($request));

				// Analyse server's response
				$response = trim(fgets($fp));
				// Not used
				fclose($fp);

				// Remove cookies and redirect to login page
				session_destroy();
				header('Location: ' . LOGIN_PAGE);
				exit();
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
						<form method="get" action="delete-account.php">
							<label>All account's data will be lost. Are you sure?</label>
							<input name="key" type="hidden" />
							<input type="submit" value="Yes, Delete My Account" class="button-long">
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