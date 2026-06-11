/*
 Navicat Premium Dump SQL

 Source Server         : OfficialFiles
 Source Server Type    : MySQL
 Source Server Version : 80041 (8.0.41)
 Source Host           : 192.168.1.163:3306
 Source Schema         : player

 Target Server Type    : MySQL
 Target Server Version : 80041 (8.0.41)
 File Encoding         : 65001

 Date: 10/06/2026 23:50:53
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for monstercard_achiev
-- ----------------------------
DROP TABLE IF EXISTS `monstercard_achiev`;
CREATE TABLE `monstercard_achiev`  (
  `account_id` int NOT NULL,
  `achiev_vnum` int NOT NULL,
  `applied` tinyint(1) NOT NULL DEFAULT 0,
  `regist_rank` tinyint NOT NULL DEFAULT 0,
  PRIMARY KEY (`account_id`, `achiev_vnum`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of monstercard_achiev
-- ----------------------------

SET FOREIGN_KEY_CHECKS = 1;
