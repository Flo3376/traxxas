/*======================*/
//    Sorties servo     //
/*======================*/
#include <ESP32Servo.h>
bool Debug_Input_Servo = false;                         //afficher toutes les données des servos

// Sortie servo pour controler des accessoires !!ne pas modifier!!
const int S_PIN_1 = 12;
const int S_PIN_2 = 13;
//const int S_PIN_3 = 12;
//variable de stockage  de la dernière position souhaitée
int S_POS_1 = 0;
int S_POS_2 = 0;
int S_POS_3 = 0;

Servo servo1;
Servo servo2;
//Servo servo3;

/*======================*/
//    Entrées servo     //
/*======================*/
// Entée servo pour écouter la télécommande !!ne pas modifier!!
const int channel_1 = 32;
const int channel_2 = 33;
const int channel_3 = 25;
const int channel_4 = 26;
const int channel_5 = 27;
const int channel_6 = 14;

//info sur la largeur d'impulsion utilisé par la télécommande pour utiliser un servo
const int minPulse = 1000;
const int maxPulse = 2000;

//variable de stockage  de la dernière position connue
int data_channel_1=0;
int data_channel_2=0;
int data_channel_3=0;
int data_channel_4=0;
int data_channel_5=0;
int data_channel_6=0;


/*======================*/
//    gyro              //
/*======================*/
#include <Wire.h>                 						// Librairie pour communication I2C
#include <Adafruit_MPU6050.h>     						// Librairie pour le GY-521
#include <Adafruit_Sensor.h>      						// Nécessaire pour Adafruit_MPU6050
bool debug_gyro = false;                     		    //afficher toutes les données du gyro

// Définit les broches I2C pour le gyroscope
#define I2C_SDA 21
#define I2C_SCL 22

// Réglage du gyro, 1 selon le prg / 2 au démarrage avec la télécommande
int gyro_set=1;

// Correction du capteur gyroscopique, si gyro_set=1 les valeurs seront prise en compte
float gyroOffsetX = 3.3, gyroOffsetY = -6.7, gyroOffsetZ = 0.0;
float accelOffsetX = 0, accelOffsetY = -0, accelOffsetZ = -9.47;


Adafruit_MPU6050 mpu;                         // Instance du capteur GY-521

// Variable des relevé du gyro
float roll = 0.0, pitch = 0.0;                // Angles calculés
float ax =-0.66, ay = -0.66, az = 9.41;

//gestion du temps pour la boucle void gyro !!ne pas modifier!!
unsigned long previousGyroMillis = 0;         // Dernière lecture du gyroscope
const unsigned long gyroInterval =  5000;      // Intervalle en millisecondes (2 secondes)

//gestion du temps pour la boucle void loop !!ne pas modifier!!
unsigned long previousMillis = 0;  // Temps de la dernière exécution
const unsigned long interval = 250; // Intervalle en millisecondes (4 Hz)


/*======================*/
//  Gestion des entrées //
/*======================*/
// Chaque entrée peu servir à surveiller quelque chose
// mettre -1 pour non attribué, sinon mettre channel_1 ou channel_2 ou channel_3 ou ......
const int steer = channel_6;
const int throttle = channel_5;
const int brake = channel_5;
const int gear_box = channel_2;
const int front_diff = -1;
const int rear_diff = -1;
const int light = -1;
const int winch = -1;
const int horn = -1;
const int light_mod= channel_3;

// Donnée des channels entrées !!ne pas modifier!!
int steer_data = -1;
int throttle_data = -1;
int gear_box_data = -1;
int front_diff_data = -1;
int rear_diff_data = -1;
int light_data = -1;
int winch_data = -1;
int horn_data = -1;
int brake_data= -1;
int light_mod_data = -1;

/*======================*/
//  Gestion des Sorties //
/*======================*/
// sortie leds et buzzers !!ne pas modifier!!
const int S_1_PWM = 19;
const int S_2_PWM = 18;
const int S_3_PWM = 5;
const int S_4_PWM = 17;
const int S_5_PWM = 16;
const int S_6_PWM = 4;
const int S_7_PWM = 2;
const int S_8_PWM = 15;

// Pour chaque fonction vous pouvez y attribuer une sortie
// Mettre -1 pour non attribué sinon mettre S_1_TOR ou S_2_PWM ou S_3_PWM ou S_4_PWM ou S_5_TOR ou S_6_TOR ou S_7_PWM ou S_8_PWM ou S_9_TOR derrière la fonction
const int L_Blink_o = S_2_PWM;             // Left Blinker (Clignotant gauche)
const int R_Blink_o = S_3_PWM;        // Right Blinker (Clignotant droit)
const int Gyro_o = -1;                     // Gyroscope Alert or Buzzer
const int Brakes = S_1_PWM;                // Brake Lights (Feux de freinage)
const int Third_Brakes = S_4_PWM;                // Brake Lights (Troisiéme feux de freinage)
const int PWM_Brake = -1;             // Brake Lights with PWM control (Feux de freinage PWM)
const int Side_Marker = -1;           // Side Marker Lights (Feux de position latéraux)
const int Reverse = -1;               // Reverse Lights (Feux de recul)
const int Tail_Lights = -1;           // Tail Lights (Feux arrière)
const int PWM_Tail_Lights = -1;       // Tail Lights with PWM control (Feux arrière PWM)
const int Headlights = S_6_PWM;            // Headlights (Phares avant)
const int PWM_Headlights = -1;        // Headlights with PWM control (Phares avant PWM)
const int Fog_Lights_Front = -1;      // Front Fog Lights (Antibrouillards avant)
const int Fog_Lights_Rear = -1;       // Rear Fog Lights (Antibrouillards arrière)
const int Winch_out = -1;              // Winch Out Control (Treuil sortie)
const int Winch_in = -1;               // Winch In Control (Treuil rentrée)
const int Angel_eye = S_8_PWM;

//si vous utilisez la même Led pour faire stop et position arrière (comme une ampoule bi-filament) mettre 1 sinon si c'est distinc mettre 0
const int brake_and_Tail = 1;

/*======================*/
//Clignotants et warning//
/*======================*/
unsigned long previousBlinkMillis = 0;   // Temps du dernier changement d'état
const unsigned long blinkInterval = 500; // Intervalle de clignotement en ms (0.5 s ON, 0.5 s OFF)
bool blinkState = LOW;                   // État actuel du clignotant
bool hazard = false; // Variable pour activer/désactiver les warnings
unsigned long hazardStartMillis = 0; // Temps du dernier déclenchement des warnings
const unsigned long hazardDuration = 5000; // Durée minimum des warnings en ms (5 secondes)

/*======================*/
//    mod éclairage     //
/*======================*/
int modeCounter = 1;           // Compteur de mode (1 à 3)
int previousState = -100;      // État précédent du bouton


/*======================*/
//       Wifi           //
/*======================*/
#include <WiFi.h>
const char* svgURL = "https://raw.githubusercontent.com/Flo3376/traxxas/refs/heads/main/artificial_horizon_b01.svg";

// Configuration du point d'accès Wi-Fi
const char* ssid = "Traxxas_Bob_One"; // Nom du réseau Wi-Fi
const char* password = "12345678";  // Mot de passe (min. 8 caractères)
#include <WebServer.h> // Bibliothèque pour un serveur web simple

WebServer server(80);  // Initialisation du serveur sur le port 80
#include "html_response.h"
#include "svg_content.h"  // Inclure le contenu SVG

/*======================*/
//    Son et buzzer     //
/*======================*/
// Sortie pour le buzzer !!ne pas modifier!!
const int S_buzzer = 23;               
// Notes et leurs périodes en microsecondes (inverses des fréquences)
#define NOTE_E4 1517  // Mi (4e octave)
#define NOTE_C4 1911  // Do (4e octave)
#define NOTE_G4 1276  // Sol (4e octave)
#define NOTE_G5 637   // Sol (5e octave)

// Durées des notes
#define EIGHTH_NOTE 200  // 1/8 de note = 200 ms
#define QUARTER_NOTE 400 // 1/4 de note = 400 ms


void loop() {
  server.handleClient(); // Gère les requêtes web
  // Vérifier si 5 secondes se sont écoulées
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
		previousMillis = currentMillis;

		readGyro();
    

    if(brake!=-1){
      brake_data=readPWM(brake);
    }
    if(brake!=-1){
      brake_data=readPWM(brake);
    }
    if(throttle!=-1){
      throttle_data=readPWM(throttle);
    }
    if(steer!=-1){
      steer_data=readPWM(steer);
    }
    if(Debug_Input_Servo){
      data_channel_1=readPWM(channel_1);
      data_channel_2=readPWM(channel_2);
      data_channel_3=readPWM(channel_3);
      data_channel_4=readPWM(channel_4);
      data_channel_5=readPWM(channel_5);
      data_channel_6=readPWM(channel_6);
      String output = String(data_channel_1) + " " +
        String(data_channel_2) + " " +
        String(data_channel_3) + " " +
        String(data_channel_4) + " " +
        String(data_channel_5) + " " +
        String(data_channel_6) + " " +
        String(roll * 6) + " " +
        String(pitch * 6);

		  Serial.println(output);
    }

    if(light_mod!=1){
      // Lecture de l'état actuel du bouton via channel_3
      int currentState = readPWM(light_mod);
      //Détection du front montant (passage de -100 à +100)
      if (previousState == -100 && currentState == 100) {
          // Incrémenter le compteur de mode
          modeCounter++;

          // Réinitialiser le compteur à 1 si on dépasse 4
          if (modeCounter > 3) {
              modeCounter = 1;
          }

          // Afficher le mode actuel pour le débogage
          Serial.print("Mode actuel : ");
          Serial.println(modeCounter);
      }

      // Mettre à jour l'état précédent pour la prochaine boucle
      previousState = currentState;
    }
    if(Angel_eye!=-1){
      if(modeCounter==1){
        analogWrite(Angel_eye, 0); // Appliquer la valeur PWM actuelle au pin
       }
      else{
        analogWrite(Angel_eye, 255);
        } // Appliquer la valeur PWM actuelle au pin}
        
    }
    if(Headlights!=-1){
      if(modeCounter==3){

        if(throttle!=-1){
          if(throttle_data>30){
            analogWrite(Headlights, 255); // Appliquer la valeur PWM actuelle au pin
          }
          else{
            analogWrite(Headlights, 128); // Appliquer la valeur PWM actuelle au pin
          }

        }
        else{
          analogWrite(Headlights, 128); // Appliquer la valeur PWM actuelle au pin
        }
        
      }
      else{
        analogWrite(Headlights, 0); // Appliquer la valeur PWM actuelle au pin
      } 
        
    }
	}
	int servoPosition = map(data_channel_6, -100, 100, 0, 180); // Mapper la plage de -100 à 100 en 0 à 180
	servoPosition = constrain(servoPosition, 0, 180); // Assurer que la valeur est dans la plage 0-180
	if (S_POS_1 != servoPosition) { // Si la position cible diffère de la position actuelle
  		if (servoPosition > S_POS_1) {
    		S_POS_1++; // Augmenter graduellement la position actuelle
    		servo1.write(S_POS_1); // Mettre à jour la position du servo
  		} else if (servoPosition < S_POS_1) {
    		S_POS_1--; // Diminuer graduellement la position actuelle
    		servo1.write(S_POS_1); // Mettre à jour la position du servo
  		}
  	}
  if (brake_data >-5)
  {
    if(modeCounter!=1 && brake_and_Tail==1){
      analogWrite(Brakes, 30); // Appliquer la valeur PWM actuelle au pin
    }
    else{
      analogWrite(Brakes, 0);
    }
    if(Third_Brakes!=-1)
    {
      analogWrite(Third_Brakes, 0);
    }
    
  }
  else{
    analogWrite(Brakes, 255); // Appliquer la valeur PWM actuelle au pin
    if(Third_Brakes!=-1)
    {
      analogWrite(Third_Brakes, 255);
    }
  }

  // Détection du dépassement de seuil de roll
    if (abs(roll) > 20) {
        hazard = true; // Activer les warnings
        hazardStartMillis = millis(); // Enregistrer le temps de début
    }

    // Désactiver les warnings après la durée minimale
    if (hazard && millis() - hazardStartMillis > hazardDuration) {
        hazard = false;
    }

  // Gestion des warnings (priorité)
    if (hazard) {
        warning(); // Active les warnings
    } else {
        digitalWrite(S_buzzer, LOW);
        // Sinon, gérer les clignotants gauche et droit
        if (steer_data > 10) {
            clignotantGauche(); // Active le clignotant gauche
            digitalWrite(R_Blink_o, LOW); // Assure que le droit est éteint
        } else if (steer_data < -10) {
            clignotantDroit(); // Active le clignotant droit
            digitalWrite(L_Blink_o, LOW); // Assure que le gauche est éteint
        } else {
            // Si aucune condition, les deux clignotants restent éteints
            digitalWrite(L_Blink_o, LOW);
            digitalWrite(R_Blink_o, LOW);
        }
    }
    testOutputPWM(S_7_PWM);
}

// Fonction pour tester une sortie PWM avec allumage et extinction progressifs
void testOutputPWM(int pin) {
    static int pwmValue = 0;        // Valeur actuelle de PWM
    static bool increasing = true; // Direction de la variation
    const int pwmStep = 5;         // Pas d'incrémentation/décrémentation
    const int maxPWM = 255;        // Valeur PWM maximale

    analogWrite(pin, pwmValue); // Appliquer la valeur PWM actuelle au pin

    // Modifier la valeur PWM en fonction de la direction
    if (increasing) {
        pwmValue += pwmStep;
        if (pwmValue >= maxPWM) { // Si la valeur maximale est atteinte, changer de direction
            pwmValue = maxPWM;
            increasing = false;
        }
    } else {
        pwmValue -= pwmStep;
        if (pwmValue <= 0) { // Si la valeur minimale est atteinte, changer de direction
            pwmValue = 0;
            increasing = true;
        }
    }

    delay(10); // Temporisation pour rendre la variation visible
}


int readPWM(int pin) {
  // Lecture de la largeur d'impulsion du signal servo
    unsigned long pulseWidth = pulseIn(pin, HIGH,20000);

    // Vérification si le signal est valide
    if (pulseWidth > 0) {
      // Conversion de la largeur d'impulsion en -100 à 100
      int scaledValue = map(pulseWidth, minPulse, maxPulse, -100, 100);

      // Limiter la valeur dans les bornes (en cas de bruits ou anomalies)
      scaledValue = constrain(scaledValue, -100, 100);
      if (Debug_Input_Servo) {
        // Affichage de la conversion
        //Serial.print("Largeur d'impulsion : ");
        //Serial.print(pulseWidth);
        //Serial.print(" µs -> Valeur convertie : ");
        //Serial.println(scaledValue);
      }

      return scaledValue;
    }
}

// Lecture des données du gyroscope
void readGyro() {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    ax =  a.acceleration.x;
    ay =  a.acceleration.y;
    az =  a.acceleration.z;

    roll = gyroOffsetX + atan2(a.acceleration.y, sqrt(a.acceleration.x * a.acceleration.x + a.acceleration.z * a.acceleration.z)) * 180 / PI;
    pitch = gyroOffsetY + atan2(-a.acceleration.x, sqrt(a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z)) * 180 / PI;
/*
    // Mise à jour des valeurs avec les offsets
    ax = accelOffsetX - a.acceleration.x;
    ay = accelOffsetY - a.acceleration.y;
    az = accelOffsetZ - a.acceleration.z;

    roll = gyroOffsetX - atan2(a.acceleration.y, sqrt(a.acceleration.x * a.acceleration.x + a.acceleration.z * a.acceleration.z)) * 180 / PI;
    pitch = gyroOffsetY - atan2(-a.acceleration.x, sqrt(a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z)) * 180 / PI;
*/

    if (debug_gyro) {
        Serial.println("=== Gyroscope Data ===");
        Serial.print("Accel X: "); Serial.println(ax);
        Serial.print("Accel Y: "); Serial.println(ay);
        Serial.print("Accel Z: "); Serial.println(az);
        Serial.print("Roll: "); Serial.println(roll);
        Serial.print("Pitch: "); Serial.println(pitch);
    }
}

void setupGyro() {
	// Initialisation du bus I2C sur les broches définies
	Wire.begin(I2C_SDA, I2C_SCL);
    if (!mpu.begin()) {
        Serial.println("Impossible de détecter le MPU6050");
        while (1); // Bloque l'exécution si le capteur n'est pas détecté
    }
    Serial.println("MPU6050 prêt!");
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_10_HZ);
}

void setupWifiServer() {
    // Route principale
    server.on("/", HTTP_GET, []() {
        String response = R"rawliteral(
        <!DOCTYPE html>
        <html>
        <head>
            <title>ESP32 Control Panel</title>
            <style>
                svg { width: 300px; height: 300px; }
                .on { fill: green; }
                .off { fill: red; }
            </style>
        </head>
        <body>
            <h1>ESP32 Control Panel</h1>
            <svg viewBox="0 0 100 100">
                <!-- Feu gauche -->
                <circle id="left_light" cx="30" cy="50" r="10" class="off"></circle>
                <!-- Feu droit -->
                <circle id="right_light" cx="70" cy="50" r="10" class="off"></circle>
            </svg>
            <script>
                // Fonction pour récupérer les données
                async function fetchData() {
                    try {
                        const response = await fetch('/get_data');
                        const data = await response.json();
                        
                        // Modifier les couleurs des SVG en fonction des données
                        document.getElementById('left_light').className.baseVal = data.left_light ? 'on' : 'off';
                        document.getElementById('right_light').className.baseVal = data.right_light ? 'on' : 'off';
                    } catch (error) {
                        console.error('Erreur de récupération des données:', error);
                    }
                }

                // Rafraîchir les données toutes les 500 ms
                setInterval(fetchData, 500);
            </script>
        </body>
        </html>
        )rawliteral";
        server.send(200, "text/html", response);
    });

    #include "html_response.h"

    server.on("/ha", HTTP_GET, []() {
        String response = String(HTML_PART_1) + String(SVG_CONTENT) + HTML_PART_2;
        server.send(200, "text/html", response);
    });

    // Route pour mettre le gyroscope à zéro
    server.on("/zero_gyro", HTTP_GET, []() {
    // Obtenez les mesures actuelles du gyroscope et de l'accéléromètre
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    // Mettre à jour les offsets en fonction des mesures actuelles
    gyroOffsetX = -atan2(a.acceleration.y, sqrt(a.acceleration.x * a.acceleration.x + a.acceleration.z * a.acceleration.z)) * 180 / PI;
    gyroOffsetY = -atan2(-a.acceleration.x, sqrt(a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z)) * 180 / PI;
    gyroOffsetZ = 0.0; // Si vous utilisez également l'axe Z

    accelOffsetX = a.acceleration.x;
    accelOffsetY = a.acceleration.y;
    accelOffsetZ = a.acceleration.z;

    // Envoyer une confirmation au client
    String jsonResponse = "{\"message\":\"Gyroscope reset successfully\",";
    jsonResponse += "\"gyroOffsetX\":" + String(gyroOffsetX) + ",";
    jsonResponse += "\"gyroOffsetY\":" + String(gyroOffsetY) + ",";
    jsonResponse += "\"accelOffsetX\":" + String(accelOffsetX) + ",";
    jsonResponse += "\"accelOffsetY\":" + String(accelOffsetY) + ",";
    jsonResponse += "\"accelOffsetZ\":" + String(accelOffsetZ) + "}";
    
    server.send(200, "application/json", jsonResponse);
    Serial.println("Gyroscope and accelerometer offsets have been updated.");
});




    // Route pour les données JSON
    server.on("/get_data", HTTP_GET, []() {
        String json = "{";
        json += "\"left_light\": " + String(digitalRead(L_Blink_o)) + ",";
        json += "\"right_light\": " + String(digitalRead(R_Blink_o))+ ",";
        json += "\"roll\": " + String(roll) + ",";
        json += "\"pitch\": " + String(pitch);
        json += "}";
        server.send(200, "application/json", json);
    });

    // Lancer le serveur
    server.begin();
    Serial.println("Serveur Wi-Fi prêt et en écoute.");
}

void setupWifi(){
	WiFi.softAP(ssid, password); // Création du hotspot
    Serial.println("Point d'accès Wi-Fi configuré !");
    Serial.print("SSID : ");
    Serial.println(ssid);
    Serial.print("Adresse IP : ");
    Serial.println(WiFi.softAPIP()); // Affiche l'adresse IP de l'ESP32 en mode AP
}

void playTone(int period, int duration) {
  int cycles = (duration * 1000L) / (period * 2); // Nombre de cycles à générer
  for (int i = 0; i < cycles; i++) {
    digitalWrite(S_buzzer, HIGH); // Activer le buzzer
    delayMicroseconds(period / 2); // Moitié de la période
    digitalWrite(S_buzzer, LOW); // Désactiver le buzzer
    delayMicroseconds(period / 2); // Moitié de la période
  }
}

void playMelody() {
  // Première mesure (C)
  playTone(NOTE_E4, EIGHTH_NOTE); // Mi (1/8)
  playTone(NOTE_E4, EIGHTH_NOTE); // Mi (1/8)
  delay(EIGHTH_NOTE);             // Silence (1/8)
  playTone(NOTE_E4, EIGHTH_NOTE); // Mi (1/8)
  delay(EIGHTH_NOTE);             // Silence (1/8)
  playTone(NOTE_C4, QUARTER_NOTE); // Do (1/4)
  playTone(NOTE_E4, QUARTER_NOTE); // Mi (1/4)

  // Deuxième mesure (G)
  playTone(NOTE_G5, EIGHTH_NOTE); // Sol (octave supérieure, 1/8)
  playTone(NOTE_G4, QUARTER_NOTE); // Sol (octave normale, 1/4)
  delay(QUARTER_NOTE);             // Silence (1/4)
  delay(EIGHTH_NOTE);              // Silence (1/8)
}

void setup() {
    // Initialisation du port série
    Serial.begin(115200);
    Serial.println("Démarrage...");

    // Initialisation des canaux d'entrée
    const int channels[] = {channel_1, channel_2, channel_3, channel_4, channel_5, channel_6};
    for (int i = 0; i < sizeof(channels) / sizeof(channels[0]); i++) {
        pinMode(channels[i], INPUT);
    }

    // Initialisation des canaux de sortie
    const int outputs[] = {S_1_PWM,S_2_PWM,S_3_PWM,S_4_PWM,S_5_PWM,S_6_PWM,S_7_PWM,S_8_PWM, S_buzzer};
    for (int i = 0; i < sizeof(outputs) / sizeof(outputs[0]); i++) {
        pinMode(outputs[i], OUTPUT);
    }

    // Configuration du point d'accès Wi-Fi
    setupWifi();
    // Configuration du serveur web
    setupWifiServer(); 
    // Configuration du gyro
    setupGyro();

    // Configuration des servos
    servo1.attach(S_PIN_1);
    servo2.attach(S_PIN_2);
    //servo3.attach(S_PIN_3);
    
    playMelody();
}

// Fonction pour gérer le clignotant gauche
void clignotantGauche() {
    unsigned long currentMillis = millis();

    // Si l'intervalle est écoulé, changer l'état
    if (currentMillis - previousBlinkMillis >= blinkInterval) {
        previousBlinkMillis = currentMillis; // Mettre à jour le dernier temps
        blinkState = !blinkState;            // Basculer l'état

        // Mettre à jour la broche du clignotant
        digitalWrite(L_Blink_o, blinkState);
    }
}

// Fonction pour gérer le clignotant gauche
void clignotantDroit() {
    unsigned long currentMillis = millis();

    // Si l'intervalle est écoulé, changer l'état
    if (currentMillis - previousBlinkMillis >= blinkInterval) {
        previousBlinkMillis = currentMillis; // Mettre à jour le dernier temps
        blinkState = !blinkState;            // Basculer l'état

        // Mettre à jour la broche du clignotant
        digitalWrite(R_Blink_o, blinkState);
    }
}

// Fonction pour gérer les warnings (hazard)
void warning() {
    unsigned long currentMillis = millis();

    // Si l'intervalle est écoulé, changer l'état
    if (currentMillis - previousBlinkMillis >= blinkInterval) {
        previousBlinkMillis = currentMillis; // Mettre à jour le dernier temps
        blinkState = !blinkState;            // Basculer l'état

        // Mettre à jour les broches des warnings (clignotant gauche et droit)
        digitalWrite(L_Blink_o, blinkState);
        digitalWrite(R_Blink_o, blinkState);
        digitalWrite(S_buzzer, blinkState);
    }
}
