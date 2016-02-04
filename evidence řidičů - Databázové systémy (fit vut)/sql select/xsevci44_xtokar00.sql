-- Datum: 2012/04/05
-- Autor: Radek Sevcik, xsevci44@stud.fit.vutbr.cz
--        Anton Tokar, xtokar00@stud.fit.vutbr.cz
-- Fakulta Informacnich Technologii, Vysoke Uceni Technicke v Brne

-- Projekt: Databazove systemy, SQL skript s dotazy SELECT
-- Zadani:  Evidence ridicu a motorovych vozidel #36


DROP TABLE "kradez" CASCADE CONSTRAINTS;
DROP TABLE "pokuta" CASCADE CONSTRAINTS;
DROP TABLE "spz" CASCADE CONSTRAINTS;
DROP TABLE "vozidlo" CASCADE CONSTRAINTS;
DROP TABLE "ridic" CASCADE CONSTRAINTS;
DROP TABLE "prestupek" CASCADE CONSTRAINTS;
DROP SEQUENCE seq_ridic_id;
DROP SEQUENCE seq_pokuta_id;
DROP SEQUENCE seq_kradez_id;
DROP SEQUENCE seq_prestupek_id;

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


CREATE SEQUENCE seq_ridic_id;
ALTER TABLE "ridic" ADD CONSTRAINT "PK_ridic" PRIMARY KEY ("id");
ALTER TABLE "ridic" ADD CONSTRAINT "UK_ridic_rc" UNIQUE ("rc");

CREATE SEQUENCE seq_prestupek_id;
ALTER TABLE "prestupek" ADD CONSTRAINT "PK_prestupek" PRIMARY KEY ("id");

CREATE SEQUENCE seq_pokuta_id;
ALTER TABLE "pokuta" ADD CONSTRAINT "PK_pokuta" PRIMARY KEY ("id");
ALTER TABLE "pokuta" ADD CONSTRAINT "FK_pokuta_ridic" FOREIGN KEY ("ridic") REFERENCES "ridic";
ALTER TABLE "pokuta" ADD CONSTRAINT "FK_pokuta_prestupek" FOREIGN KEY ("prestupek") REFERENCES "prestupek";

ALTER TABLE "vozidlo" ADD CONSTRAINT "PK_vozidlo" PRIMARY KEY ("vin");

ALTER TABLE "spz" ADD CONSTRAINT "PK_spz" PRIMARY KEY ("cislo");
ALTER TABLE "spz" ADD CONSTRAINT "FK_spz_majitel" FOREIGN KEY ("majitel") REFERENCES "ridic";
ALTER TABLE "spz" ADD CONSTRAINT "FK_spz_vozidlo" FOREIGN KEY ("vozidlo") REFERENCES "vozidlo";

CREATE SEQUENCE seq_kradez_id;
ALTER TABLE "kradez" ADD CONSTRAINT "PK_kradez" PRIMARY KEY ("id");
ALTER TABLE "kradez" ADD CONSTRAINT "FK_kradez_vozidlo" FOREIGN KEY ("vozidlo") REFERENCES "vozidlo";


INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1a) 1.', '0', 'Vozidlo bez (nebo s ciz�) SPZ');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1a) 2.', '0', 'Vozidlo se zakrytou, upravenou nebo ne�itelnou SPZ');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1a) 3.', '5', 'Technicky nezp�sobil� vozidlo');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1b)', '7', '��zen� pod vlivem alkoholu (nad 0,3 promile)');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1b)', '0', '��zen� pod vlivem alkoholu (do 0,3 promile)');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1c)', '7', '��zen� ve stavu vylu�uj�c�m zp�sobilost (alkohol, n�vykov� l�tky)');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 274 TZ', '7', '��zen� ve stavu vylu�uj�c�m zp�sobilost (alkohol a n�vykov� l�tky)');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1d)', '7', 'Odm�tnut� testu na alkohol / n�vykov� l�tky');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1e) 1.', '4', '��zen� bez �idi�sk�ho opr�vn�n�');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1e) 2.', '7', '��zen� po zadr�en� �idi�sk�ho pr�kazu na m�st�');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1e) 3.', '3', '��zen� bez profesn�ho pr�kazu');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1e) 4.', '0', '��zen� bez posudku o zdravotn� zp�sobilosti');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1e) 5.', '0', '��zen� s �idi�sk�m pr�kazem EU a pozbyt�m pr�va k ��zen� na �zem� �R');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1f) 1.', '2', '��zen� s telefonem v ruce');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1f) 2.', '5', 'Vysok� p�ekro�en� rychlosti - v obci o 40 km a v�ce a mimo obec o 50 km a v�ce');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1f) 3.', '3', 'Vy��� p�ekro�en� rychlosti - v obci o 20 km v�ce a mimo obec o 30 km a v�ce');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1f) 4.', '2', 'St�edn� p�ekro�en� rychlosti - v obci do 20 km nebo mimo obec do 30 km');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1f) 5.', '5', 'Nezastaven� na �ervenou (nebo pokyn "St�j")');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1f) 6.a)', '4', 'Ohro�en� chodce na p�echodu');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1f) 6.b)', '3', 'Nezastaven� p�ed p�echodem');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1f) 6.c)', '0', 'Ohro�en� chodce p�i odbo�ov�n�');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1f) 6.d)', '5', 'Ohro�en� chodce mimo silnici nebo p�i ot��en� nebo couv�n�');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1f) 7.', '7', 'Nedovolen� p�edj�d�n�');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1f) 8.', '4', 'Ned�n� p�ednosti v j�zd�');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1f) 9.', '7', 'Nedovolen� j�zda p�es �elezni�n� p�ejezd');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1f) 10.', '7', 'Couv�n�, ot��en� nebo j�zda v protism�ru na d�ln�ci');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1f) 11.', '0', 'St�n� na m�st� vyhrazen�m pro invalidy');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1g)', '0', 'Nedovolen� j�zda n�kladn�ho automobilu');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1h)', '0', 'Zavin�n� nehody s ubl�en�m na zdrav�');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 143, 147, 148 TZ', '7', 'Nehoda s usmrcen�m nebo t�kou �jmou na zdrav�');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1i) 1.', '7', 'Nezastaven� po nehod�');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1i) 2.', '0', 'Neohl�en� dopravn� nehody');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1i) 3.', '0', 'Neposkytnut� �daj� ostatn�m ��astn�k�m dopravn� nehody');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1i) 4.', '7', 'Ujet� od dopravn� nehody');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1j)', '0', 'Pou�it� antiradaru');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1k)', '0', 'Jin� poru�en� povinnost�');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1k)', '5', 'Ohro�en� p�i p�ej�d�n� z pruhu do pruhu');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1k)', '3', 'J�zda bez bezpe�nostn�ch p�s�');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1k)', '4', 'Nepou�it� autoseda�ky pro d�t�');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1k)', '2', 'Neozna�en� p�ek�ky zp�soben� �idi�em');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/2)', '0', 'Sv��en� ��zen� nezn�m� osob�');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 23/1f)', '0', '��zen� bez doklad� nebo z�znam� o provozu vozidla');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 23/1f)', '4', 'Nedodr�en� bezpe�nostn�ch p�est�vek');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 23/1f)', '0', 'Nep�edlo�en� z�znamu o dob� ��zen� a bezpe�nostn�ch p�est�vk�ch');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 42 a/4 c)', '3', 'P�et�en� kamion nebo autobus');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 150 TZ', '7', 'Neposkytnut� prvn� pomoci / nep�ivol�n� z�chranky');
INSERT INTO "prestupek" VALUES (seq_prestupek_id.nextval, '� 125c/1f) 4.', '0', 'Mal� p�ekro�en� rychlosti - v obci do 5 km nebo mimo obec do 10 km');

INSERT INTO "vozidlo" VALUES ('WPOZZZ99ZTS392124', TO_DATE('2010-03-22', 'YYYY-MM-DD'));
INSERT INTO "vozidlo" VALUES ('1M8GDM9AXKP042788', TO_DATE('2011-04-05', 'YYYY-MM-DD'));
INSERT INTO "vozidlo" VALUES ('JH4NA1157MT001832', TO_DATE('2010-05-18', 'YYYY-MM-DD'));
INSERT INTO "vozidlo" VALUES ('TMBGDM9A1KP042788', TO_DATE('2009-05-26', 'YYYY-MM-DD'));
INSERT INTO "vozidlo" VALUES ('LJCPCBLCX11000237', TO_DATE('2012-03-16', 'YYYY-MM-DD'));

INSERT INTO "ridic" VALUES (seq_ridic_id.nextval, 'Jan', 'Nov�k', '440726/0672', 'Brno', 'Cejl 8');
INSERT INTO "ridic" VALUES (seq_ridic_id.nextval, 'Petr', 'Vesel�', '530610/4532', 'Brno', 'Podzimn� 28');
INSERT INTO "ridic" VALUES (seq_ridic_id.nextval, 'Ivan', 'Zeman', '601001/2218', 'Brno', 'Cejl 8');
INSERT INTO "ridic" VALUES (seq_ridic_id.nextval, 'Pavel', 'Tomek', '510230/0486', 'Brno', 'Tomkova 34');
INSERT INTO "ridic" VALUES (seq_ridic_id.nextval, 'Josef', 'M�dr', '580807/9638', 'Brno', 'Svatoplukova 15');
INSERT INTO "ridic" VALUES (seq_ridic_id.nextval, 'Jana', 'Mal�', '625622/6249', 'Vy�kov', 'Brn�nsk� 56');

INSERT INTO "spz" VALUES ('4A23001', TO_DATE('2009-05-30', 'YYYY-MM-DD'), TO_DATE('2010-12-18', 'YYYY-MM-DD'), 'TMBGDM9A1KP042788', '4');
INSERT INTO "spz" VALUES ('2AU5402', TO_DATE('2011-01-03', 'YYYY-MM-DD'), null, 'WPOZZZ99ZTS392124', 4);
INSERT INTO "spz" VALUES ('3J57942', TO_DATE('2011-06-15', 'YYYY-MM-DD'), TO_DATE('2011-12-20', 'YYYY-MM-DD'), '1M8GDM9AXKP042788', '2');
INSERT INTO "spz" VALUES ('6T99994', TO_DATE('2010-07-01', 'YYYY-MM-DD'), null, 'JH4NA1157MT001832', '1');

INSERT INTO "kradez" VALUES (seq_kradez_id.nextval, 'JH4NA1157MT001832', 'Praha', TO_DATE('2011-08-13 15:30:00', 'YYYY-MM-DD HH24:MI:SS'), null);

INSERT INTO "pokuta" VALUES (seq_pokuta_id.nextval, TO_DATE('2009-06-10 14:45:02', 'YYYY-MM-DD HH24:MI:SS'), '4', '21', null);
INSERT INTO "pokuta" VALUES (seq_pokuta_id.nextval, TO_DATE('2010-07-16 17:45:48', 'YYYY-MM-DD HH24:MI:SS'), '4', '34', null);
INSERT INTO "pokuta" VALUES (seq_pokuta_id.nextval, TO_DATE('2011-11-16 08:26:49', 'YYYY-MM-DD HH24:MI:SS'), '4', '6', null);
INSERT INTO "pokuta" VALUES (seq_pokuta_id.nextval, TO_DATE('2011-12-13 12:47:17', 'YYYY-MM-DD HH24:MI:SS'), '2', '30', null);

COMMIT;


-- Vyp�e statistiku p�estupk� za rok 2011
SELECT (
  SELECT "popis" FROM "prestupek"
  WHERE "id" = "pokuta"."prestupek"
) AS "popis", COUNT(*) as "pocet"
FROM "pokuta"
WHERE TRUNC(TO_DATE('2011', 'YYYY'), 'Year') = TRUNC("datum", 'Year')
GROUP BY "prestupek"
ORDER BY "popis" ASC;

-- Vyp�e, kter� auta vlastnil �idi� s rodn�m ��slem '510230/0486'
SELECT "cislo" as "spz", "vin", "registrace" AS "vyrobeno",
       "pridelena_od" AS "od", "pridelena_do" AS "do"
FROM "spz"
INNER JOIN "vozidlo" ON "spz"."vozidlo" = "vozidlo"."vin"
INNER JOIN "ridic" ON "spz"."majitel" = "ridic"."id"
WHERE "ridic"."rc" = '510230/0486'
ORDER BY "pridelena_od" DESC;

-- Vyp�e spz v�ech kraden�ch vozidel
SELECT "kradez"."vozidlo" as "vin", "cislo" AS "spz", "misto", "datum", "pozn"
FROM "kradez"
INNER JOIN "spz" ON "kradez"."vozidlo" = "spz"."vozidlo"
WHERE "spz"."pridelena_do" IS NULL
ORDER BY "datum" DESC;

-- Vyp�e kolik, kdy a za co dostal �idi� s r� '510230/0486' bod�
SELECT "paragraf", "popis", "body" AS "pocet", "datum", "pozn"
FROM "ridic"
INNER JOIN "pokuta" ON "ridic"."id" = "pokuta"."ridic"
INNER JOIN "prestupek" ON "pokuta"."prestupek" = "prestupek"."id"
WHERE "rc" = '510230/0486'
ORDER BY "datum" DESC;

-- Vyp�e kolik je v kter�m m�st� �idi��
SELECT "obec", COUNT(*) AS "pocet" FROM "ridic"
GROUP BY "obec"
ORDER BY "obec" ASC;

-- Vyp�e majitele vozu se zna�kou '2AU-5402'
SELECT "cislo" AS "spz", "vozidlo" AS "vin", "jmeno", "prijmeni", "rc",
       "obec", "adresa" FROM "spz"
INNER JOIN "ridic" ON "spz"."majitel" = "ridic"."id"
WHERE "cislo" = '2AU5402';

-- Vyp�e vozidla ukraden� v Praze, Brn� a Vy�kov�
SELECT * FROM "kradez"
WHERE "misto" IN ('Brno', 'Vy�kov', 'Praha');

-- Vyp�e vozidla, kter� nem�la ��dn�ho majitele
SELECT * FROM "vozidlo"
WHERE NOT EXISTS (
  SELECT * FROM "spz"
  WHERE "vozidlo" = "vin"
);

