ALTER TABLE `instance`
  ADD `perm` TINYINT( 3 ) UNSIGNED NOT NULL DEFAULT '0' AFTER `difficulty`,
  ADD `encountersMask` INT( 11 ) UNSIGNED NOT NULL DEFAULT '0' AFTER `perm`;
          
UPDATE instance SET perm = 1 WHERE id IN (SELECT instance FROM character_instance WHERE permanent = 1 GROUP BY instance);
UPDATE instance SET perm = 1 WHERE id IN (SELECT instance FROM group_instance WHERE permanent = 1 GROUP BY instance);

ALTER TABLE character_instance
  DROP permanent,
  ADD extended TINYINT( 3 ) UNSIGNED NOT NULL DEFAULT '0' AFTER `instance`; 
  
DROP TABLE group_instance;