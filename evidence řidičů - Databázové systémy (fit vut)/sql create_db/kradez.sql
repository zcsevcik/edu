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

Date: 2012-03-21 15:19:36
*/


-- ----------------------------
-- Table structure for "XSEVCI44"."kradez"
-- ----------------------------
DROP TABLE "XSEVCI44"."kradez";
CREATE TABLE "XSEVCI44"."kradez" (
"id" NUMBER NOT NULL ,
"vozidlo" CHAR(17 BYTE) NOT NULL ,
"misto" VARCHAR2(20 BYTE) NOT NULL ,
"datum" DATE NOT NULL ,
"pozn" VARCHAR2(255 BYTE) NULL 
)
LOGGING
NOCOMPRESS
NOCACHE

;

-- ----------------------------
-- Records of kradez
-- ----------------------------
INSERT INTO "XSEVCI44"."kradez" VALUES ('0', 'JH4NA1157MT001832', 'Praha', TO_DATE('2011-08-13 00:00:00', 'YYYY-MM-DD HH24:MI:SS'), null);

-- ----------------------------
-- Indexes structure for table kradez
-- ----------------------------

-- ----------------------------
-- Checks structure for table "XSEVCI44"."kradez"
-- ----------------------------
ALTER TABLE "XSEVCI44"."kradez" ADD CHECK ("id" IS NOT NULL);
ALTER TABLE "XSEVCI44"."kradez" ADD CHECK ("vozidlo" IS NOT NULL);
ALTER TABLE "XSEVCI44"."kradez" ADD CHECK ("misto" IS NOT NULL);
ALTER TABLE "XSEVCI44"."kradez" ADD CHECK ("datum" IS NOT NULL);

-- ----------------------------
-- Primary Key structure for table "XSEVCI44"."kradez"
-- ----------------------------
ALTER TABLE "XSEVCI44"."kradez" ADD PRIMARY KEY ("id", "vozidlo");
