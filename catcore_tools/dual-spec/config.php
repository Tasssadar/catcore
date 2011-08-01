<?
//ocharna proti hackuntí
if (!defined('IN_CODE')): die('Nepovoleny pristup! / Hacking attempt!'); endif;

// Databáze
  $dbserver = "localhost"; // MySQL Server(napø "localhost")
  $dbname_realmd = "realmd"; // MySQL databáze "realmd"
  $dbname_characters = "characters"; // MySQL databáze "characters"
  $dbuser = "user"; // MySQL uživatel
  $dbpass = "pass"; // MySQL heslo
  $dbencoding = "cp1250"; // Kodování výstupu
  

// Server
  $svadress = "http://81.31.47.185"; // Adresa serveru
  $svpath = "/dual-spec/"; // složka ve které je web umístìn, pokud je v rootu, zapište /
  $svforum = "http://81.31.47.185/forum/"; // Adresa na fórum
  $svacc = "http://81.31.47.185/regacc/player.php?nr=0"; // Adresa na úpravu Accountù
