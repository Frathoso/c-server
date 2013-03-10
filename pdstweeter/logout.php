<?php
ob_start();

define("LOGIN_PAGE", 'index.php');
session_start();

// Terminates the session and redirects to the login page
session_destroy();
header("Location: " . LOGIN_PAGE);
exit();
?>