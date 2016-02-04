<?php
/*
 * kontakt.php:    Restaurace (IIS 2012)
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
    return <<<EOL
    <h1>Kontaktní informace</h1>
    <p><strong>U Smradlavé fusekle</strong> <br>
    Náměstí svobody 525<br>
    691 00 - Brno<br></p>
    <p>Tel.: 327 569 852<br>
    E-mail: usmradlavefusekle@usmradalvefusekle.eu</p>

    <h1> Otevírací doba</h1>
    <table>
    <tr><td>Pondělí:&nbsp;&nbsp;</td><td>11:30 - 02:00</td></tr>
    <tr><td>úterý:&nbsp;&nbsp; </td><td>11:30 - 02:00</td></tr>
    <tr><td>Středa:&nbsp;&nbsp; </td><td>11:30 - 02:00</td></tr>
    <tr><td>čtvrtek:&nbsp;&nbsp; </td><td>11:30 - 02:00</td></tr>
    <tr><td>Pátek:&nbsp;&nbsp; </td><td>11:30 - 02:00</td></tr>
    <tr><td>Sobota:&nbsp;&nbsp; </td><td>13:30 - 04:00</td></tr>
    <tr><td>Neděle:&nbsp;&nbsp; </td><td>13:30 - 04:00</td></tr>
    </table>

EOL;
}

include_once 'mainpage.php';
?>

