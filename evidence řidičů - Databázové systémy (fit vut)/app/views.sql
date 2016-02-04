-- Datum: 2012/04/21
-- Autor: Radek Sevcik, xsevci44@stud.fit.vutbr.cz
-- Fakulta Informacnich Technologii, Vysoke Uceni Technicke v Brne

-- Projekt: Databazove systemy
-- Zadani:  Evidence ridicu a motorovych vozidel #36


CREATE VIEW prestupky_ridice AS
SELECT ridic, paragraf, datum, body, popis, pozn FROM pokuta
INNER JOIN prestupek ON pokuta.prestupek = prestupek.id
ORDER BY datum DESC;


COMMIT;