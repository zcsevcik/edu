<?php
/*
 * login.php:      Restaurace (IIS 2012)
 *
 * Author(s):      Marie Kratochvilova <xkrato11@stud.fit.vutbr.cz>
 *                 Radek Sevcik        <xsevci44@stud.fit.vutbr.cz>
 *
 * Date:           $Format:%aD$
 *
 * This file is part of iis12_restaurace.
 */

require_once 'config.inc.php';

function render_head()
{
    return <<<EOL
    <style media="screen" type="text/css">
        .login {
            width: 200px;
            margin-top: 5px;
        }
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
    if (isset($_GET['f'])) {
        $errmsg = '<br/><p>Špatné jméno nebo heslo.</p>';
    }

    return <<<EOL
    <script type="text/javascript">
      function make_blank(element) {
        element.style.color = '';
        element.style.fontStyle = '';
        element.value = '';
        element.onfocus = '';
      }
      function make_blank_pwd(element) {
        element.style.color = '';
        element.style.fontStyle = '';
        element.value = '';
        element.type = 'password';
        element.onfocus = '';
      }
    </script>

    <h1>Toto je zona pouze pro registrovane!</h1>
    <p>Nepovolanym vstup zakazan!</p>
    <div style="border:#412617 solid 1px;padding:8px 10px 4px">
      <form method="post" action="https://{$_SERVER['SERVER_NAME']}{$_SERVER['SCRIPT_NAME']}?action=login">
      <input type="input" name="uname" value="login" class="login" onfocus="make_blank(this);" style="color: gray; font-style: italic;" /><br/>
      <input type="input" name="upwd" value="heslo" class="pwd" onfocus="make_blank_pwd(this);" style="color: gray; font-style: italic;" /><br/>
        <input type="submit" value="Přihlásit" style="margin-top: 5px;"/>
      </form>
      {$errmsg}
    </div>
EOL;
}

function unlog()
{
    unset($_SESSION['u_login']);
    unset($_SESSION['u_id']);
    unset($_SESSION['u_jmeno']);
    unset($_SESSION['u_telefon']);
    unset($_SESSION['u_mail']);
    unset($_SESSION['u_role']);
    session_destroy();
}

function login($uname, $upwd)
{
    if (!preg_match('/^[-._@0-9a-zA-Z]+$/', $uname))
        return false;

    $authz = MyDB::getInstance()->authenticate($uname, $upwd);
    if (empty($authz)) {
        return false;
    }

    session_regenerate_id(true);
    $_SESSION['u_login'] = $authz['login'];
    $_SESSION['u_id'] = $authz['id'];
    $_SESSION['u_jmeno'] = $authz['jmeno'];
    $_SESSION['u_telefon'] = $authz['telefon'];
    $_SESSION['u_mail'] = $authz['mail'];
    $_SESSION['u_role'] = $authz['role'];

    return true;
}

if (isset($_GET['action']) && $_GET['action'] == 'login') {
    if (login($_POST['uname'], $_POST['upwd'])) {
        header("Location: http://{$_SERVER['SERVER_NAME']}{$_SERVER['SCRIPT_NAME']}");
    }
    else {
        header("Location: http://{$_SERVER['SERVER_NAME']}{$_SERVER['SCRIPT_NAME']}?f");
    }
}
else if (isset($_GET['action']) && $_GET['action'] == 'unlog') {
    unlog();
    header("Location: http://{$_SERVER['SERVER_NAME']}{$_SERVER['SCRIPT_NAME']}");
}
else if (isset($_SESSION['u_login'])) {
    $admpage = dirname($_SERVER['SCRIPT_NAME']) . '/admin.php';
    header("Location: http://{$_SERVER['SERVER_NAME']}{$admpage}");
}
else {
    include_once 'mainpage.php';
}
?>
