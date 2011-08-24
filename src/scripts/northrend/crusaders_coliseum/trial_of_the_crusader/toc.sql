-- instance
UPDATE creature SET spawnMask = 15 WHERE map = 649;
UPDATE gameobject SET spawnMask = 15 WHERE map = 649;

-- 1st encounter - Northrend Beastts
UPDATE creature_template SET ScriptName = 'boss_gormok' WHERE entry = 34796;
UPDATE creature_template SET ScriptName = 'npc_snobold' WHERE entry = 34800;
UPDATE creature_template SET ScriptName = 'boss_jormungars' WHERE entry = 35144;
UPDATE creature_template SET ScriptName = 'boss_jormungars' WHERE entry = 34799;
UPDATE creature_template SET ScriptName = 'boss_icehowl' WHERE entry = 34797;

INSERT INTO spell_script_target VALUES (66636, 1, 34796);
UPDATE creature_template SET AIName = 'NullAI' WHERE entry = 34854;
-- TODO: immune masky pro snobolda

-- 2nd encounter - Jaraxxus
UPDATE creature_template SET ScriptName = 'boss_jaraxxus' WHERE entry=34780;
UPDATE creature_template SET ScriptName = 'npc_felflame_infernal' WHERE entry=34815;
UPDATE creature_template SET ScriptName = 'npc_mistress_of_pain' WHERE entry=34826;

-- 4th encounter - Twin Valkyr
UPDATE creature_template SET ScriptName = 'boss_twin_valkyr' WHERE entry IN (34497,34496);
UPDATE creature_template SET ScriptName = 'npc_concentrated' WHERE entry IN (34628,34630);

-- light
INSERT INTO spell_script_target VALUES (65876, 1, 34496);
INSERT INTO spell_script_target VALUES (67306, 1, 34496);
INSERT INTO spell_script_target VALUES (67307, 1, 34496);
INSERT INTO spell_script_target VALUES (67308, 1, 34496);
-- dark
INSERT INTO spell_script_target VALUES (65875, 1, 34497);
INSERT INTO spell_script_target VALUES (67303, 1, 34497);
INSERT INTO spell_script_target VALUES (67304, 1, 34497);
INSERT INTO spell_script_target VALUES (67305, 1, 34497);

-- 5th encounter - Anu'Barak
UPDATE creature_template SET ScriptName = 'boss_anubarak_toc' WHERE entry = 34564;
UPDATE creature_template SET ScriptName = 'mob_burrower' WHERE entry = 34607;
UPDATE creature_template SET ScriptName = 'mob_scarab_toc' WHERE entry = 34605;
UPDATE creature_template SET ScriptName = 'mob_frost_sphere', InhibitType = 7 WHERE entry = 34606;
UPDATE creature_template SET ScriptName = 'mob_anubarak_spike' WHERE entry = 34660;
