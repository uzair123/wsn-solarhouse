<?php
echo "~K\n";
sleep (2);
echo "rm encoded\n";
sleep(1);
echo "write encoded | null\n";
sleep (2);
$res = fopen("output.encoded", "r");

while(!feof($res)) {
	echo fread($res, 125);
	echo "\n";
	sleep(1);	
}

fclose($res);
?>
