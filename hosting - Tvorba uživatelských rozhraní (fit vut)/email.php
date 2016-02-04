<?php
/*
 * email.php:      Sprava hostingu (ITU 2012)
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

define('XMLDB_EMAIL', PATH_XMLDB . "/email.xml");
define('XSLT_EMAIL', PATH_XSLT . "/email.xslt");

function render_head()
{

}

function render_body()
{
    return xmltransform(XMLDB_EMAIL, XSLT_EMAIL);
}

include_once 'mainpage.php';
?>
