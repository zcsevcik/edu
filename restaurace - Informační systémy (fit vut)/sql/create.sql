--
-- create.sql:     Restaurace (IIS 2012)
--
-- Author(s):      Marie Kratochvilova <xkrato11@stud.fit.vutbr.cz>
--                 Radek Sevcik        <xsevci44@stud.fit.vutbr.cz>
--
-- Date:           $Format:%aD$
--
-- This file is part of iis12_restaurace.
--

CREATE TABLE IF NOT EXISTS novinky (
    id_novinky INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    datum DATE NOT NULL DEFAULT CURRENT_TIMESTAMP,
    obsah TEXT NOT NULL
);

CREATE TABLE jidelni_listek (
  id            integer PRIMARY KEY NOT NULL,
  id_kategorie  integer,
  nazev         varchar,
  popis         varchar,
  cena          integer,
  viditelnost   bool,
  /* Foreign keys */
  FOREIGN KEY (id_kategorie)
    REFERENCES kategorie(id)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION
);

CREATE TABLE kategorie (
  id     integer PRIMARY KEY AUTOINCREMENT NOT NULL,
  nazev  varchar,
  popis  varchar
);


CREATE TABLE objednavka (
  id        integer PRIMARY KEY AUTOINCREMENT NOT NULL,
  id_jidla  integer,
  id_stolu  integer,
  datum     datetime,
  cena      integer,
  zaplaceno bool default 0,
  /* Foreign keys */
  FOREIGN KEY (id_stolu)
    REFERENCES stoly(id)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  FOREIGN KEY (id_jidla)
    REFERENCES jidelni_listek(id)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION
);

CREATE TABLE rezervace (
  id                  integer PRIMARY KEY NOT NULL,
  id_stolu            integer,
  datum               datetime,
  stav                varchar,
  pocet_lidi_u_stolu  integer,
  jmeno               varchar,
  /* Foreign keys */
  FOREIGN KEY (id_stolu)
    REFERENCES stoly(id)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION
);

CREATE TABLE salonky (
  id     integer PRIMARY KEY AUTOINCREMENT NOT NULL,
  nazev  varchar,
  barva  varchar
);

CREATE TABLE stoly (
  id           integer PRIMARY KEY AUTOINCREMENT NOT NULL,
  cislo_stolu  integer,
  pocet_lidi   integer,
  id_salonku   integer,
  /* Foreign keys */
  FOREIGN KEY (id_salonku)
    REFERENCES salonky(id)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION
);

CREATE TABLE uzivatele (
  id       integer PRIMARY KEY AUTOINCREMENT NOT NULL,
  jmeno    varchar,
  telefon  integer,
  mail     varchar,
  role     varchar,
  login    varchar NOT NULL UNIQUE,
  heslo    varchar NOT NULL
);
