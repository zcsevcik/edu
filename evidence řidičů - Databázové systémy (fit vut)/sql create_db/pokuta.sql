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

Date: 2012-03-21 15:19:29
*/


-- ----------------------------
-- Table structure for "XSEVCI44"."pokuta"
-- ----------------------------
DROP TABLE "XSEVCI44"."pokuta";
CREATE TABLE "XSEVCI44"."pokuta" (
"id" NUMBER NOT NULL ,
"datum" DATE NOT NULL ,
"ridic" NUMBER NOT NULL ,
"prestupek" NUMBER NOT NULL ,
"pozn" VARCHAR2(255 BYTE) NULL 
)
LOGGING
NOCOMPRESS
NOCACHE

;

-- ----------------------------
-- Records of pokuta
-- ----------------------------
INSERT INTO "XSEVCI44"."pokuta" VALUES ('0', TO_DATE('2009-06-10 14:45:02', 'YYYY-MM-DD HH24:MI:SS'), '3', '20', null);
INSERT INTO "XSEVCI44"."pokuta" VALUES ('1', TO_DATE('2010-07-16 17:45:48', 'YYYY-MM-DD HH24:MI:SS'), '3', '33', null);
INSERT INTO "XSEVCI44"."pokuta" VALUES ('2', TO_DATE('2011-11-16 08:26:49', 'YYYY-MM-DD HH24:MI:SS'), '3', '5', null);
INSERT INTO "XSEVCI44"."pokuta" VALUES ('0', TO_DATE('2011-12-13 12:47:17', 'YYYY-MM-DD HH24:MI:SS'), '1', '29', null);

-- ----------------------------
-- Indexes structure for table pokuta
-- ----------------------------

-- ----------------------------
-- Checks structure for table "XSEVCI44"."pokuta"
-- ----------------------------
ALTER TABLE "XSEVCI44"."pokuta" ADD CHECK ("id" IS NOT NULL);
ALTER TABLE "XSEVCI44"."pokuta" ADD CHECK ("datum" IS NOT NULL);
ALTER TABLE "XSEVCI44"."pokuta" ADD CHECK ("ridic" IS NOT NULL);
ALTER TABLE "XSEVCI44"."pokuta" ADD CHECK ("prestupek" IS NOT NULL);

-- ----------------------------
-- Primary Key structure for table "XSEVCI44"."pokuta"
-- ----------------------------
ALTER TABLE "XSEVCI44"."pokuta" ADD PRIMARY KEY ("id", "ridic");
