<?php
/*
 * mainpage.php:        Sprava hostingu (ITU 2012)
 *
 * Author(s):      Jan Havlik     <xhavli30@stud.fit.vutbr.cz>
 *                 Vaclav Horazny <xhoraz01@stud.fit.vutbr.cz>
 *                 Radek Sevcik   <xsevci44@stud.fit.vutbr.cz>
 *
 * Date:           Sun, 16 Dec 2012 13:36:03 +0100
 *
 * This file is part of itu12_hosting.
 */

require_once 'config.php';

define('XMLDB_NEWS', PATH_XMLDB . "/news.xml");
define('XSLT_NEWS', PATH_XSLT . "/news.xslt");

function menu()
{
    return array(
        'Home' => 'index',
        'DNS' => 'dns',
        'WEB Hosting' => 'hosting',
        'FTP' => 'ftp',
        'Email' => 'email',
        'Databaze' => 'db',
    );
}

?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN"
"http://www.w3.org/TR/html4/strict.dtd">
<html>
  <head>
    <meta http-equiv="content-type" content="text/html; charset=iso-8859-2">
    <title>ITU projekt</title>
    <link rel="stylesheet" type="text/css" href="st/reset.css">
    <link rel="stylesheet" type="text/css" href="st/normal.css">
    <?= render_head() ?>
  </head>
  <body>
    <div class="telo">
      <a class="logo" href="index.php" title="domu"></a>
      <div class="zprava">
        <?= xmltransform(XMLDB_NEWS, XSLT_NEWS) ?>
      </div>
      <?php if (!isset($_SESSION['user'])) { ?>
        <form class="prihl" action="login.php" method="post">
          <input id="prihl_jmeno" type="text" name="uname" value="e-mail"
                 onfocus="this.style.color=''; this.style.fontStyle='';
                          this.value=''; this.onfocus='';"
                 style="color: gray; font-style: italic;" /><br />
          <input id="prihl_heslo" type="text" name="upwd" value="heslo"
                 onfocus="this.style.color=''; this.style.fontStyle='';
                          this.value=''; this.type='password'; this.onfocus='';"
                 style="color: gray; font-style: italic;" /><br />
          <input id="prihl_tlc" type="submit" value="Prihlasit" />
        </form>
      <?php } else { ?>
        <div class="prihl_p">
          <a class="prihl_jmeno" href="profil.php"><?php echo $_SESSION['user'];?></a>
          <a class="odhl" href="login.php?unlog">odhlasit</a>
        </div>
      <?php } ?>
      <div class="cl"></div> <!-- konec zahlavy -->
      <ul class="hl_menu">
        <?
            foreach (menu() as $key => $val) {
                $act = basename($_SERVER['SCRIPT_NAME']) == "$val.php";
                $cl_selected = $act ? ' class="activ"' : "";
                echo "<li><a id=\"$val\" href=\"$val.php\" title=\"$key\" $cl_selected>$key</a></li>";
            }
        ?>
      </ul>
      <div class="cl"></div> <!-- konec menu -->
        <?= render_body() ?>
      <div class="cl"></div> <!-- konec stranka -->
    </div>
  </body>
</html>
