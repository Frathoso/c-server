<?php
define("DATA_ROOT", '../tweeter-data/');
define("USERS_FILENAME", 'users');
define("FOLLOW_EXT", '.follow');
define("KEY_EMAIL", 'email');
define("KEY_FIRST_NAME", 'firstName');
define("KEY_LAST_NAME", 'lastName');
define("KEY_UNFOLLOW", 'unfollow');

session_start();

// Unfollow all selelected users
if(isset($_POST[KEY_UNFOLLOW])){
	$fileName = $_SESSION[KEY_EMAIL] . FOLLOW_EXT;
	$followNames = file_get_contents(DATA_ROOT.$fileName);
	foreach($_POST[KEY_UNFOLLOW] as $key => $selectedName){
		$followNames = str_replace(trim($selectedName)."\n", "", $followNames);				
	}
	file_put_contents(DATA_ROOT.$fileName, $followNames);
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
							// Load all users
							$fileName = $_SESSION[KEY_EMAIL] . FOLLOW_EXT;
							$fileUsers = file(DATA_ROOT . $fileName);
							foreach ($fileUsers as $user) {
								echo $user . '<input type="checkbox" name="unfollow['.$user.']" value="'.$user.'"><br>';
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