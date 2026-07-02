#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include "Model.h"
#include "Shader.h"

class AUTO3D {
public:
    glm::vec3 pos;
    float velocidad;
    float angulo;
    Model* modelo_auto;

    float tiempoAcumulado = 0.0f;
    float radioCirculo = 46.54f;

    // Centro del circuito
    glm::vec3 centroCirculo = glm::vec3(-20.84f, 0.0f, 2.82f);

    AUTO3D(Model* modelo, glm::vec3 inicio, float vel) {
        modelo_auto = modelo;
        pos = inicio;
        velocidad = vel;

        // Calculo del angulo inicial
        float dx = inicio.x - centroCirculo.x;
        float dz = inicio.z - centroCirculo.z;
        tiempoAcumulado = std::atan2(dz, dx);
        // Sincronizamos la posición real con el ángulo calculado
            pos.x = centroCirculo.x + (radioCirculo * std::cos(tiempoAcumulado));
            pos.z = centroCirculo.z + (radioCirculo * std::sin(tiempoAcumulado));
            pos.y = inicio.y;
        // Angulo inicial del vehiculo
        angulo = -tiempoAcumulado;
    }

    void update(float deltaTime) {
        // 1. Velocidad angular
        float velocidadAngular = velocidad / radioCirculo;
        tiempoAcumulado += velocidadAngular * deltaTime;

        // 2. Calcular nueva posicion circular
        pos.x = centroCirculo.x + (radioCirculo * std::cos(tiempoAcumulado));
        pos.z = centroCirculo.z + (radioCirculo * std::sin(tiempoAcumulado));
        pos.y = centroCirculo.y;

        // 3. Orientacion directa
        // Como es un circulo perfecto, no necesitamos calcular tangentes con atan2.
        // La rotacion en Y es simplemente la inversa del avance angular.
        angulo = -tiempoAcumulado; 
    }

    void draw(Shader& shader) {
        if (!modelo_auto) return;

        glm::mat4 model = glm::mat4(1.0f);

        // 2. Mover a la coordenada de la pista
        model = glm::translate(model, pos);

        // 1. Rotar sobre su eje
        model = glm::rotate(model, angulo + glm::radians(90.0f), glm::vec3(0, 1, 0));

        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, &model[0][0]);
		//Dibujar
        modelo_auto->Draw(shader);
    }
};