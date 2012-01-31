<?
	$db=mysql_connect("hanau.dvs.informatik.tu-darmstadt.de","solarhouse","Noo9rai4");  // nur aus dem DVS-Netzwerk erreichbar!!
    //$db=mysql_connect("localhost","wsn","wsn"); echo 'DBMS -> LOCALHOST';

	if($db==false){
	    die ("<p> Fehler ! Datenbank Error </p>");
	}
	       
	mysql_select_db("wsn_solarhouse",$db);
	mysql_query( "SET NAMES utf8", $db );
	mysql_query( "SET CHARACTER SET utf8", $db );
?>
