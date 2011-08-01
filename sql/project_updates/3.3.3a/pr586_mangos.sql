delete from gameobject_battleground where guid in (200080, 200081, 200082, 200083, 200088, 200089, 200078, 200079);
insert into `gameobject_battleground` values
-- pillars_far
('200080','250','0'),
('200082','250','0'),
-- pillars_near
('200081','250','1'),
('200083','250','1'),
-- pillars_eq_far
('200088','251','0'),
('200089','251','0'),
-- pillars_eq_near
('200078','251','1'),
('200079','251','1');

delete from battleground_events where map = 618 and event1 in (250, 251);
insert into `battleground_events` values
('618','250','0','pillars_far'),
('618','250','1','pillars_near'),
('618','251','0','pillars_eq_far'),
('618','251','1','pillars_eq_near');