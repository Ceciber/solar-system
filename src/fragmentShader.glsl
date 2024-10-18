#version 330 core

in vec3 fPosition; // fragment's position
in vec3 fNormal;  // Normale interpolata del frammento

// for the Phong lighting
uniform vec3 camPos; // Posizione della camera (osservatore)

out vec4 FragColor;  // Colore finale

void main() {
    // Normale normalizzata
    vec3 n = normalize(fNormal);

    // Direzione della luce hardcoded (puoi cambiarla con una uniform in futuro)
    vec3 l = normalize(vec3(1.0, 1.0, 0.0)); // Light direction vector

    // TODO: vec3 v = calculate view vector
    // Vettore verso la camera (osservatore)
    vec3 v = normalize(camPos - fPosition);

    // TODO: vec3 r = calculate reflection vector
    // Vettore di riflessione della luce rispetto alla normale
    vec3 r = reflect(-l, n);

    // TODO: vec3 ambient = set an ambient color
    // Colore ambientale (valore costante)
    vec3 ambient = vec3(0.1, 0.1, 0.1);  // Puoi regolare il livello di luce ambientale

    // TODO: vec3 diffuse = calculate the diffuse lighting
    // Calcolo della luce diffusa usando il modello di Lambert
    float diffIntensity = max(dot(n, l), 0.0);
    vec3 diffuse = vec3(0.8, 0.8, 0.8) * diffIntensity; // Colore diffuso (modifica a piacere)

    // TODO: vec3 specular = calculate the specular lighting
    // Calcolo della luce speculare con il modello Phong
    float shininess = 32.0;  // Regola la brillantezza della superficie
    float specIntensity = pow(max(dot(v, r), 0.0), shininess);
    vec3 specular = vec3(1.0, 1.0, 1.0) * specIntensity; // Colore speculare (bianco)

    // Colore finale combinato
    vec3 finalColor = ambient + diffuse + specular;

    FragColor = vec4(finalColor, 1.0); // RGBA finale
}
