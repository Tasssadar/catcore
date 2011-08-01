-- Violet hold
UPDATE creature_template SET ScriptName="npc_door_seal_vh", unit_flags=33816580 WHERE entry=30896;
UPDATE creature_template SET faction_A=35, faction_H=35 WHERE entry=30658;
UPDATE creature_template SET faction_A=1720, faction_H=1720, ScriptName="mob_vh_dragons", minlevel=70,maxlevel=70,minhealth=7000,maxhealth=8000  WHERE entry IN (30660, 30661, 30662, 30663, 30664, 30666, 30667, 30668, 32191, 30695);