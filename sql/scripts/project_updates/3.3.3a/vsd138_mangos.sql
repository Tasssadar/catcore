UPDATE `creature_template` SET `MovementType` = 0, `movementId` = 0, `ScriptName` = 'npc_geezle' WHERE `entry` = 17318;

DELETE FROM `event_scripts` WHERE id = 10675;
INSERT INTO `event_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `datalong3`, `datalong4`, `data_flags`, `dataint`, `x`, `y`, `z`, `o`) 
VALUES('10675','0','10','17318','120000','0','0','0','0','-5144.08','-11248.2','3.455','0');