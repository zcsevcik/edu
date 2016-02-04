<?php
/*
 * admin.php:      Restaurace (IIS 2012)
 *
 * Author(s):      Marie Kratochvilova <xkrato11@stud.fit.vutbr.cz>
 *                 Radek Sevcik        <xsevci44@stud.fit.vutbr.cz>
 *
 * Date:           $Format:%aD$
 *
 * This file is part of iis12_restaurace.
 */

require_once 'config.inc.php';

function change_pwd($oldpwd, $newpwd, $newpwd_r)
{
    if ($newpwd != $newpwd_r)
        return 1;

    return MyDB::getInstance()->change_passwd($_SESSION['u_login'], $oldpwd, $newpwd)
            == TRUE ? 0 : 1;
}

function render_head()
{
    return <<<EOL
    <style media="screen" type="text/css">
        .pwd {
            width: 200px;
            margin-top: 5px;
        }
    </style>
EOL;
}

function render_body()
{
    $errmsg = '';
    if (isset($_GET['f']))
        $errmsg = 'Chyba pri změně hesla.';
    elseif (isset($_GET['s']))
        $errmsg = 'Heslo bylo úspěšně změneno.';

    return <<<EOL
    <script type="text/javascript">
      function make_blank_pwd(element) {
        element.style.color = '';
        element.style.fontStyle = '';
        element.value = '';
        element.type = 'password';
        element.onfocus = '';
      }
    </script>

    <h1>Administrace</h1>
    <div style="border:#412617 solid 1px;padding:8px 10px 4px">
    <p>Přihlášen: <b>{$_SESSION['u_jmeno']}</b></p>
    <p>login: {$_SESSION['u_login']}</p>
    <p>role: {$_SESSION['u_role']}</p>
    <p>telefon: {$_SESSION['u_telefon']}</p>
    <p>email: <a href="mailto:{$_SESSION['u_mail']}">{$_SESSION['u_mail']}</a></p>
    </div>
    <br/>

    <h1>Změnit heslo</h1>
    <div style="border:#412617 solid 1px;padding:8px 10px 4px">
    <form method="post" action="https://{$_SERVER['SERVER_NAME']}{$_SERVER['SCRIPT_NAME']}?q=change_pwd">
      <input type="input" name="pwd_o" value="Stávající heslo" class="pwd" onfocus="make_blank_pwd(this);" style="color: gray; font-style: italic;" /><br/>
      <input type="input" name="pwd_1" value="Nové heslo" class="pwd" onfocus="make_blank_pwd(this);" style="color: gray; font-style: italic;" /><br/>
      <input type="input" name="pwd_2" value="Opakovat heslo" class="pwd" onfocus="make_blank_pwd(this);" style="color: gray; font-style: italic;" /><br/>
      <input type="submit" value="Potvrdit" style="margin-top: 5px;"/>
    </form>
    <p>{$errmsg}</p>
    </div>
EOL;
}

if (isset($_GET['q']) && $_GET['q'] == 'change_pwd') {
    if (empty($_POST['pwd_o']) || !($oldpwd = trim($_POST['pwd_o'])))
        return;
    if (empty($_POST['pwd_1']) || !($newpwd = trim($_POST['pwd_1'])))
        return;
    if (empty($_POST['pwd_2']) || !($newpwd_r = trim($_POST['pwd_2'])))
        return;

    if (0 == change_pwd($oldpwd, $newpwd, $newpwd_r))
        header('Location: admin.php?s');
    else
        header('Location: admin.php?f');
    exit;
}

include_once 'mainpage.php';

?>
