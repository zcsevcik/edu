<?php
/*
 * login.php:      Sprava hostingu (ITU 2012)
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
session_start();

define('XMLDB_USERS', PATH_XMLDB . '/passwd.xml');

$action = isset($_GET['unlog']) ? 'unlog' : 'log';

if ($action == 'unlog') {
    unset($_SESSION['user']);
    unset($_SESSION['role']);
    unset($_SESSION['name']);
    unset($_SESSION['surname']);
    unset($_SESSION['company']);
    session_destroy();
}
else if ($action == 'log' && isset($_POST['uname']) && isset($_POST['upwd'])) {
    $uname = $_POST['uname'];
    $upwd = hash('sha512', $_POST['upwd']);

    if (!preg_match('/^[-._@0-9a-zA-Z]+$/', $uname))
        die('invalid username');

    $doc = new DomDocument();
    $file = realpath(XMLDB_USERS);
    if ($file && $doc->load($file)) {
        $xpath = new DomXPath($doc);
        $query = @"//user[@login='${uname}' and "
                        ."normalize-space()='${upwd}']";
        $pwd_list = $xpath->query($query);

        if ($pwd_list->length == 1) {
            $pwd_item = $pwd_list->item(0);
            $login = $xpath->query('@login', $pwd_item)->item(0)->value;
            $role = $xpath->query('@role', $pwd_item)->item(0)->value;
            $name = $xpath->query('@name', $pwd_item)->item(0)->value;
            $surname = $xpath->query('@surname', $pwd_item)->item(0)->value;
            $company = $xpath->query('@company', $pwd_item)->item(0)->value;

            assert('$login');
            assert('$role');

            strenc_topage($name);
            strenc_topage($surname);
            strenc_topage($company);

            session_regenerate_id(true);
            $_SESSION['user'] = $login;
            $_SESSION['role'] = $role;
            $_SESSION['name'] = $name;
            $_SESSION['surname'] = $surname;
            $_SESSION['company'] = $company;
        }
    }
}

header('Location: index.php');
exit;

?>
