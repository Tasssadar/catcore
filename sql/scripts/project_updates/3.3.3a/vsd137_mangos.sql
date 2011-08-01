#################  human and orcish version   ################################################################
-- All 3 are availeble after finishing Children's Week (alliance)
UPDATE quest_template SET
PrevQuestId = 1468,
ExclusiveGroup = -1479
WHERE entry IN (1479,1558,1687);

-- Both availeble after finishing The Bough of the Eternals, The Stonewrought Dam, Spooky Lighthouse (alliance)
UPDATE quest_template SET
PrevQuestId = 1479,
RequiredRaces = 1101
WHERE entry IN (558,4822);

-- All 3 are availeble after finishing Children's Week (alliance)
UPDATE quest_template SET
PrevQuestId = 172,
ExclusiveGroup = -910
WHERE entry IN (1800,910,911);

-- Both availeble after finishing Lordaeron Throne Room, Down at the Docks, Gateway to the Frontier (alliance)
UPDATE quest_template SET
PrevQuestId = 910,
RequiredRaces = 690
WHERE entry IN (925,915);
   
DELETE FROM gossip_menu_option WHERE menu_id IN (5849,5848,20002,5851);
INSERT INTO gossip_menu_option VALUES
-- alliance version (return whistle of lost)
(5849, 0, 0, 'Well what do you know, this is Children\'s Week! What can I do to help?',1,1,0,0,5849,0,0,'',0,0,0, 0,0,0, 0,0,0),
(5849, 1, 0, 'GOSSIP_OPTION_QUESTGIVER', 2, 2, 0,0,0,0,0,'',0,0,0, 0,0,0, 0,0,0),
-- horde version (return whistle of lost)
(5848, 0, 0, 'Well what do you know, this is Children\'s Week! What can I do to help?',1,1,0,0,5848,0,0,'',0,0,0, 0,0,0, 0,0,0),
(5848, 1, 0, 'GOSSIP_OPTION_QUESTGIVER', 2, 2, 0,0,0,0,0,'',0,0,0, 0,0,0, 0,0,0),
-- jaina proudmore (maybe 5850?)
(20002, 0, 0, 'Lady Jaina, this may sound like an odd request... but I have a young ward who is quite shy. You are a hero to him, and he asked me to get your autograph.',1,1,20003,0,20002,0,0,'',9,558,0, 0,0,0, 0,0,0),
(20002, 1, 0, 'GOSSIP_OPTION_QUESTGIVER', 2, 2, 0,0,0,0,0,'',0,0,0, 0,0,0, 0,0,0),
-- cairne bloodhoof
(5851, 0, 0, 'I know this is rather silly but a young ward who is a bit shy would like your hoofprint.',1,1,20004,0,5851,0,0,'',9,925,0, 0,0,0, 0,0,0),
(5851, 1, 0, 'GOSSIP_OPTION_QUESTGIVER', 2, 2, 0,0,0,0,0,'',0,0,0, 0,0,0, 0,0,0);


-- Jaina Proudmore (theramoore)
UPDATE creature_template SET
gossip_menu_id = 20002, -- maybe 5850?
ScriptName = ''
WHERE entry = 4968;

DELETE FROM gossip_menu WHERE entry IN (20002,20003,20004,5851,5849,5848);
INSERT INTO gossip_menu VALUES
-- jaina proudmore
(20002, 3157,0,0,0,0,0,0),
(20003, 7012,0,0,0,0,0,0),
-- cairne bloodhoof
(5851, 7013,0,0,0,0,0,0),
(20004, 7014,0,0,0,0,0,0),
-- Orphan Matron Nightingale (alliance)
(5848, 7010,0,0,0,0,0,0),
-- Orphan Matron Nightingale (alliance)
(5849, 7011,0,0,0,0,0,0);

-- jaina proudmore, cairne bloodhoof
DELETE FROM npc_gossip WHERE npc_guid IN (SELECT guid FROM creature WHERE id IN (4968,3057,14450,14451));

DELETE FROM areatrigger_scripts WHERE entry IN (3546, 3547, 3548, 3549, 3550, 3552);
INSERT INTO areatrigger_scripts VALUES
-- Darnassian bank
(3546, 'at_childrens_week_spot'),
-- Undercity - thone room
(3547, 'at_childrens_week_spot'),
-- Stonewrought Dam
(3548, 'at_childrens_week_spot'),
-- The Mor'shan Rampart
(3549, 'at_childrens_week_spot'),
-- Ratchet Docks
(3550, 'at_childrens_week_spot'),
-- Westfall Lighthouse
(3552, 'at_childrens_week_spot');

DELETE FROM gossip_menu WHERE entry = 20005;
INSERT INTO gossip_menu VALUES
-- Orphan Matron Aria
(20005,34365,0,0,0,0,0,0);
 
DELETE FROM gossip_menu_option WHERE menu_id IN (5849,5848,20002,5851,8568,20005);
INSERT INTO gossip_menu_option (menu_id,id,option_icon,option_text,option_id,npc_option_npcflag,action_menu_id,action_script_id,cond_1,cond_1_val_1) VALUES
-- alliance version (return whistle of lost)
(5849,0,0,'Well what do you know, this is Children\'s Week! What can I do to help?',1,1,0,5849,8,1468),
(5849,1,0,'Well what do you know, this is Children\'s Week! What can I do to help?',1,1,0,5849,9,1468),
(5849,2,0,'GOSSIP_OPTION_QUESTGIVER',2,2,0,0,0,0),
-- horde version (return whistle of lost)
(5848,0,0,'Well what do you know, this is Children\'s Week! What can I do to help?',1,1,0,5848,8,172),
(5848,1,0,'Well what do you know, this is Children\'s Week! What can I do to help?',1,1,0,5848,9,172),
(5848,2,0,'GOSSIP_OPTION_QUESTGIVER',2,2,0,0,0,0),
-- Orphan Matron Mercy
(8568,0,0,'Well what do you know, this is Children\'s Week! What can I do to help?',1,1,0,8568,8,10942), -- horde
(8568,1,0,'Well what do you know, this is Children\'s Week! What can I do to help?',1,1,0,8568,9,10942),
(8568,2,0,'Well what do you know, this is Children\'s Week! What can I do to help?',1,1,0,8569,8,10943), -- alliance
(8568,3,0,'Well what do you know, this is Children\'s Week! What can I do to help?',1,1,0,8569,9,10943),
(8568,4,0,'GOSSIP_OPTION_QUESTGIVER',2,2,0,0,0,0),
-- Orphan Matron Aria
(20005,0,0,'Well what do you know, this is Children\'s Week! What can I do to help?',1,1,0,20005,8,13926), -- oracle
(20005,1,0,'Well what do you know, this is Children\'s Week! What can I do to help?',1,1,0,20005,9,13926),
(20005,2,0,'Well what do you know, this is Children\'s Week! What can I do to help?',1,1,0,20006,8,13927), -- frenzyheart
(20005,3,0,'Well what do you know, this is Children\'s Week! What can I do to help?',1,1,0,20006,9,13927),
(20005,4,0,'GOSSIP_OPTION_QUESTGIVER',2,2,0,0,0,0),
-- jaina proudmore (maybe 5850?)
(20002,0,0,'Lady Jaina, this may sound like an odd request... but I have a young ward who is quite shy. You are a hero to him, and he asked me to get your autograph.',1,1,20003,20002,9,558),
(20002,1,0,'GOSSIP_OPTION_QUESTGIVER',2,2,0,0,0,0),
-- cairne bloodhoof
(5851,0,0,'I know this is rather silly but a young ward who is a bit shy would like your hoofprint.',1,1,20004,5851,9,925),
(5851,1,0,'GOSSIP_OPTION_QUESTGIVER',2,2,0,0,0,0);
 
DELETE FROM gossip_scripts WHERE id IN (20005,20006, 5849,5848,20002);
INSERT INTO gossip_scripts (id,delay,command,datalong) VALUES
-- Orphan Matron Aria
(5849,0,15,23124),
(5848,0,15,23125),
(20002,0,15,23122,
(5851,0,15,23123),
(20005,0,15,65359),
(20006,0,15,65360);

