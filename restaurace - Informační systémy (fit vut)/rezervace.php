<?php
/*
 * rezervace.php:  Restaurace (IIS 2012)
 *
 * Author(s):      Marie Kratochvilova <xkrato11@stud.fit.vutbr.cz>
 *                 Radek Sevcik        <xsevci44@stud.fit.vutbr.cz>
 *
 * Date:           $Format:%aD$
 *
 * This file is part of iis12_restaurace.
 */

require_once 'config.inc.php';

if (isset($_GET['action']) && $_GET['action'] == 'vypis-rezervaci')
    redirect_if_not_perm(role_supervisor() || role_waiter());

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
  			$('.datepicker').datepicker();
  			$('.datepicker').datepicker("option", "dateFormat", 'yy-mm-dd');

  			$('.datatable').dataTable({
				"bStateSave": true,
			});
		});
    </script>
EOL;
}

function render_body()
{
	if($_GET['action'] == 'start')
	{
		echo '<h1>Start nové rezervace</h1>
		<p>Vyberte si, prosím, datum rezervace</p>
		<form action="rezervace.php?action=step2" method="POST">
			<input type="text" name="datum" class="datepicker" />
			<select name="hodina">';

			for ($i=8; $i <= 23; $i++)
			{
				echo '<option value="'.$i.'">'.$i.':00</option>';
			}

		echo '
			</select>
			<br /><br />Počet lidí na rezervaci:&nbsp;
			<select name="pocet_lidi">';

			for ($i=1; $i <= 5; $i++)
			{
				echo '<option value="'.$i.'">'.$i.'</option>';
			}

		echo '
			</select>
			<br /><br />
			Na jméno: <input type="text" name="name">&nbsp;
			<input type="submit" value="Pokračovat!">
		</form>';
	}
	elseif($_GET['action'] == 'step2')
	{
		if($_POST['hodina'] < 10) $hodina = '0'.$_POST['hodina'];
		else $hodina = $_POST['hodina'];
		$datum = MyDB::escape($_POST['datum'].' '.$hodina.':00:00');
		$pocet_lidi = MyDB::escape($_POST['pocet_lidi']);
		$_SESSION['datum'] = $datum;
		$_SESSION['pocet_lidi'] = $pocet_lidi;
		$_SESSION['jmeno'] = MyDB::escape($_POST['name']);

		echo '<h1>Vyber stolu</h1>
		<p>Vybrané datum: <strong>'.$datum.'</strong> pro <strong>'.$pocet_lidi.' lidi</strong> na jméno <strong>'.$_SESSION['jmeno'].'</strong></p>
		<p>Pro vyběr stolu na něj stačí kliknout, uvnitř čtverečku je uveden počet volných míst pro zvolený čas.</p>
		<h3>Salónky</h3>
			'.displayRooms($datum,$pocet_lidi).'
			';
	}
	elseif($_GET['action'] == 'step3')
	{
		MyDB::getInstance()->insertReservation(MyDB::escape($_GET['id_stolu']), $_SESSION['datum'], $_SESSION['pocet_lidi'], $_SESSION['jmeno']);
		echo '<p>Děkujeme, Vaše rezervace byla uspešně přidána!</p>';
	}
	elseif($_GET['action'] == 'potvrdit')
	{
		MyDB::getInstance()->exec('UPDATE rezervace SET stav = "potvrzeno" WHERE id = '.MyDB::escape($_GET['id']));
		echo '<meta http-equiv="refresh" content="0;URL=\'rezervace.php?action=vypis-rezervaci\'">';
	}
	elseif($_GET['action'] == 'zrusit')
	{
		MyDB::getInstance()->exec('DELETE FROM rezervace WHERE id = '.MyDB::escape($_GET['id']));
		echo '<meta http-equiv="refresh" content="0;URL=\'rezervace.php?action=vypis-rezervaci\'">';
	}
	elseif($_GET['action'] == 'vypis-rezervaci')
	{
		echo '<h1>Rezervace</h1>
		<table class="datatable">
			<thead>
				<tr><th>Jméno</th><th>Číslo stolu</th><th>Počet lidí</th><th>Čas</th><th>Stav</th><th>Možnosti</th></tr>
			</thead>
			<tbody>
				'.displayReservations().'
			</tbody>
		</table>
		<br /><br />
		<p><a href="rezervace.php?action=start">Chci rezervaci!</a></p>';
	}
	else
	{
		return '
	    <h1>Rozložení restaurace</h1>
	    <div class="telo">
	    <p>Ve čtverečcích je uvedeno číslo stolu pro telefonickou rezervaci.</p>
		<h3>Salónky</h3>
			'.displayRooms().'

		<h3>Legenda</h3>
		<div class="tabulka">
			<table>
				'.displayLegend().'
			</table>
		</div>
		<a href="rezervace.php?action=start">Chci rezervaci!</a>
	    </div>';
	}
}

function displayLegend()
{
	$res = MyDB::getInstance()->getResults('SELECT * FROM salonky ORDER BY nazev');
	$result = '';

	foreach ($res as $v)
	{
		$result .= '<tr><td class="legenda" style="background-color: '.$v['barva'].'"></td><td>'.strenc_topage($v['nazev']).'</td></tr>';
	}

	return $result;
}

function displayRooms($datum = '',$pocet_lidi = '')
{
	$res = MyDB::getInstance()->getResults('SELECT * FROM salonky ORDER BY nazev');
	$result = '';

	foreach ($res as $v)
	{
		$result .= strenc_topage($v['nazev']).'<div class="salonek" style="background-color: '.$v['barva'].'">';

		$res_stoly = MyDB::getInstance()->getResults('SELECT * FROM stoly WHERE id_salonku = '.$v['id']);
		foreach ($res_stoly as $stul)
		{
			if($datum)
			{
				$zabrana_mista = MyDB::getInstance()->getFreeSpots($stul['id'], $datum);
				$volna_mista = $stul['pocet_lidi'] - $zabrana_mista;

				if($pocet_lidi > $volna_mista) $result .= '<div class="stul disabled">'.$volna_mista.'</div>';
				else $result .= '<a href="rezervace.php?action=step3&id_stolu='.$stul['id'].'"><div class="stul" rel="'.$stul['id'].'">'.$volna_mista.'</div></a>';
			}
			else
			{
				$result .= '<div class="stul" rel="'.$stul['pocet_lidi'].'">'.$stul['cislo_stolu'].'</div>';
			}

		}

		$result .= '<div class="cleaner"></div></div>';
	}

	return $result;
}

function displayReservations()
{
	$res = MyDB::getInstance()->getResults('SELECT R.*, S.cislo_stolu FROM rezervace R JOIN stoly S ON R.id_stolu = S.id ORDER BY R.datum DESC');
	$result = '';

	foreach ($res as $v)
	{
		$result .= '<tr>
						<td>'.strenc_topage($v['jmeno']).'</td>
						<td>'.$v['cislo_stolu'].'</td>
						<td>'.$v['pocet_lidi_u_stolu'].'</td>
						<td>'.$v['datum'].'</td>
						<td>'.$v['stav'].'</td>
						<td>
							<a href="rezervace.php?action=zrusit&id='.$v['id'].'"><img src="images/cross.png"></a>
							<a href="rezervace.php?action=potvrdit&id='.$v['id'].'"><img src="images/accept.png"></a>
						</td>
					</tr>';
	}

	return $result;
}

include_once 'mainpage.php';
?>
