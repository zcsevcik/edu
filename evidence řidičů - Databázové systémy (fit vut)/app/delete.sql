-- Datum: 2012/04/21
-- Autor: Radek Sevcik, xsevci44@stud.fit.vutbr.cz
-- Fakulta Informacnich Technologii, Vysoke Uceni Technicke v Brne

-- Projekt: Databazove systemy
-- Zadani:  Evidence ridicu a motorovych vozidel #36


DROP TABLE kradez;
DROP TABLE pokuta;
DROP TABLE spz;
DROP TABLE vozidlo;
DROP TABLE ridic;
DROP TABLE prestupek;
DROP VIEW prestupky_ridice;
DROP INDEX vin_idx;
DROP INDEX spz_idx;
DROP INDEX rc_idx;
DROP PROCEDURE smaz_ridice;
DROP SEQUENCE seq_ridic_id;
DROP SEQUENCE seq_pokuta_id;
DROP SEQUENCE seq_kradez_id;
DROP SEQUENCE seq_prestupek_id;

COMMIT;
