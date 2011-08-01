UPDATE `creature_template` SET `minhealth`='8600', `maxhealth`='8600' WHERE `entry` IN (31127, 31126, 28167, 28169);
UPDATE `creature_template` SET `minhealth`='8600', `maxhealth`='8600' WHERE `entry` IN (10002, 10003, 10004, 10005);
UPDATE `creature_template` SET `faction_A`='35', `faction_H`='35' WHERE `entry` IN (31127, 31126, 28167, 28169);
UPDATE `creature_template` SET `faction_A`='35', `faction_H`='35' WHERE `entry` IN (10002, 10003, 10004, 10005);
UPDATE `creature_template` SET `minhealth`='15000', `maxhealth`='15000' WHERE `entry` IN (27737);
UPDATE `creature_template` SET `minhealth`='25000', `maxhealth`='25000' WHERE `entry` IN (31208);

DELETE FROM `creature` WHERE (`id`=27744);
DELETE FROM `creature` WHERE (`id`=26530);
DELETE FROM `creature` WHERE (`id`=26529);
DELETE FROM `creature` WHERE (`id`=26532);
DELETE FROM `creature` WHERE (`id`=26533);

UPDATE `creature_template` SET `AIName`='', `Scriptname`='npc_arthas' WHERE `entry` IN (26499);
UPDATE `creature_template` SET `AIName`='', `Scriptname`='dark_conversion' WHERE `entry` IN (31127, 31126, 28167, 28169);
UPDATE `creature_template` SET `AIName`='', `Scriptname`='npc_patricia' WHERE `entry` IN (31028);
UPDATE `creature_template` SET `AIName`='', `Scriptname`='npc_time_riftCS' WHERE `entry` IN (28409);
UPDATE `creature_template` SET `AIName`='', `Scriptname`='boss_salramm' WHERE `entry` IN (26530);
UPDATE `creature_template` SET `AIName`='', `Scriptname`='boss_meathook' WHERE `entry` IN (26529);
UPDATE `creature_template` SET `AIName`='', `Scriptname`='boss_lord_epoch' WHERE `entry` IN (26532);
UPDATE `creature_template` SET `AIName`='', `Scriptname`='boss_malganis' WHERE `entry` IN (26533);
UPDATE `instance_template` SET `script` = 'instance_culling_of_stratholme' WHERE map=595;
 
UPDATE `creature` SET `spawntimesecs`='3600' WHERE `id` IN (31127, 31126, 28167, 28169);

DELETE FROM `creature` WHERE (`guid`=4456649);
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) VALUES (4456649, 26499, 595, 1, 0, 0, 1920.87, 1287.12, 142.935, 6.25562, 25, 0, 0, 44100, 7988, 0, 0);

DELETE FROM `creature` WHERE (`guid`=4456653);
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) VALUES (4456653, 26497, 595, 1, 0, 0, 1895.48, 1292.66, 143.706, 0.023475, 25, 0, 0, 100800, 88140, 0, 0);

DELETE FROM `gameobject_template` WHERE (`entry`=188686);
INSERT INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `castBarCaption`, `faction`, `flags`, `size`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `ScriptName`) VALUES (188686, 0, 7831, 'Doodad_LD_hidden_door_room01', '', 1375, 6553632, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '');

DELETE FROM `gameobject_template` WHERE (`entry`=187711);
INSERT INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `castBarCaption`, `faction`, `flags`, `size`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `ScriptName`) VALUES (187711, 0, 3631, 'Crusaders\' Square Gate', '', 1375, 6553632, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '');

DELETE FROM `gameobject_template` WHERE (`entry`=187723);
INSERT INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `castBarCaption`, `faction`, `flags`, `size`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `ScriptName`) VALUES (187723, 0, 3631, 'Crusaders\' Square Gate', '', 1375, 6553632, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '');

UPDATE `gameobject` SET `state` = '1' WHERE `id` IN (187711);
UPDATE `gameobject` SET `spawntimesecs` = '-604800', `state` = '0' WHERE `id` IN (190663);
UPDATE gameobject_template SET faction = 0 WHERE entry=190663;