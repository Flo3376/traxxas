#ifndef WIFIWEBSOCKET_H
#define WIFIWEBSOCKET_H

#include <queue>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

// Classe responsable de la gestion du Wi-Fi et des WebSockets
class WiFiWebSocket {
public:

    AsyncWebSocket* getWebSocket() { return &ws; }

    WiFiWebSocket(); // Constructeur par défaut

    // Initialisation du Wi-Fi et du serveur WebSocket
    void begin();

    // Gestion périodique (doit être appelée dans la boucle principale)
    void handle();

    // Vérifie si le Wi-Fi est connecté
    bool isWiFiConnected();

    // Envoie un message via WebSocket à tous les clients connectés
    void send(const String &message);

private:
    const char* ssid; // SSID du Wi-Fi
    const char* password; // Mot de passe Wi-Fi
    AsyncWebServer server; // Serveur Web pour les WebSockets
    AsyncWebSocket ws; // Gestionnaire WebSocket
    bool wifiConnected = false; // État de la connexion Wi-Fi

    // Méthodes privées
    void checkWiFiConnection();
    void configureStaticIP();
    void initWebSocket();
    void onWebSocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len);
};

// Déclaration globale de la file d'attente des messages
extern std::queue<String> messageQueue;

#endif
