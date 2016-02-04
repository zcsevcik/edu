-- Datum: 2012/03/21
-- Autor: Radek Sevcik, xsevci44@stud.fit.vutbr.cz
--        Anton Tokar, xtokar00@stud.fit.vutbr.cz
-- Fakulta Informacnich Technologii, Vysoke Uceni Technicke v Brne

-- Projekt: Databazove systemy, SQL skript pro vytvoreni databaze
-- Zadani:  Evidence ridicu a motorovych vozidel #36


-- DROP TABLE "kradez";
-- DROP TABLE "pokuta";
-- DROP TABLE "spz";
-- DROP TABLE "vozidlo";
-- DROP TABLE "ridic";
-- DROP TABLE "prestupek";

-- pozn. idealni nahradit za vyraz typu DROP TABLE IF EXISTS,
-- jinak skonci chybou a zbytek se neprovede!

CREATE TABLE "ridic" (
"id" INTEGER NOT NULL,
"jmeno" VARCHAR(20) NOT NULL,
"prijmeni" VARCHAR(20) NOT NULL,
"rc" CHAR(11) NOT NULL,
"obec" VARCHAR(20) NOT NULL,
"adresa" VARCHAR(20) NOT NULL 
);

CREATE TABLE "vozidlo" (
"vin" CHAR(17) NOT NULL,
"registrace" DATE NOT NULL 
);

CREATE TABLE "spz" (
"cislo" CHAR(7) NOT NULL,
"pridelena_od" DATE NOT NULL,
"pridelena_do" DATE NULL,
"vozidlo" CHAR(17) NOT NULL,
"majitel" INTEGER NOT NULL 
);

CREATE TABLE "pokuta" (
"id" INTEGER NOT NULL,
"datum" DATE NOT NULL,
"ridic" INTEGER NOT NULL,
"prestupek" INTEGER NOT NULL,
"pozn" VARCHAR(255)
);

CREATE TABLE "prestupek" (
"id" INTEGER NOT NULL,
"paragraf" VARCHAR(20) NOT NULL,
"body" INTEGER NOT NULL,
"popis" VARCHAR(100)
);

CREATE TABLE "kradez" (
"id" INTEGER NOT NULL,
"vozidlo" CHAR(17) NOT NULL,
"misto" VARCHAR(20) NOT NULL,
"datum" DATE NOT NULL,
"pozn" VARCHAR(255)
);

ALTER TABLE "ridic" ADD CONSTRAINT PK_ridic PRIMARY KEY ("id");
ALTER TABLE "ridic" ADD CONSTRAINT UK_ridic_rc UNIQUE ("rc");
ALTER TABLE "prestupek" ADD CONSTRAINT PK_prestupek PRIMARY KEY ("id");
ALTER TABLE "pokuta" ADD CONSTRAINT PK_pokuta PRIMARY KEY ("id");
ALTER TABLE "pokuta" ADD CONSTRAINT FK_pokuta_ridic FOREIGN KEY ("ridic") REFERENCES "ridic";
ALTER TABLE "pokuta" ADD CONSTRAINT FK_pokuta_prestupek FOREIGN KEY ("prestupek") REFERENCES "prestupek";
ALTER TABLE "vozidlo" ADD CONSTRAINT PK_vozidlo PRIMARY KEY ("vin");
ALTER TABLE "spz" ADD CONSTRAINT PK_spz PRIMARY KEY ("cislo");
ALTER TABLE "spz" ADD CONSTRAINT FK_spz_majitel FOREIGN KEY ("majitel") REFERENCES "ridic";
ALTER TABLE "spz" ADD CONSTRAINT FK_spz_vozidlo FOREIGN KEY ("vozidlo") REFERENCES "vozidlo";
ALTER TABLE "kradez" ADD CONSTRAINT PK_kradez PRIMARY KEY ("id");
ALTER TABLE "kradez" ADD CONSTRAINT FK_kradez_vozidlo FOREIGN KEY ("vozidlo") REFERENCES "vozidlo";

INSERT INTO "ridic" VALUES ('0', 'Jan', 'Novák', '440726/0672', 'Brno', 'Cejl 8');
INSERT INTO "ridic" VALUES ('1', 'Petr', 'Veselý', '530610/4532', 'Brno', 'Podzimní 28');
INSERT INTO "ridic" VALUES ('3', 'Pavel', 'Tomek', '510230/0486', 'Brno', 'Tomkova 34');
INSERT INTO "ridic" VALUES ('4', 'Josef', 'Mádr', '580807/9638', 'Brno', 'Svatoplukova 15');
INSERT INTO "ridic" VALUES ('2', 'Ivan', 'Zeman', '601001/2218', 'Brno', 'Cejl 8');
INSERT INTO "ridic" VALUES ('5', 'Jana', 'Malá', '625622/6249', 'Vyškov', 'Brnìnská 56');

INSERT INTO "vozidlo" VALUES ('WPOZZZ99ZTS392124', TO_DATE('2010-03-22', 'YYYY-MM-DD'));
INSERT INTO "vozidlo" VALUES ('1M8GDM9AXKP042788', TO_DATE('2011-04-05', 'YYYY-MM-DD'));
INSERT INTO "vozidlo" VALUES ('JH4NA1157MT001832', TO_DATE('2010-05-18', 'YYYY-MM-DD'));
INSERT INTO "vozidlo" VALUES ('TMBGDM9A1KP042788', TO_DATE('2009-05-26', 'YYYY-MM-DD'));
INSERT INTO "vozidlo" VALUES ('LJCPCBLCX11000237', TO_DATE('2012-03-16', 'YYYY-MM-DD'));

INSERT INTO "spz" VALUES ('4A23001', TO_DATE('2009-05-30', 'YYYY-MM-DD'), TO_DATE('2010-12-18', 'YYYY-MM-DD'), 'TMBGDM9A1KP042788', '3');
INSERT INTO "spz" VALUES ('2AU5402', TO_DATE('2011-01-03', 'YYYY-MM-DD'), null, 'WPOZZZ99ZTS392124', '3');
INSERT INTO "spz" VALUES ('3J57942', TO_DATE('2011-06-15', 'YYYY-MM-DD'), TO_DATE('2011-12-20', 'YYYY-MM-DD'), '1M8GDM9AXKP042788', '1');
INSERT INTO "spz" VALUES ('6T99994', TO_DATE('2010-07-01', 'YYYY-MM-DD'), null, 'JH4NA1157MT001832', '0');

INSERT INTO "prestupek" VALUES ('0', '§ 125c/1a) 1.', '0', 'Vozidlo bez (nebo s cizí) SPZ');
INSERT INTO "prestupek" VALUES ('1', '§ 125c/1a) 2.', '0', 'Vozidlo se zakrytou, upravenou nebo neèitelnou SPZ');
INSERT INTO "prestupek" VALUES ('2', '§ 125c/1a) 3.', '5', 'Technicky nezpùsobilé vozidlo');
INSERT INTO "prestupek" VALUES ('3', '§ 125c/1b)', '7', 'Øízení pod vlivem alkoholu (nad 0,3 promile)');
INSERT INTO "prestupek" VALUES ('4', '§ 125c/1b)', '0', 'Øízení pod vlivem alkoholu (do 0,3 promile)');
INSERT INTO "prestupek" VALUES ('5', '§ 125c/1c)', '7', 'Øízení ve stavu vyluèujícím zpùsobilost (alkohol, návykové látky)');
INSERT INTO "prestupek" VALUES ('6', '§ 274 TZ', '7', 'Øízení ve stavu vyluèujícím zpùsobilost (alkohol a návykové látky)');
INSERT INTO "prestupek" VALUES ('7', '§ 125c/1d)', '7', 'Odmítnutí testu na alkohol / návykové látky');
INSERT INTO "prestupek" VALUES ('8', '§ 125c/1e) 1.', '4', 'Øízení bez øidièského oprávnìní');
INSERT INTO "prestupek" VALUES ('9', '§ 125c/1e) 2.', '7', 'Øízení po zadržení øidièského prùkazu na místì');
INSERT INTO "prestupek" VALUES ('10', '§ 125c/1e) 3.', '3', 'Øízení bez profesního prùkazu');
INSERT INTO "prestupek" VALUES ('11', '§ 125c/1e) 4.', '0', 'Øízení bez posudku o zdravotní zpùsobilosti');
INSERT INTO "prestupek" VALUES ('12', '§ 125c/1e) 5.', '0', 'Øízení s øidièským prùkazem EU a pozbytím práva k øízení na území ÈR');
INSERT INTO "prestupek" VALUES ('13', '§ 125c/1f) 1.', '2', 'Øízení s telefonem v ruce');
INSERT INTO "prestupek" VALUES ('14', '§ 125c/1f) 2.', '5', 'Vysoké pøekroèení rychlosti - v obci o 40 km a více a mimo obec o 50 km a více');
INSERT INTO "prestupek" VALUES ('15', '§ 125c/1f) 3.', '3', 'Vyšší pøekroèení rychlosti - v obci o 20 km více a mimo obec o 30 km a více');
INSERT INTO "prestupek" VALUES ('20', '§ 125c/1f) 6.c)', '0', 'Ohrožení chodce pøi odboèování');
INSERT INTO "prestupek" VALUES ('16', '§ 125c/1f) 4.', '2', 'Støední pøekroèení rychlosti - v obci do 20 km nebo mimo obec do 30 km');
INSERT INTO "prestupek" VALUES ('17', '§ 125c/1f) 5.', '5', 'Nezastavení na èervenou (nebo pokyn "Stùj")');
INSERT INTO "prestupek" VALUES ('18', '§ 125c/1f) 6.a)', '4', 'Ohrožení chodce na pøechodu');
INSERT INTO "prestupek" VALUES ('19', '§ 125c/1f) 6.b)', '3', 'Nezastavení pøed pøechodem');
INSERT INTO "prestupek" VALUES ('21', '§ 125c/1f) 6.d)', '5', 'Ohrožení chodce mimo silnici nebo pøi otáèení nebo couvání');
INSERT INTO "prestupek" VALUES ('22', '§ 125c/1f) 7.', '7', 'Nedovolené pøedjíždìní');
INSERT INTO "prestupek" VALUES ('23', '§ 125c/1f) 8.', '4', 'Nedání pøednosti v jízdì');
INSERT INTO "prestupek" VALUES ('24', '§ 125c/1f) 9.', '7', 'Nedovolená jízda pøes železnièní pøejezd');
INSERT INTO "prestupek" VALUES ('25', '§ 125c/1f) 10.', '7', 'Couvání, otáèení nebo jízda v protismìru na dálníci');
INSERT INTO "prestupek" VALUES ('26', '§ 125c/1f) 11.', '0', 'Stání na místì vyhrazeném pro invalidy');
INSERT INTO "prestupek" VALUES ('27', '§ 125c/1g)', '0', 'Nedovolená jízda nákladního automobilu');
INSERT INTO "prestupek" VALUES ('28', '§ 125c/1h)', '0', 'Zavinìní nehody s ublížením na zdraví');
INSERT INTO "prestupek" VALUES ('29', '§ 143, 147, 148 TZ', '7', 'Nehoda s usmrcením nebo tìžkou újmou na zdraví');
INSERT INTO "prestupek" VALUES ('30', '§ 125c/1i) 1.', '7', 'Nezastavení po nehodì');
INSERT INTO "prestupek" VALUES ('31', '§ 125c/1i) 2.', '0', 'Neohlášení dopravní nehody');
INSERT INTO "prestupek" VALUES ('32', '§ 125c/1i) 3.', '0', 'Neposkytnutí údajù ostatním úèastníkùm dopravní nehody');
INSERT INTO "prestupek" VALUES ('33', '§ 125c/1i) 4.', '7', 'Ujetí od dopravní nehody');
INSERT INTO "prestupek" VALUES ('34', '§ 125c/1j)', '0', 'Použití antiradaru');
INSERT INTO "prestupek" VALUES ('35', '§ 125c/1k)', '0', 'Jiné porušení povinností');
INSERT INTO "prestupek" VALUES ('36', '§ 125c/1k)', '5', 'Ohrožení pøi pøejíždìní z pruhu do pruhu');
INSERT INTO "prestupek" VALUES ('37', '§ 125c/1k)', '3', 'Jízda bez bezpeènostních pásù');
INSERT INTO "prestupek" VALUES ('38', '§ 125c/1k)', '4', 'Nepoužití autosedaèky pro dítì');
INSERT INTO "prestupek" VALUES ('39', '§ 125c/1k)', '2', 'Neoznaèení pøekážky zpùsobené øidièem');
INSERT INTO "prestupek" VALUES ('40', '§ 125c/2)', '0', 'Svìøení øízení neznámé osobì');
INSERT INTO "prestupek" VALUES ('41', '§ 23/1f)', '0', 'Øízení bez dokladù nebo záznamù o provozu vozidla');
INSERT INTO "prestupek" VALUES ('42', '§ 23/1f)', '4', 'Nedodržení bezpeènostních pøestávek');
INSERT INTO "prestupek" VALUES ('43', '§ 23/1f)', '0', 'Nepøedložení záznamu o dobì øízení a bezpeènostních pøestávkách');
INSERT INTO "prestupek" VALUES ('44', '§ 42 a/4 c)', '3', 'Pøetížený kamion nebo autobus');
INSERT INTO "prestupek" VALUES ('45', '§ 150 TZ', '7', 'Neposkytnutí první pomoci / nepøivolání záchranky');
INSERT INTO "prestupek" VALUES ('46', '§ 125c/1f) 4.', '0', 'Malé pøekroèení rychlosti - v obci do 5 km nebo mimo obec do 10 km');

INSERT INTO "kradez" VALUES ('0', 'JH4NA1157MT001832', 'Praha', TO_DATE('2011-08-13 15:30:00', 'YYYY-MM-DD HH24:MI:SS'), null);

INSERT INTO "pokuta" VALUES ('0', TO_DATE('2009-06-10 14:45:02', 'YYYY-MM-DD HH24:MI:SS'), '3', '20', null);
INSERT INTO "pokuta" VALUES ('1', TO_DATE('2010-07-16 17:45:48', 'YYYY-MM-DD HH24:MI:SS'), '3', '33', null);
INSERT INTO "pokuta" VALUES ('2', TO_DATE('2011-11-16 08:26:49', 'YYYY-MM-DD HH24:MI:SS'), '3', '5', null);
INSERT INTO "pokuta" VALUES ('3', TO_DATE('2011-12-13 12:47:17', 'YYYY-MM-DD HH24:MI:SS'), '1', '29', null);

COMMIT;
