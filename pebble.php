<?php
require "dbinfo.php";

$data = mysqli_real_escape_string($conn,$_GET["data"]);
$id = mysqli_real_escape_string($conn,$_GET["id"]);
$over = mysqli_real_escape_string($conn,$_GET["over"]);
$create = mysqli_real_escape_string($conn,$_GET["create"]);

if($create!=""){
   $sql = "SELECT * FROM pebIn WHERE id='{$id}'";
   $result = mysqli_query($conn, $sql) or die(mysqli_error($conn));
   if(sizeof(mysqli_fetch_assoc($result))==0){
      $sql = "INSERT INTO pebIn SET id='{$id}'";
      $result = mysqli_query($conn, $sql) or die(mysqli_error($conn));
      print("User created with id {$id}.");
   }else{
      print("User exists with id {$id}.");
   }
}elseif($id!=""){
   if($data!=""){
      $sql = "UPDATE pebIn SET command='{$data}' WHERE id='{$id}'";
      $result = mysqli_query($conn, $sql) or die(mysqli_error($conn));
   }

   $sql = "SELECT * FROM pebIn WHERE id='{$id}'";
   $result = mysqli_query($conn, $sql) or die(mysqli_error($conn));

   while($row = mysqli_fetch_assoc($result)) {
      print($row["command"]);
   }

   if($over!=""){
      $sql = "UPDATE pebIn SET command='{$over}', id='{$id}'";
      $result = mysqli_query($conn, $sql) or die(mysqli_error($conn));
   }
}
mysqli_close($conn);
?>