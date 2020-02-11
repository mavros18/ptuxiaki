<?php
	function _bccomp($a, $b, $operator = '=')
	{
		return version_compare(bccomp($a, $b), 0, $operator);
	}

	$debugprint=false;
	
	//connect and select the database 
	$mysqli = mysqli_connect("localhost", "root", "", "jsondb");
	
	if (!$mysqli) {
		echo "!!! Error: Unable to connect to MySQL." . PHP_EOL;
		echo "Debugging errno: " . mysqli_connect_errno() . PHP_EOL;
		echo "Debugging error: " . mysqli_connect_error() . PHP_EOL;
		exit;
	}

	if ($debugprint) {
		echo "Success: A proper connection to MySQL was made! The jsondb database is great." . PHP_EOL;
		echo "Host information: " . mysqli_get_host_info($mysqli) ."\n";
	}

	// get the contents of the JSON file 
	//$jsonCont = file_get_contents('sample.json');
	$jsonCont = filter_input(INPUT_POST, 'json');  
	
	// decode JSON data to PHP array 
	$content = json_decode($jsonCont, true);
	

	$stdarID = $content['ARDID'];
	
	
	// commands for arduino
	// Tell arduino to take action per sensors
	// Commands indicating priority: 
	// 0:no_action, 1:low, 2:medium, 3:high, 4:critical
	
	$myCommandsArray = array();
	$myCommand = array();
	
	$nicePrint = "\nName\t\tTime\t\tData_1\t\tData_2\n---------------------------------------------------------\n";
	
	$endSQL = ",";
	$query = "INSERT INTO sensorval(name,time,data1,data2,ardID) VALUES ";
	
	// Get current rules from database
	$queryRules = "SELECT * FROM senrules";
	if(!$result = $mysqli->query($queryRules)) 
	{ 
		printf("Invalid query: %s\nWhole query: %s\n", $mysqli->error, $queryRules);
		die();
	} 
	else{ 
		if ($result->num_rows > 0) {
			//print_r("Result:\n");
			//print_r($result);
			$rules = array();
			while($row = $result->fetch_assoc()) {
				$currentRow = array ($row['ardID'],$row['name'],$row['effective_since'],$row['alert_low'],$row['alert_medium'],$row['alert_high'],$row['alert_critical']);
				array_push($rules, $currentRow);
			}
			// print_r("--");
			// print_r($rules);
		}
		else {
			echo "No rules was found in database!";
			die();
		}
	} 
		
	
	
	for ($i = 0; $i < 10; $i++) {
		$stdName = $content['sensors'][$i]['name'];
		$stdTime = $content['sensors'][$i]['time'];
		$stdData = $content['sensors'][$i]['data'];
		
		// we have the rules. apply them...
		
		for ($y=0; $y < sizeof($rules); $y++) {
			$ruleArdID = $rules[$y]['0'];
			$ruleName = $rules[$y]['1'];
			$ruleTime = $rules[$y]['2'];
			$ruleLOW = $rules[$y]['3'];
			$ruleMEDIUM = $rules[$y]['4'];
			$ruleHIGH = $rules[$y]['5'];
			$ruleCRITICAL = $rules[$y]['6'];
			
			
			// if (floatval($stdData[0]) >= floatval($ruleLOW) && floatval($stdData[0]) < floatval($ruleMEDIUM)) {
				// echo floatval($stdData[0])." - ".$ruleArdID." - ".$ruleName." - ".$ruleTime." - ".$ruleLOW." - ".$ruleMEDIUM." - ".$ruleHIGH." - ".$ruleCRITICAL."\n";
			// }
			
			
			// if ruleDate is older than today
			if ($ruleTime < date("Y-m-d")){
				// echo strcmp($stdName, $ruleName);
				if (strcmp($stdName, $ruleName) == 0) {
					
					$pri='0';
					if (floatval($stdData[0]) >= floatval($ruleLOW) && floatval($stdData[0]) < floatval($ruleMEDIUM)) {
						$pri='1';
					} 
					elseif (floatval($stdData[0]) >= floatval($ruleMEDIUM) && floatval($stdData[0]) < floatval($ruleHIGH)){
						$pri='2';
					} 
					elseif (floatval($stdData[0]) >= floatval($ruleHIGH) && floatval($stdData[0]) < floatval($ruleCRITICAL)){
						$pri='3';
					} 
					elseif (floatval($stdData[0]) >= floatval($ruleCRITICAL)) {
						$pri='4';
					} 
					else {
						$pri='0';
					}
					
					if (count($stdData)==2) {
						if (floatval($stdData[0]) >= floatval($ruleLOW) && floatval($stdData[0]) < floatval($ruleMEDIUM)) {
							$pri='1';
						} 
						elseif (floatval($stdData[0]) >= floatval($ruleMEDIUM) && floatval($stdData[0]) < floatval($ruleHIGH)){
							$pri='2';
						} 
						elseif (floatval($stdData[0]) >= floatval($ruleHIGH) && floatval($stdData[0]) < floatval($ruleCRITICAL)){
							$pri='3';
						} 
						elseif (floatval($stdData[0]) >= floatval($ruleCRITICAL)) {
							$pri='4';
						} 
						else {
							$pri='0';
						}
					}
					$myCommand[$stdName]=$pri;
				}
				
				
				
			} 
			
		}
		$myCommand;
		
		
		
		
		
		if ($i==9) {
			$endSQL = ";";
		}
		//fix columns tab
		$x=strlen($stdName);
		while ($x<8) {
			$x++;
			$stdName .= " ";
		}
		
		if (count($stdData)==2) {
			$query .= "('$stdName','$stdTime','$stdData[0]','$stdData[1]','$stdarID')".$endSQL;
			$nicePrint .= $stdName ."\t".$stdTime."\t\t".$stdData[0]."\t".$stdData[1]."\n";
		}
		else {
			$query .= "('$stdName','$stdTime','$stdData[0]',NULL,'$stdarID')".$endSQL;
			$nicePrint .= $stdName ."\t".$stdTime."\t\t".$stdData[0]."\t\t"."   -"."\t\n";
		}
		
		// $myCommandsArray .= $myCommand;
		
	}
	
	$commander = array($stdarID,$myCommand);
	
	
	
	if (!$commander==""){
		// echo "\nCommander:\n" .$commander."}";
		$myJSON = json_encode($commander);
		echo $myJSON;
	}
	
	
	if ($debugprint) {
		echo "\nReceived from ARDUINO: ".$stdarID;
		echo $nicePrint;
	}
	
	if(!$result = $mysqli->query($query)) 
	{ 
		// echo "Query: ".$query;
		// die("\n!!! Error : Query Not Executed. Please Fix the Issue! " . mysqli_error());
		printf("Invalid query: %s\nWhole query: %s\n", $mysqli->error, $query);
		die();
	} 
	else{ 
		if ($debugprint) {
			echo "\nSuccessfuly added in database!";
		}
		
	} 
	
	mysqli_close($mysqli);
?> 