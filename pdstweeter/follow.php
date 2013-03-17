<?php
// Server details
define("SERVER_PORT", 8888);
define("SERVER_ADDR", 'tcp://localhost');
define("BUFFER_SIZE", 8192);

// Protocol keywords
define("FOLLOW_USER", 'FOLUSR');
define("GET_USER", 'GETUSR');
define("DELIMINATER", '~');
define("FAILURE", 'NO');

// Keys for identifying data
define("KEY_QUERY", 'query');
define("KEY_FOLLOW_USER", 'followuser');
define("KEY_USER_TO_FOLLOW", 'usertofollow');
define("KEY_EMAIL", 'email');
define("KEY_FIRST_NAME", 'firstName');
define("KEY_LAST_NAME", 'lastName');

session_start();

// Search if a user exists with "query" details
if (isset($_POST[KEY_QUERY])) {
	try {
		// Connect to the remote host
		$errNo = $errStr = "";
		$fp = stream_socket_client(SERVER_ADDR . ":" . SERVER_PORT, $errNo, $errStr);
		if ($fp) {
			// Send request to retrieve user details (email)
			$request = GET_USER . DELIMINATER . strtolower(trim($_POST[KEY_QUERY]));
			fputs($fp, $request, strlen($request));

			// Analyse server's response
			$results = trim(fgets($fp));
			fclose($fp);
			if ($results) {
				if (strcmp($results, GET_USER . DELIMINATER . FAILURE) != 0) {
					$reqArray = explode(DELIMINATER, $results);
					$_SESSION[KEY_USER_TO_FOLLOW] = $reqArray[1];
				}
			}
		}else{
			echo "Error Connecting: ".$errNo.": ".$errStr;
		}
	} catch(exception $ex) {
		echo "<br> Error: " . $ex;
	}
}

// Follow user
if (isset($_POST[KEY_FOLLOW_USER]) && isset($_SESSION[KEY_USER_TO_FOLLOW])) {
	// TODO: check if the user is already followed

	try {
		// Connect to the remote host
		$errNo = $errStr = "";
	} catch(exception $ex) {
		echo "<br> Connection Error: " . $ex;
	}
	$fileName = $_SESSION[KEY_EMAIL];
	$fileFollow = fopen(DATA_ROOT . $fileName . FOLLOW_EXT, 'a');
	fwrite($fileFollow, $_SESSION[KEY_USER_TO_FOLLOW]);
	fwrite($fileFollow, "\n");
	fclose($fileFollow);

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
						}
					?>
</div>

</div>
</div>
</div>
</body>
</html>