#version 330 core

in vec3 fPosition;  // Posizione del frammento nel mondo
in vec3 fNormal;    // Normale interpolata dal vertex shader

uniform vec3 lightPos;    // Posizione della luce (Sole)
uniform vec3 viewPos;     // Posizione della camera
uniform vec3 lightColor;  // Colore della luce (il Sole)
uniform vec3 ambientColor; // Colore della luce ambientale
uniform vec3 objectColor; // Colore dell'oggetto
uniform float shininess;  // Lucentezza del materiale

out vec4 FragColor;

void main() {
    // --- Componente ambientale ---
    vec3 ambient = ambientColor * objectColor;

    // --- Componente diffusa ---
    vec3 norm = normalize(fNormal); // Normalizza la normale
    vec3 lightDir = normalize(lightPos - fPosition); // Direzione della luce
    float diff = max(dot(norm, lightDir), 0.0); // Calcola la luce diffusa
    vec3 diffuse = diff * lightColor;

    // --- Componente speculare ---
    vec3 viewDir = normalize(viewPos - fPosition); // Direzione dell'osservatore
    vec3 reflectDir = reflect(-lightDir, norm); // Calcola il vettore riflesso
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess); // Forza della luce speculare
    vec3 specular = spec * lightColor; // Applica il colore della luce alla riflessione speculare

    // Somma delle componenti di illuminazione
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result * objectColor, 1.0); // Colora il frammento
}


