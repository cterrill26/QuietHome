#include "WiFi.h"
#include <FirebaseESP32.h>
#include "driver/i2s.h"
#include "ESP32_MailClient.h"

#define FIREBASE_HOST "#########################################"
#define FIREBASE_AUTH "##############################"
#define SSID "##############"
#define PASSWORD "##############"
#define PDM_SAMPLE_RATE (48 * 1000) 
#define PDM_SAMPLE_BITS_RX (16) // PDM 16bit input
#define CHANNEL_FORMAT_PDM_RX (I2S_CHANNEL_FMT_ONLY_RIGHT)
#define PDM_NUM_RX (0)     // 0 for PDM
#define MIC_OFFSET (579)  
#define GREEN GPIO_NUM_17
#define RED GPIO_NUM_16
#define BLUE GPIO_NUM_4
#define BUZZER GPIO_NUM_26
#define BUTTON GPIO_NUM_15
#define PDM_CLK GPIO_NUM_19
#define PDM_DATA GPIO_NUM_34

#define SMTP_HOST "smtp.gmail.com"

/** The smtp port e.g. 
 * 25  or esp_mail_smtp_port_25
 * 465 or esp_mail_smtp_port_465
 * 587 or esp_mail_smtp_port_587
*/
#define SMTP_PORT 465

/* The sign in credentials */
#define AUTHOR_EMAIL "quiethometeam@gmail.com"
#define AUTHOR_PASSWORD "########################"

// The Email Sending data object contains config and data to send
SMTPData smtpData;

//serial number is unique to each device
const String SERIAL_NUM = "492491";

FirebaseData firebaseData;
String home_name;
String home_path;

void setup_WIFI();
void setup_firebase();
void turnoff_firebase();
void setup_mic();
void turnoff_mic();
long read_mic();
void beep(int msec_duration);
void led_on(int pin);
void led_off(int pin);
void send_message(vector<String> recipients);

void setup() {
  
  Serial.begin(115200);
 
  setup_WIFI();
  setup_firebase();

  // configure PWM functionalitites
  ledcSetup(0, 1000, 10);
  
  // attach the channel to the GPIO19 to be controlled
  ledcAttachPin(BUZZER, 0);

  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(BUTTON, INPUT);

  //LED is red when waiting to be added to a smart home
  led_on(RED);

  setup_mic();
    
  while(1){
    Firebase.getString(firebaseData, "/devices/" + SERIAL_NUM, home_name);
    
    if(home_name != ""){
      Serial.println("Connected to " + home_name);  
      beep(400);
      led_off(RED);
      led_on(GREEN);
      home_path = "/homes/" + home_name;
      break;  
    }
    delay(3000);
  } 
  
}



void loop() {
  int quiet = 0;  
  Serial.println("Reading home status");
  Firebase.getInt(firebaseData, home_path + "/quiet", quiet);
  Serial.println(quiet);

  if(quiet){
    led_on(BLUE);  
    led_off(GREEN);  
    
    int count = 0;
    int beeps = 0;
    bool emailed = false;
    while(1){
      Serial.println("Reading home status");
      Firebase.getInt(firebaseData, home_path + "/quiet", quiet);
      Serial.println(quiet);  
      if(quiet == 0){
        led_off(BLUE);
        led_off(RED);
        led_on(GREEN);
        break;
      } 
      delay(100);

      for(int i = 0; i < 30; i++){
          long mic = read_mic();
          if(mic > 6000)
            count++;
          //Serial.println(mic);
          delay(25);
      }

      count = max(count-5, 0); 
      
      if(count >= 30){
        beep(200);
        beeps++;
        count = count/2;  
        led_off(BLUE);
        led_on(RED);
      }
      else if(count == 0){
        beeps = max(beeps-1, 0);
        led_off(RED);
        led_on(BLUE);
      }

      if(beeps > 8 && !emailed){
        for(int i = 0; i < 4; i++){
            beep(100);
            delay(100);
        }
        
        vector<String> recipients;
        Firebase.get(firebaseData, home_path + "/email");
        FirebaseJson &json = firebaseData.jsonObject();
        size_t len = json.iteratorBegin();
        String key, value = "";
        int type = 0;
        for (size_t i = 0; i < len; i++) {
            json.iteratorGet(i, type, key, value);
            if (type == FirebaseJson::JSON_OBJECT)
            {
                String address = key + "@gmail.com";
                recipients.push_back(address);
                //Serial.println(address);
            }
        }
        json.iteratorEnd();
        
        send_message(recipients);
        emailed = true;
        beeps = 0;  
      }
    }
  }
  else{
    //not quiet mode, check for button press
    int pressed = 0;
    int start = millis();   
    while(millis() < start + 1000){
      if (!digitalRead(BUTTON)){
        pressed = 1;
        delay(5); //debounce
        //wait for button to be released
        while(!digitalRead(BUTTON))  
          continue;
        delay(5); //debounce
        break;
      }
    }

    if (pressed){
      //button pressed, enter quiet mode
      led_off(GREEN);
      led_on(BLUE);
      beep(100);
      Firebase.setInt(firebaseData, home_path + "/quiet", 1);
      
      //wait for 2nd button press to exit quiet mode
      
      while(digitalRead(BUTTON) == 1)  
        continue;
      
      delay(5); //debounce
        //wait for button to be released
      while(!digitalRead(BUTTON))  
          continue;
      delay(5); //debounce
      
      led_off(BLUE);
      led_on(GREEN);
      beep(100);
      Firebase.setInt(firebaseData, home_path + "/quiet", 0);  
    } 
    else{
      //nothing currently happening
      led_on(GREEN);
      led_off(RED);
      led_off(BLUE);
    } 
  }
  
}

void send_message(vector<String> recipients){
  Serial.println("Sending email");
  smtpData.setLogin(SMTP_HOST, SMTP_PORT, AUTHOR_EMAIL, AUTHOR_PASSWORD);
  smtpData.setSender("Quiet Home Team", AUTHOR_EMAIL);
  smtpData.setPriority("Normal");
  smtpData.setSubject("Message From Quiet Home");
  String mymessage = "<div style=\"color:#000000;\"><p>Dear User,</p><p>Someone in " + home_name + " has started a quiet session. Please try to keep the noise down.</p><p>Best,</p><p>Quiet Home Team</p></div>";
  smtpData.setMessage(mymessage, true);
  for(int i = 0; i < recipients.size(); i++)
    smtpData.addRecipient(recipients[i]);
    
  //Start sending Email, can be set callback function to track the status
  if (!MailClient.sendMail(smtpData))
    Serial.println("Error sending Email, " + MailClient.smtpErrorReason());

  //Clear all data from Email object to free memory
  smtpData.empty();
  Serial.println("Email sent");
}
  
void beep(int msec_duration){
  ledcWrite(0,512);
  delay(msec_duration);
  ledcWrite(0,0);
}

void led_on(int pin){
  digitalWrite(pin, 1);
}

void led_off(int pin){
  digitalWrite(pin, 0);
}

void setup_mic(){
  i2s_config_t pdm_config_rx;
  pdm_config_rx.mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM);
  pdm_config_rx.sample_rate = PDM_SAMPLE_RATE;
  pdm_config_rx.bits_per_sample = static_cast<i2s_bits_per_sample_t>(PDM_SAMPLE_BITS_RX); // 16bit
  pdm_config_rx.channel_format = CHANNEL_FORMAT_PDM_RX;
  pdm_config_rx.communication_format = I2S_COMM_FORMAT_I2S_MSB; //pcm data format
  pdm_config_rx.dma_buf_count = 4;                              // number of buffers, 128 max.
  pdm_config_rx.dma_buf_len = 1024;                             // size of each buffer
  pdm_config_rx.use_apll = 0;
  pdm_config_rx.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1; // Interrupt level 1
  
  i2s_driver_install(static_cast<i2s_port_t>PDM_NUM_RX, &pdm_config_rx, 0, NULL);

  i2s_pin_config_t pin_pdm_config_rx;
  pin_pdm_config_rx.ws_io_num = PDM_CLK;
  pin_pdm_config_rx.data_in_num = PDM_DATA;

  i2s_set_pin(static_cast<i2s_port_t>PDM_NUM_RX, &pin_pdm_config_rx);

  Serial.println("i2s setup complete");  
}

void turnoff_mic(){
    i2s_driver_uninstall(static_cast<i2s_port_t>PDM_NUM_RX);
}

  
long read_mic(){
  int i2s_read_len = 256;
  uint16_t *i2s_read_buff = (uint16_t *)calloc(i2s_read_len, sizeof(uint16_t));
  size_t bytes_read;
  i2s_read(static_cast<i2s_port_t>PDM_NUM_RX, (void *)i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY);

  long total = 0;
  for(int i = 0; i < bytes_read/2; i++)
    total += abs(i2s_read_buff[i]-MIC_OFFSET); 
  
  free(i2s_read_buff);
  return total;
}

void setup_WIFI(){
  WiFi.begin(SSID, PASSWORD);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network"); 
}
  
void setup_firebase(){
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(firebaseData, "tiny");

  Serial.println("Connected to Firebase database");
}

void turnoff_firebase(){
  Firebase.end(firebaseData);
  Serial.println("Stopped Firebase");
}
