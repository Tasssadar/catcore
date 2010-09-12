<?
// ochrana proti hackunti
if (!defined('IN_CODE')): die('Nepovoleny pristup! / Hacking attempt!'); endif;



    
   
/*
my_affected_rows -- Vracia poèet ovplyvnených záznamov v MySQL po poslednom dotaze
my_client_encoding -- Vracia názov znakovej sady
my_close -- Ukonèí MySQL spojenie
my_dbcon -- Vytvorí spojenie s MySQL serverom
my_errno --  Vracia èíselnú hodnotu chybovej hlášky prechádzajúceho MySQL príkazu
my_error --  Vráti text chybovej správy predchádzajúceho MySQL príkazu
my_escape_string --  Upraví reazec pre bezpeèné použitie v MySQL
my_fetch_array --  Naèíta výsledný riadok asociatívneho,èíselného alebo relatívneho po¾a (oboje)
my_fetch_assoc --  Naèíta výsledný riadok do asociatívneho po¾a
my_insert_id --  Vracia generovanú hodnotu posledného príkazu INSERT
my_num_rows -- Vráti poèet záznamov vo výsledku
my_query -- Pošle MySQL dotaz
my_select_db -- Nastaví MySQL databázu
my_stat -- Vracia aktuálny stav systému
*/

function my_affected_rows($spojeni = false)
{
if ($spojeni === false):
  return mysql_affected_rows();
else:
  return mysql_affected_rows($spojeni);
endif;
}

function my_client_encoding($spojeni = false)
{
if ($spojeni === false):
  return mysql_client_encoding();
else:
  return mysql_client_encoding($spojeni);
endif;
}

function my_close($spojeni = false)
{
if ($spojeni === false):
  return mysql_close();
else:
  return mysql_close($spojeni);
endif;
}

function my_dbcon()
{
global $dbserver,$dbuser,$dbpass,$dbname_realmd,$spojeni;
@$spojeni=mysql_connect($dbserver,$dbuser,$dbpass);
if (!$spojeni):
header("HTTP/1.1 301 Moved Permanently");                              
header("Location: ".$svfull."mysql_unable_to_connect.php"); 
header("Connection: close");                                           
die('Could not connect to database server!');
endif;
mysql_select_db($dbname,$spojeni);
return $spojeni;
}
function my_dbcon2()
{
global $dbserver,$dbuser,$dbpass,$dbname_characters,$spojeni;
@$spojeni=mysql_connect($dbserver,$dbuser,$dbpass);
if (!$spojeni):
header("HTTP/1.1 301 Moved Permanently");                              
header("Location: ".$svfull."mysql_unable_to_connect.php"); 
header("Connection: close");                                           
die('Could not connect to database server!');
endif;
mysql_select_db($dbname,$spojeni);
return $spojeni;
}

function my_errno($spojeni = false)
{
if ($spojeni === false):
  return mysql_errno();
else:
  return mysql_errno($spojeni);
endif;
}

function my_error($spojeni = false)
{
if ($spojeni === false):
  return mysql_error();
else:
  return mysql_error($spojeni);
endif;
}

function my_escape_string($neupraveny_retazec)
{
return mysql_escape_string($neupraveny_retazec);
}

function my_fetch_array($vysledok,$typ_vysledku = false)
{
if ($typ_vysledku === false):
  return mysql_fetch_array($vysledok);
else:
  return mysql_fetch_array($vysledok,$typ_vysledku);
endif;
}

function my_fetch_assoc($vysledok)
{
return mysql_fetch_assoc($vysledok);
}

function my_insert_id($spojeni = false)
{
if ($spojeni === false):
  return mysql_insert_id();
else:
  return mysql_insert_id($spojeni);
endif;
}

function my_num_rows($vysledok)
{
return mysql_num_rows($vysledok);
}

function my_query($dotaz,$spojeni)
{
    

if ($spojeni === false):
   @$vysledok=mysql_query($dotaz);
  if ($vysledok==0):
    echo mysql_error();
  endif;
  return $vysledok;
else:
  @$vysledok=mysql_query($dotaz,$spojeni);
  if ($vysledok==0):
    echo mysql_error($spojeni);
  endif;
  return $vysledok;
endif;
}

function my_select_db($meno_databaze,$spojeni = false)
{
if ($spojeni === false):
  return mysql_select_db($meno_databaze);
else:
  return mysql_select_db($meno_databaze,$spojeni);
endif;
}
// otvorenie spojenia s db
$dbspojenie = mysql_connect($dbserver,$dbuser,$dbpass);


global $dbencoding;
$encode = $dbencoding;
mysql_query("SET NAMES '$encode'",$dbspojenie);

// nastavení kódování      
mysql_query("set CHARACTER SET $dbencoding", $dbspojenie);
mysql_query("SET NAMES $dbencoding", $dbspojenie);
mysql_Query("SET character_set_results=$dbencoding", $dbspojenie);
mysql_Query("SET character_set_connection=utf8", $dbspojenie);
mysql_Query("SET character_set_client=$dbencoding", $dbspojenie);





?>
