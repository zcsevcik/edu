<?php
/*
 * objednavky.php:  Restaurace (IIS 2012)
 *
 * Author(s):      Marie Kratochvilova <xkrato11@stud.fit.vutbr.cz>
 *                 Radek Sevcik        <xsevci44@stud.fit.vutbr.cz>
 *
 * Date:           $Format:%aD$
 *
 * This file is part of iis12_restaurace.
 */

require_once 'config.inc.php';

redirect_if_not_perm(role_supervisor() || role_chief() || role_waiter());

function render_head()
{
    return <<<EOL
    <link href="css/rezervace.css" rel="stylesheet" type="text/css">
    <link href="css/jquery.dataTables.css" rel="stylesheet" type="text/css">
    <link rel="stylesheet" href="http://code.jquery.com/ui/1.9.2/themes/base/jquery-ui.css" />
    <script src="//ajax.googleapis.com/ajax/libs/jquery/1.8.3/jquery.min.js"></script>
    <script src="//ajax.googleapis.com/ajax/libs/jqueryui/1.9.2/jquery-ui.min.js"></script>
    <script src="js/jquery.dataTables.min.js"></script>
    <script>
    	$(document).ready(function()
    	{
  			$('.datatable').dataTable({
				"bStateSave": true,
			});
		});
    </script>
EOL;
}

function render_body()
{
	if($_GET['action'] == 'pridej-objednavku')
	{
		echo '
		<form action="objednavky.php?action=pridej-objednavku2" method="POST">
			<select name="stul">
				'.getTables().'
			</select> Stul<br />
			Jidlo<br />
			<select name="jidlo">
				'.getFood().'
			</select>
			<input type="text" name="mnozstvi" size="3" value="1">x pocet kusu<br />
			<input type="submit" value="Pridej jidlo">
		</form>';
	}
	elseif($_GET['action'] == 'pridej-objednavku2')
	{
		if(is_numeric($_POST['mnozstvi']))
		{
			$jidlo = MyDB::getInstance()->getResults('SELECT * FROM jidelni_listek WHERE id = '.MyDB::escape($_POST['jidlo']));
			$cena = $jidlo[0]['cena'];
			for ($i=1; $i <= $_POST['mnozstvi'] ; $i++)
			{
				MyDB::getInstance()->exec('INSERT INTO objednavka ("id_jidla","id_stolu","datum","cena") VALUES ('.MyDB::escape($_POST['jidlo']).', '.MyDB::escape($_POST['stul']).', DATETIME("now"), "'.$cena.'")');
				//echo 'INSERT INTO objednavka ("id_jidla","id_stolu","datum","cena") VALUES ('.MyDB::escape($_POST['jidlo']).', '.MyDB::escape($_POST['stul']).', DATETIME("now"), "'.$cena.'")';
			}

			echo '<meta http-equiv="refresh" content="0;URL=\'objednavky.php\'">';
		}
		else echo 'Zadejte platné množství!';
	}
	elseif($_GET['action'] == 'zaplatit')
	{
		MyDB::getInstance()->exec('UPDATE objednavka SET zaplaceno = 1 WHERE id = '.MyDB::escape($_GET['id']));
		echo '<meta http-equiv="refresh" content="0;URL=\'objednavky.php\'">';
	}
	elseif($_GET['action'] == 'zrusit')
	{
		MyDB::getInstance()->exec('DELETE FROM objednavka WHERE id = '.MyDB::escape($_GET['id']));
		echo '<meta http-equiv="refresh" content="0;URL=\'objednavky.php\'">';
	}
	else
	{
		return '
	    <h1>Nezaplacené objednávky</h1>
	    <table class="datatable">
	    	<thead>
	    		<tr>
	    			<th>Stůl (salónek)</th>
	    			<th>Jídlo</th>
	    			<th>Čas</th>
	    			<th>Cena</th>
	    			<th>Možnosti</th>
	    		</tr>
	    	</thead>
	    	<tbody>
	    		'.displayOrders('neplacene').'
	    	</tbody>
	    </table>

	    <br /><br />
		<p><a href="objednavky.php?action=pridej-objednavku"><img src="images/add.png" />Pridej jidlo</a></p>

		<h1>Zaplacené objednávky</h1>
		<table class="datatable">
	    	<thead>
	    		<tr>
	    			<th>Stůl (salónek)</th>
	    			<th>Jídlo</th>
	    			<th>Čas</th>
	    			<th>Cena</th>
	    			<th>Možnosti</th>
	    		</tr>
	    	</thead>
	    	<tbody>
	    		'.displayOrders('placene').'
	    	</tbody>
	    </table>
	    ';
	}
}

function displayOrders($type)
{
	if($type == 'neplacene') $zaplaceno = 0;
	else $zaplaceno = 1;

	$res = MyDB::getInstance()->getResults('SELECT O.id, O.datum, O.cena, J.nazev, S.cislo_stolu, SAL.nazev  AS nazev_salonku
FROM objednavka O
JOIN jidelni_listek J ON O.id_jidla = J.id
JOIN stoly S ON O.id_stolu = S.id
JOIN salonky SAL ON S.id_salonku = SAL.id
WHERE O.zaplaceno = '.$zaplaceno);
	$result = '';

	foreach ($res as $v)
	{
		$result .= '<tr>
						<td>'.$v['cislo_stolu'].' ('.strenc_topage($v['nazev_salonku']).')</td>
						<td>'.strenc_topage($v['nazev']).'</td>
						<td>'.$v['datum'].'</td>
						<td>'.$v['cena'].',-</td>
						<td>
							<a href="objednavky.php?action=zrusit&id='.$v['id'].'"><img src="images/cross.png"></a>
							<a href="objednavky.php?action=zaplatit&id='.$v['id'].'"><img src="images/accept.png"></a>
						</td>
					</tr>';
	}

	return $result;
}

function getTables()
{
	$res = MyDB::getInstance()->getResults('SELECT * FROM salonky ORDER BY nazev');
	$result = '';

	foreach ($res as $v)
	{
		$result .= '<optgroup label="'.strenc_topage($v['nazev']).'">';

		$res2 = MyDB::getInstance()->getResults('SELECT * FROM stoly WHERE id_salonku = '.$v['id']);
		foreach ($res2 as $v2)
		{
			$result .= '<option value="'.$v2['id'].'">'.$v2['cislo_stolu'].'</option>';
		}

		$result .= '</optgroup>';
	}

	return $result;
}

function getFood()
{
	$res = MyDB::getInstance()->getResults('SELECT * FROM kategorie ORDER BY nazev');
	$result = '';

	foreach ($res as $v)
	{
		$result .= '<optgroup label="'.strenc_topage($v['nazev']).'">';

		$res2 = MyDB::getInstance()->getResults('SELECT * FROM jidelni_listek WHERE viditelnost = 1 AND id_kategorie = '.$v['id']);
		foreach ($res2 as $v2)
		{
			$result .= '<option value="'.$v2['id'].'">'.strenc_topage($v2['nazev']).'</option>';
		}

		$result .= '</optgroup>';
	}

	return $result;
}


include_once 'mainpage.php';
?>
