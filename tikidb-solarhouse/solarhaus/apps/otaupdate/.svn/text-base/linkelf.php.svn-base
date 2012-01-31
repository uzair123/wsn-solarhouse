<?
function makeAssocSymbtab($input) {
    $lines = split("\n", $input);
    foreach($lines as $line) {
	$line = trim($line);
	$cols = split(" ", $line);
	if(count($cols) == 3)
	    $result[$cols[2]] = $cols[1];
	else if(count($cols) == 2)
	    $result[$cols[1]] = $cols[0];
    }
    return $result;
}

if($argc!=4) {
    echo "Usage: linkelf.php <main_module> <elf_obj_folder> <elf_name>";
}
else {
    $elf_obj_folder = $argv[2];
    $main_obj = $argv[1];
    chdir($elf_obj_folder);
    $elf_obj = $elf_obj_folder.$argv[3];
    //$objlist = "wrapper.ce db-list.o dbms.o dbms-operations.o db.o db-operations.o frag_unicast.o querymanager.o querymanager-qidlist.o query-parser.o queryresolver.o queryresolver-util.o scopes-flooding.o scopes-membership.o scopes.o scopes-selfur.o tikidb.o workaround.o runicast.o netflood.o sensorstation.o humidity-sensor.o tmp102.o ipolite.o stunicast.o";
    $objlist = $argv[3];
    $finished = 0;

    while(!$finished) {
	$main_module = shell_exec("msp430-nm $main_obj");
	$elf_module = shell_exec("msp430-nm -u $elf_obj");
	$finished = 1;
	$main_symbtab = makeAssocSymbtab($main_module);
	$elf_symbtab = makeAssocSymbtab($elf_module);
	foreach($elf_symbtab as $key=>$value) {
	    if($value == "U" || $value == "u") {
		if(!isset($main_symbtab[$key]) || ($main_symbtab[$key] == "U" || $main_symbtab[$key] == "u")) {
		    //if(substr($key,0,1)!="_") {
			$finished = 0;
			echo "Symbol $key is undefined!\n";
			//Get the file with missing symbol
			$files = shell_exec("grep -l $key ".$elf_obj_folder."*.o");
			if($files=="") {
			    echo "Error: Could not find symbol $key\n";
			    exit;
			}
			$files = split("\n", $files);
			for($i=0; $i<count($files); $i++) {
				echo $files[$i]."\n";
				if($files[$i]!="") {
					//Check if the found object file contains the symbol
					$check_module = shell_exec("msp430-nm $files[$i]");
					$check_module_symbtab = makeAssocSymbtab($check_module);
					if(!array_key_exists($key, $check_module_symbtab) || $check_module_symbtab[$key]=="U") {
						//Symbol not included - continue to next file
						continue;
					}
					//Checks if the found object file is already linked
					$files[$i] = str_replace($elf_obj_folder, "", $files[$i]);
					if(strpos($objlist, $files[$i])==FALSE) {
					$file = $files[$i];
					break;
					}
				}
			}
			echo "Found in $file\n";
			$objlist = $objlist." $file";
			break;
		    //}
		}
	    }
	}
	if(!$finished) {
	    //Relink
	    $cmd = "msp430-ld --oformat elf32-msp430 -m msp430x2617 -d -r -o output ".$objlist;
	    echo "Executing: $cmd \n";
	    shell_exec($cmd);
	    $elf_obj = $elf_obj_folder."output";
	}
    }
    $cmd = "msp430-strip --strip-unneeded -g -x output";
	echo "Executing: $cmd \n";
    shell_exec($cmd);
}

?>
