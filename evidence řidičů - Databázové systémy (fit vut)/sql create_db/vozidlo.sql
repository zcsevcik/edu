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

Date: 2012-03-21 15:19:57
*/


-- ----------------------------
-- Table structure for "XSEVCI44"."vozidlo"
-- ----------------------------
DROP TABLE "XSEVCI44"."vozidlo";
CREATE TABLE "XSEVCI44"."vozidlo" (
"vin" CHAR(17 BYTE) NOT NULL ,
"registrace" DATE NOT NULL 
)
LOGGING
NOCOMPRESS
NOCACHE

;

-- ----------------------------
-- Records of vozidlo
-- ----------------------------
INSERT INTO "XSEVCI44"."vozidlo" VALUES ('WPOZZZ99ZTS392124', TO_DATE('2010-03-22 00:00:00', 'YYYY-MM-DD HH24:MI:SS'));
INSERT INTO "XSEVCI44"."vozidlo" VALUES ('1M8GDM9AXKP042788', TO_DATE('2011-04-05 00:00:00', 'YYYY-MM-DD HH24:MI:SS'));
INSERT INTO "XSEVCI44"."vozidlo" VALUES ('JH4NA1157MT001832', TO_DATE('2010-05-18 00:00:00', 'YYYY-MM-DD HH24:MI:SS'));
INSERT INTO "XSEVCI44"."vozidlo" VALUES ('TMBGDM9A1KP042788', TO_DATE('2009-05-26 00:00:00', 'YYYY-MM-DD HH24:MI:SS'));
INSERT INTO "XSEVCI44"."vozidlo" VALUES ('LJCPCBLCX11000237', TO_DATE('2012-03-16 00:00:00', 'YYYY-MM-DD HH24:MI:SS'));

-- ----------------------------
-- Indexes structure for table vozidlo
-- ----------------------------

-- ----------------------------
-- Checks structure for table "XSEVCI44"."vozidlo"
-- ----------------------------
ALTER TABLE "XSEVCI44"."vozidlo" ADD CHECK ("vin" IS NOT NULL);
ALTER TABLE "XSEVCI44"."vozidlo" ADD CHECK ("registrace" IS NOT NULL);

-- ----------------------------
-- Primary Key structure for table "XSEVCI44"."vozidlo"
-- ----------------------------
ALTER TABLE "XSEVCI44"."vozidlo" ADD PRIMARY KEY ("vin");
