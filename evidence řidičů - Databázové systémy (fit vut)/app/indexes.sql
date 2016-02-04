-- Datum: 2012/04/21
-- Autor: Radek Sevcik, xsevci44@stud.fit.vutbr.cz
-- Fakulta Informacnich Technologii, Vysoke Uceni Technicke v Brne

-- Projekt: Databazove systemy
-- Zadani:  Evidence ridicu a motorovych vozidel #36


CREATE INDEX vin_idx ON vozidlo(vin);
CREATE INDEX spz_idx ON spz(cislo);
CREATE INDEX rc_idx ON ridic(rc);

COMMIT;
