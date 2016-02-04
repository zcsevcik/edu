<?php
/*
 * config.inc.php: Restaurace (IIS 2012)
 *
 * Author(s):      Marie Kratochvilova <xkrato11@stud.fit.vutbr.cz>
 *                 Radek Sevcik        <xsevci44@stud.fit.vutbr.cz>
 *
 * Date:           $Format:%aD$
 *
 * This file is part of iis12_restaurace.
 */

define('PATH_DB', 'db');
define('PATH_DOC', 'doc');
define('PATH_SQL', 'sql');
define('PATH_XSLT', 'xslt');

date_default_timezone_set("Europe/Prague");
header('Content-Type: text/html; charset=utf-8');
session_start();

include_once 'db.php';
include_once 'roles.php';

function xmltransform($xml, $xslt_f)
{
    $xslt = new XSLTProcessor();
    $xmldoc = DomDocument::loadXML($xml, LIBXML_COMPACT);
    $xsltdoc = DomDocument::load($xslt_f, LIBXML_COMPACT);

    $xslt->importStylesheet($xsltdoc);
    return $xslt->transformToXML($xmldoc);
}

function strenc_todb(&$val) {
    return $val /*= iconv('iso-8859-2', 'utf-8', $val)*/;
}

function strenc_topage(&$val) {
    return $val /*= iconv('utf-8', 'iso-8859-2', $val)*/;
}

?>
