<?php

$datetime = date('Y.m.d.H:i:s');
$ipaddress = $_POST['ip'];
$window_s = $_POST['window'];
$boiler_s = $_POST['boiler'];
$boiler_low = $_POST['boilerlow'];
$boiler_high = $_POST['boilerhigh'];
$pad_s = $_POST['pad'];
$pad_low = $_POST['padlow'];
$pad_high = $_POST['padhigh'];

$connect->query("INSERT INTO SETUP (datetime, window_s, pad_s, boiler_s, pad_low, pad_high, boiler_low, boiler_high) VALUES ('$datetime', '$window_s', '$pad_s', '$pad_low', '$pad_high', '$boiler_s', '$boiler_low', '$boiler_high')");

header("HTTP/1.1 302 Temporarily Moved");
header("Location: http://seo.chaegeon.com");*/

/*
$sql = "SELECT window_s, pad_s, boiler_s, pad_low, pad_high, boiler_low, boiler_high FROM setup ORDER BY datetime DESC LIMIT 1";
$result = mysqli_query($connect, $sql);

while ($row = mysqli_fetch_array($result)) {
    if ($row['window_s'] > 0) echo "ws=".$row['window_s'].".";
	if ($row['pad_s'] > 0) echo "ps=".$row['pad_s'].".";
	if ($row['boiler_s'] > 0) echo "bs=".$row['boiler_s'].".";
	if ($row['pad_low'] > 0) echo "pl=".$row['pad_low'].".";
	if ($row['pad_high'] > 0) echo "ph=".$row['pad_high'].".";
	if ($row['boiler_low'] > 0) echo "bl=".$row['boiler_low'].".";
	if ($row['boiler_high'] > 0) echo "bh=".$row['boiler_high'].".";
}
?>
