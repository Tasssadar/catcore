-- RandomDungeonDaily
INSERT INTO game_event (entry, start_time, end_time, occurence, length, holiday, description) VALUES( 60, '2010-01-03 06:00:00', '2020-01-03 09:00:00', 1440, 1440, 0, 'Daily Hero: Ingvar!');
INSERT INTO game_event (entry, start_time, end_time, occurence, length, holiday, description) VALUES( 61, '2010-01-03 06:00:00', '2020-01-03 09:00:00', 5184000, 1440, 0, 'Daily Hero: Keristrasza!');
INSERT INTO game_event (entry, start_time, end_time, occurence, length, holiday, description) VALUES( 62, '2010-01-03 06:00:00', '2020-01-03 09:00:00', 5184000, 1440, 0, 'Daily Hero: Leyguard!');
INSERT INTO game_event (entry, start_time, end_time, occurence, length, holiday, description) VALUES( 63, '2010-01-03 06:00:00', '2020-01-03 09:00:00', 5184000, 1440, 0, 'Daily Hero: KingYmiron!');
INSERT INTO game_event (entry, start_time, end_time, occurence, length, holiday, description) VALUES( 64, '2010-01-03 06:00:00', '2020-01-03 09:00:00', 5184000, 1440, 0, 'Daily Hero: ProphetTharon!');
INSERT INTO game_event (entry, start_time, end_time, occurence, length, holiday, description) VALUES( 65, '2010-01-03 06:00:00', '2020-01-03 09:00:00', 5184000, 1440, 0, 'Daily Hero: Galdarah!');
INSERT INTO game_event (entry, start_time, end_time, occurence, length, holiday, description) VALUES( 66, '2010-01-03 06:00:00', '2020-01-03 09:00:00', 5184000, 1440, 0, 'Daily Hero: Malganis!');
INSERT INTO game_event (entry, start_time, end_time, occurence, length, holiday, description) VALUES( 67, '2010-01-03 06:00:00', '2020-01-03 09:00:00', 5184000, 1440, 0, 'Daily Hero: Sjonnir!');
INSERT INTO game_event (entry, start_time, end_time, occurence, length, holiday, description) VALUES( 68, '2010-01-03 06:00:00', '2020-01-03 09:00:00', 5184000, 1440, 0, 'Daily Hero: Loken!');
INSERT INTO game_event (entry, start_time, end_time, occurence, length, holiday, description) VALUES( 69, '2010-01-03 06:00:00', '2020-01-03 09:00:00', 5184000, 1440, 0, 'Daily Hero: Anubarak!');
INSERT INTO game_event (entry, start_time, end_time, occurence, length, holiday, description) VALUES( 70, '2010-01-03 06:00:00', '2020-01-03 09:00:00', 5184000, 1440, 0, 'Daily Hero: Herald!');
INSERT INTO game_event (entry, start_time, end_time, occurence, length, holiday, description) VALUES( 71, '2010-01-03 06:00:00', '2020-01-03 09:00:00', 5184000, 1440, 0, 'Daily Hero: Cyanigosa!');
DELETE FROM creature_questrelation WHERE quest IN (13245, 13247, 13249, 13251, 13253, 13255, 13246, 13248, 13250, 13252, 13254, 13256);
INSERT INTO game_event_creature_quest VALUES
(20735, 13245, 60),
(20735, 13247, 61),
(20735, 13249, 62),
(20735, 13251, 63),
(20735, 13253, 64),
(20735, 13255, 65),
(20735, 13246, 66),
(20735, 13248, 67),
(20735, 13250, 68),
(20735, 13252, 69),
(20735, 13254, 70),
(20735, 13256, 71);

-- RandomTimearForeseesDaily
INSERT INTO game_event (entry, start_time, end_time, occurence, length, holiday, description) VALUES( 72, '2010-01-03 06:00:00', '2020-01-03 09:00:00', 1440, 1440, 0, 'Daily Timear: Centrifuge Constructs!');
INSERT INTO game_event (entry, start_time, end_time, occurence, length, holiday, description) VALUES( 73, '2010-01-03 06:00:00', '2020-01-03 09:00:00', 5184000, 1440, 0, 'Daily Timear: Ymirjar Berserkers!');
INSERT INTO game_event (entry, start_time, end_time, occurence, length, holiday, description) VALUES( 74, '2010-01-03 06:00:00', '2020-01-03 09:00:00', 5184000, 1440, 0, 'Daily Timear: Infinite Agents!');
INSERT INTO game_event (entry, start_time, end_time, occurence, length, holiday, description) VALUES( 75, '2010-01-03 06:00:00', '2020-01-03 09:00:00', 5184000, 1440, 0, 'Daily Timear: Titanium Vanguards!');
DELETE FROM creature_questrelation WHERE quest IN (13240, 13243, 13241, 13244);
INSERT INTO game_event_creature_quest VALUES
(31439, 13240, 72),
(31439, 13243, 73),
(31439, 13241, 74),
(31439, 13244, 75);