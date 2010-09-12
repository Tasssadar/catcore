<form action="index.php?akce=login" method="post">
<table cellspacing = "0" cellpadding = "0" border = "0" width = "100%" height = "100%">
<tr><td><img src="img/pixel.gif" width= "1" height = "26"></td></tr>
<tr>
	<td valign="top" align="center">
			<table border="0" cellpadding="0" cellspacing="0" background="img/login-bgclean.jpg" style="background-position:center top; background-repeat: no-repeat;">
      <tr>
				<td valign = "top">
					<div style = "position: relative;">
              <div style = " height: 68px; width: 390px; position: absolute; left: 112px; top: -20px;">
            </div>
					</div>
				</td>
				<td style="text-align:center"><img src="img/pixel.gif" width= "1" height = "169"></br>CHYBA!</td>
        <td></td>
			</tr>
			<tr>
				<td><img src="img/pixel.gif" width="203" height="1"></td>
				<td>
				
				<img src = "img/pixel.gif" width = "203" height = "200">
        </td>
				<td><img src = "img/pixel.gif" width = "217" height = "1"></td>
			</tr>
			<tr>
				<td colspan = "3">
					<table border="0" cellpadding="0" cellspacing="0" align='center'>
					<tr>
						<td colspan = "3"><img src = "img/pixel.gif" width = "1" height = "17"></td>
					</tr>
                            		<tr>
						<td width = "106"><img src = "img/pixel.gif" width = "106" height = "1"></td>
						<td width = "410">
						<small style="display:block; text-align:center; margin:6px 0 0;">
              <?php echo $_SESSION["kecy"]; ?>
              </small>
              
            </td>
            
						<td width = "124"><img src = "img/pixel.gif" width = "124" height = "1"></td>
					</tr>					<tr>
						<td colspan = "3"><img src = "img/pixel.gif" width = "1" height = "60"></td>
					</tr>
					</table>
		  		</td>
			</tr>
			</table>
	</td>
</tr>
</table>
</form>