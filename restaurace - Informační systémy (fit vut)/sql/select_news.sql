--
-- select_news.sql:     Restaurace (IIS 2012)
--
-- Author(s):      Marie Kratochvilova <xkrato11@stud.fit.vutbr.cz>
--                 Radek Sevcik        <xsevci44@stud.fit.vutbr.cz>
--
-- Date:           $Format:%aD$
--
-- This file is part of iis12_restaurace.
--

SELECT datetime(datum, 'localtime') as 'datum', obsah FROM novinky
ORDER BY datum DESC
LIMIT 0,3;
