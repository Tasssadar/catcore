UPDATE `creature_template` SET `minlevel` = 78, `maxlevel` = 78, `minhealth` = 5885, `maxhealth` = 5885, `armor` = 10000, `faction_A` = 14, `faction_H` = 14, `mindmg` = 2000, `maxdmg` = 3000, `attackpower` = 750, `resistance1` = 20, `resistance2` = 20, `resistance3` = 20, `resistance4` = 20, `resistance5` = 20, `resistance6` = 20, `mechanic_immune_mask` = 8388624 WHERE `entry` = 29713;

UPDATE `creature_template` SET `armor` = 10000, `faction_A` = 14, `faction_H` = 14, `mindmg` = 4000, `maxdmg` = 6000, `attackpower` = 750, `resistance1` = 50, `resistance2` = 50, `resistance3` = 50, `resistance4` = 50, `resistance5` = 50, `resistance6` = 50, `mechanic_immune_mask` = 8388624 WHERE `entry` = 30943;

UPDATE `creature_template` SET `minlevel` = 77, `maxlevel` = 77, `minhealth` = 4552, `maxhealth` = 4552, `armor` = 10000, `faction_A` = 14, `faction_H` = 14, `mindmg` = 3000, `maxdmg` = 4000, `attackpower` = 750, `resistance1` = 30, `resistance2` = 30, `resistance3` = 30, `resistance4` = 30, `resistance5` = 30, `resistance6` = 30, `mechanic_immune_mask` = 8388624 WHERE `entry` = 29680;

UPDATE `creature_template` SET `armor` = 10000, `faction_A` = 14, `faction_H` = 14, `mindmg` = 5000, `maxdmg` = 6000, `attackpower` = 750, `resistance1` = 50, `resistance2` = 50, `resistance3` = 50, `resistance4` = 50, `resistance5` = 50, `resistance6` = 50, `mechanic_immune_mask` = 8388624 WHERE `entry` = 30940;

UPDATE `creature_template` SET ScriptName = 'boss_gal_darah' WHERE `entry` IN (29306,31368);
UPDATE `creature_template` SET ScriptName = 'boss_drakkari_colossus' WHERE `entry` IN (29307,31365);
UPDATE `creature_template` SET ScriptName = 'boss_drakkari_elemental' WHERE `entry` IN (29573,31367);
UPDATE `creature_template` SET ScriptName = 'npc_living_mojo' WHERE `entry` IN (29830,30938);
UPDATE `creature_template` SET ScriptName = 'boss_slad_ran' WHERE `entry` IN (29304,31370);
UPDATE `creature_template` SET ScriptName = 'npc_ruins_dweller' WHERE `entry` IN (29920,30939);
UPDATE `creature_template` SET ScriptName = 'boss_eck' WHERE `entry` = 29932;

-- 9.7.2010 12:49:23
DELETE FROM `gameobject` WHERE `id`=192633;

-- 9.7.2010 16:43:46
DELETE FROM `gameobject` WHERE `id`=193188;

UPDATE `creature_template` SET `minlevel` = 73, `maxlevel` = 73, `minhealth` = 117700, `maxhealth` = 117700, `minmana` = 3809, `maxmana` = 3809, `armor` = 10000, `faction_A` = 14, `faction_H` = 14, `mindmg` = 5000, `maxdmg` = 8000, `attackpower` = 750, `resistance1` = 70, `resistance2` = 70, `resistance3` = 70, `resistance4` = 70, `resistance5` = 70, `resistance6` = 70, `mechanic_immune_mask` = 113974106 WHERE `entry` = 29573;

UPDATE `creature_template` SET `armor` = 10000, `faction_A` = 14, `faction_H` = 14, `mindmg` = 8000, `maxdmg` = 10000, `attackpower` = 750, `resistance1` = 70, `resistance2` = 70, `resistance3` = 70, `resistance4` = 70, `resistance5` = 70, `resistance6` = 70, `mechanic_immune_mask` = 113974106 WHERE `entry` = 31367;

UPDATE `creature_template` SET `LootID`= 29932 WHERE `entry` = 29932;
DELETE FROM `creature_loot_template` WHERE `entry` = 29932;
INSERT INTO `creature_loot_template` (`entry`,`item`,`ChanceOrQuestChance`,`Groupid`,`mincountOrRef`,`maxcount`,`lootcondition`,`condition_value1`,`condition_value2`) VALUES
(29932, 43312, 0, 1, 1, 1, 0, 0, 0),
(29932, 43311, 0, 1, 1, 1, 0, 0, 0),
(29932, 43313, 0, 1, 1, 1, 0, 0, 0),
(29932, 43310, 0, 1, 1, 1, 0, 0, 0),
(29932, 43228, 24, 2, 4, 4, 0, 0, 0),
(29932, 33470, 4.9, 0, 2, 7, 0, 0, 0),
(29932, 43852, 2.7, 0, 1, 1, 0, 0, 0),
(29932, 33454, 1.2, 0, 1, 1, 0, 0, 0),
(29932, 33445, 0.6, 0, 1, 1, 0, 0, 0),
(29932, 33447, 0.3, 0, 1, 1, 0, 0, 0);