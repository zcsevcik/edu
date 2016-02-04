<?php
/*
 * ftp.php:        Sprava hostingu (ITU 2012)
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

define('XMLDB_FTP', PATH_XMLDB . "/ftp.xml");
define('XSLT_FTP', PATH_XSLT . "/ftp.xslt");

function render_head()
{

}

function render_body()
{
    return xmltransform(XMLDB_FTP, XSLT_FTP);
}

include_once 'mainpage.php';
?>
