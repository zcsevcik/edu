-- Datum: 2012/04/21
-- Autor: Radek Sevcik, xsevci44@stud.fit.vutbr.cz
-- Fakulta Informacnich Technologii, Vysoke Uceni Technicke v Brne

-- Projekt: Databazove systemy
-- Zadani:  Evidence ridicu a motorovych vozidel #36


CREATE OR REPLACE
PROCEDURE smaz_ridice(id INTEGER)
IS
BEGIN
  DELETE FROM ridic
  WHERE ridic.id = id;
END;
