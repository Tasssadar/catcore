-- Thorim
DELETE FROM creature WHERE id IN (32892, 32908, 32907, 32885, 32883);
DELETE FROM creature WHERE guid = 136490;

-- Razorscale
UPDATE gameobject_template SET data10 = 0, flags = 48, ScriptName = 'go_razorscale_harpoon' WHERE entry = 194519;
UPDATE gameobject_template SET data10 = 0, flags = 48, ScriptName = 'go_razorscale_harpoon' WHERE entry = 194541;
UPDATE gameobject_template SET data10 = 0, flags = 48, ScriptName = 'go_razorscale_harpoon' WHERE entry = 194542;
UPDATE gameobject_template SET data10 = 0, flags = 48, ScriptName = 'go_razorscale_harpoon' WHERE entry = 194543;
DELETE FROM gameobject WHERE id in (194565, 194519, 194541, 194542, 194543);
INSERT INTO gameobject VALUES
(32, 194519, 603, 2, 1, 607.075, -140.036, 391.487, 4.35077, 0, 0, 0.822736, -0.568424, 25, 0, 1),
(33, 194541, 603, 3, 1, 590.412, -132.929, 391.518, 4.68956, 0, 0, 0.715131, -0.698991, 25, 0, 1),
(34, 194542, 603, 3, 1, 572.174, -134.697, 391.518, 4.83799, 0, 0, 0.661335, -0.75009, 25, 0, 1),
(35, 194543, 603, 2, 1, 559.222, -138.388, 391.517, 5.23182, 0, 0, 0.501804, -0.864982, 25, 0, 1);
DELETE FROM spell_script_target WHERE entry IN (62505);
INSERT INTO spell_script_target VALUES (62505, 1, 33186);
UPDATE `creature_template` SET `scale` = 0.7, `unit_flags` = 33554432 WHERE `entry` = 33184;
DELETE FROM creature WHERE id = 33287;
UPDATE creature_template SET AIName = 'NullAI' WHERE entry = 33287;

-- Ignis
INSERT INTO creature_template_addon VALUES (33118, 0 ,0 ,0,0, 0, 342, NULL, NULL);

-- General Vezax
UPDATE `creature_template` SET `mechanic_immune_mask` = 617299839, `ScriptName` = 'boss_generalvezax' WHERE `entry` = 33271;
UPDATE `creature_template` SET `minlevel` = 83, `maxlevel` = 83, `minhealth` = 1575785, `maxhealth` = 1575785, `faction_A` = 16, `faction_H` = 16, `mindmg` = 412, `maxdmg` = 562, `attackpower` = 536, `mechanic_immune_mask` = 650854271, `ScriptName` = 'boss_saroniteanimus' WHERE `entry` = 33524;
UPDATE `creature_template` SET `minhealth` = 12600, `maxhealth` = 12600, `minlevel` = 80, `maxlevel` = 80, `faction_A` = 32, `faction_H` = 32, `ScriptName` = 'npc_saronitevapors' WHERE `entry` = 33488;

-- XT 002
UPDATE `creature_template` SET `unit_flags` = 32768 WHERE `entry` = 33329;

-- ALGALON
INSERT INTO spell_script_target VALUES (62304, 1, 33104);
INSERT INTO spell_script_target VALUES (64597, 1, 33104);

UPDATE creature_template SET modelid_A = 11686, modelid_A2 = 0, modelid_H = 11686, modelid_H2 = 0, unit_flags = 33554946 WHERE entry in (33105, 33104);

-- kolagarn
DELETE from spell_script_target WHERE targetentry = 32930;
INSERT INTO spell_script_target VALUES (62056, 1, 32930);
INSERT INTO spell_script_target VALUES (63981, 1, 32930);
--INSERT INTO spell_script_target VALUES (63676, 1, 33632);
--INSERT INTO spell_script_target VALUES (63702, 1, 33802);
INSERT INTO spell_script_target VALUES (63676, 1, 32930);
INSERT INTO spell_script_target VALUES (63702, 1, 32930);

UPDATE `mangostest`.`creature_template` SET `mindmg` = '0',
`maxdmg` = '0',
`attackpower` = '0',
`dmg_multiplier` = '0' WHERE `creature_template`.`entry` =33632;

UPDATE `mangostest`.`creature_template` SET `mindmg` = '0',
`maxdmg` = '0',
`attackpower` = '0',
`dmg_multiplier` = '0' WHERE `creature_template`.`entry` =33802;

UPDATE `mangostest`.`creature_model_info` SET `bounding_radius` = '3',
`combat_reach` = '3' WHERE `creature_model_info`.`modelid` =11686;

delete from creature where id in (33802, 33632);
UPDATE creature_template SET scriptname = 'mob_eyebeam' where entry in (33802, 33632);