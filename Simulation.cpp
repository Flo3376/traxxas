#include "Simulation.h"

// Constructeur
Simulation::Simulation(AsyncWebSocket *webSocket)
    : roll(0), pitch(0), active(false), lastUpdate(0), startTime(0),
      interval(250), duration(30000), amplitude(35.0), frequency(0.1),
      phaseShift(M_PI / 2), count(0), ws(webSocket) {}

// Génère les données à partir des sinusoïdes
void Simulation::generateData() {
    unsigned long elapsedTime = millis() - startTime;
    float timeInSeconds = elapsedTime / 1000.0;
    roll = amplitude * sin(2 * M_PI * frequency * timeInSeconds);
    pitch = amplitude * sin(2 * M_PI * frequency * timeInSeconds + phaseShift);
}

// Formate les données en JSON
String Simulation::formatData() {
    String json = "{";
    json += "\"type\":\"simulation\",";
    json += "\"count\":" + String(count) + ",";
    json += "\"roll\":" + String(roll, 1) + ",";
    json += "\"pitch\":" + String(pitch, 1);
    json += "}";
    return json;
}

// Envoie les données via WebSocket
void Simulation::sendData() {
    count++;
    String data = formatData();
    ws->textAll(data);
    Serial.println(data);
}

// Démarre une simulation
void Simulation::start() {
    reset();
    active = true;
    Serial.println("Simulation démarrée.");
}

// Arrête une simulation
void Simulation::stop() {
    active = false;
    Serial.println("Simulation arrêtée.");
}

// Réinitialise les variables de simulation
void Simulation::reset() {
    count = 0;
    roll = 0;
    pitch = 0;
    startTime = millis();
    lastUpdate = millis();
}

// Met à jour la simulation
void Simulation::update() {
    if (!active) return;

    if (millis() - startTime > duration) {
        stop();
        return;
    }

    if (millis() - lastUpdate >= interval) {
        lastUpdate = millis();
        generateData();
        sendData();
    }
}

// Retourne l'état de la simulation
bool Simulation::isActive() {
    return active;
}
