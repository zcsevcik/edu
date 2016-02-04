<?php
/*
 * users.php:      Restaurace (IIS 2012)
 *
 * Author(s):      Marie Kratochvilova <xkrato11@stud.fit.vutbr.cz>
 *                 Radek Sevcik        <xsevci44@stud.fit.vutbr.cz>
 *
 * Date:           $Format:%aD$
 *
 * This file is part of iis12_restaurace.
 */

require_once 'config.inc.php';

define('XSLT_USERS', PATH_XSLT . '/users.xslt');

redirect_if_not_perm(role_supervisor());

function change_pwd($login, $pwd)
{
    $login = MyDB::escape($login);
    $pwd = hash('sha512', $pwd);
    $pwd = MyDB::escape($pwd);

    MyDB::getInstance()->exec("UPDATE uzivatele SET heslo='{$pwd}' WHERE login='{$login}';");
    return 0;
}

function change_name($login, $name)
{
    $login = MyDB::escape($login);
    $name = MyDB::escape($name);

    MyDB::getInstance()->exec("UPDATE uzivatele SET jmeno='{$name}' WHERE login='{$login}';");
    return 0;
}

function change_mail($login, $mail)
{
    if (!preg_match('/^[a-zA-Z0-9_.-]+@[a-zA-Z0-9-]+.[a-zA-Z0-9-.]+$/', $mail))
        return 1;

    $login = MyDB::escape($login);
    $mail = MyDB::escape($mail);

    MyDB::getInstance()->exec("UPDATE uzivatele SET mail='{$mail}' WHERE login='{$login}';");
    return 0;
}

function change_phone($login, $phone)
{
    $login = MyDB::escape($login);
    $phone = MyDB::escape($phone);

    MyDB::getInstance()->exec("UPDATE uzivatele SET telefon='{$phone}' WHERE login='{$login}';");
    return 0;
}

function change_role($login, $role)
{
    $login = MyDB::escape($login);
    $role = MyDB::escape($role);

    MyDB::getInstance()->exec("UPDATE uzivatele SET role='{$role}' WHERE login='{$login}';");
    return 0;
}

function user_create($login)
{
    $login = MyDB::escape($login);

    MyDB::getInstance()->exec("INSERT INTO uzivatele (login, heslo) VALUES ('{$login}', '');");
    return 0;
}

function user_delete($login)
{
    $login = MyDB::escape($login);

    MyDB::getInstance()->exec("DELETE FROM uzivatele WHERE login='{$login}';");
    return 0;
}

function getXmlUsers()
{
    $d = new DomDocument('1.0', 'utf-8');
    $root_e = $d->createElement('users');

    foreach(MyDB::getInstance()->getResults("SELECT id, login, jmeno, telefon, mail, role FROM uzivatele;") as $user) {
        $user_e = $d->createElement('user');
        $user_e->setAttribute('id', $user['id']);
        $user_e->setAttribute('login', $user['login']);
        $user_e->setAttribute('jmeno', $user['jmeno']);
        $user_e->setAttribute('telefon', $user['telefon']);
        $user_e->setAttribute('mail', $user['mail']);
        $user_e->setAttribute('role', $user['role']);

        $root_e->appendChild($user_e);
    }

    $d->appendChild($root_e);
    return $d->saveXML();
}

function render_head()
{
    return <<<EOL
    <style media="screen" type="text/css">
        .pwd {
            width: 200px;
            margin-top: 5px;
        }
        .spacer {
            height: 1em;
        }
    </style>
EOL;
}

function render_body()
{
    $u = xmltransform(getXmlUsers(), XSLT_USERS);

    return <<<EOL
    <script type="text/javascript">
      function make_blank_pwd(element) {
        element.style.color = '';
        element.style.fontStyle = '';
        element.value = '';
        element.type = 'password';
        element.onfocus = '';
      }

      function createXmlHttpRequestObject() {
        var xmlhttp;
        try {
            xmlHttp = new XMLHttpRequest(); //should work on all browsers except IE6 or older
        } catch (e) {
            try {
                xmlHttp = new ActiveXObject("Microsoft.XMLHttp"); //browser is IE6 or older
            } catch (e) {
            // ignore error
            }
        }
        if (!xmlHttp) {
            alert ("Error creating the XMLHttpRequest object.");
        } else {
            return xmlHttp;
        }
      }

      function change_name(element) {
        try {
            var xmlhttp = createXmlHttpRequestObject();
            xmlhttp.open('GET', 'users.php?a=change_name&uid=' + element.name + '&val=' + element.value);
            xmlhttp.send(null);
        }
        catch (e) {
            alert(e.toString());
        }

        return false;
      }

      function change_mail(element) {
        try {
            var xmlhttp = createXmlHttpRequestObject();
            xmlhttp.open('GET', 'users.php?a=change_mail&uid=' + element.name + '&val=' + element.value);
            xmlhttp.send(null);
        }
        catch (e) {
            alert(e.toString());
        }

        return false;
      }

      function change_phone(element) {
        try {
            var xmlhttp = createXmlHttpRequestObject();
            xmlhttp.open('GET', 'users.php?a=change_phone&uid=' + element.name + '&val=' + element.value);
            xmlhttp.send(null);
        }
        catch (e) {
            alert(e.toString());
        }

        return false;
      }

      function change_role(element) {
        try {
            var xmlhttp = createXmlHttpRequestObject();
            xmlhttp.open('GET', 'users.php?a=change_role&uid=' + element.name + '&val=' + element.value);
            xmlhttp.send(null);
        }
        catch (e) {
            alert(e.toString());
        }

        return false;
      }

      function delete_user(element) {
        if (confirm("Skutečně chcete vymazat účet '" + element.name + "'"));
          window.location = 'users.php?a=deluid&uid=' + element.name;
        return false;
      }

      function new_user(element) {
        window.location = 'users.php?a=newuid&val=' + element.value;
        return false;
      }

      function new_pwd(element) {
        val = prompt('Zadejte implicitní heslo', '');
        if (val == null || val == '')
            return;

        window.location = 'users.php?a=newpwd&uid=' + element.name + '&val=' + val;
        return false;
      }
    </script>

    <h1>Správa uživatelů</h1>
    <div style="border:#412617 solid 1px;padding:8px 10px 4px">
      {$u}
    </div>
EOL;
}

if (isset($_GET['a']) && ($_GET['a'] == 'change_name')) {
    if (empty($_GET['val']) || !($val = trim($_GET['val'])))
        return;
    if (empty($_GET['uid']) || !($login = trim($_GET['uid'])))
        return;

    strenc_todb($val);
    echo change_name($login, $val);
    exit;
}
elseif (isset($_GET['a']) && ($_GET['a'] == 'change_mail')) {
    if (empty($_GET['val']) || !($val = trim($_GET['val'])))
        return;
    if (empty($_GET['uid']) || !($login = trim($_GET['uid'])))
        return;

    strenc_todb($val);
    echo change_mail($login, $val);
    exit;
}
elseif (isset($_GET['a']) && ($_GET['a'] == 'change_phone')) {
    if (empty($_GET['val']) || !($val = trim($_GET['val'])))
        return;
    if (empty($_GET['uid']) || !($login = trim($_GET['uid'])))
        return;

    strenc_todb($val);
    echo change_phone($login, $val);
    exit;
}
elseif (isset($_GET['a']) && ($_GET['a'] == 'change_role')) {
    if (empty($_GET['val']) || !($val = trim($_GET['val'])))
        return;
    if (empty($_GET['uid']) || !($login = trim($_GET['uid'])))
        return;

    strenc_todb($val);
    echo change_role($login, $val);
    exit;
}
elseif (isset($_GET['a']) && ($_GET['a'] == 'deluid')) {
    if (empty($_GET['uid']) || !($login = trim($_GET['uid'])))
        return;

    strenc_todb($val);
    user_delete($login);
    header('Location: users.php');
    exit;
}
elseif (isset($_GET['a']) && ($_GET['a'] == 'newuid')) {
    if (empty($_GET['val']) || !($login = trim($_GET['val'])))
        return;

    strenc_todb($val);
    user_create($login);
    header('Location: users.php');
    exit;
}
elseif (isset($_GET['a']) && ($_GET['a'] == 'newpwd')) {
    if (empty($_GET['uid']) || !($login = trim($_GET['uid'])))
        return;
    if (empty($_GET['val']) || !($pwd = trim($_GET['val'])))
        return;

    strenc_todb($val);
    change_pwd($login, $pwd);
    header('Location: users.php');
    exit;
}

include_once 'mainpage.php';

?>
