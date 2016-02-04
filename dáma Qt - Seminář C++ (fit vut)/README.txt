
   dama2013  8 Kve 2013
   Navrhnete a implementujte aplikaci pro hru Dama 

   Radek Sevcik
   Fakulta informacnich technologii
   Vysokeho uceni technickeho v Brne

   POPIS APLIKACE
   --------------

   Aplikace zname deskove hry Dama.
   
   dama2013 je s grafickym rozhranim Qt4.
   dama2013-cli je textoveho rozhrani.

   PREHLED
   -------

   Aplikace si hlida korektnost tahu podle ceskych pravidel. Zobrazuje
   povinne tahy a take zobrazuje napovedu pro mozne tahy.

   Aplikace umoznuje hru proti umele inteligenci, dva hraci na jednom
   pocitaci a nechybi ani hra po TCP/IP (v4,v6). Je mozne nacist a dohrat
   rozehranou partii a kdykoliv znovu ulozit.

   Partie jsou ulozene ve vlastnim binarnim formatu, textove
   zakladni algebraicke notaci a XML.

   Provedene tahy jsou zaznamenany.

   Pri sitove hre se klienti navzajem kontroluji, jestli nepodvadi.

   KOMPILACE PROGRAMU
   ------------------

   $ make

   OVLADANI Z TEXTOVEHO ROZHRANI
   -----------------------------

   V rozehrane hre existuji nektere prikazy, kterym aplikace rozumi a to:
   !quit        -- ukonci partii a vrati se do menu
   !save <file> -- ulozi partii do souboru file
   ?<a1..h8>    -- zobrazi mozne tahy kamene na souradnici

   TESTOVANI
   ---------

   Tato aplikace byla otestovana na skolnim serveru merlin (CentOS) a
   Debian 7. Pouzity prekladac: gcc 4.7.2. Qt toolkit: 4.7.3, 4.8.2.

   ZNAME PROBLEMY
   --------------

   Na FreeBSD gcc 4.7.4 se nepodarilo prelozit. Chybi nektere funkce
   z c++11, konkretne std::to_string(), aj..

   V konzolove verzi funguje korektne prekreslovani v PuTTY,
   v linuxovem terminalu (xterm) vubec.

