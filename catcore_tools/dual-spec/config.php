<?
//ocharna proti hackunt�
if (!defined('IN_CODE')): die('Nepovoleny pristup! / Hacking attempt!'); endif;

// Datab�ze
  $dbserver = "localhost"; // MySQL Server(nap� "localhost")
  $dbname_realmd = "realmd"; // MySQL datab�ze "realmd"
  $dbname_characters = "characters"; // MySQL datab�ze "characters"
  $dbuser = "user"; // MySQL u�ivatel
  $dbpass = "pass"; // MySQL heslo
  $dbencoding = "cp1250"; // Kodov�n� v�stupu
  

// Server
  $svadress = "http://81.31.47.185"; // Adresa serveru
  $svpath = "/dual-spec/"; // slo�ka ve kter� je web um�st�n, pokud je v rootu, zapi�te /
  $svforum = "http://81.31.47.185/forum/"; // Adresa na f�rum
  $svacc = "http://81.31.47.185/regacc/player.php?nr=0"; // Adresa na �pravu Account�
