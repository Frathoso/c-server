<?php
// Server details
$SERVERS = array(array('tcp://localhost', 8800), array('tcp://localhost', 8801), array('tcp://localhost', 8802));

define("BUFFER_SIZE", 8192);

define("HOME_PAGE", 'home.php');
define("LOGIN_PAGE", 'index.php');

// Protocol keywords
define("AUTHENTICATE_USER", "AUTUSR");
define("ADD_USER", "ADDUSR");
define("REMOVE_USER", "REMUSR");
define("FOLLOW_USER", "FOLUSR");
define("UNFOLLOW_USERS", "UNFUSR");
define("GET_USER", "GETUSR");
define("GET_USER_TWEETS", "GETTWT");
define("PUT_USER_TWEET", "PUTTWT");
define("GET_FOLLOWED_USERS", "GETFOL");
define("UNKNOWN_REQUEST", "UNKNOW");
define("SUCCESS", "YES");
define("FAILURE", "NO");
define("DELIMITER", "~");

// Keys for identifying data
define("KEY_EMAIL", 'email');
define("KEY_PASSWORD", 'password');
define("KEY_FIRST_NAME", 'firstName');
define("KEY_LAST_NAME", 'lastName');
define("KEY_QUERY", 'query');
define("KEY_FOLLOW_USER", 'followuser');
define("KEY_USER_TO_FOLLOW", 'usertofollow');
define("KEY_UNFOLLOW", 'unfollow');
define("KEY_TWEET", 'tweet');
define("KEY_MESSAGE", 'message');
?>