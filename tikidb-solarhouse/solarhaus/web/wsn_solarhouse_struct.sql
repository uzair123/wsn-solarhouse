-- phpMyAdmin SQL Dump
-- version 3.3.7deb5
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Erstellungszeit: 31. Mai 2011 um 12:09
-- Server Version: 5.1.49
-- PHP-Version: 5.3.3-7+squeeze1

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Datenbank: `wsn_solarhouse`
--

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `sensorreadings`
--

CREATE TABLE IF NOT EXISTS `sensorreadings` (
  `time_stamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `node_id` int(10) unsigned NOT NULL,
  `epoch` int(10) unsigned DEFAULT NULL,
  `scope_id` float DEFAULT NULL,
  `voltage` float DEFAULT NULL,
  `temperature` float DEFAULT NULL,
  `humidity` float DEFAULT NULL,
  `light_par` float DEFAULT NULL,
  `light_tsr` float DEFAULT NULL,
  `rssi` float DEFAULT NULL,
  `parent_node_id` float DEFAULT NULL,
  `extern_CO2` float DEFAULT NULL,
  `extern_temperature` float DEFAULT NULL,
  `extern_humidity` float DEFAULT NULL,
  PRIMARY KEY (`time_stamp`,`node_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
