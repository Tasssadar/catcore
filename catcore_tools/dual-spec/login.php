<?php

if($_SESSION["prihlasen"] != "true" AND $_GET["akce"] != "login" AND $_GET["akce"] != "kecy" AND $_GET["akce"] != "logout" AND $_GET["akce"] != ""){ 
if($_GET["redirected"] != "true"){
header("HTTP/1.1 301 Moved Permanently");
header("Location: ".$svfull."index.php");
header("Connection: close");
}
}else{

if($_GET["akce"] == "login" AND $_POST["username"] != "" AND $_POST["password"] != ""){
$pass = sha1(strtoupper($_POST['username']).":".strtoupper($_POST['password']));
mysql_select_db($dbname_realmd,$dbspojenie);
$select_p = "SELECT * FROM account WHERE username='".$_POST['username']."' AND sha_pass_hash='$pass'";    
$select_v = mysql_query($select_p,$dbspojenie);
$select_z = mysql_fetch_array($select_v);
if($select_z["id"] != ""){
$_SESSION["prihlasen"] = "true";
$_SESSION["id"] = $select_z["id"];
$_SESSION["username"] = $select_z["username"];
header("HTTP/1.1 301 Moved Permanently");
header("Location: ".$svfull."index.php?akce=reset");
header("Connection: close");

}else{
$_SESSION["kecy"] = "Nemùžu se pøihlásit k Acc. Možná jste zadali špatné jméno/heslo.";
header("HTTP/1.1 301 Moved Permanently");
header("Location: ".$svfull."index.php?akce=kecy");
header("Connection: close");

}
}else{
if($_GET["akce"] == "login"){
$_SESSION["kecy"] = "Nìkteré pole je nevyplnìné.";
header("HTTP/1.1 301 Moved Permanently");
header("Location: ".$svfull."index.php?akce=kecy");
header("Connection: close");
}
}
if($_GET["akce"] == "logout"){
$_SESSION["prihlasen"] = "";
$_SESSION["id"] = "";
$_SESSION["username"] = "";
header("HTTP/1.1 301 Moved Permanently");
header("Location: ".$svfull."index.php");
header("Connection: close");

}
if($_SESSION["id"] == "" OR $_SESSION["username"] == ""){
if($_GET["akce"] != ""){
header("HTTP/1.1 301 Moved Permanently");
header("Location: ".$svfull."index.php");
header("Connection: close");
}
}
}

?>