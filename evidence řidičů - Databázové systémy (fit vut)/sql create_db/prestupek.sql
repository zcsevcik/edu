/*
Navicat Oracle Data Transfer
Oracle Client Version : 11.2.0.2.0

Source Server         : berta
Source Server Version : 100200
Source Host           : berta.fit.vutbr.cz:1521
Source Schema         : XSEVCI44

Target Server Type    : ORACLE
Target Server Version : 100200
File Encoding         : 65001

Date: 2012-03-21 15:19:43
*/


-- ----------------------------
-- Table structure for "XSEVCI44"."prestupek"
-- ----------------------------
DROP TABLE "XSEVCI44"."prestupek";
CREATE TABLE "XSEVCI44"."prestupek" (
"id" NUMBER NOT NULL ,
"paragraf" VARCHAR2(20 BYTE) NOT NULL ,
"body" NUMBER NOT NULL ,
"popis" VARCHAR2(100 BYTE) NULL 
)
LOGGING
NOCOMPRESS
NOCACHE

;

-- ----------------------------
-- Records of prestupek
-- ----------------------------
INSERT INTO "XSEVCI44"."prestupek" VALUES ('0', '§ 125c/1a) 1.', '0', 'Vozidlo bez (nebo s cizí) SPZ');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('1', '§ 125c/1a) 2.', '0', 'Vozidlo se zakrytou, upravenou nebo nečitelnou SPZ');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('2', '§ 125c/1a) 3.', '5', 'Technicky nezpůsobilé vozidlo');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('3', '§ 125c/1b)', '7', 'Řízení pod vlivem alkoholu (nad 0,3 promile)');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('4', '§ 125c/1b)', '0', 'Řízení pod vlivem alkoholu (do 0,3 promile)');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('5', '§ 125c/1c)', '7', 'Řízení ve stavu vylučujícím způsobilost (alkohol, návykové látky)');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('6', '§ 274 TZ', '7', 'Řízení ve stavu vylučujícím způsobilost (alkohol a návykové látky)');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('7', '§ 125c/1d)', '7', 'Odmítnutí testu na alkohol / návykové látky');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('8', '§ 125c/1e) 1.', '4', 'Řízení bez řidičského oprávnění');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('9', '§ 125c/1e) 2.', '7', 'Řízení po zadržení řidičského průkazu na místě');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('10', '§ 125c/1e) 3.', '3', 'Řízení bez profesního průkazu');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('11', '§ 125c/1e) 4.', '0', 'Řízení bez posudku o zdravotní způsobilosti');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('12', '§ 125c/1e) 5.', '0', 'Řízení s řidičským průkazem EU a pozbytím práva k řízení na území ČR');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('13', '§ 125c/1f) 1.', '2', 'Řízení s telefonem v ruce');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('14', '§ 125c/1f) 2.', '5', 'Vysoké překročení rychlosti - v obci o 40 km a více a mimo obec o 50 km a více');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('15', '§ 125c/1f) 3.', '3', 'Vyšší překročení rychlosti - v obci o 20 km více a mimo obec o 30 km a více');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('20', '§ 125c/1f) 6.c)', '0', 'Ohrožení chodce při odbočování');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('16', '§ 125c/1f) 4.', '2', 'Střední překročení rychlosti - v obci do 20 km nebo mimo obec do 30 km');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('17', '§ 125c/1f) 5.', '5', 'Nezastavení na červenou (nebo pokyn "Stůj")');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('18', '§ 125c/1f) 6.a)', '4', 'Ohrožení chodce na přechodu');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('19', '§ 125c/1f) 6.b)', '3', 'Nezastavení před přechodem');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('21', '§ 125c/1f) 6.d)', '5', 'Ohrožení chodce mimo silnici nebo při otáčení nebo couvání');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('22', '§ 125c/1f) 7.', '7', 'Nedovolené předjíždění');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('23', '§ 125c/1f) 8.', '4', 'Nedání přednosti v jízdě');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('24', '§ 125c/1f) 9.', '7', 'Nedovolená jízda přes železniční přejezd');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('25', '§ 125c/1f) 10.', '7', 'Couvání, otáčení nebo jízda v protisměru na dálníci');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('26', '§ 125c/1f) 11.', '0', 'Stání na místě vyhrazeném pro invalidy');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('27', '§ 125c/1g)', '0', 'Nedovolená jízda nákladního automobilu');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('28', '§ 125c/1h)', '0', 'Zavinění nehody s ublížením na zdraví');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('29', '§ 143, 147, 148 TZ', '7', 'Nehoda s usmrcením nebo těžkou újmou na zdraví');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('30', '§ 125c/1i) 1.', '7', 'Nezastavení po nehodě');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('31', '§ 125c/1i) 2.', '0', 'Neohlášení dopravní nehody');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('32', '§ 125c/1i) 3.', '0', 'Neposkytnutí údajů ostatním účastníkům dopravní nehody');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('33', '§ 125c/1i) 4.', '7', 'Ujetí od dopravní nehody');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('34', '§ 125c/1j)', '0', 'Použití antiradaru');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('35', '§ 125c/1k)', '0', 'Jiné porušení povinností');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('36', '§ 125c/1k)', '5', 'Ohrožení při přejíždění z pruhu do pruhu');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('37', '§ 125c/1k)', '3', 'Jízda bez bezpečnostních pásů');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('38', '§ 125c/1k)', '4', 'Nepoužití autosedačky pro dítě');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('39', '§ 125c/1k)', '2', 'Neoznačení překážky způsobené řidičem');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('40', '§ 125c/2)', '0', 'Svěření řízení neznámé osobě');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('41', '§ 23/1f)', '0', 'Řízení bez dokladů nebo záznamů o provozu vozidla');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('42', '§ 23/1f)', '4', 'Nedodržení bezpečnostních přestávek');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('43', '§ 23/1f)', '0', 'Nepředložení záznamu o době řízení a bezpečnostních přestávkách');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('44', '§ 42 a/4 c)', '3', 'Přetížený kamion nebo autobus');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('45', '§ 150 TZ', '7', 'Neposkytnutí první pomoci / nepřivolání záchranky');
INSERT INTO "XSEVCI44"."prestupek" VALUES ('46', '§ 125c/1f) 4.', '0', 'Malé překročení rychlosti - v obci do 5 km nebo mimo obec do 10 km');

-- ----------------------------
-- Indexes structure for table prestupek
-- ----------------------------

-- ----------------------------
-- Checks structure for table "XSEVCI44"."prestupek"
-- ----------------------------
ALTER TABLE "XSEVCI44"."prestupek" ADD CHECK ("id" IS NOT NULL);
ALTER TABLE "XSEVCI44"."prestupek" ADD CHECK ("paragraf" IS NOT NULL);
ALTER TABLE "XSEVCI44"."prestupek" ADD CHECK ("body" IS NOT NULL);

-- ----------------------------
-- Primary Key structure for table "XSEVCI44"."prestupek"
-- ----------------------------
ALTER TABLE "XSEVCI44"."prestupek" ADD PRIMARY KEY ("id");
