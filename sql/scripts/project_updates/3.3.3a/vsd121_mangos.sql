UPDATE creature_template SET ScriptName = 'boss_freya' WHERE entry = 32906;
UPDATE creature_template SET ScriptName = 'boss_elder_stonebark' WHERE entry = 32914;
UPDATE creature_template SET ScriptName = 'boss_elder_ironbranch' WHERE entry = 32913;
UPDATE creature_template SET ScriptName = 'boss_elder_brightleaf' WHERE entry = 32915;
UPDATE creature_template SET ScriptName = 'mob_freya_ground' WHERE entry IN (34129,33228, 33215, 33170, 33050);
UPDATE creature_template SET ScriptName = 'mob_iron_roots' WHERE entry IN (33088);
UPDATE creature_template SET ScriptName = 'mob_str_iron_roots' WHERE entry IN (33168);
UPDATE creature_template SET ScriptName = 'mob_freya_spawned' WHERE entry IN (33202, 32916, 32919, 33203,32918);
INSERT IGNORE INTO spell_script_target VALUES (62385, 1, 32906);
INSERT IGNORE INTO spell_script_target VALUES (62387, 1, 32906);
INSERT IGNORE INTO spell_script_target VALUES (62386, 1, 32906);
INSERT IGNORE INTO spell_script_target VALUES (62584, 1, 32906);
INSERT IGNORE INTO spell_script_target VALUES (62619, 1, 32906);
INSERT IGNORE INTO spell_script_target VALUES (63294, 1, 32906);
INSERT IGNORE INTO spell_script_target VALUES (63292, 1, 32906);
INSERT IGNORE INTO spell_script_target VALUES (63295, 1, 32906);
INSERT IGNORE INTO spell_script_target VALUES (63295, 1, 32906);
INSERT IGNORE INTO spell_script_target VALUES (63292, 1, 32906);
INSERT IGNORE INTO spell_script_target VALUES (62584, 1, 32906);
INSERT IGNORE INTO spell_script_target VALUES (63947, 1, 32906);
INSERT IGNORE INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `ScriptName`) VALUES('194324','3','8628','Freya\'s Gift','','','','0','0','1','0','0','0','0','0','0','1634','0','0','1','0','0','0','0','0','0','0','1','0','1','0','1','0','0','0','0','0','0','0','0','');
INSERT IGNORE INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `ScriptName`) VALUES('194325','3','8628','Freya\'s Gift','','','','0','0','1','0','0','0','0','0','0','1634','0','0','1','0','0','0','0','0','0','0','1','0','1','0','1','0','0','0','0','0','0','0','0','');
INSERT IGNORE INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `ScriptName`) VALUES('194326','3','8628','Freya\'s Gift','','','','0','0','1','0','0','0','0','0','0','1634','0','0','1','0','0','0','0','0','0','0','1','0','1','0','1','0','0','0','0','0','0','0','0','');
INSERT IGNORE INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `ScriptName`) VALUES('194327','3','8628','Freya\'s Gift','','','','0','0','1','0','0','0','0','0','0','1634','0','0','1','0','0','0','0','0','0','0','1','0','1','0','1','0','0','0','0','0','0','0','0','');
UPDATE `creature_template` SET scale = 0.1 WHERE entry = 33228;
UPDATE `creature` SET `MovementType` = 0 WHERE `guid` = 136554;
UPDATE `creature_template` SET `mechanic_immune_mask` = 650854271 WHERE `entry` = 32906;
UPDATE `creature` SET MovementType = 0 WHERE guid = 136554;
UPDATE `creature_template` SET `flags_extra` = 384 WHERE `entry` = 34129;
UPDATE `creature_template` SET `flags_extra` = 2 WHERE `entry` = 32918;
-- 194324 - normal, 194325 - normal hard, 194326 - hc, 194327 - hc hard

-- Ulduar: Freya yells
DELETE FROM scriptdev2.script_texts WHERE entry BETWEEN  -1700173 AND -1700152;
INSERT INTO scriptdev2.script_texts (entry,content_default,sound,TYPE,emote,COMMENT) VALUES
(-1700152,'The Conservatory must be protected!',15526,1,0,'freya - aggro'),
(-1700153,'Elders, grant me your strength!',15527,1,0,'freya - hardmode aggro'),
(-1700154,'Eonar, your servant requires aid!',15528,1,0,'freya - summon conservator'),
(-1700155,'Children, assist me!',15533,1,0,'freya - summon elementals'),
(-1700156,'The swarm of the elements shall overtake you!',15534,1,0,'freya - summon lashers'),
(-1700157,'Forgive me.',15529,1,0,'freya - slay 01'),
(-1700158,'From your death springs life anew!',15530,1,0,'freya - slay 02'),
(-1700159,'You have strayed too far, wasted too much time!',15532,1,0,'freya - berserk'),
(-1700160,'His hold on me dissipates. I can see clearly once more. Thank you, heroes.',15531,1,0,'freya - defeated'),
(-1700161,'Eonar, your servant calls for your blessing!',15535,1,0,'freya - yoggsaron event'),

-- Ulduar: Elder Brightleaf yells
(-1700162,'Matron, the Conservatory has been breached!',15483,1,0,'brightleaf - aggro'),
(-1700163,'Fertilizer.',15485,1,0,'brightleaf - slay 01'),
(-1700164,'Your corpse will nourish the soil!',15486,1,0,'brightleaf - slay 0s2'),
(-1700165,'Matron, one has fallen!',15487,1,0,'brightleaf - death'),

-- Ulduar: Elder Stonebark yells
(-1700166,'This place will serve as your graveyard.',15500,1,0,'stonebark - aggro'),
(-1700167,'<Angry roar>',15501,3,0,'stonebark - slay 01'),
(-1700168,'Such a waste.',15502,1,0,'stonebark - slay 02'),
(-1700169,'Matron, flee! They are ruthless....',15503,1,0,'stonebark - death'),

-- Ulduar: Elder Ironbranch yells
(-1700170,'Mortals have no place here!',15493,1,0,'ironbranch - aggro'),
(-1700171,'I return you whence you came!',15494,1,0,'ironbranch - slay 01'),
(-1700172,'BEGONE!',15495,1,0,'ironbranch - slay 02'),
(-1700173,'Freya! They come for you.',15496,1,0,'ironbranch - death');

DELETE FROM mangos.gameobject_loot_template WHERE entry IN (194324,194325,194326,194327);
-- Freya - 10-man  emblemy - Conquest
INSERT INTO gameobject_loot_template VALUES 
(194324,45624,100,0,2,2,0,0,0),
(194324,45788,-100,7,1,1,0,0,0),
(194324,45087,18,2,1,1,0,0,0),
(194324,46110,63,8,1,1,0,0,0),
(194324,46348,2,4,1,1,0,0,0),
(194324,46027,2,5,1,1,0,0,0),
(194324,82105,100,6,-82105,1,0,0,0),
(194324,82104,100,3,-82104,2,0,0,0),
-- 10 hard mode
(194325,45624,100,0,2,2,0,0,0),
(194325,45788,-100,7,1,1,0,0,0),
(194325,45087,18,2,1,1,0,0,0),
(194325,46110,63,8,1,1,0,0,0),
(194325,46348,2,4,1,1,0,0,0),
(194325,46027,2,5,1,1,0,0,0),
(194325,82105,100,6,-82105,1,0,0,0),
(194325,82106,100,3,-82106,2,0,0,0),
-- 25 man
(194326,45624,100,0,2,2,0,0,0),
(194326,45814,-100,7,1,1,0,0,0),
(194326,45087,36,2,1,3,0,0,0),
(194326,46110,70,8,1,1,0,0,0),
(194326,46348,2,4,1,1,0,0,0),
(194326,46027,2,5,1,1,0,0,0),
(194326,82107,100,6,-82107,2,0,0,0),
(194326,82108,100,3,-82108,3,0,0,0),
-- 25 man Hard mode
(194327,45624,100,0,2,2,0,0,0),
(194327,45814,-100,7,1,1,0,0,0),
(194327,45087,36,2,1,3,0,0,0),
(194327,46110,70,8,1,1,0,0,0),
(194327,46348,2,4,1,1,0,0,0),
(194327,46027,2,5,1,1,0,0,0),
(194327,82107,100,6,-82107,2,0,0,0),
(194327,82109,100,3,-82109,4,0,0,0);

DELETE FROM reference_loot_template WHERE entry IN (82104,82105,82106,82107,82108,82109);

INSERT INTO reference_loot_template VALUES
-- Freya - 10-man 
(82105, 45646, 0, 6, 1, 1, 0, 0, 0),
(82105, 45644, 0, 6, 1, 1, 0, 0, 0),
(82105, 45645, 0, 6, 1, 1, 0, 0, 0),
(82104, 45935, 0, 3, 1, 1, 0, 0, 0),
(82104, 45936, 0, 3, 1, 1, 0, 0, 0),
(82104, 45940, 0, 3, 1, 1, 0, 0, 0),
(82104, 45934, 0, 3, 1, 1, 0, 0, 0),
(82104, 45941, 0, 3, 1, 1, 0, 0, 0),
-- 10 hard mode
(82106, 45946, 0, 3, 1, 1, 0, 0, 0),
(82106, 45943, 0, 3, 1, 1, 0, 0, 0),
(82106, 45945, 0, 3, 1, 1, 0, 0, 0),
(82106, 45947, 0, 3, 1, 1, 0, 0, 0),
(82106, 45294, 0, 3, 1, 1, 0, 0, 0),
-- 25 normal
(82107, 45655, 0, 6, 1, 1, 0, 0, 0),
(82107, 45654, 0, 6, 1, 1, 0, 0, 0),
(82107, 45653, 0, 6, 1, 1, 0, 0, 0),
(82108, 45482, 0, 3, 1, 1, 0, 0, 0),
(82108, 45483, 0, 3, 1, 1, 0, 0, 0),
(82108, 45481, 0, 3, 1, 1, 0, 0, 0),
(82108, 45480, 0, 3, 1, 1, 0, 0, 0),
(82108, 45479, 0, 3, 1, 1, 0, 0, 0),
-- 25 hard mode
(82109, 45613, 0, 3, 1, 1, 0, 0, 0),
(82109, 45484, 0, 3, 1, 1, 0, 0, 0),
(82109, 45485, 0, 3, 1, 1, 0, 0, 0),
(82109, 45487, 0, 3, 1, 1, 0, 0, 0),
(82109, 45486, 0, 3, 1, 1, 0, 0, 0),
(82109, 45488, 0, 3, 1, 1, 0, 0, 0);

UPDATE gameobject_template SET data1=194324 where entry=194324;
UPDATE gameobject_template SET data1=194325 where entry=194325;
UPDATE gameobject_template SET data1=194326 where entry=194326;
UPDATE gameobject_template SET data1=194327 where entry=194327;