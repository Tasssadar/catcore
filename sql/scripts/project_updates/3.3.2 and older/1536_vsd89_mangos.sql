UPDATE creature_template SET AIname="EventAI" WHERE entry=30176;
INSERT IGNORE INTO `creature_ai_scripts` (`id`, `creature_id`, `event_type`, `event_inverse_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action1_type`, `action1_param1`, `action1_param2`, `action1_param3`, `action2_type`, `action2_param1`, `action2_param2`, `action2_param3`, `action3_type`, `action3_param1`, `action3_param2`, `action3_param3`, `comment`) VALUES
('3017610', '30176', '11', '0', '100', '6', '0', '0', '0', '0', '11', '56151', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', ''),
('3017611', '30176', '8', '0', '100', '6', '56153', '0', '0', '0', '28', '0', '56153', '0', '0', '0', '0', '0', '0', '0', '0', '0', '');
