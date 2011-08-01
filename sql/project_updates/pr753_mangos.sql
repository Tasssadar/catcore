
DROP TABLE broadcast_messages;

CREATE TABLE `broadcast_messages` (
`ID` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY ,
`RepeatMins` INT UNSIGNED NOT NULL DEFAULT '0',
`text` INT NOT NULL ,
`enable` TINYINT( 3 ) NOT NULL DEFAULT '1'
) ENGINE = InnoDB;

DELETE FROM command WHERE name LIKE "broadcast%"; 
INSERT INTO `command` (`name`, `security`, `help`) VALUES
('broadcast', '3', 'Syntax: .broadcast

Allows modify world broadcast.'),
('broadcast list', '3', 'Syntax: .broadcast list

Show all broadcast messages and their timers.'),
('broadcast send', '3', 'Syntax: .broadcast send $id

Sends broadcast with given ID. This does not reset its timer.'),
('broadcast reset', '3', 'Syntax: .broadcast reset $id

Reset timer of broadcast with given ID.');