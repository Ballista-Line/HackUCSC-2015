<?php
require "dbinfo.php";

$data = mysqli_real_escape_string($conn,$_GET["data"]);

if($data!=""){
   $sql = "UPDATE pebIn SET command='{$data}'";
   $result = mysqli_query($conn, $sql) or die(mysqli_error($conn));
}

$sql = "SELECT * FROM pebIn";
$result = mysqli_query($conn, $sql) or die(mysqli_error($conn));

mysqli_close($conn);

while($row = mysqli_fetch_assoc($result)) {
   print($row["command"]);
}
?>