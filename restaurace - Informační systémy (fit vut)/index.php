<?php
/*
 * index.php:      Restaurace (IIS 2012)
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
    <h1>Vítejte v restauraci U Smradlavé fusekle!</h1>
    <p>Naše restaurace patří mezi uplnou novinku. Byla otevřena 10. prosince 2012. Její interiéry zdobí luxusní interiery, malby ponožek z 90.let minulého století.</p>
    <p>Rasturace se chlubí svou letní zahrádkou, v jejímž středě stojí nádherný bílí dub. Na tomto místě najde, každá spřízněná duše pokoj a mír, ale hlavně únik před letním žárem sluníčka.</p>
    <p>Kromě několika hodového menu nabízíme i luxusní vína a piva všeho druhu.</p>
    <p>Resturace je dobře dostupná a nachází se v centru Brna.</p>
EOL;
}

include_once 'mainpage.php';
?>
