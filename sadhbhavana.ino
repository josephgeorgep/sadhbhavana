#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>



// Network Credentials

const char* ssid     = "WIFI";
const char* password = "PASSWORD";




// OpenAI API Key

const char* apiKey = "APIKEY";

String apiUrl = "https://api.openai.com/v1/chat/completions";
String finalPayload = "";




bool initialPrompt = true;
bool gettingResponse = true;

HTTPClient http;

void setup() {
  // Initialize Serial
  Serial.begin(115200);

  // Connect to Wi-Fi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print('.');
      delay(1000);
  }
  
  // WiFi Connected
  Serial.println("\nConnected to WiFi");
  
  // Display welcome message after WiFi connection
  Serial.println("Welcome to Sadhbhavana AI. How may I help you today?");

  http.begin(apiUrl);
}

void loop() {
    if(Serial.available() > 0)
    {
      String prompt = Serial.readStringUntil('\n');
      prompt.trim();
      Serial.print("USER: ");
      Serial.println(prompt);
      gettingResponse = true;
      chatGptCall(prompt);
    }
    delay(1);
} 

void chatGptCall(String payload)
{
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + String(apiKey));

  if(initialPrompt)
  {
    finalPayload = "{\"model\": \"gpt-3.5-turbo\",\"messages\": [{\"role\": \"user\", \"content\": \"" + payload + "\"}]}";
    initialPrompt = false;
  }
  else{
    finalPayload = finalPayload + ",{\"role\": \"user\", \"content\": \"" + payload + "\"}]}";
  }

  while(gettingResponse)
  {
    int httpResponseCode = http.POST(finalPayload);

    if (httpResponseCode == 200) {
      String response = http.getString();
  
      // Parse JSON response
      DynamicJsonDocument jsonDoc(1024);
      deserializeJson(jsonDoc, response);
      String outputText = jsonDoc["choices"][0]["message"]["content"];
      outputText.remove(outputText.indexOf('\n'));
      Serial.print("SADHHAVANA: ");
      Serial.println(outputText);
      String returnResponse = "{\"role\": \"assistant\", \"content\": \"" + outputText + "\"}";

      finalPayload = removeEndOfString(finalPayload);
      finalPayload = finalPayload + "," + returnResponse;
      gettingResponse = false;
    } 
    else {
      //Serial.printf("Error %i \n", httpResponseCode);
      // Serial.println("Trying again");
    }
    getDelay();
  }
}

String removeEndOfString(String originalString)
{
  int stringLength = originalString.length();
  String newString = originalString.substring(0, stringLength - 2);
  return(newString);
}

void getDelay(){
  unsigned long initialMillis = millis();
  while ((initialMillis + 5000) >= millis()) {    
  }
}
