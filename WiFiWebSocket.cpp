#include "WiFiWebSocket.h"
#include "config.h"
#include <ArduinoJson.h>
#include <queue>

// File d'attente pour les messages reçus via WebSocket
std::queue<String> messageQueue;

// Constructeur
WiFiWebSocket::WiFiWebSocket()
    : ssid(WIFI_SSID), password(WIFI_PASSWORD), server(80), ws("/ws") {}

// Initialisation du Wi-Fi et du serveur WebSocket
void WiFiWebSocket::begin() {
    Serial.println("Connexion au Wi-Fi...");
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWi-Fi connecté !");
    Serial.print("Adresse IP (DHCP) : ");
    Serial.println(WiFi.localIP());

    configureStaticIP(); // Configurer l'IP fixe (si activé)
    initWebSocket();     // Initialiser le WebSocket
    server.begin();      // Démarrer le serveur Web
    Serial.println("Serveur WebSocket démarré !");
}

// Boucle de gestion principale
void WiFiWebSocket::handle() {
    checkWiFiConnection(); // Vérifie la connexion Wi-Fi
}

// Envoi de message via WebSocket
/*
void WiFiWebSocket::send(const String &message) {
    ws.textAll(message); // Envoie à tous les clients connectés
    Serial.print ("message : vers le socket"+message);
    }*/
    void WiFiWebSocket::send(const String &message) {
    Serial.println("Envoi d'un message via WebSocket...");
    Serial.print("Nombre de clients connectés : ");
    Serial.println(ws.count()); // Vérifie combien de clients sont connectés

    if (ws.count() > 0) {
        ws.textAll(message); // Envoie à tous les clients connectés
        Serial.println("Message envoyé : " + message);
    } else {
        Serial.println("Aucun client connecté. Message non envoyé.");
    }
}

// Configuration d'une IP statique basée sur la passerelle
void WiFiWebSocket::configureStaticIP() {
    IPAddress gateway = WiFi.gatewayIP();
    IPAddress staticIP(gateway[0], gateway[1], gateway[2], 100);
    IPAddress subnet(255, 255, 255, 0);

    WiFi.config(staticIP, gateway, subnet);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWi-Fi reconnecté avec IP statique !");
    Serial.print("Adresse IP (statique) : ");
    Serial.println(WiFi.localIP());
}

// Vérifie l'état du Wi-Fi et tente une reconnexion si nécessaire
void WiFiWebSocket::checkWiFiConnection() {
    if (WiFi.status() == WL_CONNECTED) {
        if (!wifiConnected) {
            wifiConnected = true;
            Serial.println("Wi-Fi connecté !");
        }
    } else {
        if (wifiConnected) {
            wifiConnected = false;
            Serial.println("Wi-Fi perdu, tentative de reconnexion...");
        }
        WiFi.begin(ssid, password);
    }
}

// Initialisation du WebSocket
void WiFiWebSocket::initWebSocket() {
    ws.onEvent([this](AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
        this->onWebSocketEvent(server, client, type, arg, data, len);
    });
    server.addHandler(&ws);
}

// Gestion des événements WebSocket
void WiFiWebSocket::onWebSocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
    if (type == WS_EVT_CONNECT) {
        Serial.println("Client WebSocket connecté !");
    } else if (type == WS_EVT_DISCONNECT) {
        Serial.println("Client WebSocket déconnecté !");
    } else if (type == WS_EVT_DATA) {
        String message;
        for (size_t i = 0; i < len; i++) {
            message += (char)data[i];
        }
        Serial.println("Message reçu via WebSocket ici: " + message);
        messageQueue.push(message); // Ajout du message à la file d'attente
    }
}
