<?
// ochrana proti hackunti
if (!defined('IN_CODE')): die('Nepovoleny pristup! / Hacking attempt!'); endif;



    
   
/*
my_affected_rows -- Vracia po�et ovplyvnen�ch z�znamov�v�MySQL po poslednom dotaze
my_client_encoding -- Vracia n�zov znakovej sady
my_close -- Ukon�� MySQL spojenie
my_dbcon -- Vytvor� spojenie�s�MySQL serverom
my_errno --  Vracia ��seln� hodnotu chybovej hl�ky prech�dzaj�ceho MySQL pr�kazu
my_error --  Vr�ti text chybovej spr�vy predch�dzaj�ceho MySQL pr�kazu
my_escape_string --  Uprav� re�azec pre bezpe�n� pou�itie�v�MySQL
my_fetch_array --  Na��ta v�sledn� riadok asociat�vneho,��seln�ho alebo relat�vneho po�a (oboje)
my_fetch_assoc --  Na��ta v�sledn� riadok do asociat�vneho po�a
my_insert_id --  Vracia generovan� hodnotu posledn�ho pr�kazu INSERT
my_num_rows -- Vr�ti po�et z�znamov vo v�sledku
my_query -- Po�le MySQL dotaz
my_select_db -- Nastav� MySQL datab�zu
my_stat -- Vracia aktu�lny stav syst�mu
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
// otvorenie spojenia�s�db
$dbspojenie = mysql_connect($dbserver,$dbuser,$dbpass);


global $dbencoding;
$encode = $dbencoding;
mysql_query("SET NAMES '$encode'",$dbspojenie);

// nastaven� k�dov�n�      
mysql_query("set CHARACTER SET $dbencoding", $dbspojenie);
mysql_query("SET NAMES $dbencoding", $dbspojenie);
mysql_Query("SET character_set_results=$dbencoding", $dbspojenie);
mysql_Query("SET character_set_connection=utf8", $dbspojenie);
mysql_Query("SET character_set_client=$dbencoding", $dbspojenie);





?>
