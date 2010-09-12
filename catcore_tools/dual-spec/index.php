<?php 
session_start();
define('IN_CODE',true);
require("config.php");
require("db/file0653.php");
require("login.php");
?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<html>
<head>
  <title>World of Warcraft - Web Dual-Spec</title>
  <meta http-equiv="Content-Type" content="text/html; charset=windows-1250">
  
  <link rel="Shorcut Icon" href="icon/favicon.ico">
  <link rel="stylesheet" href="css/style.css" type="text/css" id="bnetstyle">
<link rel="shortcut icon" href="http://wow.herniweb.cz/favicon.ico">
  <script type="text/javascript" language="javascript">
  <!--
    var styleSheet;
    var agt     = navigator.userAgent.toLowerCase();
    var appVer  = navigator.appVersion.toLowerCase();
    var verInt  = parseInt(appVer);
    var ie      = (appVer.indexOf('msie') != -1);
    var opera   = (agt.indexOf('opera') != -1);
    var mozilla = ((agt.indexOf('mozilla')!=-1) && (agt.indexOf('opera')==-1)
      && (agt.indexOf('spoofer')==-1) && (agt.indexOf('compatible') == -1)
      && (agt.indexOf('webtv')==-1) && (agt.indexOf('hotjava')==-1));
    var ns4     = (mozilla && (verInt == 4));

    if (ie && !opera) {
      document.styleSheets["bnetstyle"].addRule ("input", "background-color: #040D1A");
      document.styleSheets["bnetstyle"].addRule ("input", "border-style: solid");
      document.styleSheets["bnetstyle"].addRule ("input", "border-width: 1px");
      document.styleSheets["bnetstyle"].addRule ("input", "border-color: #7F7F7F");
      document.styleSheets["bnetstyle"].addRule ("input", "color: #FFAC04");

      document.styleSheets["bnetstyle"].addRule ("body", "scrollbar-Base-color: #000");
      document.styleSheets["bnetstyle"].addRule ("body", "scrollbar-Arrow-color: #f90");
      document.styleSheets["bnetstyle"].addRule ("body", "scrollbar-DarkShadow-Color: #000");
      document.styleSheets["bnetstyle"].addRule ("body", "scrollbar-Face-Color: #000");
      document.styleSheets["bnetstyle"].addRule ("body", "scrollbar-Highlight-Color: #f90");
      document.styleSheets["bnetstyle"].addRule ("body", "scrollbar-Shadow-Color: #f90");
      document.styleSheets["bnetstyle"].addRule ("body", "scrollbar-Track-Color: #000");

      document.styleSheets["bnetstyle"].addRule ("textarea", "background-color: #040D1A");
      document.styleSheets["bnetstyle"].addRule ("textarea", "border-style: solid");
      document.styleSheets["bnetstyle"].addRule ("textarea", "border-width: 1px");
      document.styleSheets["bnetstyle"].addRule ("textarea", "border-color: #7F7F7F");
      document.styleSheets["bnetstyle"].addRule ("textarea", "color: #FFAC04");

      document.styleSheets["bnetstyle"].addRule ("textarea", "scrollbar-Base-Color: #012158");
      document.styleSheets["bnetstyle"].addRule ("textarea", "scrollbar-Arrow-Color: #7F7F7F");
      document.styleSheets["bnetstyle"].addRule ("textarea", "scrollbar-3dLight-Color: #7F7F7F");
      document.styleSheets["bnetstyle"].addRule ("textarea", "scrollbar-DarkShadow-Color: black");
      document.styleSheets["bnetstyle"].addRule ("textarea", "scrollbar-Highlight-Color: black");
      document.styleSheets["bnetstyle"].addRule ("textarea", "scrollbar-Shadow-Color: #00B3FF");

      document.styleSheets["bnetstyle"].addRule ("select", "background-color: #040D1A");
      document.styleSheets["bnetstyle"].addRule ("select", "color: #FFAC04");

      document.styleSheets["bnetstyle"].addRule ("select.gray", "background-color: #040D1A");
      document.styleSheets["bnetstyle"].addRule ("select.gray", "color: #FFAC04");

      document.styleSheets["bnetstyle"].addRule ("ul.thread", "margin-left: 22px;");
    }

    var cleanURL = document.URL.replace("locale=[^&]*","");
  //-->
  </script>
  <script type="text/javascript">
  function passtext(name)
  {
    document.getElementById('stone').value = name;
  }
  function showhide(name)
  {
      var elem = document.getElementById("p"+name);
      if (elem.style.display=="") {
          elem.style.display="none";
      } else {
          elem.style.display="";
      }
      var elem2 = document.getElementById("pb"+name);
      if (elem.style.display=="") {
          elem2.src = "./img/minus.gif";
      } else {
          elem2.src = "./img/plus.gif";
      }
  }
  </script>

</head>
<body style="text-align: center" onload = "javascript: document.forms[0].elements[0].focus();" bgcolor="#000000" text="#CCCC99" link="#FFFFBB" vlink="AAAAAA" marginwidth=0 marginheight=0 topmargin=0 leftmargin=0>
<?php
if($_GET["akce"] == ""){
include("main.php");
}
if($_GET["akce"] == "kecy"){
include("kecy.php");
}
if($_GET["akce"] == "reset"){
include("talents.php");
}


?>
<div aling="center"><small>&copy 2009 Vojtěch "Tassadar" Boček</small></div>
<script>
if (!document.layers)
  document.write('<div id="divStayTopLeft" style="position:absolute">')
</script>
<layer id="divStayTopLeft">
<table border="0" background="./img/menu.png" width="146" height="124" cellspacing="0" cellpadding="0">
  <tr class=title>
    <td style='padding-top: 14px; padding-left: 18px;'>
      <img src="./img/square.gif" style='vertical-align: middle;' width="15" height="15"><a href="<?php echo $svacc; ?>" style='color: #FFB019; font-weight: bold;'><font size=1>User Account</font></a><br>
      <img src="./img/square.gif" style='vertical-align: middle;' width="15" height="15"><a href="javascript:history.back();" onmouseover="this.style.textDecoration='underline'" onmouseout="this.style.textDecoration='none'" style='text-decoration: none; color: #FFB019; font-weight: bold;'><font size=1>Předchozí stránka</font></a><br>
      <img src="./img/square.gif" style='vertical-align: middle;' width="15" height="15"><a href="<?php echo $svforum; ?>" onmouseover="this.style.textDecoration='underline'" onmouseout="this.style.textDecoration='none'" style='text-decoration: none; color: #FFB019; font-weight: bold;'><font size=1>Valhalla Forum</font></a><br>
      <?php if($_SESSION["prihlasen"] == "true"){
      ?>
      <a href="index.php?akce=logout" style='color: #FFB019;'><img src = "img/logout.gif" width="127" height="22" border="0" style="position: absolute; left:10px; top:93px;"></a><br>
      <?php
      } ?>
    </td>
  </tr>
</table>
</layer>
<script type="text/javascript">
var verticalpos="fromtop"

if (!document.layers)
document.write('</div>')

function JSFX_FloatTopDiv()
{
	var startX = 10,
	startY = 8;
	var ns = (navigator.appName.indexOf("Netscape") != -1);
	var d = document;
	function ml(id)
	{
		var el=d.getElementById?d.getElementById(id):d.all?d.all[id]:d.layers[id];
		if(d.layers)el.style=el;
		el.sP=function(x,y){this.style.left=x;this.style.top=y;};
		el.x = startX;
		if (verticalpos=="fromtop")
		el.y = startY;
		else{
		el.y = ns ? pageYOffset + innerHeight : document.body.scrollTop + document.body.clientHeight;
		el.y -= startY;
		}
		return el;
	}
	window.stayTopLeft=function()
	{
		if (verticalpos=="fromtop"){
		var pY = ns ? pageYOffset : document.body.scrollTop;
		ftlObj.y += (pY + startY - ftlObj.y)/8;
		}
		else{
		var pY = ns ? pageYOffset + innerHeight : document.body.scrollTop + document.body.clientHeight;
		ftlObj.y += (pY - startY - ftlObj.y)/8;
		}
		ftlObj.sP(ftlObj.x, ftlObj.y);
		setTimeout("stayTopLeft()", 10);
	}
	ftlObj = ml("divStayTopLeft");
	stayTopLeft();
}
JSFX_FloatTopDiv();
</script><script type="text/javascript" src="script/wz_tooltip.js"></script>
</body>
</html>