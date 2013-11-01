<?php
include_once ('protocol.php');

session_start();

// Unfollow all selelected users
if (isset($_POST[KEY_UNFOLLOW]) && isset($_SESSION[KEY_EMAIL])) {
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
				$request = UNFOLLOW_USERS . DELIMITER . $_SESSION[KEY_EMAIL];
				foreach ($_POST[KEY_UNFOLLOW] as $key => $selectedName) {
					$request .= DELIMITER . trim($selectedName);
				}
				fputs($fp, $request, strlen($request));

				// Analyse server's response
				$results = trim(fgets($fp));
				fclose($fp);
				break;
			} else {
				//echo "Error Connecting: " . $errNo . ": " . $errStr;
			}
		} catch(exception $ex) {
			//echo "<br> Error: " . $ex;
		}
	}
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
					<div>
						<form method="post" action="unfollow.php">
							<?php
							// Request for all users being followed
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
										$request = GET_FOLLOWED_USERS . DELIMITER . $_SESSION[KEY_EMAIL];
										fputs($fp, $request, strlen($request));

										// Analyse server's response
										while (!feof($fp)) {
											$results = trim(fgets($fp));
											if ($results) {
												if (strcmp($results, GET_FOLLOWED_USERS . DELIMITER . FAILURE) == 0) {
													break;
												}
												$users = explode(DELIMITER, $results);
												//for ($K = 0; $K < count($users); $K++) {
												foreach ($users as $key => $user) {
													if (strcmp($user, GET_FOLLOWED_USERS) == 0 || strlen($user) < 5) {
														continue;
													}
													echo $user . '<input type="checkbox" name="unfollow[' . $user . ']" value="' . $user . '"><br>';
												}
											}
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
						?>
<input type="submit" value="Unfollow Selected People" class="button" style="width:200px">
</form>
</div>
</div>
</div>
</div>
</body>
</html>