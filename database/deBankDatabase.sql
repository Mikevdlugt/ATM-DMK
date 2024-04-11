-- MySQL dump 10.13  Distrib 8.0.36, for Linux (x86_64)
--
-- Host: localhost    Database: deBankDatabase
-- ------------------------------------------------------
-- Server version	8.0.36-0ubuntu0.22.04.1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!50503 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `ATM`
--

DROP TABLE IF EXISTS `ATM`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `ATM` (
  `idATM` int NOT NULL AUTO_INCREMENT,
  `postcode` varchar(45) DEFAULT NULL,
  `streetName` varchar(45) DEFAULT NULL,
  `fivePoundBills` smallint NOT NULL,
  `tenPoundBills` smallint NOT NULL,
  `twentyPoundBills` smallint NOT NULL,
  `fiftyPoundBills` smallint NOT NULL,
  `FK_idAdmin` int NOT NULL,
  PRIMARY KEY (`idATM`,`FK_idAdmin`),
  KEY `fk_ATM_Admin1_idx` (`FK_idAdmin`),
  CONSTRAINT `fk_ATM_Admin1` FOREIGN KEY (`FK_idAdmin`) REFERENCES `Admin` (`idAdmin`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `ATM`
--

LOCK TABLES `ATM` WRITE;
/*!40000 ALTER TABLE `ATM` DISABLE KEYS */;
/*!40000 ALTER TABLE `ATM` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Account`
--

DROP TABLE IF EXISTS `Account`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `Account` (
  `idAccount` int NOT NULL AUTO_INCREMENT,
  `accountNumber` varchar(45) NOT NULL,
  `balanceInCents` int NOT NULL,
  `maxDebtPounds` int DEFAULT NULL,
  `Fk_idUser` int NOT NULL,
  PRIMARY KEY (`idAccount`,`Fk_idUser`),
  KEY `fk_Rekening_User1_idx` (`Fk_idUser`),
  CONSTRAINT `fk_Rekening_User1` FOREIGN KEY (`Fk_idUser`) REFERENCES `User` (`idUser`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8mb3;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Account`
--

LOCK TABLES `Account` WRITE;
/*!40000 ALTER TABLE `Account` DISABLE KEYS */;
INSERT INTO `Account` VALUES (1,'IM67DMKB8194650327',0,NULL,1),(2,'IM92DMKB2675104893',0,NULL,2),(3,'IM83DMKB5037216894',0,NULL,3),(4,'IM74DMKB7265493810',0,NULL,4),(5,'IM58DMKB3986241570',0,NULL,5);
/*!40000 ALTER TABLE `Account` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Admin`
--

DROP TABLE IF EXISTS `Admin`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `Admin` (
  `idAdmin` int NOT NULL AUTO_INCREMENT,
  `userName` varchar(45) NOT NULL,
  `password` varchar(45) NOT NULL,
  PRIMARY KEY (`idAdmin`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Admin`
--

LOCK TABLES `Admin` WRITE;
/*!40000 ALTER TABLE `Admin` DISABLE KEYS */;
INSERT INTO `Admin` VALUES (1,'admin','admin');
/*!40000 ALTER TABLE `Admin` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Card`
--

DROP TABLE IF EXISTS `Card`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `Card` (
  `idCard` int NOT NULL AUTO_INCREMENT,
  `pinCode` varchar(4) NOT NULL,
  `cardNumber` varchar(18) NOT NULL,
  `Fk_idAccount` int NOT NULL,
  PRIMARY KEY (`idCard`,`Fk_idAccount`),
  KEY `fk_Passen_Rekening_idx` (`Fk_idAccount`),
  CONSTRAINT `fk_Passen_Rekening` FOREIGN KEY (`Fk_idAccount`) REFERENCES `Account` (`idAccount`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Card`
--

LOCK TABLES `Card` WRITE;
/*!40000 ALTER TABLE `Card` DISABLE KEYS */;
INSERT INTO `Card` VALUES (1,'1234','22 0E 9D D4'),(2,'4321','3F 91 A6 15'),(3,'1111','81 B3 F3 73'),(4,'2222','A6 23 09 13'),(5,'3333','A5 12 17 4F');
/*!40000 ALTER TABLE `Card` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Transaction`
--

DROP TABLE IF EXISTS `Transaction`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `Transaction` (
  `idTransaction` int NOT NULL AUTO_INCREMENT,
  `FK_idATM` int NOT NULL,
  `FK_idCard` int NOT NULL,
  `amountInPounds` int NOT NULL,
  `time` datetime NOT NULL,
  PRIMARY KEY (`idTransaction`,`FK_idATM`,`FK_idCard`),
  KEY `fk_Transaction_ATM1_idx` (`FK_idATM`),
  KEY `fk_Transaction_Card1_idx` (`FK_idCard`),
  CONSTRAINT `fk_Transaction_ATM1` FOREIGN KEY (`FK_idATM`) REFERENCES `ATM` (`idATM`),
  CONSTRAINT `fk_Transaction_Card1` FOREIGN KEY (`FK_idCard`) REFERENCES `Card` (`idCard`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Transaction`
--

LOCK TABLES `Transaction` WRITE;
/*!40000 ALTER TABLE `Transaction` DISABLE KEYS */;
/*!40000 ALTER TABLE `Transaction` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `User`
--

DROP TABLE IF EXISTS `User`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `User` (
  `idUser` int NOT NULL AUTO_INCREMENT,
  `fullName` varchar(45) NOT NULL,
  `postcode` varchar(6) NOT NULL,
  `emailAddress` varchar(45) DEFAULT NULL,
  `country` varchar(30) NOT NULL,
  `streetName` varchar(45) NOT NULL,
  `houseNumber` varchar(6) NOT NULL,
  PRIMARY KEY (`idUser`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=utf8mb3;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `User`
--

LOCK TABLES `User` WRITE;
/*!40000 ALTER TABLE `User` DISABLE KEYS */;
INSERT INTO `User` VALUES (1,'John Smith','IM11AA','john.smith@example.com','Isle of Man','Douglas Street','10'),(2,'Alice Johnson','IM22BB','alice.johnson@email.com','Isle of Man','Bridson Street','123'),(3,'Mara Garca','IM33CC','maria.garcia@email.es','Isle of Man','Nelson Street','7'),(4,'Jean Dupont','IM44DD','jean.dupont@email.fr','Isle of Man','Victoria Road','1'),(5,'Hans Mller','IM55EE','hans.mueller@email.de','Isle of Man','Peel Street','67');
/*!40000 ALTER TABLE `User` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2024-04-11  9:34:41
