<?

$connect = new mysqli($mysql_hostname, $mysql_username, $mysql_password, $mysql_database);

$yesterday = date("Y_m_d", $_SERVER['REQUEST_TIME'] - 86400);
?>
<!DOCTYPE HTML>
<html lang='ko'>
<head>
<link rel='shortcut icon' href='https://chaegeon.com/files/attach/xeicon/favicon.ico'>
<link rel='apple-touch-icon' href='https://chaegeon.com/files/attach/xeicon/apple.png'>
<meta charset='UTF-8'>
<meta name='viewport' content='initial-scale=1.0,user-scalable=yes,maximum-scale=1,width=device-width'>
<meta http-equiv='X-UA-Compatible' content='IE=edge'>
<title>베이스포트</title>
<style>
body {font-family:-apple-system,BlinkMacSystemFont,Apple SD Gothic Neo,sans-serif;font-size:14px;}
::-webkit-scrollbar {display:none;}
.bigbox {width:100%;max-width:620px;margin:0 auto;border:0;border-top: solid 3px silver;}
.bigboxtd {background-color:#C2C2C2;}
.bixboxtd1 {width:100%;max-width:618px;border:1px solid #DFDFDF;border-top:none;text-align:center;}
.bixboxh1 {text-align:center;}
.middlebox {width:100%;max-width:610px;margin:0 1px 0 1px;text-align:center;}
.middleboxdiv {margin:0 0 20px 0;text-align:left;}
.smallbox {width:100%;table-layout:fixed;border-collapse:collapse;border-spacing:0;color:#444444;}
.mainbar {padding:3px;background:#C2C2C2;border:1px solid #C2C2C2;text-align:center;}
.valueinfo {padding:5px;background:#E0E0E0;border:1px solid silver;text-align:center;}
.value {padding:5px;border:1px solid silver;text-align:center;}
.emphasis {background:#FFA8A8 !important;}
.button {all:unset;cursor:pointer;}
</style>
<script src='https://code.jquery.com/jquery-2.2.4.min.js'></script>
</head>
<body>
<script>
setInterval(function() {
	$('#a').load(location.href+' #a>*','');
}, 1499);
history.replaceState({},null,location.pathname);
</script>
<table class='bigbox'>
<tbody>
<tr>
<td class='bigboxtd'>
<h1 class='bixboxh1' style='font-size:26px !important;'>SMART HOME HISTORY <?echo "(".date("Y.m.d.", $_SERVER['REQUEST_TIME'] - 86400).")"?></h1>
</td>
</tr>
<tr>
<td class='bixboxtd1'>
<table class='middlebox'>
<tbody id='a'>
<tr>
<td>
<div class='middleboxdiv'>
<table class='smallbox'>
<colgroup>
<col style='width:12.5%'>
<col style='width:12.5%'>
<col style='width:12.5%'>
<col style='width:12.5%'>
<col style='width:12.5%'>
<col style='width:12.5%'>
<col style='width:12.5%'>
<col style='width:12.5%'>
</colgroup>
<tbody>
<?
$valuerx = mysqli_query($connect, "SELECT * FROM DATE_".$yesterday." ORDER BY TIME DESC Limit 1");

while ($row = mysqli_fetch_array($valuerx)) {

	$lpg = ($row['gas5'] + $row['boiler5']) / 2;
	$co = ($row['gas7'] + $row['boiler7']) / 2;
	
    echo "<tr>
<td class='mainbar' colspan='8'><b>실내</b></td>
</tr>
<tr>
<td class='valueinfo' colspan='2'>온도</td>
<td class='valueinfo' colspan='2'>습도</td>
<td class='valueinfo' colspan='2'>체감온도</td>
<td class='valueinfo' colspan='2'>불쾌지수</td>
</tr>
<tr>
<td class='value' colspan='2'>".$row['room_t']."℃</td>
<td class='value' colspan='2'>".$row['room_h']."%</td>
<td class='value' colspan='2'>".$row['room_hic']."℃</td>
<td class='value' colspan='2'>".$row['room_index']."</td>
</tr>
<tr>
<td class='mainbar' colspan='8'><b>보일러</b></td>
</tr>
<tr>
<td class='valueinfo' colspan='2'>온도</td>
<td class='valueinfo' colspan='2'>습도</td>
<td class='valueinfo' colspan='2'>LPG 감지</td>
<td class='valueinfo' colspan='2'>CO 감지</td>
</tr>
<tr>
<td class='value' colspan='2'>".$row['boiler_t']."℃</td>
<td class='value' colspan='2'>".$row['boiler_h']."%</td>
<td class='value' colspan='2'>".$lpg."ppm</td>
<td class='value' colspan='2'>".$co."ppm</td>
</tr>
<tr>
<td class='mainbar' colspan='8'><b>전기매트</b></td>
</tr>
<tr>
<td class='valueinfo' colspan='2'>온도</td>
<td class='valueinfo' colspan='2'>습도</td>
<td class='valueinfo' colspan='4'>체감온도</td>
</tr>
<tr>
<td class='value' colspan='2'>".$row['pad_t']."℃</td>
<td class='value' colspan='2'>".$row['pad_h']."%</td>
<td class='value' colspan='4'>".$row['pad_hic']."℃</td>
</tr>
<tr>
<td class='mainbar' colspan='8'><b>실외</b></td>
</tr>
<tr>
<td class='valueinfo'>온도</td>
<td class='valueinfo'>체감온도</td>
<td class='valueinfo' colspan='2'>습도</td>
<td class='valueinfo' colspan='2'>불쾌지수</td>
<td class='valueinfo' colspan='2'>수분감지</td>
</tr>
<tr>
<td class='value'>".$row['outdoor_t']."℃</td>
<td class='value'>".$row['outdoor_hic']."℃</td>
<td class='value' colspan='2'>".$row['outdoor_h']."%</td>
<td class='value' colspan='2'>".$row['outdoor_hic']."</td>
<td class='value' colspan='2'>".$row['outdoor_moisture']."</td>
</tr>
";
}
?>
</tbody>
</table>
</div>
<p>Designed by chaegeon in south korea</p>
</td>
</tr>
</tbody>
</table>
</td>
</tr>
</tbody>
</table>
</body>