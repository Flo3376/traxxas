#ifndef SIMULATION_H
#define SIMULATION_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

class Simulation {
private:
    float roll;                  // X
    float pitch;                 // Y
    bool active;                 // Indicateur de simulation en cours
    unsigned long lastUpdate;    // Dernière mise à jour
    unsigned long startTime;     // Heure de début de simulation
    const unsigned long interval; // Intervalle entre deux envois (ms)
    const unsigned long duration; // Durée totale de la simulation (ms)
    float amplitude;             // Amplitude des sinusoïdes
    float frequency;             // Fréquence des sinusoïdes
    float phaseShift;            // Déphasage des sinusoïdes
    int count;                   // Compteur de messages envoyés
    AsyncWebSocket *ws;          // WebSocket pour envoyer les données

    void generateData();         // Génère les données à envoyer
    String formatData();         // Formate les données en JSON
    void sendData();             // Envoie les données via WebSocket

public:
    Simulation(AsyncWebSocket *webSocket); // Nouveau constructeur avec paramètre
    void setWebSocket(AsyncWebSocket *webSocket); // Définit le WebSocket
    void start();                // Démarre une simulation
    void stop();                 // Arrête une simulation
    void reset();                // Réinitialise les variables de simulation
    void update();               // Met à jour la simulation (appelé dans `loop`)
    bool isActive();             // Retourne l'état de la simulation
};

#endif // SIMULATION_H
