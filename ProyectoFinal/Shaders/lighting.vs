#version 330 core

// Atributos del vértice
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

// Atributos para animación por huesos
layout (location = 5) in ivec4 bone_ids;
layout (location = 6) in vec4 weights;

// Variables de salida hacia el fragment shader
out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

// Matrices de transformación
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Control para pantallas/texturas animadas
uniform int isAnimatedScreen;
uniform vec2 uvOffset;

// Control de animación esquelética
uniform bool isAnimatedModel;

// Máximo número de huesos soportados
const int MAX_BONES = 100;

// Transformaciones de huesos
uniform mat4 bones[MAX_BONES];

void main()
{
    // Si el modelo tiene animación esquelética
    if(isAnimatedModel) 
    {
        // Combina las transformaciones de los 4 huesos
        // según su peso de influencia
        mat4 bone_transform = bones[bone_ids[0]] * weights[0];
        bone_transform += bones[bone_ids[1]] * weights[1];
        bone_transform += bones[bone_ids[2]] * weights[2];
        bone_transform += bones[bone_ids[3]] * weights[3];
        
        // Posición final del vértice animado
        vec4 boned_position = bone_transform * vec4(position, 1.0);

        // Posición final en pantalla
        gl_Position = projection * view * model * boned_position;

        // Posición del fragmento en espacio mundial
        FragPos = vec3(model * boned_position);

        // Transformación de normales usando huesos
        mat3 boneNormal =
            mat3(bones[bone_ids[0]]) * weights[0] +
            mat3(bones[bone_ids[1]]) * weights[1] +
            mat3(bones[bone_ids[2]]) * weights[2] +
            mat3(bones[bone_ids[3]]) * weights[3];

        // Normal final corregida
        Normal = normalize(
            mat3(transpose(inverse(model))) * boneNormal * normal
        );
    }
    else 
    {
        // Transformación para modelos comunes sin animación esquelética
        gl_Position = projection * view * model * vec4(position, 1.0f);

        // Posición del fragmento en espacio global
        FragPos = vec3(model * vec4(position, 1.0f)); 

        // Transformación de normales
        Normal = mat3(transpose(inverse(model))) * normal;
    }

    // Si la textura está animada, desplaza las UV
    if(isAnimatedScreen == 1) {
        TexCoords = texCoords + uvOffset;
    } 
    else {
        // UV normales
        TexCoords = texCoords;
    }
}