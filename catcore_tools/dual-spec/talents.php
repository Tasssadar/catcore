<?PHP 
if($_GET["modul"] == ""){
?>
<div style="width: 700px; position: relative; text-align: left; margin: auto">
<div class="talents_top" align="center"><h3 style="padding-top:170px">Vyberte postavu, na kter� chcete aktivovat nebo<br> pou��t dual-spec</h3></div>
<div class="talents" align="center">
<table width="350px" align="center">
<tr>
<td align='left' nowrap='nowrap' style='text-decoration: underline; color: #FFB019;'>Jm�no</td>

<td align='left' nowrap='nowrap' style='text-decoration: underline; color: #FFB019;'>Status</td>
<td align='left' nowrap='nowrap' style='text-decoration: underline; color: #FFB019;'>Akce</td>
</tr>
<?PHP 

mysql_select_db($dbname_characters,$dbspojenie);
$select_p = "SELECT * FROM characters WHERE account='".$_SESSION["id"]."' AND level>'39'";
$select_v = mysql_query($select_p,$dbspojenie);

while($select_z = mysql_fetch_array($select_v)){
$select_p2 = "SELECT * FROM dual_spec WHERE guid='".$select_z["guid"]."'";    
$select_v2 = mysql_query($select_p2,$dbspojenie);
$select_z2 = mysql_fetch_array($select_v2);

if($select_z2["active"] == "1"){
$ds_stav = "aktivn�";
$modul = "use";
$action = "<a href=\"index.php?akce=reset&modul=$modul&guid=".$select_z["guid"]."\">Pou��t</a>";
}else{
$ds_stav = "neaktivn�";
$modul = "active";
$action = "<a href=\"index.php?akce=reset&modul=$modul&guid=".$select_z["guid"]."\" onclick=\"return confirm('Opravdu chcete aktivovat Dual-spec? Jeho cena je 1000 G!');\">Aktivovat</a>";

}
?>
<tr>
<td align='left' nowrap='nowrap'><?php echo $select_z["name"]; ?></td>
<td align='left' nowrap='nowrap'><?php echo $ds_stav; ?></td>
<td align='left' nowrap='nowrap'><?php echo $action; ?></td>
</tr>

<?php
 
 }
 
  ?>
<tr>
<td align='left' nowrap='nowrap' style='text-decoration: underline; color: #FFB019;'>Jm�no</td>

<td align='left' nowrap='nowrap' style='text-decoration: underline; color: #FFB019;'>Status</td>
<td align='left' nowrap='nowrap' style='text-decoration: underline; color: #FFB019;'>Akce</td>
</tr>
  </table>
</div>
<div class="talents_back" align="center"><p style="padding-top:50px">Jsou zobrazeny jen postavy s lvlem 40+.</p></div>
</div>
<?php
}
if($_GET["modul"] == "active"){
$guid_f = mysql_escape_string($_GET["guid"]);
mysql_select_db($dbname_characters,$dbspojenie);
$select_p = "SELECT * FROM characters WHERE guid='".$guid_f."'"; // AND level>'39'
$select_v = mysql_query($select_p,$dbspojenie);
$select_z = mysql_fetch_array($select_v);
$select_p2 = "SELECT * FROM dual_spec WHERE guid='".$guid_f."'";    
$select_v2 = mysql_query($select_p2,$dbspojenie);
$select_z2 = mysql_fetch_array($select_v2);
$cooper = substr($select_z["money"], -2);
$silver = substr($select_z["money"], -4, -2);
$gold = substr($select_z["money"], 0, -4);
?>
<div style="width: 700px; position: relative; text-align: left; margin: auto">
<div class="talents_top" align="center"><h3 style="padding-top:170px">Aktivace Dual-spec</h3></div>
<div class="talents" align="center">

<?PHP 
if($select_z["account"] != $_SESSION["id"]){
echo "Nem��u aktivovat dual-spec, tohle nen� va�e postava!<br>";
$error = "1";
}
if($gold < "1000"){
echo "Nem��u aktivovat dual-spec, nem�te dost pen�z!<br>";
$error = "1";
}
if($select_z["level"] <= "39"){
echo "Nem��u aktivovat dual-spec, nem�te level 40+!<br>";
$error = "1";
}
if($guid_f == ""){
echo "Nebylo p�ed�no Va�e GUID!<br>";
$error = "1";
}
if($select_z["online"] == "1"){
echo "Nem��u aktivovat dual-spec, tahle postava je p�ihl�en� do hry!<br>";
$error = "1";
}
if($select_z2["active"] == "1"){
echo "Nem��u aktivovat dual-spec, proto�e na t�to postav� u� je aktivn�!";
$error = "1";
}

if($error != "1"){
$insert_p = "INSERT INTO dual_spec (guid, active,count, money_before, money_after) VALUES ('$guid_f', '1','0', '".$select_z["money"]."', '0')";
$insert_v = mysql_query($insert_p,$dbspojenie);
$gold_m = $gold - 1000;
$money = "$gold_m$silver$cooper";
$control_p = "SELECT * FROM dual_spec WHERE guid='$guid_f'";
$control_v = mysql_query($control_p,$dbspojenie);
$control_z = mysql_fetch_array($control_v);
if($control_z["active"] == "1"){
$update_p = "UPDATE characters SET money='$money' WHERE guid='$guid_f'";
$update_v = mysql_query($update_p,$dbspojenie);
$select_p3 = "SELECT money FROM characters WHERE guid='".$guid_f."'"; // AND level>'39'
$select_v3 = mysql_query($select_p3,$dbspojenie);
$select_z3 = mysql_fetch_array($select_v3);
$update_p2 = "UPDATE dual_spec SET money_after='".$select_z3["money"]."' WHERE guid='$guid_f'";
$update_v2 = mysql_query($update_p2,$dbspojenie);
echo "Dual-Spec bylo aktivov�no a z va�eho ��tu bylo strhnuto 1000 G.<br><a href=\"index.php?akce=reset\">Zp�t na seznam postav</a><br><a href=\"index.php?akce=reset&modul=use&guid=$guid_f\">Pou��t Dual-Spec</a>";
}else{
echo "N�kde se stala chyba, z�pis v tabule se nevytvo�il!<br><a href=\"index.php?akce=reset\">Zp�t na seznam postav</a><br><a href=\"index.php?akce=reset&modul=use&guid=$guid_f\">Pou��t Dual-Spec</a>";
}
}else{
echo "<br><a href=\"index.php?akce=reset\">Zp�t na seznam postav</a><br><a href=\"index.php?akce=reset&modul=use&guid=$guid_f\">Pou��t Dual-Spec</a>";
}

  ?>
</div>
<div class="talents_back" align="center"><p style="padding-top:50px"></p></div>
</div>
<?php
}
if($_GET["modul"] == "use"){
$guid_f = mysql_escape_string($_GET["guid"]);
mysql_select_db($dbname_characters,$dbspojenie);
$select_p = "SELECT * FROM characters WHERE guid='".$guid_f."'"; // AND level>'39'
$select_v = mysql_query($select_p,$dbspojenie);
$select_z = mysql_fetch_array($select_v);
$select_p2 = "SELECT * FROM dual_spec WHERE guid='".$guid_f."'";    
$select_v2 = mysql_query($select_p2,$dbspojenie);
$select_z2 = mysql_fetch_array($select_v2);
$cooper = substr($select_z["money"], -2);
$silver = substr($select_z["money"], -4, -2);
$gold = substr($select_z["money"], 0, -4);
?>
<div style="width: 700px; position: relative; text-align: left; margin: auto">
<div class="talents_top" align="center"><h3 style="padding-top:170px">Pou�it� Dual-spec</h3></div>
<div class="talents" align="center">

<?PHP 
if($select_z["account"] != $_SESSION["id"]){
echo "Nem��u pou��t dual-spec, tohle nen� va�e postava!<br>";
$error = "1";
}
if($select_z["resettalents_cost"] == "0"){
echo "Mezi pou�it�mi dual-spec mus�te alespo� jednou resetovat talenty.<br>";
$error = "1";
}
if($select_z["online"] == "1"){
echo "Nem��u pou��t dual-spec, tahle postava je p�ihl�en� do hry!<br>";
$error = "1";
}
if($select_z["level"] <= "39"){
echo "Nem��u pou��t dual-spec, nem�te level 40+!<br>";
$error = "1";
}
if($select_z2["active"] != "1"){
echo "Nem��u pou��t dual-spec, proto�e na t�to postav� nen� aktivn�!";
$error = "1";
}
if($guid_f == ""){
echo "Nebylo p�ed�no Va�e GUID!<br>";
$error = "1";
}
if($error != "1"){
$gold_m = $gold + 1;
$money = "$gold_m$silver$cooper";
$update_p = "UPDATE characters SET at_login='4' WHERE guid='$guid_f'";
$update_v = mysql_query($update_p,$dbspojenie);
$ds_use = $select_z2["count"]+1;
$update_p = "UPDATE dual_spec SET count='$ds_use' WHERE guid='$guid_f'";
$update_v = mysql_query($update_p,$dbspojenie);
echo "Dual-Spec bylo pou�ito. Talenty byly resetov�ny. <br><a href=\"index.php?akce=reset\">Zp�t na seznam postav</a><br><a href=\"index.php?akce=reset&modul=use&guid=$guid_f\">Pou��t Dual-Spec</a>";
}else{
echo "<br><a href=\"index.php?akce=reset\">Zp�t na seznam postav</a><br><a href=\"index.php?akce=reset&modul=use&guid=$guid_f\">Pou��t Dual-Spec</a>";
}
  ?>
</div>
<div class="talents_back" align="center"><p style="padding-top:50px"></p></div>
</div>
<?php
}
?>