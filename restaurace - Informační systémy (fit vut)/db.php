<?php
/*
 * db.php:         Restaurace (IIS 2012)
 *
 * Author(s):      Marie Kratochvilova <xkrato11@stud.fit.vutbr.cz>
 *                 Radek Sevcik        <xsevci44@stud.fit.vutbr.cz>
 *
 * Date:           $Format:%aD$
 *
 * This file is part of iis12_restaurace.
 */

require_once 'config.inc.php';

define('SQLITEDB', PATH_DB . '/iis.db3');

class MyDB {
    private static $instance;

    private $sql = NULL;

    private function __construct()
    {
        $this->sql = new SQLite3(SQLITEDB);
        $this->setPragmas();
    }

    /*
    private function __destruct()
    {
        $this->sql->close();
    }
    */

    public static function getInstance()
    {
        if (!self::$instance)
            self::$instance = new self();

        return self::$instance;
    }

    public static function escape($s)
    {
        return SQLite3::escapeString($s);
    }

    private function setPragmas()
    {
        $pragmas = array(
            'foreign_keys' => 'ON',
            'encoding' => 'UTF-8',
        );

        foreach ($pragmas as $key => $val) {
            $this->sql->exec("PRAGMA $key = \"$val\";");
        }

        return;
    }

    public static function getQuery($filename)
    {
        return file_get_contents(PATH_SQL . "/$filename.sql");
    }

    public function getResults($query)
    {
        $res = $this->sql->query($query);
        $return = Array();

        while($r = $res->fetchArray(SQLITE3_ASSOC))
        {
            $return[] = $r;
        }

        return $return;
    }

    public function getParamResults($query, $param)
    {
        $stmt = $this->sql->prepare($query);
        foreach ($param as $key => $val) {
            $stmt->bindValue($key, self::escape($val));
        }

        $res = $stmt->execute();
        $return = Array();

        while($r = $res->fetchArray(SQLITE3_ASSOC))
        {
            $return[] = $r;
        }

        $stmt->close();
        return $return;
    }

    public function exec($query, &$row_id = NULL)
    {
        $f = $this->sql->exec($query);
        if ($f) {
            $row_id = $this->sql->lastInsertRowID();
        }

        return $f;
    }

    public function getFreeSpots($id,$datum)
    {
        $res = $this->sql->query('SELECT SUM(pocet_lidi_u_stolu) AS zabrano, id_stolu FROM rezervace WHERE datum = datetime("'.$datum.'") AND id_stolu = '.$id);

        $a = $res->fetchArray(SQLITE3_ASSOC);
        if($a['zabrano']) return $a['zabrano'];
        else return 0;
    }

    public function insertReservation($id_table, $date, $ppl, $name)
    {
        $this->sql->query('INSERT INTO "rezervace" ("id_stolu","datum","pocet_lidi_u_stolu","jmeno","stav") VALUES ("'.$id_table.'",datetime("'.$date.'"),"'.$ppl.'","'.$name.'","nova")');
    }

    public function authenticate($uname, $upwd)
    {
        $uname = self::escape($uname);
        $upwd = hash('sha512', $upwd);
        $upwd = self::escape($upwd);
        return $this->sql->querySingle("SELECT id, jmeno, telefon, mail, role, login FROM uzivatele " .
                                       "WHERE login='{$uname}' AND heslo='{$upwd}';", true);
    }

    public function change_passwd($uname, $oldpwd, $newpwd)
    {
        $uname = self::escape($uname);
        $oldpwd = hash('sha512', $oldpwd);
        $oldpwd = self::escape($oldpwd);
        $newpwd = hash('sha512', $newpwd);
        $newpwd = self::escape($newpwd);

        // exec vrati pokazde TRUE, ikdyz se nic nezmenilo
        $q = $this->getParamResults("SELECT login FROM uzivatele " .
                                    "WHERE LOGIN=:login AND heslo=:pwd;",
                                    array(':login' => $uname, ':pwd' => $oldpwd));

        $this->exec("UPDATE uzivatele SET heslo='{$newpwd}' " .
                    "WHERE login='{$uname}' AND heslo='{$oldpwd}';");

        return !empty($q);
    }

    public function errno(&$errmsg)
    {
        $errmsg = $this->sql->lastErrorMsg();
        return $this->sql->lastErrorCode();
    }

    public function initDb()
    {
        $this->sql->exec('PRAGMA foreign_keys = OFF;');

        $drop = $this->getQuery('drop');
        $this->sql->exec($drop);

        $schema = $this->getQuery('create');
        $this->sql->exec($schema);

        $data = $this->getQuery('testdata');
        $this->sql->exec($data);
    }
}

?>
