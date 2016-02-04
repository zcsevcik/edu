<?php
/*
 * mainpage.php:   Restaurace (IIS 2012)
 *
 * Author(s):      Marie Kratochvilova <xkrato11@stud.fit.vutbr.cz>
 *                 Radek Sevcik        <xsevci44@stud.fit.vutbr.cz>
 *
 * Date:           $Format:%aD$
 *
 * This file is part of iis12_restaurace.
 */

require_once 'config.inc.php';

define('XSLT_NEWS', PATH_XSLT . '/news.xslt');
define('SELECT_NEWS', 'select_news');

function getXmlNews()
{
    $d = new DomDocument('1.0', 'utf-8');
    $root_e = $d->createElement('news');
    foreach (MyDB::getInstance()->getResults(MyDB::getQuery(SELECT_NEWS)) as $row) {
        $item_e = $d->createElement('item', $row['obsah']);
        $item_e->setAttribute('date', $row['datum']);
        $root_e->appendChild($item_e);
    }
    $d->appendChild($root_e);
    return $d->saveXML();
}

function menu()
{
    $r = array(
        'Home' => 'index.php',
        'Jídelní lístek' => 'napojak.php',
        'Online Rezervace' => 'rezervace.php',
        'Kontakt'=> 'kontakt.php',
    );

    if (role_waiter() || role_supervisor())
        $r['Výpis rezervací'] = 'rezervace.php?action=vypis-rezervaci';

    if (role_chief() || role_waiter() || role_supervisor())
        $r['Objednávky'] = 'objednavky.php';

    if (role_supervisor())
        $r['Správa uživatelů'] = 'users.php';

    return $r;
}

function menu_b()
{
    $r = array(
        'Administrace' => 'login.php',
        'Home' => 'index.php',
    );

    if (isset($_SESSION['u_login']))
        $r['Odhlásit'] = 'login.php?action=unlog';

    return $r;
}

?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN""http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
    <title>IIS 2012 - Restaurace</title>
    <link href="css/style.css" rel="stylesheet" type="text/css">
    <?php echo render_head(); ?>
</head>
<body>
    <div id="page">
        <div id="contentwrap">
            <div id="sidebar">
                <div id="logo">U Smradlavé fusekle</div>
                <div id="menu">
                    <ul>
                        <?php
                        foreach(menu() as $key => $val) {
                            $act = basename($_SERVER['SCRIPT_NAME']) == $val;
                            echo '<li>';
                            if ($act) echo '&gt;&nbsp;';
                            echo "<a href=\"$val\">$key</a>";
                            if ($act) echo '&nbsp;&lt;';
                            echo '</li>';
                        }
                        ?>
                    </ul>
                </div>
                <div id="news">
                    <h2>Novinky</h2>
                    <?php echo xmltransform(getXmlNews(), XSLT_NEWS); ?>
                </div>
            </div>
            <div id="header"></div>
            <div id="body_area">
                <?php echo render_body(); ?>
            </div>
        </div>
        <div id="footer">
            <div class="top"></div>
            <div id="bottom_menu"> |
                <?php
                foreach(menu_b() as $key => $val) {
                    echo "<a href=\"$val\">$key</a> | ";
                }
                ?>
            </div>
            <div id="bottom_addr">
                2012 IIS | Restaurace<br/>
                Marie Kratochvílová | xkrato11 (at) stud.fit.vutbr.cz<br/>
                Radek Ševčík | xsevci44 (at) stud.fit.vutbr.cz<br/>
                Designed by <a href="http://anvisionwebtemplates.com/" target="_blank">Anvision Web Templates</a> &amp; <a href="http://webdesignfinders.net/" target="_blank">Webdesignfinders.net</a>
            </div>
            <div class="bottom"></div>
        </div>
    </div>
</body>
</html>
