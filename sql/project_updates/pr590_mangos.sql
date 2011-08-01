CREATE TABLE `arena_log_2`
( 
   `date` TIMESTAMP NOT NULL ,
   `rat_change` VARCHAR( 10 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
   `winner_team` LONGTEXT CHARSET utf8 NOT NULL ,
   `winner_orig_rat` INT( 4 ) UNSIGNED NOT NULL ,
   `loser_team` LONGTEXT CHARSET utf8 NOT NULL ,
   `loser_orig_rat` INT( 4 ) UNSIGNED NOT NULL ,
   `winner_member_1` VARCHAR( 12 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
   `winner_member_1_ip` VARCHAR( 15 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
   `winner_member_2` VARCHAR( 12 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
   `winner_member_2_ip` VARCHAR( 15 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
   `loser_member_1` VARCHAR( 12 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
   `loser_member_1_ip` VARCHAR( 15 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
   `loser_member_2` VARCHAR( 12 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
   `loser_member_2_ip` VARCHAR( 15 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
   `arena_duration` INT( 4 ) NOT NULL DEFAULT '0'
);

CREATE TABLE `arena_log_3`
( 
   `date` TIMESTAMP NOT NULL ,
   `rat_change` VARCHAR( 10 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
   `winner_team` LONGTEXT CHARSET utf8 NOT NULL ,
   `winner_orig_rat` INT( 4 ) UNSIGNED NOT NULL ,
   `loser_team` LONGTEXT CHARSET utf8 NOT NULL ,
   `loser_orig_rat` INT( 4 ) UNSIGNED NOT NULL ,
   `winner_member_1` VARCHAR( 12 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
   `winner_member_1_ip` VARCHAR( 15 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
   `winner_member_2` VARCHAR( 12 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
   `winner_member_2_ip` VARCHAR( 15 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
   `winner_member_3` VARCHAR( 12 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
   `winner_member_3_ip` VARCHAR( 15 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
   `loser_member_1` VARCHAR( 12 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
   `loser_member_1_ip` VARCHAR( 15 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
   `loser_member_2` VARCHAR( 12 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
   `loser_member_2_ip` VARCHAR( 15 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
   `loser_member_3` VARCHAR( 12 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
   `loser_member_3_ip` VARCHAR( 15 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
   `arena_duration` INT( 4 ) NOT NULL DEFAULT '0'
);

CREATE TABLE `arena_log_5`
( 
   `date` TIMESTAMP NOT NULL ,
   `rat_change` VARCHAR( 10 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
   `winner_team` LONGTEXT CHARSET utf8 NOT NULL ,
   `winner_orig_rat` INT( 4 ) UNSIGNED NOT NULL ,
   `loser_team` LONGTEXT CHARSET utf8 NOT NULL ,
   `loser_orig_rat` INT( 4 ) UNSIGNED NOT NULL ,
   `winner_member_1` VARCHAR( 12 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
   `winner_member_1_ip` VARCHAR( 15 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
   `winner_member_2` VARCHAR( 12 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
   `winner_member_2_ip` VARCHAR( 15 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
   `winner_member_3` VARCHAR( 12 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
   `winner_member_3_ip` VARCHAR( 15 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
   `winner_member_4` VARCHAR( 12 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
   `winner_member_4_ip` VARCHAR( 15 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
   `winner_member_5` VARCHAR( 12 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
   `winner_member_5_ip` VARCHAR( 15 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
   `loser_member_1` VARCHAR( 12 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
   `loser_member_1_ip` VARCHAR( 15 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
   `loser_member_2` VARCHAR( 12 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
   `loser_member_2_ip` VARCHAR( 15 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
   `loser_member_3` VARCHAR( 12 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
   `loser_member_3_ip` VARCHAR( 15 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
   `loser_member_4` VARCHAR( 12 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
   `loser_member_4_ip` VARCHAR( 15 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
   `loser_member_5` VARCHAR( 12 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL ,
   `loser_member_5_ip` VARCHAR( 15 ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
   `arena_duration` INT( 4 ) NOT NULL DEFAULT '0'
);