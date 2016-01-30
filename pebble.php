<?php
$data = $_GET["data"];
require "dbinfo.php";

// 
// $sql = "";
// $badNames = array();
// foreach ($list as $key => $value) {
// 	// lol this is really vulnerable to an SQL injection
// 	$msg = mysqli_real_escape_string($conn,$value->msg);
// 	if(reallyBadWord($msg)){
// 		array_push($badNames,$value->from);
// 	}
// 	$sql .= "INSERT INTO chatlog (fromUser, fromId, toUser, msg, serverId, gameName, timeOfInsert) VALUES ('{$value->from}','{$value->fromId}','{$value->to}','{$msg}','{$value->serverId}','{$value->gameName}',NOW());";
// }
// if (mysqli_multi_query($conn, $sql)) {
// 	do{
// 		$result = mysqli_store_result($conn);
// 		mysqli_free_result($result);
// 	}while(mysqli_next_result($conn));
// 	$sql = "SELECT * FROM Kicks";
// 	$result = mysqli_query($conn, $sql) or die(mysqli_error($conn));
// 	$pairs = array();
// 	while($row = mysqli_fetch_assoc($result)) {
// 		$pairs[] = array($row['Username'],$row['Reason'],$row['action']);
// 	}
// 	echo json_encode(array($badNames,$pairs));
// } else {
//     echo "Error: " . $sql . "<br>" . mysqli_error($conn);
// }
$sql = "SELECT * FROM pebIn";
$result = mysqli_query($conn, $sql) or die(mysqli_error($conn));
mysqli_close($conn);

while($row = mysqli_fetch_assoc($result)) {
   print($row["command"]);
}
?>