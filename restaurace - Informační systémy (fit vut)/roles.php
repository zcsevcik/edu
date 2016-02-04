<?php
/*
 * roles.php:      Restaurace (IIS 2012)
 *
 * Author(s):      Marie Kratochvilova <xkrato11@stud.fit.vutbr.cz>
 *                 Radek Sevcik        <xsevci44@stud.fit.vutbr.cz>
 *
 * Date:           $Format:%aD$
 *
 * This file is part of iis12_restaurace.
 */

function role_supervisor()
{
    return isset($_SESSION['u_role']) && ($_SESSION['u_role'] == 'admin');
}

function role_chief()
{
    return isset($_SESSION['u_role']) && ($_SESSION['u_role'] == 'sefkuchar');
}

function role_waiter()
{
    return isset($_SESSION['u_role']) && ($_SESSION['u_role'] == 'cisnik');
}

function role_customer()
{
    return !isset($_SESSION['u_role']);
}

function redirect_if_not_perm($b)
{
    if (!$b) {
        header("HTTP/1.1 404 Not Found");
        exit;
    }
}

?>
