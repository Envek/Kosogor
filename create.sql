CREATE TABLE `checksums` (
  `filename` CHAR(12) NOT NULL,
  `record_count` INTEGER NOT NULL,
  `total` BIGINT NOT NULL,
  PRIMARY KEY (`filename`)
);

CREATE TABLE `payments` (
  `filename` CHAR(12) NOT NULL,
  `record_number` INTEGER NOT NULL,
  `bank_short` VARCHAR(10),
  `branch_number` INTEGER,
  `sheet_number` INTEGER,
  `sberbank_branch_code` VARCHAR(10),
  `sberbank_code` VARCHAR(10),
  `source_code` VARCHAR(10),
  `muszi_code` VARCHAR(10),
  `surname` VARCHAR(30) NOT NULL,
  `name` VARCHAR(30) NOT NULL,
  `patronymic` VARCHAR(30) NOT NULL,
  `account_number` VARCHAR(30) NOT NULL,
  `pansion` BIGINT NOT NULL DEFAULT 0,
  `mayor_surcharge` BIGINT DEFAULT 0,
  `additional_surcharge` BIGINT DEFAULT 0,
  `uvov_surcharge` BIGINT DEFAULT 0,
  `children_surcharge` BIGINT DEFAULT 0,
  `phone_surcharge` BIGINT DEFAULT 0,
  `total` BIGINT NOT NULL DEFAULT 0,
  `currency_code` CHAR(3) NOT NULL DEFAULT '810',
  `creation_date` DATE NOT NULL,
  `result` INTEGER NOT NULL DEFAULT 0,
  `paid` BOOLEAN NOT NULL DEFAULT 0,
  `processed_date` DATE,
  PRIMARY KEY (`filename`, `record_number`),
  FOREIGN KEY(`filename`) REFERENCES `checksums`(`filename`) ON UPDATE CASCADE ON DELETE CASCADE,
  FOREIGN KEY(`account_number`) REFERENCES `accounts`(`account_number`)  ON UPDATE CASCADE ON DELETE CASCADE
);

CREATE TABLE `accounts` (
  `account_number` VARCHAR(30) NOT NULL,
  `passport_number` VARCHAR(10) NOT NULL,
  `closed` SMALLINT NOT NULL DEFAULT 0,
  `abuse` SMALLINT NOT NULL DEFAULT 0,
  PRIMARY KEY (`account_number`),
  FOREIGN KEY(`passport_number`) REFERENCES `clients`(`passport_number`) ON UPDATE CASCADE ON DELETE CASCADE
);

CREATE TABLE `clients` (
  `passport_number` VARCHAR(10) NOT NULL,
  `surname` VARCHAR(30) NOT NULL,
  `name` VARCHAR(30) NOT NULL,
  `patronymic` VARCHAR(30) NOT NULL,
  `cons_account` VARCHAR(20),
  PRIMARY KEY (`passport_number`)
);

CREATE INDEX `fullname` ON `clients` (`surname`, `name`, `patronymic`);
