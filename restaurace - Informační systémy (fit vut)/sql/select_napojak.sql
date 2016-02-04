--
-- select_napojak.sql:  Restaurace (IIS 2012)
--
-- Author(s):      Marie Kratochvilova <xkrato11@stud.fit.vutbr.cz>
--                 Radek Sevcik        <xsevci44@stud.fit.vutbr.cz>
--
-- Date:           $Format:%aD$
--
-- This file is part of iis12_restaurace.
--

SELECT nazev, popis, cena, viditelnost as en, id
FROM jidelni_listek
WHERE id_kategorie = :kat_id
ORDER BY nazev, popis ASC;
