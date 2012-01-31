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

if($argc!=3) {
    echo "Usage: checksymbols.php <main_module> <elf_module>";
}
else {
    $main_module = shell_exec("msp430-nm $argv[1]");
    $elf_module = shell_exec("msp430-nm -u $argv[2]");

    $main_symbtab = makeAssocSymbtab($main_module);
    $elf_symbtab = makeAssocSymbtab($elf_module);
    foreach($elf_symbtab as $key=>$value) {
	if($value == "U" || $value == "u") {
	    if(!isset($main_symbtab[$key]) || ($main_symbtab[$key] == "U" || $main_symbtab[$key] == "u")) {
		echo "Symbol $key is undefined!\n";
	    }
	}
    }
}

?>
