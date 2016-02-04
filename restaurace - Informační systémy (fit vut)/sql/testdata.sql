--
-- testdata.sql:   Restaurace (IIS 2012)
--
-- Author(s):      Marie Kratochvilova <xkrato11@stud.fit.vutbr.cz>
--                 Radek Sevcik        <xsevci44@stud.fit.vutbr.cz>
--
-- Date:           $Format:%aD$
--
-- This file is part of iis12_restaurace.
--

INSERT INTO novinky (datum, obsah) VALUES (date('2012-12-01'), "Dnes jsme zacali pracovat na projektu :-)");
INSERT INTO novinky (datum, obsah) VALUES (date('2012-12-08'), "Točíme o stošest a navíc podáváme jablečný štrůdl...");
INSERT INTO novinky (datum, obsah) VALUES (date('2012-12-09'), "A je to zde, okamžik na který jsme všichni netrpělivě čekali.. Otevíráme naši jedinečnou zimní zahrádku, přijďte se podívat a zažít něco víc..");
INSERT INTO novinky (datum, obsah) VALUES (date('2012-12-10'), "Dnes nám krásně sněží a my jako jediní točíme bílé pivko.");

/* Data for table kategorie */
INSERT INTO kategorie (id, nazev, popis) VALUES (1, 'Nápoje', 'nápoje všeho druhu');
INSERT INTO kategorie (id, nazev, popis) VALUES (2, 'Polévky', 'speciality restaurace');
INSERT INTO kategorie (id, nazev, popis) VALUES (3, 'Hlavní jídla', 'neco k jídlu');
INSERT INTO kategorie (id, nazev, popis) VALUES (4, 'Zákusky', 'sladké pochutnání');
INSERT INTO kategorie (id, nazev, popis) VALUES (5, 'Ostatní', 'neco slaného');

/* Data for table jidelni_listek */
INSERT INTO jidelni_listek (id, id_kategorie, nazev, popis, cena, viditelnost) VALUES (1, 1, 'Poutník výčepní 10°', '0,5l', 24, 1);
INSERT INTO jidelni_listek (id, id_kategorie, nazev, popis, cena, viditelnost) VALUES (2, 1, 'Poutník Výčepní 10°', '0,3l', 15, 0);
INSERT INTO jidelni_listek (id, id_kategorie, nazev, popis, cena, viditelnost) VALUES (3, 1, 'Staropramen', '0,5l', 32, 1);
INSERT INTO jidelni_listek (id, id_kategorie, nazev, popis, cena, viditelnost) VALUES (4, 1, 'Grena', '0,5l', 20, 1);
INSERT INTO jidelni_listek (id, id_kategorie, nazev, popis, cena, viditelnost) VALUES (5, 1, 'Kofola', '0,5l', 28, 1);
INSERT INTO jidelni_listek (id, id_kategorie, nazev, popis, cena, viditelnost) VALUES (6, 2, 'Cibulačka s krutony', '3 dl', 35, 1);
INSERT INTO jidelni_listek (id, id_kategorie, nazev, popis, cena, viditelnost) VALUES (7, 2, 'Hovězí vývar s nudličkami', '3 dl', 45, 1);
INSERT INTO jidelni_listek (id, id_kategorie, nazev, popis, cena, viditelnost) VALUES (8, 3, 'Kuřecí steak s kozím sýrem, rajčaty a bazalkou', '165 g', 122, 1);
INSERT INTO jidelni_listek (id, id_kategorie, nazev, popis, cena, viditelnost) VALUES (9, 3, 'Kuřecí kousky s cuketou, hřiby a smetanou', '150 g', 105, 1);
INSERT INTO jidelni_listek (id, id_kategorie, nazev, popis, cena, viditelnost) VALUES (10, 3, 'Grilovaná panenka s omáčkou ze zeleného pepře a smetanou na lůžku z listového špenátu', '250 g', 187, 1);
INSERT INTO jidelni_listek (id, id_kategorie, nazev, popis, cena, viditelnost) VALUES (11, 3, 'Pikantní panenka špikovaná česnekem a chilli papričkami', '250 g', 162, 1);
INSERT INTO jidelni_listek (id, id_kategorie, nazev, popis, cena, viditelnost) VALUES (12, 4, 'Jablečný štrůdl se šlehačkou', '2 ks', 35, 1);
INSERT INTO jidelni_listek (id, id_kategorie, nazev, popis, cena, viditelnost) VALUES (13, 4, 'Palačinky s lesním ovocem, zmrzlinou a šlehačkou', '2 ks', 67, 1);
INSERT INTO jidelni_listek (id, id_kategorie, nazev, popis, cena, viditelnost) VALUES (14, 5, 'Bramborové lupínku solené', '100 g', 25, 1);
INSERT INTO jidelni_listek (id, id_kategorie, nazev, popis, cena, viditelnost) VALUES (15, 5, 'Bramoborové lupínky Česnek', '100 g', 25, 1);
INSERT INTO jidelni_listek (id, id_kategorie, nazev, popis, cena, viditelnost) VALUES (16, 5, 'Tyčinky Bohemia', '85 g', 25, 1);
INSERT INTO jidelni_listek (id, id_kategorie, nazev, popis, cena, viditelnost) VALUES (17, 5, 'Arašídy', '100 g', 30, 1);
INSERT INTO jidelni_listek (id, id_kategorie, nazev, popis, cena, viditelnost) VALUES (18, 5, 'Zapalovač', '1 ks', 10, 1);
INSERT INTO jidelni_listek (id, id_kategorie, nazev, popis, cena, viditelnost) VALUES (19, 5, 'Krupky arašídové', '70 g', 15, 1);
INSERT INTO jidelni_listek (id, id_kategorie, nazev, popis, cena, viditelnost) VALUES (20, 5, 'Kešu solené', '60 g', 30, 1);


/* Data for table salonky */
INSERT INTO salonky (id, nazev, barva) VALUES (1, 'Růžový', '#FC9FF1');
INSERT INTO salonky (id, nazev, barva) VALUES (2, 'Modrý', '#98B8F9');
INSERT INTO salonky (id, nazev, barva) VALUES (3, 'Fialový', '#B276CC');
INSERT INTO salonky (id, nazev, barva) VALUES (4, 'Kuřácký', '#E4E3E5');
INSERT INTO salonky (id, nazev, barva) VALUES (5, 'Nekuřácký', '#652D00');


/* Data for table stoly */
INSERT INTO stoly (id, cislo_stolu, pocet_lidi, id_salonku) VALUES (1, 1, 2, 1);
INSERT INTO stoly (id, cislo_stolu, pocet_lidi, id_salonku) VALUES (2, 2, 4, 2);
INSERT INTO stoly (id, cislo_stolu, pocet_lidi, id_salonku) VALUES (3, 3, 6, 3);
INSERT INTO stoly (id, cislo_stolu, pocet_lidi, id_salonku) VALUES (4, 4, 10, 4);
INSERT INTO stoly (id, cislo_stolu, pocet_lidi, id_salonku) VALUES (5, 5, 15, 5);

/* Data for table objednavka */
INSERT INTO objednavka (id, id_jidla, id_stolu, datum, cena) VALUES (1, 5, 1, '2. 12. 2012 19:00:00', 40);
INSERT INTO objednavka (id, id_jidla, id_stolu, datum, cena) VALUES (2, 7, 5, '13. 12. 2012 5:50:00', 675);
INSERT INTO objednavka (id, id_jidla, id_stolu, datum, cena) VALUES (3, 1, 2, '15. 12. 2012 1:00:00', 1200);



/* Data for table rezervace */
INSERT INTO rezervace (id, id_stolu, datum, stav, pocet_lidi_u_stolu, jmeno) VALUES (1, 4, '5. 12. 2012 18:00:00', 'rezervováno', 6, 'Miloš Navrátil');
INSERT INTO rezervace (id, id_stolu, datum, stav, pocet_lidi_u_stolu, jmeno) VALUES (2, 1, '8. 12. 2012 19:00:00', 'rezervovano', 2, 'Alice Pyšná');


/* Data for table uzivatele */
INSERT INTO uzivatele (id, jmeno, telefon, mail, role, login, heslo) VALUES (1, 'Marie Kratochvilova', 123456789, 'maruska@seznam.cz', 'admin', 'maruska', 'c7ad44cbad762a5da0a452f9e854fdc1e0e7a52a38015f23f3eab1d80b931dd472634dfac71cd34ebc35d16ab7fb8a90c81f975113d6c7538dc69dd8de9077ec');
INSERT INTO uzivatele (id, jmeno, telefon, mail, role, login, heslo) VALUES (2, 'Radek Sevcik', 987654321, 'radek@seznam.cz', 'admin', 'radek', 'c7ad44cbad762a5da0a452f9e854fdc1e0e7a52a38015f23f3eab1d80b931dd472634dfac71cd34ebc35d16ab7fb8a90c81f975113d6c7538dc69dd8de9077ec');
INSERT INTO uzivatele (id, jmeno, telefon, mail, role, login, heslo) VALUES (3, 'Jan Novak', 125963874, 'novak@gmail.com', 'sefkuchar', 'kuchar', 'df6dfd1bd3f8f251d69602605c0862241993121879aad93f672f93f3c1e07d7cdd2f2f3a2dfebd268985f298a32894ab14583921367c4a6d99bad3e80dda89a1');
INSERT INTO uzivatele (id, jmeno, telefon, mail, role, login, heslo) VALUES (4, 'Eva Farna', 852136974, 'farna@email.com', 'cisnik', 'farna', '59a3b6b5b72949150fffc32d345b6ba4878433321d9a863918befe9c3898999056957efb95c93804764d9a80176db88333c74bd675965097837c63a157345fa2');
INSERT INTO uzivatele (id, jmeno, telefon, mail, role, login, heslo) VALUES (5, 'Petr Kozina', 789321456, 'kozina@kozine.eu', 'cisnik', 'kozina', '7c49d3499032cf042135d4bfc2b202a94a31b4e825b49ed327fbf30cf2006c0022ea8ba55a11a1afcd37ed4d93c8da32f8f7df2053218497ef9854f577de6259');

