INSERT INTO `creature_template_addon` (`entry` ,`mount` ,`bytes1` ,`bytes2` ,`emote` ,`moveflags` ,`vehicle_id` ,`passengers` ,`auras` )VALUES
('33113', '0', '0', '0', '0', '0', '340', NULL , NULL ),
('33114', '0', '0', '0', '0', '0', '341', NULL , NULL );

INSERT INTO `vehicle_seat_data` (`seat`, `flags`) VALUES
('3043', '0'), ('3065', '0'), ('3071', '0'), ('3072', '0'),
('3070', '0'), ('3044', '0'), ('3075', '0'), ('3076', '0');

REPLACE INTO `creature_template_addon` (
`entry` ,
`mount` ,
`bytes1` ,
`bytes2` ,
`emote` ,
`moveflags` ,
`vehicle_id` ,
`passengers` ,
`auras` 
)
VALUES (
'33060', '0', '0', '0', '0', '0', '336', NULL , NULL 
), (
'33062', '0', '0', '0', '0', '0', '335', NULL , NULL 
), (
'33067', '0', '0', '0', '0', '0', '337', NULL , NULL 
), (
'33109', '0', '0', '0', '0', '0', '338', NULL , NULL 
), (
'33167', '0', '0', '0', '0', '0', '345', NULL , NULL 
);

REPLACE INTO `vehicle_data` (
`entry` ,
`flags` ,
`Spell1` ,
`Spell2` ,
`Spell3` ,
`Spell4` ,
`Spell5` ,
`Spell6` ,
`Spell7` ,
`Spell8` ,
`Spell9` ,
`Spell10` ,
`req_aura` 
)
VALUES (
'336', '0', '62345', '62522', '62346', '0', '0', '0', '0', '0', '0', '0', '0'
), (
'337', '1', '64979', '62358', '64677', '0', '0', '0', '0', '0', '0', '0', '0'
), (
'335', '0', '62299', '62974', '62286', '64660', '0', '0', '0', '0', '0', '0', '0'
), (
'338', '0', '62306', '62490', '62308', '62324', '0', '0', '0', '0', '0', '0', '0'
), (
'345', '1', '62634', '64979', '62482', '62471', '64414', '0', '0', '0', '0', '0', '0'
);

REPLACE INTO `mangostest`.`vehicle_seat_data` (
`seat` ,
`flags` 
)
VALUES (
'3006', '3'
), (
'4026', '2'
), (
'4027', '2'
), (
'3009', '2'
), (
'3005', '3'
), (
'3004', '2'
), (
'3010', '3'
), (
'3011', '3'
), (
'3146', '2'
), (
'3013', '2'
), (
'3147', '2'
), (
'3077', '3'
), (
'3106', '10'
);

UPDATE`npc_spellclick_spells` SET `spell_id`= 60968 WHERE npc_entry IN (33060, 33062, 33067, 33109, 33167);


UPDATE `creature_template` SET `InhabitType` = '4' WHERE `entry`=30702;