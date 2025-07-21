<?php

$servername = "localhost";

$dbname = "id21004173_example_esp_data";
// REPLACE with Database user
$username = "id21004173_esp";
// REPLACE with Database user password
$password = "290347aculsiA@";


// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);
// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
} 

$sql = "SELECT id, value1, value2, value3, reading_time FROM Sensor order by reading_time desc limit 40";

$result = $conn->query($sql);

while ($data = $result->fetch_assoc()){
    $sensor_data[] = $data;
}

$readings_time = array_column($sensor_data, 'reading_time');

// ******* Uncomment to convert readings time array to your timezone ********
/*$i = 0;
foreach ($readings_time as $reading){
    // Uncomment to set timezone to - 1 hour (you can change 1 to any number)
    $readings_time[$i] = date("Y-m-d H:i:s", strtotime("$reading - 1 hours"));
    // Uncomment to set timezone to + 4 hours (you can change 4 to any number)
    //$readings_time[$i] = date("Y-m-d H:i:s", strtotime("$reading + 4 hours"));
    $i += 1;
}*/

$value1 = json_encode(array_reverse(array_column($sensor_data, 'value1')), JSON_NUMERIC_CHECK);
$value2 = json_encode(array_reverse(array_column($sensor_data, 'value2')), JSON_NUMERIC_CHECK);
$value3 = json_encode(array_reverse(array_column($sensor_data, 'value3')), JSON_NUMERIC_CHECK);
$reading_time = json_encode(array_reverse($readings_time), JSON_NUMERIC_CHECK);

/*echo $value1;
echo $value2;
echo $value3;
echo $reading_time;*/

$result->free();
$conn->close();
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Device Control</title>
    <link rel="stylesheet" href="styles.css">
    <link rel="stylesheet" href="css/bootstrap.min.css">
    <script src="https://code.highcharts.com/highcharts.js"></script>
    <style>
        body {
        min-width: 310px;
            max-width: 1280px;
            height: 500px;
        margin: 0 auto;
        }
        h2 {
        font-family: Arial;
        font-size: 2.5rem;
        text-align: center;
        }
    </style>
</head>
<body>
    <div class="container">
        <header class="d-flex flex-wrap justify-content-center py-3 mb-4 border-bottom">
          <a href="/" class="d-flex align-items-center mb-3 mb-md-0 me-md-auto link-body-emphasis text-decoration-none">
            <span class="fs-4">Smartfarm Cricket</span>
          </a>
    
          <ul class="nav nav-pills">
            <li class="nav-item"><a href="#" class="nav-link active" aria-current="page">Home</a></li>
            <li class="nav-item"><a href="#" class="nav-link">Features</a></li>
            <li class="nav-item"><a href="#" class="nav-link">Pricing</a></li>
            <li class="nav-item"><a href="#" class="nav-link">FAQs</a></li>
            <li class="nav-item"><a href="#" class="nav-link">About</a></li>
          </ul>
        </header>
      </div>
    <div class="main">
    <div class="device_control">
        <h1>Device Control</h1>
        <div class="switch-container">
            <label class="switch-label">Food</label>
            <label class="switch">
                <input type="checkbox" id="food" class="device-switch" data-device="food">
                <span class="slider round"></span>
            </label>
        </div>
        <div class="switch-container">
            <label class="switch-label">Water</label>
            <label class="switch">
                <input type="checkbox" id="water" class="device-switch" data-device="water">
                <span class="slider round"></span>
            </label>
        </div>
        <div class="switch-container">
            <label class="switch-label">Fan Left</label>
            <label class="switch">
                <input type="checkbox" id="fanl" class="device-switch" data-device="fanl">
                <span class="slider round"></span>
            </label>
        </div>
        <div class="switch-container">
            <label class="switch-label">Fan Right</label>
            <label class="switch">
                <input type="checkbox" id="fanr" class="device-switch" data-device="fanr">
                <span class="slider round"></span>
            </label>
        </div>
    </div>

    <div class="Monitor">
        <h1>Monitor</h1>
        <div class="values">
            <p>Temperature: <span id="temperatureValue"></span> °C</p>
            <p>Humidity: <span id="humidityValue"></span> %</p>
            <p>FoodLevel: <span id="foodLevelValue"></span> %</p>
            <p>WaterLevel: <span id="waterLevelValue"></span> %</p>
        </div>
    </div>
    </div>
    <div class="Level">
        <div class="bottle food-bottle">
            <h2>Food Bottle</h2>
            <div class="water" id="foodWater"></div>
            <div class="indicator food-indicator" style="top: 0;"></div>
            <div class="indicator food-indicator" style="top: 25%;"></div>
            <div class="indicator food-indicator" style="top: 50%;"></div>
            <div class="indicator food-indicator" style="top: 75%;"></div>
            <div class="indicator food-indicator" style="top: 100%;"></div>
        </div>

        <div class="bottle water-bottle">
            <h2>Water Bottle</h2>
            <div class="water" id="waterWater"></div>
            <div class="indicator water-indicator" style="top: 0;"></div>
            <div class="indicator water-indicator" style="top: 25%;"></div>
            <div class="indicator water-indicator" style="top: 50%;"></div>
            <div class="indicator water-indicator" style="top: 75%;"></div>
            <div class="indicator water-indicator" style="top: 100%;"></div>
        </div>
    </div>

    <button style="margin-left: 50%;">Toggle</button>

    <div class="graph">
    <h2>ESP Weather Station</h2>
        <div id="chart-temperature" class="container"></div>
        <div id="chart-humidity" class="container"></div>
        <div id="chart-pressure" class="container"></div>
    </div>
    <script>
        var value1 = <?php echo $value1; ?>;
        var value2 = <?php echo $value2; ?>;
        var value3 = <?php echo $value3; ?>;
        var reading_time = <?php echo $reading_time; ?>;

        var chartT = new Highcharts.Chart({
        chart:{ renderTo : 'chart-temperature' },
        title: { text: 'DHT22 Temperature' },
        series: [{
            showInLegend: false,
            data: value1
        }],
        plotOptions: {
            line: { animation: false,
            dataLabels: { enabled: true }
            },
            series: { color: '#059e8a' }
        },
        xAxis: { 
            type: 'datetime',
            categories: reading_time
        },
        yAxis: {
            title: { text: 'Temperature (Celsius)' }
            //title: { text: 'Temperature (Fahrenheit)' }
        },
        credits: { enabled: false }
        });

        var chartH = new Highcharts.Chart({
        chart:{ renderTo:'chart-humidity' },
        title: { text: 'DHT22 Humidity' },
        series: [{
            showInLegend: false,
            data: value2
        }],
        plotOptions: {
            line: { animation: false,
            dataLabels: { enabled: true }
            }
        },
        xAxis: {
            type: 'datetime',
            //dateTimeLabelFormats: { second: '%H:%M:%S' },
            categories: reading_time
        },
        yAxis: {
            title: { text: 'Humidity (%)' }
        },
        credits: { enabled: false }
        });

        /*
        var chartP = new Highcharts.Chart({
        chart:{ renderTo:'chart-pressure' },
        title: { text: 'BME280 Pressure' },
        series: [{
            showInLegend: false,
            data: value3
        }],
        plotOptions: {
            line: { animation: false,
            dataLabels: { enabled: true }
            },
            series: { color: '#18009c' }
        },
        xAxis: {
            type: 'datetime',
            categories: reading_time
        },
        yAxis: {
            title: { text: 'Pressure (hPa)' }
        },
        credits: { enabled: false }
        });*/
    </script>

    <div class="container">
        <footer class="d-flex flex-wrap justify-content-between align-items-center py-3 my-4 border-top">
          <p class="col-md-4 mb-0 text-body-secondary">© 2023 Company, Inc</p>
      
          <a href="/" class="col-md-4 d-flex align-items-center justify-content-center mb-3 mb-md-0 me-md-auto link-body-emphasis text-decoration-none">
            <svg class="bi me-2" width="40" height="32"><use xlink:href="#bootstrap"></use></svg>
          </a>
      
          <ul class="nav col-md-4 justify-content-end">
            <li class="nav-item"><a href="#" class="nav-link px-2 text-body-secondary">Home</a></li>
            <li class="nav-item"><a href="#" class="nav-link px-2 text-body-secondary">Features</a></li>
            <li class="nav-item"><a href="#" class="nav-link px-2 text-body-secondary">Pricing</a></li>
            <li class="nav-item"><a href="#" class="nav-link px-2 text-body-secondary">FAQs</a></li>
            <li class="nav-item"><a href="#" class="nav-link px-2 text-body-secondary">About</a></li>
          </ul>
        </footer>
    </div>
    <script src="js/bootstrap.min.js"></script>
    <script src="https://www.gstatic.com/firebasejs/9.4.0/firebase-app-compat.js"></script>
    <script src="https://www.gstatic.com/firebasejs/9.4.0/firebase-database-compat.js"></script>
    <script src="app.js"></script>
</body>
</html>