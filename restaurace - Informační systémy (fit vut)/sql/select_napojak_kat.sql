--
-- select_napojak_kat.sql:      Restaurace (IIS 2012)
--
-- Author(s):      Marie Kratochvilova <xkrato11@stud.fit.vutbr.cz>
--                 Radek Sevcik        <xsevci44@stud.fit.vutbr.cz>
--
-- Date:           $Format:%aD$
--
-- This file is part of iis12_restaurace.
--

SELECT nazev, popis, id
FROM kategorie
ORDER BY id ASC;
