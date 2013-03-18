<?php
include_once ('protocol.php');

session_start();

// Check if a new tweet is being posted by user
if (isset($_POST[KEY_TWEET]) && isset($_SESSION[KEY_EMAIL])) {
	// Upload the tweet
	try {
		$errNo = $errStr = "";
		$fp = stream_socket_client(SERVER_ADDR . ":" . SERVER_PORT, $errNo, $errStr, 0);
		if ($fp) {
			$request = PUT_USER_TWEET . DELIMINATER . $_SESSION[KEY_EMAIL] . DELIMINATER . $_POST[KEY_TWEET];
			fputs($fp, $request, strlen($request));

			$response = fgets($fp);
			// response is not used for now
			fclose($fp);
		} else {
			echo "Error: $errStr";
		}
	} catch(exception $ex) {
		echo "Error: $ex";
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
				<div id="home-top">
					<?php
					echo "<span>Welcome <b>" . $_SESSION[KEY_FIRST_NAME] . "</b>!</span>";
					if (isset($_SESSION[KEY_EMAIL])) {
						echo "<span id='follow'><a href='follow.php'>Follow New People</a></span>";
						echo "<span id='unfollow'><a href='unfollow.php'>Unfollow People</a></span>";
						echo "<span id='log-out'><a href='logout.php'>Log Out</a></span>";
					}
					?>
				</div>
				<div id="home-body">
					<div id="home-new-tweet">
						<form method="post" action="home.php">
							<input type="text" id="new-tweet" name="tweet" min="1" max="140" required>
							<input type="submit" value="Tweet" class="button">
						</form>
					</div>
					<?php
					if (isset($_SESSION[KEY_EMAIL])) {
						try {
							$errNo = $errStr = "";
							$fp = stream_socket_client(SERVER_ADDR . ":" . SERVER_PORT, $errNo, $errStr, 0);
							if ($fp) {
								/* Retrieve and display tweets for the user */
								$request = GET_USER_TWEETS . DELIMINATER . $_SESSION[KEY_EMAIL];
								fputs($fp, $request, strlen($request));
								$response = trim(fgets($fp));
								fclose($fp);

								if (strcmp($response, GET_USER_TWEETS . DELIMINATER . FAILURE) != 0) {
									displayTweets($_SESSION[KEY_EMAIL], $response);
								}

								/* Retrieve and display tweets for users being followed */
								// Get all users being followed
								$request = GET_FOLLOWED_USERS . DELIMINATER . $_SESSION[KEY_EMAIL];
								$fp = stream_socket_client(SERVER_ADDR . ":" . SERVER_PORT, $errNo, $errStr, 0);
								fputs($fp, $request, strlen($request));
								$response = trim(fgets($fp));
								fclose($fp);

								// Retrieve users' tweets if available
								if (strcmp($response, GET_FOLLOWED_USERS . DELIMINATER . FAILURE) != 0) {
									$followedUsers = explode(DELIMINATER, $response);
									for ($K = 1; $K < count($followedUsers); $K++) {
										$fp = stream_socket_client(SERVER_ADDR . ":" . SERVER_PORT, $errNo, $errStr, 0);
										$request = GET_USER_TWEETS . DELIMINATER . trim($followedUsers[$K]);
										fputs($fp, $request, strlen($request));
										$response = trim(fgets($fp));
										fclose($fp);
										displayTweets($followedUsers[$K], $response);
									}
								}
							} else {
								echo "Error [$errNo] : $errStr";
							}
						} catch(exception $ex) {
							echo "Error: $ex";
						}
					} else {
						// No user has logged in, display login/sign-up links
						echo '<p>You need to <a href="index.php">Log In</a> or <a href="index.php">Sign Up</a></p>';
					}

					// Helper function to display tweets for a user
					function displayTweets($user, $response) {
						$tweets = explode(DELIMINATER, $response);
						foreach ($tweets as $tweet) {
							if (strcmp($tweet, GET_USER_TWEETS) != 0)
								echo "<div class='tweet'><span>$tweet</span><span class='tweet-user'>by $user</span></div>";
						}
					}
					?>
				</div>
			</div>
		</div>
	</body>
</html>