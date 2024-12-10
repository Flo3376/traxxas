#include "WiFiWebSocket.h"
#include "esp_log.h"
#include <ArduinoJson.h>
#include "Simulation.h" // Inclusion du simulateur

// Instance utilisant les valeurs de config.h
WiFiWebSocket wifiManager;

WiFiWebSocket wifiWebSocket;
Simulation simulation(wifiWebSocket.getWebSocket()); // Utilisation de la méthode `getWebSocket`

void setup() {
    Serial.begin(115200);
    // Réduire les logs Wi-Fi au minimum
    esp_log_level_set("wifi", ESP_LOG_NONE); // Options : ESP_LOG_NONE, ESP_LOG_ERROR, ESP_LOG_WARN, etc.

    wifiWebSocket.begin(); // Initialisation du Wi-Fi et des WebSockets

    // Initialisation du Wi-Fi et WebSocket
    wifiManager.begin();

    //simulation.start();    // Démarrage de la simulation
}

void loop() {
    // Gestion Wi-Fi et WebSocket
    wifiManager.handle();
    simulation.update(); // Mise à jour de la simulation

    // Exemple : Envoyer un message simulé
    static unsigned long lastMessageTime = 0;
    if (millis() - lastMessageTime >= 1000) { // Toutes les secondes
        lastMessageTime = millis();
        //wifiManager.sendMessage("{\"test\": \"Hello from ESP!\"}");
    }

    // Si un message est présent dans la file d'attente, on le traite
    if (!messageQueue.empty()) {
        handleMessage(); // Appel à une fonction pour gérer un seul message
    }
}

void handleMessage() {
    // Vérifie si la file d'attente contient un message
    if (messageQueue.empty()) {
        return;
    }

    // Récupérer le premier message de la file d'attente
    String message = messageQueue.front();

    // Affiche le message pour le débogage
    Serial.println("Traitement du message : " + message);

    // Parse le message en JSON
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, message);

    if (!error) {
        // Vérifie si le message contient un champ "type"
        if (doc.containsKey("type")) {
            String type = doc["type"];

            if (type == "ping") {
                Serial.println("Ping reçu !");
                wifiWebSocket.send("{\"type\":\"pong\"}");
                //wifiManager->textAll("{\"type\":\"pong\"}");
            } else if (type == "command") {
                // Gérer une commande spécifique
                String command = doc["command"];
                Serial.println("Commande reçue, enfin je crois : " + command);

                if (command == "start") {
                    Serial.println("Démarrage de la simulation !");
                    simulation.start(); // Appel à la méthode pour démarrer la simulation
                } else if (command == "stop") {
                    Serial.println("Arrêt de la simulation !");
                    simulation.stop(); // Appel à la méthode pour arrêter la simulation
                } else {
                    Serial.println("Commande inconnue : " + command);
                }
            }
        } else {
            Serial.println("Type de message non reconnu.");
        }
    } else {
        Serial.println("Erreur lors de la lecture du JSON.");
    }

    // Supprimer le message de la file d'attente après traitement
    messageQueue.pop();
}

