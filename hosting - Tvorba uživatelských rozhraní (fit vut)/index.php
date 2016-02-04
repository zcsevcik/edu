<?php
/*
 * index.php:     Sprava hostingu (ITU 2012)
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

function render_head()
{
}

function render_body()
{
    return <<<EOL
    <div class="ftp">
      Ví­tejte na stránkách firmy LOGO, poskytujicí hosting.
    </div>
EOL;
}

include_once 'mainpage.php';
?>
