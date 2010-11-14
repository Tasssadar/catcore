/* Item - Druid T8 Restoration 4P Bonus proc */
DELETE FROM spell_proc_event WHERE entry IN (64760);
INSERT INTO spell_proc_event VALUES
(64760,0,7,16,0,0,16384,0,0,100,0);
/* Item - Druid T8 Restoration 4P Bonus bonus */
DELETE FROM spell_bonus_data WHERE entry IN (64801);
INSERT INTO spell_bonus_data VALUES
(64801, 0, 0, 0, "Item - Druid T8 Restoration 4P Bonus");