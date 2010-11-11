/* Item - Paladin T8 Holy 2P Bonus proc */
DELETE FROM spell_proc_event WHERE entry IN (64890);
INSERT INTO spell_proc_event VALUES
(64890,0,10,0,65536,0,0,0,0,0,0);
/* Item - Paladin T8 Holy 2P Bonus bonus */
DELETE FROM spell_bonus_data WHERE entry IN (64891);
INSERT INTO spell_bonus_data VALUES
(64891, 0, 0, 0, "Item - Paladin T8 Holy 2P Bonus");