<?php
/*
 * napojak.php:    Restaurace (IIS 2012)
 *
 * Author(s):      Marie Kratochvilova <xkrato11@stud.fit.vutbr.cz>
 *                 Radek Sevcik        <xsevci44@stud.fit.vutbr.cz>
 *
 * Date:           $Format:%aD$
 *
 * This file is part of iis12_restaurace.
 */

require_once 'config.inc.php';

define('XSLT_NAPOJAK', PATH_XSLT . '/napojak.xslt');
define('XSLT_NAPOJAK_EDIT', PATH_XSLT . '/napojak_edit.xslt');
define('SELECT_NAPOJAK', 'select_napojak');
define('SELECT_NAPOJAK_KAT', 'select_napojak_kat');

function getXmlNapojak()
{
    $d = new DomDocument('1.0', 'utf-8');
    $root_e = $d->createElement('napojak');

    foreach(MyDB::getInstance()->getResults(MyDB::getQuery(SELECT_NAPOJAK_KAT)) as $kat) {
        $kat_e = $d->createElement('kategorie');
        $kat_e->setAttribute('id', $kat['id']);
        $kat_e->setAttribute('nazev', $kat['nazev']);
        $kat_e->setAttribute('popis', $kat['popis']);

        $param = array(':kat_id' => $kat['id']);
        foreach (MyDB::getInstance()->getParamResults(MyDB::getQuery(SELECT_NAPOJAK), $param) as $row) {
            $item_e = $d->createElement('item');
            $item_e->setAttribute('id', $row['id']);
            $item_e->setAttribute('nazev', $row['nazev']);
            $item_e->setAttribute('popis', $row['popis']);
            $item_e->setAttribute('cena', $row['cena']);

            $en = (bool)$row['en'] ? "true" : "false";
            $item_e->setAttribute('en', $en);

            $kat_e->appendChild($item_e);
        }
        $root_e->appendChild($kat_e);
    }

    $d->appendChild($root_e);
    return $d->saveXML();
}

function change_visibility($id, $en)
{
    $param = array(
        ':id' => $id,
        ':en' => $en,
    );
    MyDB::getInstance()->getParamResults('UPDATE jidelni_listek SET viditelnost=:en WHERE id=:id;', $param);
    $errmsg = '';
    return MyDB::getInstance()->errno($errmsg);
}

function change_name($id, $val)
{
    $param = array(
        ':id' => $id,
        ':val' => $val,
    );
    MyDB::getInstance()->getParamResults('UPDATE jidelni_listek SET nazev=:val WHERE id=:id;', $param);
    $errmsg = '';
    return MyDB::getInstance()->errno($errmsg);
}

function change_desc($id, $val)
{
    $param = array(
        ':id' => $id,
        ':val' => $val,
    );
    MyDB::getInstance()->getParamResults('UPDATE jidelni_listek SET popis=:val WHERE id=:id;', $param);
    $errmsg = '';
    return MyDB::getInstance()->errno($errmsg);
}

function change_price($id, $val)
{
    $param = array(
        ':id' => $id,
        ':val' => $val,
    );
    MyDB::getInstance()->getParamResults('UPDATE jidelni_listek SET cena=:val WHERE id=:id;', $param);
    $errmsg = '';
    return MyDB::getInstance()->errno($errmsg);
}

function change_kat_name($id, $val)
{
    $param = array(
        ':id' => $id,
        ':val' => $val,
    );
    MyDB::getInstance()->getParamResults('UPDATE kategorie SET nazev=:val WHERE id=:id;', $param);
    $errmsg = '';
    return MyDB::getInstance()->errno($errmsg);
}

function change_kat_desc($id, $val)
{
    $param = array(
        ':id' => $id,
        ':val' => $val,
    );
    MyDB::getInstance()->getParamResults('UPDATE kategorie SET popis=:val WHERE id=:id;', $param);
    $errmsg = '';
    return MyDB::getInstance()->errno($errmsg);
}

function add_kat($val, &$row_id)
{
    $val = MyDB::escape($val);
    return MyDB::getInstance()->exec("INSERT INTO kategorie (nazev, popis) VALUES ( '$val', '' );", $row_id);
}

function add_item($val, $kat_id, &$row_id)
{
    $val = MyDB::escape($val);
    $kat_id = MyDB::escape($kat_id);
    return MyDB::getInstance()->exec("INSERT INTO jidelni_listek (id_kategorie, nazev, popis, cena, viditelnost) VALUES "
                                   . "( '$kat_id', '$val', '', 0, 0 );", $row_id);
}

function render_head()
{
    return <<<EOL
    <style media="screen" type="text/css">
        .spacer {
            height: 1em;
        }
    </style>
EOL;
}

function render_body()
{
    $content = '';
    if (isset($_SESSION['u_role'])) {
        $content .= <<<EOL
        <script type="text/javascript">
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

            function change_visibility(element) {
              try {
                var xmlhttp = createXmlHttpRequestObject();
                xmlhttp.open('GET', 'napojak.php?chng_vis=' + element.name + '&en=' + Number(element.checked));
                xmlhttp.send(null);
              }
              catch (e) {
                alert(e.toString());
              }

              return false;
            }

            function change_item_name(element) {
                try {
                    var xmlhttp = createXmlHttpRequestObject();
                    xmlhttp.open('GET', 'napojak.php?chng_name=' + element.name + '&val=' + element.value);
                    xmlhttp.send(null);
                }
                catch (e) {
                    alert(e.toString());
                }

                return false;
            }

            function change_item_desc(element) {
                try {
                    var xmlhttp = createXmlHttpRequestObject();
                    xmlhttp.open('GET', 'napojak.php?chng_desc=' + element.name + '&val=' + element.value);
                    xmlhttp.send(null);
                }
                catch (e) {
                    alert(e.toString());
                }

                return false;
            }

            function change_item_price(element) {
                try {
                    var xmlhttp = createXmlHttpRequestObject();
                    xmlhttp.open('GET', 'napojak.php?chng_price=' + element.name + '&val=' + element.value);
                    xmlhttp.send(null);
                }
                catch (e) {
                    alert(e.toString());
                }

                return false;
            }

            function change_kat_name(element) {
                try {
                    var xmlhttp = createXmlHttpRequestObject();
                    xmlhttp.open('GET', 'napojak.php?chng_kname=' + element.name + '&val=' + element.value);
                    xmlhttp.send(null);
                }
                catch (e) {
                    alert(e.toString());
                }

                return false;
            }

            function change_kat_desc(element) {
                try {
                    var xmlhttp = createXmlHttpRequestObject();
                    xmlhttp.open('GET', 'napojak.php?chng_kdesc=' + element.name + '&val=' + element.value);
                    xmlhttp.send(null);
                }
                catch (e) {
                    alert(e.toString());
                }

                return false;
            }


            function add_new_kat(element) {
                window.location = 'napojak.php?add_kat&val=' + element.value;
                return false;
            }

            function add_new_item(element, kat_id) {
                window.location = 'napojak.php?add_item&kat=' + kat_id + '&val=' + element.value;
                return false;
            }

        </script>
EOL;

        $content .= xmltransform(getXmlNapojak(), XSLT_NAPOJAK_EDIT);
    }
    else {
        $content .= xmltransform(getXmlNapojak(), XSLT_NAPOJAK);
    }

    return $content;
}

if (isset($_GET['chng_vis'])) {
    echo change_visibility($_GET['chng_vis'], $_GET['en']);
}
else if (isset($_GET['chng_name'])) {
    if (empty($_GET['val']) || !($val = trim($_GET['val'])))
        return;

    strenc_todb($val);
    echo change_name($_GET['chng_name'], $val);
}
else if (isset($_GET['chng_desc'])) {
    if (empty($_GET['val']) || !($val = trim($_GET['val'])))
        return;

    strenc_todb($val);
    echo change_desc($_GET['chng_desc'], $val);
}
else if (isset($_GET['chng_price'])) {
    if (empty($_GET['val']) || !($val = trim($_GET['val'])))
        return;

    strenc_todb($val);
    echo change_price($_GET['chng_price'], $val);
}
else if (isset($_GET['chng_kname'])) {
    if (empty($_GET['val']) || !($val = trim($_GET['val'])))
        return;

    strenc_todb($val);
    echo change_kat_name($_GET['chng_kname'], $val);
}
else if (isset($_GET['chng_kdesc'])) {
    if (empty($_GET['val']) || !($val = trim($_GET['val'])))
        return;

    strenc_todb($val);
    echo change_kat_desc($_GET['chng_kdesc'], $val);
}
else if (isset($_GET['add_kat'])) {
    if (empty($_GET['val']) || !($val = trim($_GET['val'])))
        return;

    $rid = 0;

    strenc_todb($val);
    add_kat($val, $rid);
    header("Location: napojak.php#k_$rid");
    exit;
}
else if (isset($_GET['add_item'])) {
    if (empty($_GET['val']) || !($val = trim($_GET['val'])))
        return;

    $rid = 0;

    strenc_todb($val);
    add_item($val, $_GET['kat'], $rid);
    header("Location: napojak.php#i_$rid");
    exit;
}
else {
    include_once 'mainpage.php';
}
?>
