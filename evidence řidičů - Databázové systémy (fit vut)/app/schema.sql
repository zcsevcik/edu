-- Datum: 2012/04/21
-- Autor: Radek Sevcik, xsevci44@stud.fit.vutbr.cz
-- Fakulta Informacnich Technologii, Vysoke Uceni Technicke v Brne

-- Projekt: Databazove systemy
-- Zadani:  Evidence ridicu a motorovych vozidel #36


CREATE TABLE ridic (
id INTEGER NOT NULL,
jmeno VARCHAR(20) NOT NULL,
prijmeni VARCHAR(20) NOT NULL,
rc CHAR(11) NOT NULL,
obec VARCHAR(20) NOT NULL,
adresa VARCHAR(20) NOT NULL
);

CREATE TABLE vozidlo (
vin CHAR(17) NOT NULL,
registrace DATE NOT NULL
);

CREATE TABLE spz (
cislo CHAR(7) NOT NULL,
pridelena_od DATE NOT NULL,
pridelena_do DATE NULL,
vozidlo CHAR(17) NOT NULL,
majitel INTEGER NOT NULL
);

CREATE TABLE pokuta (
id INTEGER NOT NULL,
datum DATE NOT NULL,
ridic INTEGER NOT NULL,
prestupek INTEGER NOT NULL,
pozn VARCHAR(255)
);

CREATE TABLE prestupek (
id INTEGER NOT NULL,
paragraf VARCHAR(20) NOT NULL,
body INTEGER NOT NULL,
popis VARCHAR(100)
);

CREATE TABLE kradez (
id INTEGER NOT NULL,
vozidlo CHAR(17) NOT NULL,
misto VARCHAR(20) NOT NULL,
datum DATE NOT NULL,
pozn VARCHAR(255)
);


CREATE SEQUENCE seq_ridic_id;
ALTER TABLE ridic ADD CONSTRAINT PK_ridic PRIMARY KEY (id);
ALTER TABLE ridic ADD CONSTRAINT UK_ridic_rc UNIQUE (rc);

CREATE SEQUENCE seq_prestupek_id;
ALTER TABLE prestupek ADD CONSTRAINT PK_prestupek PRIMARY KEY (id);

CREATE SEQUENCE seq_pokuta_id;
ALTER TABLE pokuta ADD CONSTRAINT PK_pokuta PRIMARY KEY (id);
ALTER TABLE pokuta ADD CONSTRAINT FK_pokuta_ridic FOREIGN KEY (ridic) REFERENCES ridic ON DELETE CASCADE;
ALTER TABLE pokuta ADD CONSTRAINT FK_pokuta_prestupek FOREIGN KEY (prestupek) REFERENCES prestupek;

ALTER TABLE vozidlo ADD CONSTRAINT PK_vozidlo PRIMARY KEY (vin);

ALTER TABLE spz ADD CONSTRAINT PK_spz PRIMARY KEY (cislo);
ALTER TABLE spz ADD CONSTRAINT FK_spz_majitel FOREIGN KEY (majitel) REFERENCES ridic ON DELETE CASCADE;
ALTER TABLE spz ADD CONSTRAINT FK_spz_vozidlo FOREIGN KEY (vozidlo) REFERENCES vozidlo ON DELETE CASCADE;

CREATE SEQUENCE seq_kradez_id;
ALTER TABLE kradez ADD CONSTRAINT PK_kradez PRIMARY KEY (id);
ALTER TABLE kradez ADD CONSTRAINT FK_kradez_vozidlo FOREIGN KEY (vozidlo) REFERENCES vozidlo ON DELETE CASCADE;


COMMIT;
