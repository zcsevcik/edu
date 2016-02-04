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

Date: 2012-03-21 15:19:50
*/


-- ----------------------------
-- Table structure for "XSEVCI44"."spz"
-- ----------------------------
DROP TABLE "XSEVCI44"."spz";
CREATE TABLE "XSEVCI44"."spz" (
"cislo" CHAR(7 BYTE) NOT NULL ,
"pridelena_od" DATE NOT NULL ,
"pridelena_do" DATE NULL ,
"vozidlo" CHAR(17 BYTE) NOT NULL ,
"majitel" NUMBER NOT NULL 
)
LOGGING
NOCOMPRESS
NOCACHE

;

-- ----------------------------
-- Records of spz
-- ----------------------------
INSERT INTO "XSEVCI44"."spz" VALUES ('4A23001', TO_DATE('2009-05-30 00:00:00', 'YYYY-MM-DD HH24:MI:SS'), TO_DATE('2010-12-18 00:00:00', 'YYYY-MM-DD HH24:MI:SS'), 'TMBGDM9A1KP042788', '3');
INSERT INTO "XSEVCI44"."spz" VALUES ('2AU5402', TO_DATE('2011-01-03 00:00:00', 'YYYY-MM-DD HH24:MI:SS'), null, 'WPOZZZ99ZTS392124', '3');
INSERT INTO "XSEVCI44"."spz" VALUES ('3J57942', TO_DATE('2011-06-15 00:00:00', 'YYYY-MM-DD HH24:MI:SS'), TO_DATE('2011-12-20 00:00:00', 'YYYY-MM-DD HH24:MI:SS'), '1M8GDM9AXKP042788', '1');
INSERT INTO "XSEVCI44"."spz" VALUES ('6T99994', TO_DATE('2010-07-01 00:00:00', 'YYYY-MM-DD HH24:MI:SS'), null, 'JH4NA1157MT001832', '0');

-- ----------------------------
-- Indexes structure for table spz
-- ----------------------------

-- ----------------------------
-- Checks structure for table "XSEVCI44"."spz"
-- ----------------------------
ALTER TABLE "XSEVCI44"."spz" ADD CHECK ("cislo" IS NOT NULL);
ALTER TABLE "XSEVCI44"."spz" ADD CHECK ("pridelena_od" IS NOT NULL);
ALTER TABLE "XSEVCI44"."spz" ADD CHECK ("vozidlo" IS NOT NULL);
ALTER TABLE "XSEVCI44"."spz" ADD CHECK ("majitel" IS NOT NULL);

-- ----------------------------
-- Primary Key structure for table "XSEVCI44"."spz"
-- ----------------------------
ALTER TABLE "XSEVCI44"."spz" ADD PRIMARY KEY ("cislo");
