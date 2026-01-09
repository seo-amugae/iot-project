<?php

$datetime = date('YmdHis'); $date = "DATE_".date('Y_m_d', strtotime($datetime)); $time = date('His');

$room_temp = $_POST['RT']; $room_humi = $_POST['RH']; $room_hic = $_POST['RC']; $room_index = $_POST['RI']; $room_lux = $_POST['RL'];
$roomtemp = $_POST['rt']; $roomhumi = $_POST['rh']; $roomhic = $_POST['rc']; $roomindex = $_POST['ri'];

$window_status = $_POST['WS']; $window_move = $_POST['WM']; $window_error = $_POST['WE']; $window_voltage = $_POST['WV'];

$outdoor_temp = $_POST['OT']; $outdoor_humi = $_POST['OH']; $outdoor_hic = $_POST['OC']; $outdoor_index = $_POST['OI']; $outdoor_moisture = $_POST['OM'];
$outdoortemp = $_POST['ot']; $outdoorhumi = $_POST['oh']; $outdoorhic = $_POST['oc']; $outdoorindex = $_POST['oi'];

$pad_temp = $_POST['PT']; $pad_hic = $_POST['PC'];
$padtemp = $_POST['pt']; $padhic = $_POST['pc'];

$pad_status = $_POST['PS']; $pad_relay = $_POST['PR']; $padsettemp = $_POST['ps'];

$boiler_status = $_POST['BS']; $boiler_relay = $_POST['BR']; $boilersettemp = $_POST['bs'];

$lpg = $_POST['g5']; $co = $_POST['g7'];

$detect = $_POST['d'];

$length = $_POST['l'];

$connect->query("DROP TABLE IF EXISTS DATE_1970_01_01");
$connect->query("CREATE TABLE IF NOT EXISTS $date(TIME time NOT NULL, room_t float NOT NULL, room_h float NOT NULL, room_hic float NOT NULL, room_index float NOT NULL, room_lux float NOT NULL, outdoor_t float NOT NULL, outdoor_h float NOT NULL, outdoor_hic float NOT NULL, outdoor_index float NOT NULL, outdoor_moisture float NOT NULL, pad_t float NOT NULL, pad_hic float NOT NULL, pad_s tinyint(1) NOT NULL, pad_r tinyint(1) NOT NULL, pad_set_temp tinyint(1) NOT NULL, window_v float NOT NULL, window_s tinyint(1) NOT NULL, window_m tinyint(1) NOT NULL, window_e tinyint(1) NOT NULL, boiler_s tinyint(1) NOT NULL, boiler_r tinyint(1) NOT NULL, boiler_set_temp tinyint(1) NOT NULL, gas5 float NOT NULL, gas7 float NOT NULL, detect tinyint(1) NOT NULL, roomtemp tinyint(1) NOT NULL, roomhumi tinyint(1) NOT NULL, roomhic tinyint(1) NOT NULL, roomindex tinyint(1) NOT NULL, outdoortemp tinyint(1) NOT NULL, outdoorhumi tinyint(1) NOT NULL, outdoorhic tinyint(1) NOT NULL, outdoorindex tinyint(1) NOT NULL, padtemp tinyint(1) NOT NULL, padhic tinyint(1) NOT NULL, length SMALLINT(3) UNSIGNED NOT NULL, PRIMARY KEY(`TIME`)) ENGINE=MyISAM DEFAULT CHARSET=utf8"); // 날짜테이블이 생성되지 않았으면 DATE_yyyy_mm_dd 형식의 테이블 생성

$connect->query("INSERT INTO $date(TIME, room_t, room_h, room_hic, room_index, room_lux, outdoor_t, outdoor_h, outdoor_hic, outdoor_index, outdoor_moisture, pad_t, pad_hic, pad_s, pad_r, pad_set_temp, window_v, window_s, window_m, window_e, boiler_s, boiler_r, boiler_set_temp, gas5, gas7, detect, roomtemp, roomhumi, roomhic, roomindex, outdoortemp, outdoorhumi, outdoorhic, outdoorindex, padtemp, padhic, length)VALUES('$time', '$room_temp', '$room_humi', '$room_hic', '$room_index', '$room_lux', '$outdoor_temp', '$outdoor_humi', '$outdoor_hic', '$outdoor_index', '$outdoor_moisture', '$pad_temp', '$pad_hic', '$pad_status', '$pad_relay', '$padsettemp', '$window_voltage', '$window_status', '$window_move', '$window_error', '$boiler_status', '$boiler_relay', '$boilersettemp', '$lpg', '$co', '$detect', '$roomtemp', '$roomhumi', '$roomhic', '$roomindex', '$outdoortemp', '$outdoorhumi', '$outdoorhic', '$outdoorindex', '$padtemp', '$padhic', '$length')"); // DATE_yyyy_mm_dd 테이블에 값들 삽입

$identifier = $_POST['id']; $command = $_POST['c']; $useragent = preg_replace('/\r\n|\r|\n/','',$_POST['ua']); $ipaddress = $_POST['ip'];

if (!empty($ipaddress) && !empty($identifier) && !empty($command) && !empty($useragent)) { // POST 파라미터 명령이 넘어온경우
	$connect->query("CREATE TABLE IF NOT EXISTS LOG(DATETIME datetime NOT NULL, ipaddress varchar(60) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL, identifier text NOT NULL, command text NOT NULL, useragent varchar(250) NOT NULL, PRIMARY KEY(`DATETIME`)) ENGINE=MyISAM DEFAULT CHARSET=utf8"); // LOG 테이블이 생성되지 않았으면 테이블 생성

	$connect->query("INSERT INTO LOG(DATETIME, ipaddress, identifier, command, useragent)VALUES('$datetime', '$ipaddress', '$identifier', '$command', '$useragent')"); // LOG 테이블에 명령 수신 시간, IP, 작업 명령어, 유저 에이전트 기록
}

$lastboot = $_POST['lb']; $dhcpip = $_POST['dip'];

if (!empty($lastboot) && !empty($dhcpip)) { // POST 파라미터 명령이 넘어온경우
	$connect->query("CREATE TABLE IF NOT EXISTS LASTBOOT(DATETIME datetime NOT NULL, lastboot datetime NOT NULL, dhcpip varchar(60) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL, PRIMARY KEY(`DATETIME`)) ENGINE=MyISAM DEFAULT CHARSET=utf8"); // LASTBOOT 테이블이 생성되지 않았으면 테이블 생성

	$connect->query("INSERT INTO LASTBOOT(DATETIME, lastboot, dhcpip)VALUES('$datetime', '$lastboot', '$dhcpip')"); // LASTBOOT 테이블에 마지막 부트 기록
}

$window_overload_datetime = $_POST['wd']; $window_overload_voltage = $_POST['wv'];

if (!empty($window_overload_datetime)) { // POST 파라미터 명령이 넘어온경우
	$connect->query("CREATE TABLE IF NOT EXISTS OVERLOAD(DATETIME datetime NOT NULL, window_ov_datetime datetime NOT NULL, window_ov float NOT NULL, PRIMARY KEY(`window_ov_datetime`)) ENGINE=MyISAM DEFAULT CHARSET=utf8"); // OVERLOAD 테이블이 생성되지 않았으면 테이블 생성

    $connect->query("INSERT INTO OVERLOAD(DATETIME, window_ov_datetime, window_ov)VALUES('$datetime', '$window_overload_datetime', '$window_overload_voltage')"); // OVERLOAD 테이블에 마지막 과부하 감지 기록
}
?>
