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

Date: 2012-03-21 15:19:19
*/


-- ----------------------------
-- Table structure for "XSEVCI44"."ridic"
-- ----------------------------
DROP TABLE "XSEVCI44"."ridic";
CREATE TABLE "XSEVCI44"."ridic" (
"id" NUMBER NOT NULL ,
"jmeno" VARCHAR2(20 BYTE) NOT NULL ,
"prijmeni" VARCHAR2(20 BYTE) NOT NULL ,
"rc" CHAR(11 BYTE) NOT NULL ,
"obec" VARCHAR2(20 BYTE) NOT NULL ,
"adresa" VARCHAR2(20 BYTE) NOT NULL 
)
LOGGING
NOCOMPRESS
NOCACHE

;

-- ----------------------------
-- Records of ridic
-- ----------------------------
INSERT INTO "XSEVCI44"."ridic" VALUES ('0', 'Jan', 'Novák', '440726/0672', 'Brno', 'Cejl 8');
INSERT INTO "XSEVCI44"."ridic" VALUES ('1', 'Petr', 'Veselý', '530610/4532', 'Brno', 'Podzimní 28');
INSERT INTO "XSEVCI44"."ridic" VALUES ('3', 'Pavel', 'Tomek', '510230/0486', 'Brno', 'Tomkova 34');
INSERT INTO "XSEVCI44"."ridic" VALUES ('4', 'Josef', 'Mádr', '580807/9638', 'Brno', 'Svatoplukova 15');
INSERT INTO "XSEVCI44"."ridic" VALUES ('2', 'Ivan', 'Zeman', '601001/2218', 'Brno', 'Cejl 8');
INSERT INTO "XSEVCI44"."ridic" VALUES ('5', 'Jana', 'Malá', '625622/6249', 'Vyškov', 'Brněnská 56');

-- ----------------------------
-- Indexes structure for table ridic
-- ----------------------------

-- ----------------------------
-- Checks structure for table "XSEVCI44"."ridic"
-- ----------------------------
ALTER TABLE "XSEVCI44"."ridic" ADD CHECK ("id" IS NOT NULL);
ALTER TABLE "XSEVCI44"."ridic" ADD CHECK ("jmeno" IS NOT NULL);
ALTER TABLE "XSEVCI44"."ridic" ADD CHECK ("prijmeni" IS NOT NULL);
ALTER TABLE "XSEVCI44"."ridic" ADD CHECK ("rc" IS NOT NULL);
ALTER TABLE "XSEVCI44"."ridic" ADD CHECK ("obec" IS NOT NULL);
ALTER TABLE "XSEVCI44"."ridic" ADD CHECK ("adresa" IS NOT NULL);

-- ----------------------------
-- Primary Key structure for table "XSEVCI44"."ridic"
-- ----------------------------
ALTER TABLE "XSEVCI44"."ridic" ADD PRIMARY KEY ("id");
