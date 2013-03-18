<?php
include_once('protocol.php');

session_start();

// Check if a new tweet is being posted by user
if (isset($_POST[KEY_TWEET]) && isset($_SESSION[KEY_EMAIL])) {
	// Upload the tweet
	$fileName = $_SESSION[KEY_EMAIL]; // user email is used as his/her tweets filename
	$fileTweets = fopen(DATA_ROOT . $fileName . TWEETS_EXT, 'a');
	$tweet = array("tweet" => $_POST[KEY_TWEET]);
	fwrite($fileTweets, json_encode($tweet));
	fwrite($fileTweets, "\n");
	fclose($fileTweets);
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
					echo "<span>Welcome <b>".$_SESSION[KEY_FIRST_NAME]."</b>!</span>";
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
						// Retrieve and display tweets
						$fileName = $_SESSION[KEY_EMAIL];
						$otherUsersFile = file(DATA_ROOT . $fileName . FOLLOW_EXT);
						displayTweets($fileName);
						foreach ($otherUsersFile as $otherUser) {
							displayTweets(trim($otherUser));
						}

					} else {
						// No user has logged in, display login/sign-up links
						echo '<p>You need to <a href="index.php">Log In</a> or <a href="index.php">Sign Up</a></p>';
					}

					function displayTweets($otherUser) {
						$fileTweets = file(DATA_ROOT . $otherUser . TWEETS_EXT);
						foreach ($fileTweets as $tweetLine) {
							$tweet = json_decode($tweetLine, true);
							$tweetText = $tweet['tweet'];
							echo "<div class='tweet'><span>$tweetText</span><span class='tweet-user'>by $otherUser</span></div>";
						}
					}
					?>
				</div>
			</div>
		</div>
	</body>
</html>