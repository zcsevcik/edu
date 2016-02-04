<?php
/*
 * profil.php:     Sprava hostingu (ITU 2012)
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
    <form>
      <div class="prof">
      {$_SESSION['company']}<br/>
      {$_SESSION['name']} {$_SESSION['surname']}<br/>
        <ul>
          <li>
            <div class="p_tnew">nove heslo:</div>
            <input class="p_new" type="password" value="jmeno" />
          </li>
          <li>
            <div class="p_trep">zopakovat:</div>
            <input class="p_rep" type="password" value="jmeno" />
          </li>
          <li>
            <div class="p_told">stare heslo:</div>
            <input class="p_old" type="password" value="jmeno" />
          </li>
        </ul>
      </div>
      </form>
EOL;
}

include_once 'mainpage.php';
?>
