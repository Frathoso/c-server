<?php
define("DATA_ROOT", '../tweeter-data/');
define("USERS_FILENAME", 'users');	
define("FOLLOW_EXT", '.follow');
define("KEY_QUERY", 'query');
define("KEY_FOLLOW_USER", 'followuser');
define("KEY_USER_TO_FOLLOW", 'usertofollow');
define("KEY_EMAIL", 'email');
define("KEY_FIRST_NAME", 'firstName');
define("KEY_LAST_NAME", 'lastName');

session_start();

// Search if a user exists with "query" details
if(isset($_POST[KEY_QUERY])){
	$fileUsers = file(DATA_ROOT.USERS_FILENAME);
	foreach($fileUsers as $line){
		$user = json_decode($line, true);
		if(strcasecmp($user[KEY_FIRST_NAME], $_POST[KEY_QUERY]) == 0 || 
		   strcasecmp($user[KEY_LAST_NAME], $_POST[KEY_QUERY]) == 0 ||
		   strcasecmp($user[KEY_EMAIL], $_POST[KEY_QUERY]) == 0){
			$_SESSION[KEY_USER_TO_FOLLOW] = $user[KEY_EMAIL];
			break;
		}
	}
}
	
// Follow user
if(isset($_POST[KEY_FOLLOW_USER]) && isset($_SESSION[KEY_USER_TO_FOLLOW])){
	// TODO: check if the user is already followed
	$fileName = $_SESSION[KEY_EMAIL];
	$fileFollow = fopen(DATA_ROOT.$fileName.FOLLOW_EXT, 'a');
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
						if(isset($_SESSION[KEY_USER_TO_FOLLOW])){
						$form = '<form method="post" action="follow.php">'.
								$_SESSION[KEY_USER_TO_FOLLOW].'<input type="checkbox" name="followuser">'.
								'<input type="submit" value="Follow" class="button">'.
								'</form>';
						echo $form;
						}else if(isset($_POST[KEY_QUERY])){
							echo "User not available";
						}
						?>
					</div>
					
				</div>
			</div>
		</div>
	</body>
</html>