<!DOCTYPE html>
<html>
	<head>
		<link rel="stylesheet" type="text/css" href="style.css">
		<title>Sensor's Data</title>
		<meta http-equiv="refresh" content="100" />
	</head>
	<body>
		<form action="emptyTable.php" method="post">
			<!-- <button type="submit" name="emptyTable" id="emptyTable" value="emptyTable">Delete All</button><br/> -->
			Query: <input type="text" name="query" value="DELETE FROM sensorval" size="100">
			<input type="submit" name="emptyTable" value="Go"><br/>
		</form>
		<br><u>Quick Copy/Paste:</u>
		<br>SELECT * | DELETE | FROM | WHERE | LIMIT
		<br>sensorval | ardID | name | time | data1 | data2

	<?php

		//connect and select the database 
		$mysqli = mysqli_connect("localhost", "root", "", "jsondb");
		
		if (!$mysqli) {
			echo "Error: Unable to connect to MySQL." . PHP_EOL;
			echo "Debugging errno: " . mysqli_connect_errno() . PHP_EOL;
			echo "Debugging error: " . mysqli_connect_error() . PHP_EOL;
			exit;
		}
		
		$query = "SELECT * FROM sensorval";
			
		// $result = $mysqli->query($query);
		// print_r($result);
		
		if ( ($result = $mysqli->query($query))===false )
		{
		  printf("Invalid query: %s<br>Whole query: %s<br>", $mysqli->error, $query);
		  exit();
		}
		
		mysqli_close($mysqli);
		
		if ($result->num_rows > 0) {
			$i=1;
			
			// name,time,data1,data2,arID
			echo "<table class='blueTable'><caption><b>Sensor's Data</b></caption><thead><tr><th>A/A</th><th>arID</th><th>name</th><th>time</th><th>data1</th><th>data2</th></tr></thead><tbody>";
			
			// output data of each row
			while($row = $result->fetch_assoc()) {
				echo "<tr>
				<td>".$i++."</td>
				<td>".$row["ardID"]."</td>
				<td>".$row["name"]."</td>
				<td>".$row["time"]."</td>
				<td>".$row["data1"]."</td>
				<td>".$row["data2"]."</td>
				</tr>";
			}
			echo "</tbody></table>";
		} else {
			echo "0 results";
		}

	?>

	</body>
</html>