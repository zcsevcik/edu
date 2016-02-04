<?php
/*
 * dns.php:     Sprava hostingu (ITU 2012)
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

define('XMLDB_DNS', PATH_XMLDB . "/dns.xml");
define('XSLT_DNS', PATH_XSLT . "/dns.xslt");

function change_domain($id, $val)
{
    $xmldom = DomDocument::load(XMLDB_DNS);
    $xpath = new DomXPath($xmldom);
    $query = @"//domain[@id={$id}]";

    $domain = $xpath->query($query);
    if ($domain->length != 1) {
        return 1;
    }

    $domain = $domain->item(0);
    strenc_todb($val);
    $domain->setAttribute('name', $val);

    if (FALSE == $xmldom->save(XMLDB_DNS))
        return 1;

    return 0;
}

function change_domain_name($id, $val)
{
    $xmldom = DomDocument::load(XMLDB_DNS);
    $xpath = new DomXPath($xmldom);
    $query = @"//record[@id={$id}]";

    $domain = $xpath->query($query);
    if ($domain->length != 1) {
        return 1;
    }

    $domain = $domain->item(0);
    strenc_todb($val);
    $domain->setAttribute('name', $val);

    if (FALSE == $xmldom->save(XMLDB_DNS))
        return 1;

    return 0;
}

function change_domain_4($id, $val)
{
    $xmldom = DomDocument::load(XMLDB_DNS);
    $xpath = new DomXPath($xmldom);
    $query = @"//record[@id={$id}]";

    if (ip2long($val) == FALSE)
        return 1;

    $domain = $xpath->query($query);
    if ($domain->length != 1) {
        return 1;
    }

    $domain = $domain->item(0);
    $domain->setAttribute('a', $val);

    if (FALSE == $xmldom->save(XMLDB_DNS))
        return 1;

    return 0;
}

function change_domain_6($id, $val)
{
    $xmldom = DomDocument::load(XMLDB_DNS);
    $xpath = new DomXPath($xmldom);
    $query = @"//record[@id={$id}]";

    if (ip2long($val) != FALSE)
        return 1;
    if (inet_pton($val) == FALSE)
        return 1;


    $domain = $xpath->query($query);
    if ($domain->length != 1) {
        return 1;
    }

    $domain = $domain->item(0);
    $domain->setAttribute('aaaa', $val);

    if (FALSE == $xmldom->save(XMLDB_DNS))
        return 1;

    return 0;
}

function render_head()
{
    return <<<EOL

EOL;
}

function render_body()
{
    $content = <<<EOL
        <script type="text/javascript">
        function createXmlHttpRequestObject() {
            var xmlhttp;
            try {
                xmlhttp = new XMLHttpRequest(); //should work on all browsers except IE6 or older
            } catch (e) {
                try {
                    xmlhttp = new ActiveXObject("Microsoft.XMLHttp"); //browser is IE6 or older
                } catch (e) {
                // ignore error
                }
            }
            if (!xmlhttp) {
                alert ("Error creating the XMLHttpRequest object.");
            } else {
                return xmlhttp;
            }
        }

        function dom4_changed() {
            if ((xmlhttp.readyState == 4) && (xmlhttp.status == 200)) {
                if (xmlhttp.responseText != '0')
                    alert('Chyba');
            }
        }

        function change_domain(element) {
            try {
                var xmlhttp = createXmlHttpRequestObject();
                xmlhttp.open('GET', 'dns.php?action=chng_dom&id=' + element.name + '&val=' + element.value);
                xmlhttp.send(null);
            }
            catch (e) {
                alert(e.toString());
            }

            return false;
        }

        function change_domain_name(element) {
            try {
                var xmlhttp = createXmlHttpRequestObject();
                xmlhttp.open('GET', 'dns.php?action=chng_name&id=' + element.name + '&val=' + element.value);
                xmlhttp.send(null);
            }
            catch (e) {
                alert(e.toString());
            }

            return false;
        }

        function change_domain_4(element) {
            try {
                var xmlhttp = createXmlHttpRequestObject();
                xmlhttp.open('GET', 'dns.php?action=chng_4&id=' + element.name + '&val=' + element.value);
                xmlhttp.onreadystatechanged = dom4_changed;
                xmlhttp.send(null);
            }
            catch (e) {
                alert(e.toString());
            }

            return false;
        }

        function change_domain_6(element) {
            try {
                var xmlhttp = createXmlHttpRequestObject();
                xmlhttp.open('GET', 'dns.php?action=chng_6&id=' + element.name + '&val=' + element.value);
                xmlhttp.send(null);
            }
            catch (e) {
                alert(e.toString());
            }

            return false;

        }
        </script>
EOL;

    return $content . xmltransform(XMLDB_DNS, XSLT_DNS);
}

if (isset($_GET['action'])) {
    if ($_GET['action'] == 'chng_dom') {
        echo change_domain($_GET['id'], $_GET['val']);
    }
    else if ($_GET['action'] == 'chng_name') {
        echo change_domain_name($_GET['id'], $_GET['val']);
    }
    else if ($_GET['action'] == 'chng_4') {
        echo change_domain_4($_GET['id'], $_GET['val']);
    }
    else if ($_GET['action'] == 'chng_6') {
        echo change_domain_6($_GET['id'], $_GET['val']);
    }
    exit;
}
else {
    include_once 'mainpage.php';
}
?>
