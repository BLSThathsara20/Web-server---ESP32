#include <WiFi.h>
#include <WebServer.h>

// WiFi credentials
const char *ssid = "AndroidAP";
const char *password = "thathsara2";

// Pin definitions
const int SOUND_DIGITAL_PIN = 13; // DO pin - GPIO13
const int SOUND_ANALOG_PIN = 32;  // AO pin - GPIO32 (ADC1_CH4)
const int SOUND_THRESHOLD = 2048; // Adjust this value based on your sensor's sensitivity
const int LED_BUILTIN = 2;        // ESP32 built-in LED pin

// Create web server object on port 80
WebServer server(80);

void setup()
{
    // Start serial communication with proper encoding
    Serial.begin(115200);
    delay(1000);

    // Configure ADC for sound sensor
    analogSetPinAttenuation(SOUND_ANALOG_PIN, ADC_11db);
    analogSetWidth(12);

    // Configure pins
    pinMode(SOUND_DIGITAL_PIN, INPUT);
    pinMode(SOUND_ANALOG_PIN, INPUT);

    // Set WiFi mode explicitly
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    Serial.println("\n");
    Serial.println("Connecting to WiFi...");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\n----------------------------------------");
    Serial.println("Successfully connected to WiFi network!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("----------------------------------------");

    // Initialize sound sensor pins with proper mode
    pinMode(SOUND_DIGITAL_PIN, INPUT); // Digital pin for threshold detection
    pinMode(SOUND_ANALOG_PIN, INPUT);  // Analog pin for sound level

    // Add ADC configuration for better readings
    analogSetWidth(12);             // Set ADC width to 12 bits
    analogSetAttenuation(ADC_11db); // Set ADC attenuation for 3.3V range

    // Add LED setup
    pinMode(LED_BUILTIN, OUTPUT);

    // Define server routes
    server.on("/", handleRoot);
    server.on("/getSoundLevel", handleGetSoundLevel);
    server.begin();
    Serial.println("HTTP server started");
}

void loop()
{
    server.handleClient();

    // Debug sound levels
    static unsigned long lastDebug = 0;
    if (millis() - lastDebug > 100) // Check every 100ms
    {
        lastDebug = millis();
        int analogValue = analogRead(SOUND_ANALOG_PIN);
        int digitalValue = digitalRead(SOUND_DIGITAL_PIN);

        // Print detailed debug info
        Serial.println("\n--- Sound Sensor Values ---");
        Serial.printf("Analog Level: %d\n", analogValue);
        Serial.printf("Digital State: %s\n", digitalValue ? "HIGH (Sound Detected)" : "LOW (Quiet)");

        // Visual representation of sound level
        Serial.print("Level: [");
        int bars = map(analogValue, 0, 4095, 0, 50);
        for (int i = 0; i < 50; i++)
        {
            Serial.print(i < bars ? '|' : ' ');
        }
        Serial.println("]");
    }
}

void handleRoot()
{
    String html = R"(<!DOCTYPE html><html>
    <head>
        <title>ESP32 Real-time Sound Monitor</title>
        <script src='https://cdn.tailwindcss.com'></script>
        <script src='https://cdn.jsdelivr.net/npm/chart.js'></script>
        <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    </head>
    <body class='bg-gradient-to-br from-gray-50 to-gray-100 min-h-screen'>
        <!-- Project Description -->
        <div class='container mx-auto px-4 py-4 sm:py-8 max-w-7xl'>
            <div class='bg-white/80 backdrop-blur-sm rounded-lg shadow-lg p-4 sm:p-6 mb-4 sm:mb-8 border border-blue-100'>
                <div class='max-w-3xl mx-auto text-center'>
                    <h1 class='text-2xl sm:text-4xl font-bold text-blue-600 mb-2 sm:mb-4'>ESP32 Real-time Sound Monitor</h1>
                    <p class='text-sm sm:text-base text-gray-600 mb-4'>An IoT project that demonstrates real-time sound level monitoring using ESP32 and KY-037 Sound Sensor. Features include live sound level visualization, digital state detection, and responsive web interface.</p>
                    <div class='flex flex-wrap justify-center gap-2 sm:gap-4 text-xs sm:text-sm'>
                        <span class='px-2 sm:px-3 py-1 bg-blue-100 text-blue-800 rounded-full'>ESP32</span>
                        <span class='px-2 sm:px-3 py-1 bg-green-100 text-green-800 rounded-full'>Real-time Monitoring</span>
                        <span class='px-2 sm:px-3 py-1 bg-purple-100 text-purple-800 rounded-full'>Sound Detection</span>
                        <span class='px-2 sm:px-3 py-1 bg-yellow-100 text-yellow-800 rounded-full'>IoT</span>
                        <span class='px-2 sm:px-3 py-1 bg-red-100 text-red-800 rounded-full'>WebSocket</span>
                    </div>
                </div>
            </div>

            <!-- Main Dashboard -->
            <div class='bg-white rounded-lg shadow-lg p-4 sm:p-6 max-w-full mx-auto border border-gray-200'>
                <!-- Status Bar -->
                <div class='flex flex-col sm:flex-row sm:items-center justify-between mb-4 sm:mb-8 gap-4'>
                    <div class='flex items-center gap-3'>
                        <h2 class='text-xl sm:text-2xl font-bold text-gray-800'>Dashboard</h2>
                        <div class='hidden sm:block h-6 w-px bg-gray-300'></div>
                        <span class='hidden sm:block text-gray-500 text-sm'>Live Monitoring</span>
                    </div>
                    <div class='flex flex-wrap gap-2 sm:gap-3'>
                        <span id='serverStatus' class='px-3 py-1 rounded-full text-xs sm:text-sm font-semibold bg-gray-200 flex items-center gap-2'>
                            <span class='w-2 h-2 rounded-full bg-gray-400'></span>
                            Connecting...
                        </span>
                        <span id='sensorStatus' class='px-3 py-1 rounded-full text-xs sm:text-sm font-semibold bg-gray-200 flex items-center gap-2'>
                            <span class='w-2 h-2 rounded-full bg-gray-400'></span>
                            Checking...
                        </span>
                    </div>
                </div>
                
                <!-- Grid Layout -->
                <div class='grid grid-cols-1 lg:grid-cols-4 gap-4 sm:gap-6'>
                    <!-- Left Column - Info -->
                    <div class='lg:col-span-1 space-y-4 sm:space-y-6'>
                        <div class='bg-gray-50 p-3 sm:p-4 rounded-lg'>
                            <h2 class='text-base sm:text-lg font-semibold mb-2'>Sound Level</h2>
                            <div class='flex items-center'>
                                <div class='w-full bg-gray-200 rounded-full h-3 sm:h-4'>
                                    <div id='soundLevel' class='bg-blue-600 h-3 sm:h-4 rounded-full transition-all duration-300' style='width: 0%'></div>
                                </div>
                                <span id='soundValue' class='ml-2 sm:ml-4 font-mono text-sm sm:text-base min-w-[3ch] sm:min-w-[4ch]'>0</span>
                            </div>
                        </div>

                        <div class='bg-gray-50 p-3 sm:p-4 rounded-lg'>
                            <h2 class='text-base sm:text-lg font-semibold mb-2'>Digital Status</h2>
                            <div id='digitalStatus' class='text-center py-2 rounded font-semibold text-sm sm:text-base'>Waiting...</div>
                        </div>

                        <div class='bg-gray-50 p-3 sm:p-4 rounded-lg'>
                            <h2 class='text-base sm:text-lg font-semibold mb-2'>Server Info</h2>
                            <div class='space-y-2 text-xs sm:text-sm'>
                                <div class='flex justify-between items-center'>
                                    <span class='text-gray-600'>IP Address:</span>
                                    <span class='font-mono'>)";

    html += WiFi.localIP().toString();

    html += R"(</span>
                                </div>
                                <div class='flex justify-between items-center'>
                                    <span class='text-gray-600'>Memory:</span>
                                    <span class='font-mono'>)";

    html += String(ESP.getFreeHeap()) + " bytes free";

    html += R"(</span>
                                </div>
                            </div>
                        </div>
                    </div>

                    <!-- Right Column - Chart -->
                    <div class='lg:col-span-3 bg-gray-50 p-3 sm:p-4 rounded-lg h-[300px] sm:h-[400px] lg:h-[450px]'>
                        <h2 class='text-base sm:text-lg font-semibold mb-2'>Sound Level History</h2>
                        <div class='h-[250px] sm:h-[350px] lg:h-[400px]'>
                            <canvas id='soundChart'></canvas>
                        </div>
                    </div>
                </div>
            </div>

            <!-- Footer -->
            <div class='mt-4 sm:mt-8 text-center text-gray-500 text-xs sm:text-sm'>
                <p class='mt-1 sm:mt-2'>Created by Savindu Thathsara &#169; 2024</p>
            </div>
        </div>
        <script>
            // Define baseline value for sound detection
            const baseline = 1900;  // This should match the value in handleGetSoundLevel()
            
            const ctx = document.getElementById('soundChart').getContext('2d');
            const soundChart = new Chart(ctx, {
                type: 'line',
                data: {
                    labels: Array(100).fill(''),
                    datasets: [{
                        label: 'Sound Level',
                        data: Array(100).fill(1900),  // Use actual number instead of baseline
                        borderColor: 'rgb(37, 99, 235)',
                        tension: 0.3,
                        fill: true,
                        backgroundColor: 'rgba(37, 99, 235, 0.1)',
                        pointRadius: 0
                    }]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    scales: {
                        y: {
                            beginAtZero: true,
                            max: 4095,
                            grid: {
                                color: 'rgba(0, 0, 0, 0.1)'
                            },
                            ticks: {
                                callback: function(value) {
                                    return value.toFixed(0);
                                }
                            }
                        },
                        x: {
                            display: false,
                            grid: {
                                display: false
                            }
                        }
                    },
                    plugins: {
                        legend: {
                            display: false
                        }
                    },
                    animation: false
                }
            });

            // Optimize update frequency
            let lastUpdate = 0;
            const updateInterval = 50;  // 50ms update interval
            let connectionFails = 0;

            function updateSoundLevel() {
                const now = Date.now();
                if (now - lastUpdate < updateInterval) return;
                lastUpdate = now;

                fetch('/getSoundLevel')
                    .then(response => response.json())
                    .then(data => {
                        connectionFails = 0;
                        
                        // Update server status
                        document.getElementById('serverStatus').innerHTML = 
                            '<span class="w-2 h-2 rounded-full bg-green-500"></span>Server Online';
                        document.getElementById('serverStatus').className = 
                            'px-3 py-1 rounded-full text-sm font-semibold bg-green-100 text-green-800 flex items-center gap-2';
                        
                        // Update sensor status with improved styling
                        const sensorDot = data.analog > 1900 ? 'bg-green-500' : 'bg-yellow-500 animate-pulse';
                        const sensorClass = data.analog > 1900 ? 
                            'bg-green-100 text-green-800' : 'bg-yellow-50 text-yellow-800 border border-yellow-300';
                        const sensorText = data.analog > 1900 ? 'Sensor Active' : 'No Signal';
                        
                        document.getElementById('sensorStatus').innerHTML = 
                            `<span class="w-2 h-2 rounded-full ${sensorDot}"></span>
                             <span>${sensorText}</span>`;
                        document.getElementById('sensorStatus').className = 
                            `px-3 py-1 rounded-full text-sm font-semibold ${sensorClass} flex items-center gap-2 transition-all duration-300`;
                        
                        // Update sound level bar
                        const percentage = (data.analog / 4095) * 100;
                        document.getElementById('soundLevel').style.width = `${percentage}%`;
                        document.getElementById('soundValue').textContent = data.analog;
                        
                        // Update digital status
                        const statusDiv = document.getElementById('digitalStatus');
                        if (data.digital) {
                            statusDiv.textContent = 'Sound Detected!';
                            statusDiv.className = 'bg-green-500 text-white text-center py-2 rounded font-semibold animate-pulse';
                        } else {
                            statusDiv.textContent = 'Quiet';
                            statusDiv.className = 'bg-gray-300 text-gray-700 text-center py-2 rounded font-semibold';
                        }
                        
                        // Update chart
                        const chartData = soundChart.data.datasets[0].data;
                        chartData.push(data.analog);
                        if (chartData.length > 100) {
                            chartData.shift();
                        }
                        soundChart.update('none');
                    })
                    .catch(err => {
                        connectionFails++;
                        if(connectionFails > 5) {
                            document.getElementById('serverStatus').innerHTML = 
                                '<span class="w-2 h-2 rounded-full bg-red-500"></span>Server Offline';
                            document.getElementById('serverStatus').className = 
                                'px-3 py-1 rounded-full text-sm font-semibold bg-red-100 text-red-800 flex items-center gap-2';
                        }
                    });
            }

            // Start updates
            setInterval(updateSoundLevel, 20);
        </script>
    </body>
    </html>)";

    server.send(200, "text/html", html);
}

void handleGetSoundLevel()
{
    // Increase sensitivity with more frequent sampling
    int maxValue = 0;
    const int samples = 200; // Increased samples

    for (int i = 0; i < samples; i++)
    {
        int value = analogRead(SOUND_ANALOG_PIN);
        maxValue = max(maxValue, value);
        delayMicroseconds(50); // Faster sampling
    }

    // Enhanced sensitivity calculation
    static int lastValue = 0;
    static int baseline = 1900; // Adjust this based on your ambient noise
    int analogValue = (maxValue + lastValue) / 2;
    lastValue = maxValue;

    // Amplify the difference from baseline
    int amplifiedValue = baseline + (analogValue - baseline) * 2;
    amplifiedValue = constrain(amplifiedValue, 0, 4095);

    // More sensitive digital detection
    bool soundDetected = (analogValue > baseline + 100); // Adjust threshold
    digitalWrite(LED_BUILTIN, soundDetected);            // Control LED

    String json = "{\"analog\":" + String(amplifiedValue) + ",\"digital\":" + String(soundDetected) + "}";
    server.send(200, "application/json", json);
}
