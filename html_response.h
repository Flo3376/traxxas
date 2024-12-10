#ifndef HTML_RESPONSE_H
#define HTML_RESPONSE_H



const char* HTML_PART_1 = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Horizon Artificiel</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            text-align: center;
            background-color: #222;
            color: white;
        }
        svg {
            width: 300px;
            height: 300px;
            margin: 20px auto;
        }
        .horizon {
            transform-origin: 50% 50%;
        }
        .sky { fill: #87CEEB; }
        .ground { fill: #8B4513; }
    </style>
</head>
<body>
    <div id="svg-container">
)rawliteral";

const char* HTML_PART_2 = R"rawliteral(
    </div>
    <div id="values-display">
        <p>Rotation : <span id="rotation-value">0</span>°</p>
        <p>Position Y : <span id="y-value">0</span>°</p>
        <button id="reset-gyro">Reset Gyroscope</button>
    </div>
</body>
<script>
          const svg = document.querySelector("svg");
            const billeLayer = svg.getElementById("layer3"); // Calque "bille"

            let rotation = 0; // Angle de rotation actuel
            let yDegrees = 0; // Position Y actuel
            let manualDrift = 0; // Dérive manuelle actuelle

            let targetRotation = 0; // Angle cible
            let targetYDegrees = 0; // Position Y cible
            let targetManualDrift = 0; // Dérive cible

            const billeCenterX = 140.5; // Coordonnée X du centre de rotation
            const billeCenterY = 140.5; // Coordonnée Y du centre de rotation
            const maxY = 125; // Déplacement maximum en pixels
            const maxAngle = 90; // Angle maximum en degrés
            const smoothingFactor = 0.1; // Plus bas = plus fluide mais plus lent

            const yRatio = maxY / maxAngle; // Ratio d'étalonnage

            // Fonction pour convertir les degrés en pixels
            function degreesToPixels(degrees) {
                return degrees * yRatio;
            }

            // Fonction pour calculer la dérive dynamique
            function calculateDrift(yDegrees, rotationDegrees) {
                const k = -2.85; // Facteur d'ajustement
                const radianRotation = (rotationDegrees * Math.PI) / 180;
                return k * yDegrees * Math.sin(radianRotation);
            }

            // Fonction d'interpolation
            function interpolate(current, target, factor) {
                return current + (target - current) * factor;
            }

            // Fonction pour mettre à jour les valeurs interpolées
            function smoothUpdate() {
                rotation = interpolate(rotation, targetRotation, smoothingFactor);
                yDegrees = interpolate(yDegrees, targetYDegrees, smoothingFactor);
                manualDrift = interpolate(manualDrift, targetManualDrift, smoothingFactor);

                updateBilleTransform();

                // Continue l'animation si les valeurs ne sont pas encore proches des cibles
                if (
                    Math.abs(rotation - targetRotation) > 0.01 ||
                    Math.abs(yDegrees - targetYDegrees) > 0.01 ||
                    Math.abs(manualDrift - targetManualDrift) > 0.01
                ) {
                    requestAnimationFrame(smoothUpdate);
                }
            }

            // Fonction pour appliquer les transformations
            function updateBilleTransform() {
                const radianRotation = (rotation * Math.PI) / 180;

                const height = degreesToPixels(yDegrees);
                const dynamicDrift = calculateDrift(yDegrees, rotation);

                const translateX = height * Math.sin(radianRotation) + dynamicDrift + manualDrift;
                const translateY = height * Math.cos(radianRotation);

                const transformString = `
                    translate(${translateX}, ${translateY})
                    rotate(${rotation}, ${billeCenterX}, ${billeCenterY})`
                ;
                billeLayer.setAttribute("transform", transformString);

                // Mettre à jour l'affichage des valeurs
                updateValuesDisplay();
            }

            

            // Fonction pour mettre à jour l'affichage des valeurs
            function updateValuesDisplay() {
                document.getElementById("rotation-value").textContent = rotation.toFixed(1);
                document.getElementById("y-value").textContent = yDegrees.toFixed(1);
                }

            // Commandes pour mettre à jour les cibles
            window.setX = function (newRotation) {
                targetRotation = newRotation;
                requestAnimationFrame(smoothUpdate);
            };

            window.setY = function (degrees) {
                targetYDegrees = degrees;
                requestAnimationFrame(smoothUpdate);
            };

            window.setD = function (driftAdjustment) {
                targetManualDrift = driftAdjustment;
                requestAnimationFrame(smoothUpdate);
            };

            document.getElementById("reset-gyro").addEventListener("click", async function () {
                try {
                    const response = await fetch('/zero_gyro');
                    const result = await response.json();
                    alert(result.message);
                } catch (error) {
                    console.error("Erreur lors de la réinitialisation du gyroscope :", error);
                }
            });

            

            async function fetchData() {
                try {
                    const response = await fetch('/get_data');
                    const data = await response.json();

                    // Mettre à jour les transformations SVG
                    setY(data.roll);
                    setX(data.pitch);
                } catch (error) {
                    console.error('Erreur lors de la récupération des données:', error);
                }
            }

            // Récupération périodique des données
            setInterval(fetchData, 250);

            console.log("Commandes disponibles :");
            console.log("setX(degrees) - Définit la rotation cible.");
            console.log("setY(degrees) - Définit la hauteur cible.");
            console.log("setD(pixels) - Définit la dérive cible.");
        
        </script>
</html>
)rawliteral";

#endif // HTML_RESPONSE_H
