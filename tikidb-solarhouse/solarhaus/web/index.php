<?
    // start php session
    session_start(); 
    
    // initial parameters if new session
    if( !isset( $_SESSION['selectedNodes'] ) ) {
      $_SESSION['selectedNodes'] = '';
      $_SESSION['selectedSensorType'] = '';
      $_SESSION['refreshCycle'] = 60;
    }
    
    // connect to database
    include("db_open.php");
    
    // list all nodes represented by the database
    global $nodelist;
    $nodelist = array();
    
    $result = mysql_query( "SELECT DISTINCT node_id FROM `sensorreadings` ORDER BY node_id ASC", $db );
    for( $i=0; $row=mysql_fetch_assoc($result); $i++ ) {
        $nodeID = $row['node_id'];
        array_push( $nodelist, $nodeID );
    }
    //echo 'nodelist: '; print_r($nodelist); echo '<br>';
    
    // handle selectDeselect
    if( isset( $_POST['selectDeselect'] ) ) {
        $selectedNodes = '';
        $selectedSensorType = $_POST['selectedSensorType'];
        // check for all possible nodes if they are selected
        foreach( $nodelist as $nodeID ) {
            if( isset( $_POST['node'.$nodeID] ) ) $selectedNodes .= ';node'.$nodeID;
        }
        $_SESSION[ 'selectedNodes' ] = $selectedNodes;
        $_SESSION[ 'selectedSensorType' ] = $selectedSensorType;
    }
    
    // handle refreshCycle
    if( isset( $_POST['refreshCycle'] ) ) {
        $refreshCycle = $_POST['refreshCycle'];
        $_SESSION['refreshCycle'] = $refreshCycle;
    }
    
    // load data from database
    $sqlNodes = '';
    $subs = explode( ';', $_SESSION[ 'selectedNodes' ] );
    foreach( $subs as $sub ) {
        if( $sub != '' ) $sqlNodes .= "node_id = '".substr( $sub, 4 )."' OR ";
    }
    $sqlNodes = substr( $sqlNodes, 0, strlen( $sqlNodes ) - 3 );
    
    //echo $sqlNodes."<br>";
    //echo $_SESSION['selectedSensorType']."<br>";
    
    $data = mysql_query( "SELECT node_id, ".$_SESSION['selectedSensorType'].", time_stamp FROM `sensorreadings` WHERE ".$sqlNodes." ORDER BY time_stamp ASC", $db );
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>WSN - Solarhaus</title>

<link rel="stylesheet" href="style.css" /></head>

<meta http-equiv="refresh" content="<? echo $_SESSION['refreshCycle'] ?>; URL=./?<? echo "".session_name()."=".session_id() ?>">

<script type='text/javascript' src='https://www.google.com/jsapi'></script>
    <script type='text/javascript'>
      google.load('visualization', '1', {'packages':['annotatedtimeline']});
      google.setOnLoadCallback(drawChart);
      dateFormat(now, "yyyy-mm-dd HH:MM:ss");
      
      function drawChart() {
        var data = new google.visualization.DataTable();
        data.addColumn('datetime', 'Date');
        <?
            // arrays to map from a node to a setting
            if( $data ) {
                global $data_rows_valid, $data_rows_data;
                $data_rows_valid = array(); // is node i a selected/valid node?
                $data_rows_data = array();  // value for node i
                
                $subs = explode( ';', $_SESSION[ 'selectedNodes' ] );
                foreach( $subs as $sub ) {
                    if( $sub != '' ) echo "data.addColumn('number', '".$sub."');\n";
                    $data_rows_valid[ substr( $sub, 4 ) ] = 1;  // node i is selected => valid
                    $data_rows_data[ substr( $sub, 4 ) ] = 0;   // value of node i is 0
                }
                // remove node0 (this node is set by array() in line 79/80)
                unset( $data_rows_valid[0] );
                unset( $data_rows_data[0] );
                
                //echo 'data_rows_valid: '; print_r($data_rows_valid); echo '<br>';
                //echo 'data_rows_data: '; print_r($data_rows_data); echo '<br>';
            } else {
                echo "data.addColumn('number', 'empty');\n";
            }
        ?>
        data.addRows([
        <?
            $date_last = '';
            $date_string = '';
            $date_raw = '';
            $rowcount = 0;
            $newdata = 0;
            
            // check values from database
            if( $data ) {
                for( $i=0; $row=mysql_fetch_assoc( $data ); $i++ ) {
                    $date_raw = $row['time_stamp'];
                    
                    // group values/nodes by time_stamp
                    if( $date_last == '' ) {
                        $date_last = $date_raw;
                        $date_string = str_replace( ' ', ', ', $date_raw );
                        $date_string = str_replace( '-', ', ', $date_string );
                        $date_string = str_replace( ':', ', ', $date_string );
                        
                        // decrement month, because in the anotated timeline the month-value is zero-indexed
                        $date_string_month = substr( $date_string, 6, 2 );
                        $date_string_month = sprintf( "%02u", ( intval( $date_string_month, 10 ) - 1 ) );
                        $date_string = substr_replace( $date_string, $date_string_month, 6, 2 );
                    }
                    
                    // if time_stamp has changed, then write data
                    if( ( $date_last != '' ) & ( $date_last != $date_raw ) ) {
                        $newdata = 0;
                        
                        // comma to seperate entrys only if this is not the first entry
                        if( $rowcount++ != 0 ) echo ",\n";
                        
                        // build new line
                        echo '[new Date('.$date_string.'), ';
                        $iii = 0;
                        foreach( $data_rows_data as $data_data ) {
                            // comma to seperate values only if this is not the first value
                            if( $iii != 0 ) echo ', ';
                            $iii += 1;
                            
                            // convert values depending on their type
                            if( $_SESSION['selectedSensorType'] == 'extern_humidity' ) $data_converted = $data_data / 100.0;
                            if( $_SESSION['selectedSensorType'] == 'extern_temperature' ) $data_converted = $data_data / 100.0;
                            if( $_SESSION['selectedSensorType'] == 'extern_CO2' ) $data_converted = $data_data;
                            if( $_SESSION['selectedSensorType'] == 'rssi' ) $data_converted = $data_data;
                            
                            echo $data_converted;
                        }
                        echo ']';
                        
                        // remember current time_stamp
                        $date_last = $date_raw;
                        $date_string = str_replace( ' ', ', ', $date_raw );
                        $date_string = str_replace( '-', ', ', $date_string );
                        $date_string = str_replace( ':', ', ', $date_string );
                        
                        // decrement month, because in the anotated timeline the month-value is zero-indexed
                        $date_string_month = substr( $date_string, 6, 2 );
                        $date_string_month = sprintf( "%02u", ( intval( $date_string_month, 10 ) - 1 ) );
                        $date_string = substr_replace( $date_string, $date_string_month, 6, 2 );
                    }
                    
                    // save value of this node i into the array on position i
                    if( $data_rows_valid[ $row['node_id'] ] == 1 ) {
                        $newdata = 1;
                        $data_rows_data[ $row['node_id'] ] = $row[ $_SESSION['selectedSensorType'] ];
                    }
                }
            }
            
            if( $newdata == 1 ) {
                // comma to seperate entrys only if this is not the first entry
                if( $rowcount++ != 0 ) echo ",\n";
                
                // build new line
                echo '[new Date('.$date_string.'), ';
                $iii = 0;
                foreach( $data_rows_data as $data_data ) {
                    // comma to seperate values only if this is not the first value
                    if( $iii != 0 ) echo ', ';
                    $iii += 1;
                    
                    // convert values depending on their type
                    if( $_SESSION['selectedSensorType'] == 'extern_humidity' ) $data_converted = $data_data / 100.0;
                    if( $_SESSION['selectedSensorType'] == 'extern_temperature' ) $data_converted = $data_data / 100.0;
                    if( $_SESSION['selectedSensorType'] == 'extern_CO2' ) $data_converted = $data_data;
                    if( $_SESSION['selectedSensorType'] == 'rssi' ) $data_converted = $data_data;
                    
                    echo $data_converted;
                }
                echo ']';
            }
        ?>
        ]);

        var options = {'displayAnnotations': true, 'legendPosition': 'newRow', 'scaleType': 'maximized', 'displayRangeSelector': true};
        var chart = new google.visualization.AnnotatedTimeLine(document.getElementById('chart_div'));
        chart.draw(data, options);
      }
    </script>
</head>
<body>
<table width="100%">
    <tr>
        <td id="headline"><center>WSN Viewer</center></td>
    </tr>
</table>
<table width="100%">
    <tr>
        <td width="20%">
            <form action="./" method="post">
                <input type="text" name="refreshCycle" value="<? echo $_SESSION['refreshCycle'] ?>" size="3"><br>
                <input type="hidden" name="<? echo session_name() ?>" value="<? echo session_id() ?>">
                <input type="submit" name="submit" value="set refreshrate">
            </form>
        </td>
        <td width="80%">
            <table width="100%">
                <tr>
                    <td>
                        <!-- graph -->
                        <div id='chart_div' style='width: 700px; height: 400px;'></div>
                    </td>
                </tr>
                <tr height="10px"></tr>
                <tr>
                    <td>
                        <form action="./" method="post">
                            <?
                                // for each node in the database show a checkbox and check it if this node is checked by the user
                                foreach( $nodelist as $nodeID ) {
                                    if( ( 40 < $nodeID ) & ( $nodeID < 80 ) ) {
                                        echo '<input type="checkbox" name="node'.$nodeID.'" value="node'.$nodeID.'"';
                                        if( strpos( $_SESSION['selectedNodes'], 'node'.$nodeID ) !== false ) {
                                            // Knoten ist ausgewählt
                                            echo ' checked';
                                        }
                                        echo '>'.$nodeID."\n                            ";
                                    }
                                }
                                
                                // for each sensor create an entry and select it is a sensor is selected by the user
                                echo '<select name="selectedSensorType" size="1">'."\n";
                                echo '                                <option ';
                                if( $_SESSION['selectedSensorType'] == 'extern_humidity' ) echo 'selected ';
                                echo 'value="extern_humidity">humidity (in %)</option>'."\n";
                                echo '                                <option ';
                                if( $_SESSION['selectedSensorType'] == 'extern_temperature' ) echo 'selected ';
                                echo 'value="extern_temperature">temperature (in &#176;C)</option>'."\n";
                                echo '                                <option ';
                                if( $_SESSION['selectedSensorType'] == 'extern_CO2' ) echo 'selected ';
                                echo 'value="extern_CO2">CO2 (in ppm)</option>'."\n";
                                echo '                                <option ';
                                if( $_SESSION['selectedSensorType'] == 'rssi' ) echo 'selected ';
                                echo 'value="rssi">rssi</option>'."\n";
                                echo '                            </select>'."\n";
                            ?>
                            <input type="hidden" name="<? echo session_name() ?>" value="<? echo session_id() ?>">
                            <input type="hidden" name="selectDeselect" value="0">
                            <input type="submit" name="submit" value="select">
                        </form>
                    </td>
                </tr>
                <tr height="10px"></tr>
                <tr>
                    <td>
                        <!-- tabelle mit daten -->
                        <!-- <table id="values" border="0" cellspacing="0" cellpadding="3" width="100%">
                            <thead>
                                <tr>
                                    <td class="table_head_right">ID</td>
                                    <td class="table_head_right">node_id</td>
                                    <td class="table_head_right">humidity</td>
                                    <td class="table_head_right">temperature</td>
                                    <td class="table_head_right">extern_CO2</td>
                                    <td class="table_head_right">Voltage</td>
                                    <td style="font-weight:bold">time_stamp</td>
                                    <td width="20"></td>
                                </tr>
                            </thead>
                            <tbody id="table_nodes_live">
                            </tbody>
                        </table> -->
                    </td>
                </tr>
            </table>
        </td>
    </tr>
</table>
</body>
</html>