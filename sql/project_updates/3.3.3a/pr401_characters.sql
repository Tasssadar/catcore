ALTER TABLE `character_loginequip` ADD `into_bag` TINYINT( 3 ) NOT NULL DEFAULT '0';

DELETE FROM `character_learnspells` WHERE class=8;
INSERT INTO `character_learnspells` (`class`, `team`, `trainer_entry`, `spell`) VALUES
(8, 469, 28958, 0),
(8, 67, 28958, 0),
(8, 0, 0, 53142),
(8, 469, 4165, 0),
(8, 469, 0, 33691),
(8, 469, 0, 33690),
(8, 469, 2485, 0),
(8, 469, 16755, 0),
(8, 469, 2489, 0),
(8, 469, 27703, 0),
(8, 67, 5957, 0),
(8, 67, 0, 35717),
(8, 67, 0, 35715),
(8, 67, 2492, 0),
(8, 67, 27705, 0),
(8, 67, 16654, 0),
(8, 67, 5958, 0),
(8, 0, 28958, 0),
(8, 0, 0, 53140);

DELETE FROM `character_loginequip` WHERE class=8;
INSERT INTO `character_loginequip` (`class`, `level`, `slot`, `entry`, `count`, `into_bag`) VALUES
(8, 78, 0, 43971, 1, 0),
(8, 78, 3, 23473, 1, 0),
(8, 78, 24, 44607, 20, 1),
(8, 78, 9, 41516, 1, 0),
(8, 78, 5, 43969, 1, 0),
(8, 78, 7, 43970, 1, 0),
(8, 78, 4, 43972, 1, 0),
(8, 78, 2, 43973, 1, 0),
(8, 78, 8, 43974, 1, 0),
(8, 78, 6, 43975, 1, 0),
(8, 78, 15, 36975, 1, 0),
(8, 78, 17, 37824, 1, 0),
(8, 78, 14, 36983, 1, 0),
(8, 78, 10, 37058, 1, 0),
(8, 78, 11, 37079, 1, 0),
(8, 78, 12, 36972, 1, 0),
(8, 78, 13, 38358, 1, 0),
(8, 78, 1, 36988, 1, 0),
(8, 78, 25, 42777, 20, 1);