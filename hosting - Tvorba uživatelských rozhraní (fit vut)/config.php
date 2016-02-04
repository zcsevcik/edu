<?php
/*
 * config.php:     Sprava hostingu (ITU 2012)
 *
 * Author(s):      Jan Havlik     <xhavli30@stud.fit.vutbr.cz>
 *                 Vaclav Horazny <xhoraz01@stud.fit.vutbr.cz>
 *                 Radek Sevcik   <xsevci44@stud.fit.vutbr.cz>
 *
 * Date:           Sun, 16 Dec 2012 13:36:03 +0100
 *
 * This file is part of itu12_hosting.
 */

define('PATH_XMLDB', 'db');
define('PATH_XSLT', 'xslt');

date_default_timezone_set("Europe/Prague");
session_start();

function xmltransform($xml_f, $xslt_f)
{
    $xslt = new XSLTProcessor();
    $xmldoc = DomDocument::load($xml_f);
    $xsltdoc = DomDocument::load($xslt_f);

    $xslt->importStylesheet($xsltdoc);
    if (isset($_SESSION['user'])) {
        $xslt->setParameter("", "login", $_SESSION['user']);
    }
    else {
        $xslt->setParameter("", "login", "_nobody");
    }
    return $xslt->transformToXML($xmldoc);
}

function strenc_topage(&$val) {
    return $val = iconv('utf-8', 'iso-8859-2', $val);
}

function strenc_todb(&$val) {
    //return $val = iconv('iso-8859-2', 'utf-8', $val);
    return $val;
}

?>
