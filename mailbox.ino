#include <Arduino.h>


#include <Ultrasonic.h>
 

#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif 
#include <ESP_Mail_Client.h>

#define WIFI_SSID "HUAWEI Y6p"
#define WIFI_PASSWORD "walid1234"

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 587

/* The sign in credentials */
#define AUTHOR_EMAIL "intelligenteboite@gmail.com"
#define AUTHOR_PASSWORD "lnikgisyhtgkmvtc"

/* Recipient's email*/
#define RECIPIENT_EMAIL "ii_benamirouche@esi.dz"

/* The SMTP Session object used for Email sending */
SMTPSession smtp;

/* Callback function to get the Email sending status */


const int PIN = 14;
int v0;
int v1;
const int LED = 16;
const int BUTTON = 13;
int BUTTONstate = 0;


Ultrasonic ultrasonic(PIN);
bool valable = false; 
int cpt = 0 ;
void smtpCallback(SMTP_Status status);

//mail = 1 : lettre reçue
//mail = 2 : reset
void Send_mail(int mail){
   
  smtp.debug(1);

  /* Set the callback function to get the sending results */
  smtp.callback(smtpCallback);

  /* Declare the session config data */
  ESP_Mail_Session session;

  /* Set the session config */
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "";

  /* Declare the message class */
  SMTP_Message message;

  /* Set the message headers */
  message.sender.name = "Intelligente Box";
  message.sender.email = AUTHOR_EMAIL;
  
  message.addRecipient("Wassim", RECIPIENT_EMAIL);

  
  //Send raw text message
  String textMsg;
  if(mail==1){
    textMsg = "Vous avez reçu une lettre , Veuillez vérifier votre boite.\n";
    textMsg= textMsg + "Nombre total de lettres : ";
    textMsg = textMsg + cpt;
    message.subject = "Nouvelle lettre reçue";
    }else
   {
    textMsg = "La boite aux lettres est vidée.\n";
    textMsg= textMsg + "Nombre total de lettres : 0";
    message.subject = "Boite vide";
    }
   
  
  message.text.content = textMsg.c_str();
  message.text.charSet = "us-ascii";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
  message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;
  /* Set the custom message header */
  //message.addHeader("Message-ID: <abcde.fghij@gmail.com>");

  /* Connect to server with the session config */
  if (!smtp.connect(&session))
    return;

  /* Start sending Email and close the session */
  if (!MailClient.sendMail(&smtp, &message))
    Serial.println("Error sending Email, " + smtp.errorReason());
}



/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status){
  /* Print the current status */
  Serial.println(status.info());

  /* Print the sending result */
  if (status.success()){
    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message envoye avec succes: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Erreur d'envoi de message: %d\n", status.failedCount());
    Serial.println("----------------\n");
    struct tm dt;

    for (size_t i = 0; i < smtp.sendingResult.size(); i++){
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);
      time_t ts = (time_t)result.timestamp;
      localtime_r(&ts, &dt);

      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients);
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject);
    }
    Serial.println("----------------\n");
  }
}





void setup(){
  Serial.begin(9600);
  Serial.println();
  Serial.print("Connection au réseau wifi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }
  Serial.println("");
  Serial.println("WiFi Connecté.");
  Serial.println("Adresse IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT);

  //Send_mail();
}

void loop(){
//Serial.println(ultrasonic.MeasureInCentimeters()); // or MeasureInInches()
 v1 = ultrasonic.MeasureInCentimeters();
 Serial.print("Distance actuelle : ");
 Serial.print(v1);
 Serial.println("cm");
 if(v1 < 10){  

  if(!valable){
    valable = true;
    digitalWrite(LED, HIGH);
  }
   cpt++;
   Send_mail(1);
   delay(100);
 
 }

  BUTTONstate = digitalRead(BUTTON);
  if(BUTTONstate == HIGH){
    digitalWrite(LED, LOW);
    valable = false ;
    cpt = 0;
    Send_mail(2);
    
  }
  
  delay(100);
}
