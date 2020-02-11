<!DOCTYPE html>
<html>
	<head>
		<link rel="stylesheet" type="text/css" href="style.css">
		<title>Query Result</title>
	</head>
	<body>
	<!--
		<form action="emptyTable.php" method="post">
			Query: <input type="text" name="query" value="DELETE FROM sensorval">
			<input type="submit" name="emptyTable" value="Go"><br/>
		</form>
		-->
<?php
if (isset($_POST['emptyTable']))
{
	// echo htmlspecialchars($_POST["query"]);
    // connect and select the database 
	$mysqli = mysqli_connect("localhost", "root", "", "jsondb");
	
	if (!$mysqli) {
		echo "Error: Unable to connect to MySQL." . PHP_EOL;
		echo "Debugging errno: " . mysqli_connect_errno() . PHP_EOL;
		echo "Debugging error: " . mysqli_connect_error() . PHP_EOL;
		exit;
	}
	
	// $query = "DELETE FROM stdtable";
	$query = htmlspecialchars($_POST["query"]);
	
	$result = $mysqli->query($query);
	
	if ( $mysqli->query($query))
	{
	  printf("Invalid query: %s<br>Whole query: %s<br>", $mysqli->error, $query);
	  exit();
	}
	else{
		echo "Query: ".$query;
		var_dump($result);
	}
	
	mysqli_close($mysqli);
	
	print_r($result);
	
	if ($result->num_rows > 0) {
		$i=1;
			
			// name,time,data1,data2,arID
			echo "<table class='blueTable'><caption><b>Query Results</b></caption><thead><tr><th>A/A</th><th>arID</th><th>name</th><th>time</th><th>data1</th><th>data2</th></tr></thead><tbody>";
			
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
	}
	elseif ($result->num_rows == 0)
	{
		echo "<br>Rows: ".$result->num_rows;
	}
	else{
		header("Location: /", true, 301);
	}
	
}
else{
	echo "Oooops!";
}
?>

	</body>
</html>