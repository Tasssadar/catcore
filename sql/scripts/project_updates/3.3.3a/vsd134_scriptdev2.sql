/*
SQLyog Community v8.5 Beta1
MySQL - 5.1.46-community 
*********************************************************************
*/

DELETE FROM script_waypoint WHERE entry=9598;
insert into `script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) values('9598','0','6003.08','-1181.81','376.546','0','');
insert into `script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) values('9598','1','5995.12','-1155.39','383.219','0','');
insert into `script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) values('9598','2','6025.64','-1132.44','385.197','0','');
insert into `script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) values('9598','3','6092.13','-1125.78','383.425','0','');
insert into `script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) values('9598','4','6111.62','-1129.07','376.179','0','');
insert into `script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) values('9598','5','6128.79','-1148.69','374.401','0','');
insert into `script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) values('9598','6','6128.65','-1168.68','367.871','0','');
insert into `script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) values('9598','7','6124.21','-1189.03','371.162','0','');
insert into `script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) values('9598','8','6114.89','-1198.22','373.8','0','');
insert into `script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) values('9598','9','6126.35','-1215.83','374.101','0','');
insert into `script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) values('9598','10','6133.03','-1247.05','370.554','0','roleplay elementals');
insert into `script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) values('9598','11','6154.58','-1263.22','369.744','0','');
insert into `script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) values('9598','12','6149.49','-1278.30','373.2538','0','');
insert into `script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) values('9598','13','6186.60','-1363.29','378.7241','0','');
insert into `script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) values('9598','14','6246.49','-1409.45','371.4800','0','');
insert into `script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) values('9598','15','6341.52','-1409.84','369.9637','0','');
insert into `script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) values('9598','16','6379.56','-1401.52','372.2570','0','');
insert into `script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) values('9598','17','6408.36','-1343.48','376.5208','0','');
insert into `script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) values('9598','18','6409.59','-1284.97','379.8452','0','');
insert into `script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) values('9598','19','6438.36','-1232.23','388.5157','0','');
insert into `script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) values('9598','20','6457.20','-1218.22','396.9465','0','');
insert into `script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) values('9598','21','6487.75','-1210.42','413.9411','0','');
insert into `script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) values('9598','22','6538.49','-1214.89','436.5118','0','');
insert into `script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) values('9598','23','6576.57','-1215.49','444.2266','0','roleplay treants');
insert into `script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) values('9598','24','6581.51','-1215.51','444.8110','0','roleplay end');


REPLACE into `script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) values('-1361000','Please, help me to get through this cursed forest, $r.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'0','0','0','0','npc_arei SAY_START'),
('-1361001','This creature suffers from the effects of the fel... We must end its misery!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'0','0','0','0','npc_arei SAY_AGGRO'),
('-1361002','That I must fight against my own kind deeply suddens me.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'0','0','0','0','npc_arei SAY_FRAG'),
('-1361003','I can sense it now, $N. Ashenvale lies down this path.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'0','0','0','0','npc_arei SAY_END1'),
('-1361004','I feel... something strange...',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'0','0','0','0','npc_arei SAY_END2'),
('-1361005','$N, my form has now changed! The true strength of my spirit is returning to me now... The cursed grasp of the forest is leaving me.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'0','0','0','0','npc_arei SAY_END3'),
('-1361006','Thank you, $N. Now my spirit will finally be at peace.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'0','0','0','0','npc_arei SAY_END4');
