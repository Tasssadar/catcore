DELETE FROM creature where map=616;

INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) VALUES
(132309, 30118, 616, 3, 1, 14501, 0, 652.417, 1200.52, 295.972, 0.785398, 3600, 0, 0, 7212, 0, 0, 0),
(132310, 30118, 616, 3, 1, 14501, 0, 847.67, 1408.05, 295.972, 3.97935, 3600, 0, 0, 7212, 0, 0, 0),
(132311, 30118, 616, 3, 1, 14501, 0, 647.675, 1403.8, 295.972, 5.53269, 3600, 0, 0, 7212, 0, 0, 0),
(132312, 30118, 616, 3, 1, 14501, 0, 843.182, 1215.42, 295.972, 2.35619, 3600, 0, 0, 7212, 0, 0, 0),
(9334996, 22517, 616, 3, 1, 0, 0, 754.421, 1302.09, 266.171, 4.6472, 25, 0, 0, 2500, 0, 0, 0),
(9336942, 28859, 616, 3, 1, 0, 0, 821.641, 1220.77, 296.243, 2.32949, 25, 0, 0, 6972500, 212900, 0, 0);


UPDATE creature_template SET faction_A=35, faction_H=35 WHERE entry=30118;

UPDATE `creature_template` SET `difficulty_entry_1` = '31734' WHERE `entry` =28859;
 
UPDATE `creature_template` SET `minmana` = '212900',
`maxmana` = '212900',
`armor` = '12300',
`mindmg` = '657',
`maxdmg` = '743',
`attackpower` = '823',
`dmg_multiplier` = '35',
`unit_flags` = '0',
`InhabitType` = '7',
`mechanic_immune_mask` = '617299803',
`flags_extra` = '1',
`ScriptName` = 'boss_malygos' WHERE `entry` =31734;

UPDATE `creature_template` SET `faction_A` = '16',
`faction_H` = '16' WHERE `entry` =31750;

UPDATE `creature_template` SET `faction_A` = '16',
`faction_H` = '16',`InhabitType` = '4' WHERE `entry` =31751;

REPLACE INTO `gameobject` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES
(2024, 193905, 616, 1, 1, 754.165, 1300.88, 268.271, 0.0463451, 0, 0, 0.0231705, 0.999732, -604800, 0, 1),
(2032, 193967, 616, 2, 1, 754.165, 1300.88, 268.271, 0.0463451, 0, 0, 0.0231705, 0.999732, -604800, 0, 1),
(67449, 193070, 616, 3, 1, 754.346, 1300.87, 256.249, 3.14159, 0, 0, 0, 1, 180, 0, 1),
(67450, 193908, 616, 3, 1, 724.684, 1332.92, 267.234, -0.802851, 0, 0, 0, 1, 180, 0, 1),
(14531552, 193958, 616, 1, 1, 759.731, 1306.12, 266.171, 5.01343, 0, 0, 0.593077, -0.805146, 25, 0, 1),
(14531553, 193960, 616, 2, 1, 759.731, 1306.12, 266.171, 5.01343, 0, 0, 0.593077, -0.805146, 25, 0, 1);

UPDATE `creature_template` SET `minlevel` = '80',
`maxlevel` = '80',
`minhealth` = '37800',
`maxhealth` = '37800',
`faction_A` = '16',
`faction_H` = '16',
`InhabitType` = '7' WHERE `entry` =32187;