-- instance
UPDATE creature SET spawnMask = 15 WHERE map = 649;
UPDATE gameobject SET spawnMask = 15 WHERE map = 649;
DELETE FROM creature WHERE id IN (35651, 35458, 36095, 36097, 35820, 35821);
DELETE FROM gameobject WHERE id IN (195631, 195633);
UPDATE creature_template SET ScriptName = 'npc_toc_announcer' WHERE entry =34816;
INSERT IGNORE spell_script_target VALUES (67888, 1, 35458);

-- 1st encounter - Northrend Beastts
UPDATE creature_template SET ScriptName = 'boss_gormok' WHERE entry = 34796;
UPDATE creature_template SET ScriptName = 'npc_snobold' WHERE entry = 34800;
UPDATE creature_template SET ScriptName = 'boss_jormungars' WHERE entry = 35144;
UPDATE creature_template SET ScriptName = 'boss_jormungars' WHERE entry = 34799;
UPDATE creature_template SET ScriptName = 'boss_icehowl' WHERE entry = 34797;

INSERT IGNORE spell_script_target VALUES (66636, 1, 34796);
UPDATE creature_template SET AIName = 'NullAI' WHERE entry = 34854;
-- TODO: immune masky pro snobolda

-- 2nd encounter - Jaraxxus
UPDATE creature_template SET InhabitType = 7 WHERE entry = 22517;

UPDATE creature_template SET ScriptName = 'boss_jaraxxus' WHERE entry=34780;
UPDATE creature_template SET ScriptName = 'npc_felflame_infernal' WHERE entry=34815;
UPDATE creature_template SET ScriptName = 'npc_mistress_of_pain' WHERE entry=34826;

-- 3nd encounter - Factioned Champion
UPDATE creature_template SET faction_A = 14, faction_H = 14, mindmg = 6000, maxdmg = 6500, baseattacktime = 3500, speed_walk = 1.2, speed_run = 1.2, dmg_multiplier = 1, resistance1 = 75, resistance2 = 75, resistance3 = 75, resistance4 = 75, resistance5 = 75, resistance6 = 75, armor = 10643, mechanic_immune_mask = 0 WHERE entry in (34461,34458,34471,34456,34475,34453); -- war, dk, retri
UPDATE creature_template SET faction_A = 14, faction_H = 14, mindmg = 2500, maxdmg = 3000, baseattacktime = 1600, speed_walk = 1.2, speed_run = 1.2, dmg_multiplier = 1, resistance1 = 75, resistance2 = 75, resistance3 = 75, resistance4 = 75, resistance5 = 75, resistance6 = 75, armor = 9643, mechanic_immune_mask = 0 WHERE entry in (34460,34451,34465,34445,34470,34444,34469,34459); -- holy, balance, restosham, restodruid
UPDATE creature_template SET faction_A = 14, faction_H = 14, mindmg = 4000, maxdmg = 5000, baseattacktime = 2550, minoffdmg = 2200, maxoffdmg = 2800, dmg_offmultiplier= 1, offattacktime = 2550, speed_walk = 1.2, speed_run = 1.2, dmg_multiplier = 1, resistance1 = 75, resistance2 = 75, resistance3 = 75, resistance4 = 75, resistance5 = 75, resistance6 = 75, armor = 8043, mechanic_immune_mask = 0 WHERE entry in (34472,34454,34463,34455); -- enha, rogue
UPDATE creature_template SET faction_A = 14, faction_H = 14, mindmg = 2000, maxdmg = 2500, baseattacktime = 1600, speed_walk = 1.2, speed_run = 1.2, dmg_multiplier = 1, resistance1 = 75, resistance2 = 75, resistance3 = 75, resistance4 = 75, resistance5 = 75, resistance6 = 75, armor = 6343, mechanic_immune_mask = 0 WHERE entry in (34468,34449,34466,34447,34473,34441,34474,34450); -- mage, spriest, disko, lock
UPDATE creature_template SET faction_A = 14, faction_H = 14, mindmg = 3500, maxdmg = 4000, baseattacktime = 2600, speed_walk = 1.2, speed_run = 1.2, dmg_multiplier = 1, resistance1 = 75, resistance2 = 75, resistance3 = 75, resistance4 = 75, resistance5 = 75, resistance6 = 75, armor = 8043, mechanic_immune_mask = 0 WHERE entry in (34467,34448); -- hunt
UPDATE creature_template SET faction_A = 14, faction_H = 14, mindmg = 1800, maxdmg = 2200, baseattacktime = 1700, speed_walk = 1.2, speed_run = 1.2, dmg_multiplier = 1, resistance1 = 75, resistance2 = 75, resistance3 = 75, resistance4 = 75, resistance5 = 75, resistance6 = 75, armor = 7043, mechanic_immune_mask = 0 WHERE entry in (35465,35610); -- peti
UPDATE creature_template SET minhealth = 403200, maxhealth = 403200 WHERE entry in (34461,34471,34460,34469,34467,34465,34471,34472,34463,34470,34475,34458,34451,34459,34448,34445,34456,34454,34455,34444,34453);
UPDATE creature_template SET minhealth = 322560, maxhealth = 322560 WHERE entry in (34468,34466,34473,34474,34449,34447,34441,34450);
UPDATE creature_template SET minhealth = 126000, maxhealth = 126000 WHERE entry in (35465);
UPDATE creature_template SET minhealth = 53420, maxhealth = 53420 WHERE entry in (35610);

-- 4th encounter - Twin Valkyr
UPDATE creature_template SET ScriptName = 'boss_twin_valkyr', InhabitType = 7 WHERE entry IN (34497,34496);
UPDATE creature_template SET ScriptName = 'npc_concentrated' WHERE entry IN (34628,34630);
UPDATE creature_template SET ScriptName = 'npc_toc_essence', IconName = 'Interact', AIName = 'NullAI', npcflag = 1 WHERE entry IN (34567,34568);

-- light
INSERT IGNORE spell_script_target VALUES (65876, 1, 34496);
INSERT IGNORE spell_script_target VALUES (67306, 1, 34496);
INSERT IGNORE spell_script_target VALUES (67307, 1, 34496);
INSERT IGNORE spell_script_target VALUES (67308, 1, 34496);
-- dark
INSERT IGNORE spell_script_target VALUES (65875, 1, 34497);
INSERT IGNORE spell_script_target VALUES (67303, 1, 34497);
INSERT IGNORE spell_script_target VALUES (67304, 1, 34497);
INSERT IGNORE spell_script_target VALUES (67305, 1, 34497);

-- 5th encounter - Anu'Barak
UPDATE creature_template SET ScriptName = 'boss_anubarak_toc' WHERE entry = 34564;
UPDATE creature_template SET ScriptName = 'mob_burrower' WHERE entry = 34607;
UPDATE creature_template SET ScriptName = 'mob_scarab_toc' WHERE entry = 34605;
UPDATE creature_template SET ScriptName = 'mob_frost_sphere', InhabitType = 7 WHERE entry = 34606;
UPDATE creature_template SET ScriptName = 'mob_anubarak_spike' WHERE entry = 34660;

-- script text
USE scriptdev2;
REPLACE INTO `script_texts` VALUES
(-1649123, 'Is this the best the Horde has to offer?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, '34990'),
(-1649122, 'Worthless scrub.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, '34990'),
(-1649121, 'Hardly a challenge!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, '34990'),
(-1649120, 'HAH!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, '34990'),
(-1649118, 'Lok''tar!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, '34995'),
(-1649117, 'Overpowered.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, '34995'),
(-1649116, 'Pathetic!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, '34995'),
(-1649115, 'Weakling!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, '34995'),
(-1649110, 'Let me hand you the chests as a reward, and let its contents will serve you faithfully in the campaign against Arthas in the heart of the Icecrown Citadel!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, '36095'),
(-1649109, 'Champions, you are alive! Not only have you defeated every challenge of the Trial of the Crusader, but thwarted Arthas directly! Your skill and cunning will prove to be a powerful weapon against the Scourge. Well done! Allow one of my mages to transport you back to the surface!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, '36095'),
(-1649108, 'The souls of your fallen champions will be mine, Fordring.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16323, 1, 0, 0, '35877'),
(-1649107, 'The Nerubians built an empire beneath the frozen wastes of Northrend. An empire that you so foolishly built your structures upon. MY EMPIRE.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16322, 1, 0, 0, '35877'),
(-1649106, 'Arthas! You are hopelessly outnumbered! Lay down Frostmourne and I will grant you a just death.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16052, 1, 0, 0, '34996'),
(-1649105, 'You will have your challenge, Fordring.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16321, 1, 0, 0, '35877'),
(-1649104, 'A mighty blow has been dealt to the Lich King! You have proven yourselves able bodied champions of the Argent Crusade. Together we will strike at Icecrown Citadel and destroy what remains of the Scourge! There is no challenge that we cannot face united!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16051, 1, 0, 0, '34996'),
(-1649103, 'Do you still question the might of the horde paladin? We will take on all comers.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16025, 1, 0, 0, '34995'),
(-1649102, 'Not even the Lich King most powerful minions can stand against the Alliance! All hail our victors!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16068, 1, 0, 0, '34990'),
(-1649101, 'Let the games begin!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16037, 1, 0, 0, '34996'),
(-1649100, 'Only by working together will you overcome the final challenge. From the depths of Icecrown come two of the Scourges most powerful lieutenants: fearsome valkyr, winged harbingers of the Lich King!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16050, 1, 0, 0, '34996'),
(-1649099, 'A shallow and tragic victory. We are weaker as a whole from the losses suffered today. Who but the Lich King could benefit from such foolishness? Great warriors have lost their lives. And for what? The true threat looms ahead - the Lich King awaits us all in death.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16049, 1, 0, 0, '34996'),
(-1649098, 'That was just a taste of what the future brings. FOR THE HORDE!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16024, 1, 0, 0, '34995'),
(-1649097, 'GLORY OF THE ALLIANCE!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16067, 1, 0, 0, '34990'),
(-1649096, 'Show them no mercy, Horde champions! LOK-TAR OGAR!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16022, 1, 0, 0, '34995'),
(-1649095, 'Fight for the glory of the Alliance, heroes! Honor your king and your people!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16065, 1, 0, 0, '34990'),
(-1649094, 'Very well, I will allow it. Fight with honor!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16048, 1, 0, 0, '34996'),
(-1649093, 'The Horde demands justice! We challenge the Alliance. Allow us to battle in place of your knights, paladin. We will show these dogs what it means to insult the Horde!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16023, 1, 0, 0, '34995'),
(-1649092, 'Our honor has been besmirched! They make wild claims and false accusations against us. I demand justice! Allow my champions to fight in place of your knights, Tirion. We challenge the Horde!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16066, 1, 0, 0, '34990'),
(-1649091, 'The next battle will be against the Argent Crusades most powerful knights! Only by defeating them will you be deemed worthy...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16047, 1, 0, 0, '34996'),
(-1649090, 'Everyone, calm down! Compose yourselves! There is no conspiracy at play here. The warlock acted on his own volition - outside of influences from the Alliance. The tournament must go on!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16046, 1, 0, 0, '34996'),
(-1649089, 'The Alliance doesnt need the help of a demon lord to deal with Horde filth. Come, pig!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16064, 1, 0, 0, '34990'),
(-1649088, 'Treacherous Alliance dogs! You summon a demon lord against warriors of the Horde!? Your deaths will be swift!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16021, 1, 0, 0, '34995'),
(-1649087, 'The loss of Wilfred Fizzlebang, while unfortunate, should be a lesson to those that dare dabble in dark magic. Alas, you are victorious and must now face the next challenge.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16045, 1, 0, 0, '34996'),
(-1649086, 'Quickly, heroes! Destroy the demon lord before it can open a portal to its twisted demonic realm!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16044, 1, 0, 0, '34996'),
(-1649085, '...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 1, 0, 0, '35458'),
(-1649084, 'But I am in charge here-', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16271, 1, 0, 0, '35458'),
(-1649083, 'Ah ha! Behold the absolute power of Wilfred Fizzlebang, master summoner! You are bound to ME, demon!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16270, 1, 0, 0, '35458'),
(-1649082, 'Prepare for oblivion!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16269, 1, 0, 0, '35458'),
(-1649081, 'Thank you, Highlord! Now challengers, I will begin the ritual of summoning! When I am done, a fearsome Doomguard will appear!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16268, 1, 0, 0, '35458'),
(-1649080, 'Grand Warlock Wilfred Fizzlebang will summon forth your next challenge. Stand by for his entry!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16043, 1, 0, 0, '34996'),
(-1649077, 'Tragic... They fought valiantly, but the beasts of Northrend triumphed. Let us observe a moment of silence for our fallen heroes.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16042, 1, 0, 0, '34996'),
(-1649076, 'The monstrous menagerie has been vanquished!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16041, 1, 0, 0, '34996'),
(-1649075, 'The air freezes with the introduction of our next combatant, Icehowl! Kill or be killed, champions!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16040, 1, 0, 0, '34996'),
(-1649074, 'Steel yourselves, heroes, for the twin terrors Acidmaw and Dreadscale. Enter the arena!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16039, 1, 0, 0, '34996'),
(-1649073, 'I have seen  more  worthy  challenges in the Ring of Blood, you waste our time paladin.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16026, 1, 0, 0, '34995'),
(-1649072, 'Your beast will be no match for my champions Tirion!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16069, 1, 0, 0, '34990'),
(-1649071, 'Hailing from the deepest, darkest carverns of the Storm Peaks, Gormok the Impaler! Battle on, heroes!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16038, 1, 0, 0, '34996'),
(-1649070, 'Welcome champions, you have heard the call of the Argent Crusade and you have boldly answered. It is here in the crusaders coliseum that you will face your greatest challenges. Those of you who survive the rigors of the coliseum will join the Argent Crusade on its marsh to ice crown citadel.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16036, 1, 0, 0, '34996'),
(-1649063, 'The swarm shall overtake you!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16241, 1, 0, 0, '34564'),
(-1649062, '%s produces a swarm of beetles Peon to restore your health!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 3, 0, 0, '34564'),
(-1649061, 'Auum na-l ak-k-k-k, isshhh. Rise, minions. Devour...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16240, 1, 0, 0, '34564'),
(-1649060, '%s spikes  pursuing $n!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 3, 0, 0, '34564'),
(-1649059, 'I have failed you, master...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16238, 1, 0, 0, '34564'),
(-1649058, 'Another soul to sate the host.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16237, 1, 0, 0, '34564'),
(-1649057, 'F-lakkh shir!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16236, 1, 0, 0, '34564'),
(-1649056, 'This place will serve as your tomb!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16234, 1, 0, 0, '34564'),
(-1649055, 'Ahhh... Our guests arrived, just as the master promised.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16235, 1, 0, 0, '34564'),
(-1649050, 'Let the dark consume you!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16278, 1, 0, 0, '34496'),
(-1649049, '%s begins to read a spell |cFF9932CDBlack Vortex!|R switch to |cFF9932CDBlack|r essence!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 3, 0, 0, '34496'),
(-1649048, 'Let the light consume you!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16279, 1, 0, 0, '34496'),
(-1649047, '%s begins to read a spell |cFFFFFFFFLight Vortex!|R switch to |cFFFFFFFFLight|r essence!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 3, 0, 0, '34497'),
(-1649046, 'You appreciated and acknowledged nothing.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16276, 1, 0, 0, '34497'),
(-1649045, 'UNWORTHY!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16277, 1, 0, 0, '34496'),
(-1649044, 'CHAOS!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16274, 3, 0, 0, '34497'),
(-1649043, '%s begins to read the spell Treaty twins!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 3, 0, 0, '34497'),
(-1649042, 'YOU ARE FINISHED!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16273, 1, 0, 0, '34497'),
(-1649041, 'The Scourge cannot be stopped...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16275, 1, 0, 0, '34496'),
(-1649040, 'In the name of our dark master. For the Lich King. You. Will. Die.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16272, 1, 0, 0, '34497'),
(-1649039, 'INFERNO!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16151, 1, 0, 0, '34780'),
(-1649038, '%s creates an |cFF00FF00Infernal Volcano!|R', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 3, 0, 0, '34780'),
(-1649037, 'Come forth, sister! Your master calls!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16150, 1, 0, 0, '34780'),
(-1649036, '%s creates the gates of the Void!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 3, 0, 0, '34780'),
(-1649035, '|cFFFF0000Legion Flame|R on $n', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 3, 0, 0, '34780'),
(-1649034, 'FLESH FROM BONE!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16149, 1, 0, 0, '34780'),
(-1649033, '$n has |cFF00FFFFIncinerate flesh!|R Heal $ghim:her;!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 3, 0, 0, '34780'),
(-1649032, 'Another will take my place. Your world is doomed.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16147, 1, 0, 0, '34780'),
(-1649031, 'You face Jaraxxus, eredar lord of the Burning Legion!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16144, 1, 0, 0, '34780'),
(-1649030, 'Trifling gnome, your arrogance will be your undoing!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 16143, 1, 0, 0, '34780'),
(-1649022, '|3-3(%s) covers boiling rage, and he tramples all in its path!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 3, 0, 0, '34797'),
(-1649021, '%s crashes into a wall of the Colosseum and lose focus!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 3, 0, 0, '34797'),
(-1649020, '%s looks at |3-3($n) and emits a guttural howl!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 3, 0, 0, '34797'),
(-1649012, 'Upon seeing its companion parish, %s becomes enraged!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 3, 0, 0, '34799'),
(-1649011, '%s getting out of the ground!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 3, 0, 0, '34564'),
(-1649010, '%s buries itself in the earth!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 3, 0, 0, '34564'),
(-1649000, 'My slaves! Destroy the enemy!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 3, 0, 0, '34796');
