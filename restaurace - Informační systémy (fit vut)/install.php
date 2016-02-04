<?php
/*
 * install.php:    Restaurace (IIS 2012)
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

}

function render_body()
{

}

MyDB::getInstance()->initDb();
header('Location: index.php');
?>
