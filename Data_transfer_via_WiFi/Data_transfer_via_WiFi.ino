String response;
String request = "";

#define COMMAND_START_TCP_CONNECTION "AT+CIPSTART=\"TCP\",\"10.0.1.5\",80"
#define COMMAND_SEND_MESSAGE_SIZE "AT+CIPSENDEX="
#define AMP "&"
#define CRLF "\r\n"
#define SCORE_1 "score_1="
#define SCORE_2 "score_2="
#define POST_PREFIX "POST /index.php HTTP/1.0\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: "
#define TIMEOUT 10000
unsigned long timeout;

String GET_REQUEST = "GET /?score_1=10&score_2=2 HTTP/1.0\r\n";
String POST_REQUEST = "POST /index.php HTTP/1.0\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 21\r\n\r\nscore_1=2&score_2=45";

byte size = POST_REQUEST.length() + 2;
byte getRequestSize = GET_REQUEST.length() + 2;
byte postRequestSize = POST_REQUEST.length() + 2;

bool isSent = false;

void setup()
{
  response = "";
  timeout = 0;
  Serial.begin(115200);
  Serial1.begin(115200); // your esp's baud rate might be different
  Serial.write("---\n");
}
 
void loop()
{
  if(Serial1.available()) // check if the esp is sending a message 
  {
    while(Serial1.available())
    {
      // The esp has data so display its output to the serial window 
      response = Serial1.readString(); // read the next character.
      Serial.println(response);
    }  
  }

//      if(response.substring(response.length()-9, response.length()-2) == "SEND OK") {
//        Serial.println("need to close connection");
//      }

      if(!isSent || (millis() - timeout) > TIMEOUT){
//      if(!isSent){
          createPostRequest();
        isSent = true;
        Serial.print("timeout before = ");
        Serial.println(timeout);
        timeout = millis();
        Serial.print("timeout after = ");
        Serial.println(timeout);
      }
      
      if(response.substring(response.length()-2, response.length()-1) == ">") {
          sendPostRequest();
          response = "";
      }

  
  if(Serial.available())
  {
    // the following delay is required because otherwise the arduino will read the first letter of the command but not the rest
    // In other words without the delay if you use AT+RST, for example, the Arduino will read the letter A send it, then read the rest and send it
    // but we want to send everything at the same time.
    delay(100);
    
    String command="";
    while(Serial.available()) // read the command character by character
    {
        // read one character
      command+=(char)Serial.read();
    }
    Serial.println(command);
    Serial.println(command.length());
    Serial1.println(command); // send the read character to the esp8266
  }
}

void sendGetRequestMeta() {
   Serial1.println(COMMAND_START_TCP_CONNECTION);
//        Serial.println(COMMAND_START_TCP_CONNECTION); //log

   delay(50);
   Serial1.print(COMMAND_SEND_MESSAGE_SIZE);
   Serial1.println(getRequestSize);
}

void sendGetRequest() {
  Serial1.println(GET_REQUEST);
}

void sendPostRequestMeta() {
  Serial1.println(COMMAND_START_TCP_CONNECTION);
//        Serial.println(COMMAND_START_TCP_CONNECTION); //log

   delay(500);
   Serial1.print(COMMAND_SEND_MESSAGE_SIZE);
   Serial1.println(postRequestSize);
}

void createPostRequest() {
  Serial1.println(COMMAND_START_TCP_CONNECTION);
//        Serial.println(COMMAND_START_TCP_CONNECTION); //log

   delay(500);
   Serial1.print(COMMAND_SEND_MESSAGE_SIZE);
   request = getPostRequestString();
   Serial1.println(request.length());
}

void sendPostRequest() {
  Serial1.println(request);
  Serial1.flush();
}

void testPostRequest() {
  Serial.println(getPostRequestString());
}

String getPostRequestString(){
  String body = SCORE_1;
  body += String(random(16));
  body += AMP;
  body += SCORE_2;
  body += String(random(16));

  String postRequest = POST_PREFIX;
  postRequest += String(body.length());
  postRequest += CRLF;
  postRequest += CRLF;
  postRequest += body;
  return postRequest;
}

