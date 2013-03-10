<?php
ob_start();
?>
<!DOCTYPE html>
<html>
	<head>
		<meta charset="UTF-8" />
		<title>My Tweeter</title>
		<link rel="stylesheet" href="css/layout.css" type="text/css" />
	</head>
	<body>
		<div id="cont-page">
			<?php
			include_once ('login.php');
			include_once ('signup.php');
			?>
			<div id="cont-login">
				<form method="post" action="index.php">
					<h3 class="title" >Login</h3>
					<div class="inputrow">
						<label class="field">Email:</label>
						<input type="text" name="email" class="textinput" value="<?php echo $loginEmail; ?>" required>
					</div>
					<div class="inputrow">
						<label class="field">Password:</label>
						<input type="password" name="password" class="textinput" required>
					</div>
					<div class="inputrow">
						<input type="submit" value="Login" class="button">
					</div>
				</form>
			</div>
			<div id="cont-signup">
				<form method="post" action="index.php">
					<h3 class="title" >Sign Up</h3>
					<div class="inputrow">
						<label class="field">First Name:</label>
						<input type="text" name="firstName" class="textinput" value="<?php echo $firstName; ?>" required>
					</div>
					<div class="inputrow">
						<label class="field">Last Name:</label>
						<input type="text" name="lastName" class="textinput" value="<?php echo $lastName; ?>" required>
					</div>
					<div class="inputrow">
						<label class="field">Email: </label>
						<input type="email" name="email" class="textinput" value="<?php echo $signEmail; ?>" required>
					</div>
					<div class="inputrow">
						<label class="field">Password:</label>
						<input type="password" name="password" class="textinput" required>
					</div>
					<div class="inputrow">
						<input type="submit" value="Sign Up" class="button">
					</div>
				</form>
			</div>
		</div>
	</body>
</html>