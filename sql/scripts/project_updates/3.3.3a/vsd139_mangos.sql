UPDATE `creature_template` SET `speed_walk` = 1.5, `ScriptName` = 'npc_skywing' WHERE `entry` = 22424;
UPDATE `creature_template` SET `ScriptName` = 'mob_luanga_the_imprisoner' WHERE `entry` = 18533;

DELETE FROM `creature` WHERE `id` = 18533;