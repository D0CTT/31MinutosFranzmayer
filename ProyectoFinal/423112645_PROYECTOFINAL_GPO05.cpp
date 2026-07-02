#include <iostream>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"


// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"
#include "modelAnim.h"
#include "Auto3D.h"
#include <vector>

// Function prototypes
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void RenderLoadingScreen(GLFWwindow* window, Shader& shader, GLuint textureID);
void DoMovement();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera(glm::vec3(-8.0f, 2.0f, 20.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;
float range = 0.0f;
float rot = 0.0f;
float movCamera = 0.0f;

// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
glm::vec3 PosIni(0.0f, 0.0f, 0.0f);
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f);

bool active;

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame


glm::vec3 LightEnable = glm::vec3(0);


// Positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(-14.00,0.68,8.45),
	glm::vec3(-3.87, 0.8, 3.00),
	glm::vec3(-18.31,0.18,2.63),
	glm::vec3(-4.47,1.35,7.68)
};
// Variable para prender o apagar la luz de ON AIR
glm::vec3 LightP1;





// Variables Animacion
	//Puerta cristal
	float rotp = 0;
	bool anim1 = false;
	bool modeanim1 = false;
	//Puerta madera
	float rotp2 = 0;
	bool anim2 = false;
	bool modeanim2 = false;
	//Camara
	float rotC = 0;
	bool anim3 = false;
	bool modeanim3 = false;
	//Dardo
	// Estados del dardo
	enum EstadoDardo { INACTIVO, EN_MANO, EN_VUELO };
	EstadoDardo estadoDardo = INACTIVO;

	// Vectores f�sicos
	glm::vec3 dardoPos(0.0f);
	glm::vec3 dardoVel(0.0f);
	float dardoGravedad = -9.8f; // Fuerza de gravedad
	// Control de la Animaci�n de Bodoque y el Dardo
	bool animBodoqueActive = false;
	float animBodoqueTime = 0.0f;
	float animBodoqueStartTime = 0.0f;

	// F�sica del dardo
	glm::vec3 dardoTarget(-4.36f, 1.14f, -0.30f); // 
	float tiempoVueloDardo = 0.8f; // Cuanto tarda en llegar al objetivo (segundos)
	float tiempoLanzamiento = 1.3f; // Momento exacto de la animacion en el que lo suelta (segundos)
	float tiempoActualVuelo = 0.0f;
	//Animacion tramoyas
	//Rotacion brazo derecho
	float rottr = 0.0f;
	// Rotacion brazo izquierdo
	float rottl = 0.0f;
	//Variables para detener o comenzar la animacion
	bool tramAnim = false;
	bool tramAniml = false;
	bool tramAnimr = false;


	//animacion de auto
	// Objeto auto
	AUTO3D* vehiculoAutonomo = nullptr;
	// Variable para detener y avanzar el auto
	bool moverAuto = false;

	//Pantallas fondo:
	struct DatosPantalla {
		glm::vec3 posicion;
		float rotacionY;
		int canalActual;
	};
	// Valores de localizacion de las pantallas, es decir, donde se van a encontrar en el espacio
	std::vector<DatosPantalla> todasLasPantallas = {
		{ glm::vec3(-4.93f, 1.27f, -2.52f), 20.0f, rand() % 16 },
		{ glm::vec3(-4.71f, 1.27f, -1.87f), 20.023f, rand() % 16 },
		{ glm::vec3(-4.49f, 1.27f, -1.22f), 20.023f, rand() % 16 },
		{ glm::vec3(-4.34f, 1.27f,  5.28f), 0.0f, rand() % 16 },
		{ glm::vec3(-4.34f, 1.27f,  6.02f), 0.0f, rand() % 16 },
		{ glm::vec3(-4.93f, 0.52f, -2.52f), 20.023f, rand() % 16 },
		{ glm::vec3(-4.70f, 0.52f, -1.87f), 20.023f, rand() % 16 },
		{ glm::vec3(-4.48f, 0.52f, -1.22f), 20.023f, rand() % 16 },
		{ glm::vec3(-4.34f, 0.52f,  5.28f), 0.0f, rand() % 16 },
		{ glm::vec3(-4.93f, -0.27f, -2.52f),20.023f, rand() % 16 },
		{ glm::vec3(-4.70f, -0.27f, -1.87f),20.023f, rand() % 16 },
		{ glm::vec3(-4.48f, -0.27f, -1.22f),20.023f, rand() % 16 },
		{ glm::vec3(-4.35f, -0.27f,  5.28f),0.0f, rand() % 16 },
		{ glm::vec3(-4.35f, -0.27f,  6.02f),0.0f, rand() % 16 }
	};
	// Posiciones de las imagenes en la textura, esto es lo que se le pasa al shader para que cambie a una imagen exactamente
	// evitando offsets dificiles de posicionar correctamente
	glm::vec2 posicionesAtlas[16] = {
		// Fila 0 (Imagenes 0 al 3)
		glm::vec2(0.00f, 0.00f), glm::vec2(0.24f, 0.00f), glm::vec2(0.475f, 0.00f), glm::vec2(0.7f, 0.00f),

		// Fila 1 (Imagenes 4 al 7)
		glm::vec2(0.00f, 0.24f), glm::vec2(0.24f, 0.22f), glm::vec2(0.47f, 0.22f), glm::vec2(0.69f, 0.235f),

		// Fila 2 (Imagenes 8 al 11)
		glm::vec2(0.00f, 0.465f), glm::vec2(0.24f, 0.47f), glm::vec2(0.47f, 0.47f), glm::vec2(0.7f, 0.47f),

		// Fila 3 (Imagenes 12 al 15)
		glm::vec2(0.00f, 0.7f), glm::vec2(0.25f, 0.69f), glm::vec2(0.47f, 0.695f), glm::vec2(0.69f, 0.69f)
	};


int main()
{
	// Init GLFW
	glfwInit();

	// Estado inicial del dardo
	estadoDardo = EN_MANO;
	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "31 MINUTOS", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	printf("%f", glfwGetTime());

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	// Inicializacion del shader de la pantalla de carga
	Shader loadingShader("Shaders/loading.vs", "Shaders/loading.frag");
	// Arreglo que contiene las imagnes de carga
	GLuint loadFrames[4];
	loadFrames[0] = TextureLoading::LoadTexture((GLchar*)"Textures/load1.png");//25%
	loadFrames[1] = TextureLoading::LoadTexture((GLchar*)"Textures/load2.png");//50%
	loadFrames[2] = TextureLoading::LoadTexture((GLchar*)"Textures/load3.png");//75%
	loadFrames[3] = TextureLoading::LoadTexture((GLchar*)"Textures/load4.png");//99%
	RenderLoadingScreen(window, loadingShader, loadFrames[0]);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Inicializacion de los shaders
	//Shader de iluminacion global que contiene tanto la interaccion de la luz con los objetos como 
	//el vertex shader que manipula segun que objeto esta manipulando (pantallas, esqueletal, comun)
	Shader lightingShader("Shaders/lighting.vs", "Shaders/lighting.frag");
	//Shader de luz para objetos emisores de luz (ya no se usa)
	Shader lampShader("Shaders/lamp.vs", "Shaders/lamp.frag");
	//Shader del skybox
	Shader SkyBoxshader("Shaders/SkyBox.vs", "Shaders/SkyBox.frag");
	// Shader de animacion esqueletal, al ser todo trasladado al lightning shader tampoco se usa
	Shader animShader("Shaders/anim.vs", "Shaders/anim.frag");
	//Declaracion de modelos
	Model tramoya((char*)"Models/tramoya/tramoya.obj");
	Model tl((char*)"Models/tramoya/TL.obj");
	Model tr((char*)"Models/tramoya/tr.obj");
	Model tramoya2((char*)"Models/tramoya2/tramoya.obj");
	Model tl2((char*)"Models/tramoya2/TL.obj");
	Model tr2((char*)"Models/tramoya2/tr.obj");
	RenderLoadingScreen(window, loadingShader, loadFrames[1]); //Proceso de carga 25%
	Model juanin((char*)"Models/juanin/juanin.obj");
	Model tulio((char*)"Models/tulio/tulio.obj");
	Model fachada((char*)"Models/Fachada/fachada.obj");
	Model GDUP((char*)"Models/GlassDoor/Up.obj");
	Model GDLEF((char*)"Models/GlassDoor/Left.obj");
	Model GDLEFP((char*)"Models/GlassDoor/lefpul.obj");
	Model GDRIG((char*)"Models/GlassDoor/Right.obj");
	Model GDRIGP((char*)"Models/GlassDoor/rightpul.obj");
	Model DOOR((char*)"Models/Door/door.obj");
	Model CUP((char*)"Models/Camara/Cup.obj");
	Model CDOWN((char*)"Models/Camara/Cdown.obj");
	Model dardo((char*)"Models/dart/dardo.obj");
	Model autoM((char*)"Models/auto/auto.obj");
	RenderLoadingScreen(window, loadingShader, loadFrames[2]); //Proceso de carga 50%
	Model imac1((char*)"Models/imacs/imac1.obj");
	Model imac2((char*)"Models/imacs/imac2.obj");
	Model pantalla((char*)"Models/pantalla/pantalla.obj");
	// --- CONFIGURACIÓN DEL AUTO ---
	glm::vec3 punto_inicial(-1.99f, 0.0f, 48.09f);
	vehiculoAutonomo = new AUTO3D(&autoM, punto_inicial, 10.0f);

	// ----------------------------------------------------------------



	// Build and compile our shader program
	//Modelo animacion
	ModelAnim BodoqueDardo("Animaciones/Bodoque/BodoqueDardo.dae");
	RenderLoadingScreen(window, loadingShader, loadFrames[3]);//Proceso de carga 75%
	BodoqueDardo.initShaders(lightingShader.Program);
	glDeleteTextures(4, loadFrames); //Limpiamos memoria del proceso de carga

	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] =
	{
		// Positions            // Normals              // Texture Coords
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,  	1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f
	};


	GLfloat skyboxVertices[] = {
		// Positions
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};


	GLuint indices[] =
	{  // Note that we start from 0!
		0,1,2,3,
		4,5,6,7,
		8,9,10,11,
		12,13,14,15,
		16,17,18,19,
		20,21,22,23,
		24,25,26,27,
		28,29,30,31,
		32,33,34,35
	};
	// First, set the container's VAO (and VBO)
	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);
	// Normals attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// Texture Coordinate attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	// Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	// We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Set the vertex attributes (only position data for the lamp))
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0); // Note that we skip over the other data in our buffer object (we don't need the normals/textures, only positions).
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);


	//SkyBox
	GLuint skyboxVBO, skyboxVAO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1,&skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices),&skyboxVertices,GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);

	// Load textures
	vector<const GLchar*> faces;
	faces.push_back("SkyBox/right.tga");
	faces.push_back("SkyBox/left.tga");
	faces.push_back("SkyBox/top.tga");
	faces.push_back("SkyBox/bottom.tga");
	faces.push_back("SkyBox/front.tga");
	faces.push_back("SkyBox/back.tga");

	GLuint cubemapTexture = TextureLoading::LoadCubemap(faces);

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 1000.0f);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{

		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		DoMovement();

		//Animacion macs
		// 1. Calcula qu� cuadro del atlas toca segun el tiempo actual
		float animSpeed = 4.0f; // Cambia de imagen 4 veces por segundo 
		int totalFrames = 16;
		int currentFrame2 = (int)(glfwGetTime() * animSpeed) % totalFrames;

		// 2. Convierte el frame (0 a 15) a columna y fila
		int col = currentFrame2% 4;
		// Restamos de 3 porque OpenGL lee el eje Y de la textura de abajo hacia arriba
		int row = 3 - (currentFrame2 / 4);

		// 3. Calcula el salto en UV (1.0 / 4 columnas = 0.25)
		glm::vec2 uvOffset = glm::vec2(col * 0.25f, row * 0.25f);

		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();
		glUniform1i(glGetUniformLocation(lightingShader.Program, "isAnimatedModel"), 0);
		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 16.0f);
		glUniform1i(glGetUniformLocation(lightingShader.Program, "material.specular"), 1);
		// == ==========================
		// Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
		// the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
		// by defining light types as classes and set their values in there, or by using a more efficient uniform approach
		// by using 'Uniform buffer objects', but that is something we discuss in the 'Advanced GLSL' tutorial.
		// == ==========================
		// Directional light
		// Que la luz imacte directamente en las ventanas para poder apreciar sus detalles en los brillos
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), 0.0f, 0.0f, -1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.45f, 0.45f, 0.45f); 
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.65f, 0.65f, 0.65f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 1.0f, 1.0f, 1.0f);

		glUniform1i(glGetUniformLocation(lightingShader.Program, "isAnimatedScreen"), 0); 
		// Point lights
		glm::vec3 lightColor[] = {
			glm::vec3(1.0f,1.0f, 1.0f),
			glm::vec3(1.0f,1.0f, 1.0f),
			glm::vec3(0.0f,0.0f, 0.0f),
			glm::vec3(1.0f,1.0f, 1.0f),
		};
		// Permite p prender y apagar la luz de On Air
		lightColor[3] = lightColor[3] * LightEnable;

		// Point light 1
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), lightColor[0].x, lightColor[0].y, lightColor[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), lightColor[0].x, lightColor[0].y, lightColor[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), lightColor[0].x, lightColor[0].y, lightColor[0].z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.35f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.44f);



		// Point light 2
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"),lightColor[1].x * 0.8f,lightColor[1].y * 0.8f,lightColor[1].z * 0.8f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"),lightColor[1].x * 0.8f,lightColor[1].y * 0.8f,lightColor[1].z * 0.8f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"),1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.35f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.44f);

		// Point light 3
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"), lightColor[2].x, lightColor[2].y, lightColor[2].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), lightColor[2].x, lightColor[2].y, lightColor[2].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), 1.0f, 1.0f,1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 0.045f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 0.075f);

		// Point light 4
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].ambient"), lightColor[3].x, lightColor[3].y, lightColor[3].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"), lightColor[3].x, lightColor[3].y, lightColor[3].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].linear"), 1.4f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].quadratic"), 10.0f);

		// SpotLight
		glm::vec3 pivotPos = glm::vec3(-14.0f, 0.47f, -2.52f); 
		glm::vec3 lightPos = glm::vec3(-21.55f, 4.34f, 4.76f);
		glm::vec3 direction = glm::normalize(pivotPos - lightPos);

		// SpotLight
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"),lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"),direction.x, direction.y, direction.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"),0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"),1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"),1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.04f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.01f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"),glm::cos(glm::radians(6.0f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"),glm::cos(glm::radians(8.0f)));

		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 16.0f);

		// Create camera transformations
		glm::mat4 view;
		view = camera.GetViewMatrix();


		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// Bind diffuse map
		//glBindTexture(GL_TEXTURE_2D, texture1);*/

		// Bind specular map
		/*glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);*/


		glBindVertexArray(VAO);
		glm::mat4 tmp = glm::mat4(1.0f); // Matriz temporal
		// Calculo del tiempo de la animacion desde cuando empieza
		if (animBodoqueActive) {
			animBodoqueTime = glfwGetTime() - animBodoqueStartTime;
		}


		//Carga de modelos
		//Personaje
		glUniform1i(glGetUniformLocation(lightingShader.Program, "activetrans"), 0);
		view = camera.GetViewMatrix();
		glm::mat4 model(1);
		model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(lightingShader.Program, "activetrans"), 0);
		fachada.Draw(lightingShader);
		model = glm::mat4(1);
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		juanin.Draw(lightingShader);
		model = glm::mat4(1);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 4.0f);
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		tulio.Draw(lightingShader);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 16.0f);
		model = glm::mat4(1);
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		tramoya.Draw(lightingShader);
		//Brazo derecho
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-10.73f, -0.13f, 4.01f));
		//Animacion sencilla movimiento de brazos tramoyas
		model = glm::rotate(model, glm::radians(-rottr), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(10.73f, 0.13f, -4.01f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		tr.Draw(lightingShader);
		//Brazo izquierdo
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-10.78f, -0.11f, 3.87f));
		//Animacion sencilla movimiento de brazos tramoyas
		model = glm::rotate(model, glm::radians(-rottl), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(10.78f, 0.11f, -3.87f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		tl.Draw(lightingShader);
		//Tramoya 2
		model = glm::mat4(1);
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		tramoya2.Draw(lightingShader);
		//Brazo derecho
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-10.73f, -0.13f, 1.66f));
		//Animacion sencilla movimiento de brazos tramoyas
		model = glm::rotate(model, glm::radians(-rottr), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(10.73f, 0.13f, -1.66f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		tr2.Draw(lightingShader);
		//Brazo izquierdo
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-10.69f, -0.11f, 1.52f));
		//Animacion sencilla movimiento de brazos tramoyas
		model = glm::rotate(model, glm::radians(-rottl), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(10.69f, 0.11f, -1.52f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		tl2.Draw(lightingShader);
		//Animacion puerta abriendose/cerrandose, pero este es solo la perilla
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-14.62f, -0.72f, 13.40f));
		model = glm::rotate(model, glm::radians(rotp), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(14.62f, 0.72f, -13.40f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		GDLEFP.Draw(lightingShader);
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-14.71f, -0.63f, 12.12f));
		model = glm::rotate(model, glm::radians(-rotp), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(14.71f, 0.63f, -12.12f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		GDRIGP.Draw(lightingShader);
		//Animacion puerta de madera abriendose/cerrandose
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-43.08f, -0.63f, -0.34f));
		model = glm::rotate(model, glm::radians(rotp2), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(43.08f, 0.63f, 0.34f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		DOOR.Draw(lightingShader);
		//Animacion sencilla de la camara girando
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-23.73f, 0.62f, 3.64f));
		model = glm::rotate(model, glm::radians(rotC), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(23.73f, -0.62f, -3.64f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		CUP.Draw(lightingShader);
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		CDOWN.Draw(lightingShader);
		//Dibujado y actualizado del automovil
		if (vehiculoAutonomo != nullptr) {
			if (moverAuto) {
				vehiculoAutonomo->update(deltaTime);
			}
			vehiculoAutonomo->draw(lightingShader);
		}

		//pantallas
		//macs
		glUniform1i(glGetUniformLocation(lightingShader.Program, "isAnimatedScreen"), 1);
		glUniform2f(glGetUniformLocation(lightingShader.Program, "uvOffset"), uvOffset.x, uvOffset.y);
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		imac1.Draw(lightingShader);
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		imac2.Draw(lightingShader);
		//fondo
		for (int i = 0; i < todasLasPantallas.size(); i++) {
			// --- MATRIZ DE MODELO ---
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, todasLasPantallas[i].posicion);
			model = glm::rotate(model, glm::radians(todasLasPantallas[i].rotacionY), glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

			// --- LOGICA ALEATORIA (Con parpadeo frecuente) ---
			if (rand() % 200 == 0) {
				todasLasPantallas[i].canalActual = rand() % 16;
			}

			// --- NUEVO SISTEMA DE OFFSET ---
			// Usamos el 'canalActual' (que va de 0 a 15) para sacar las coordenadas exactas del arreglo
			glm::vec2 offsetElegido = posicionesAtlas[todasLasPantallas[i].canalActual];

			// Enviamos ese offset directamente al shader
			glUniform2f(glGetUniformLocation(lightingShader.Program, "uvOffset"), offsetElegido.x, offsetElegido.y);

			// --- DIBUJAR ---
			pantalla.Draw(lightingShader);
		}

		// 3. REGRESAR A ESTADO NORMAL
		glUniform1i(glGetUniformLocation(lightingShader.Program, "isAnimatedScreen"), 0);

		// Matriz estatica de Bodoque 
		glm::mat4 modelBodoque = glm::mat4(1.0f);
		modelBodoque = glm::translate(modelBodoque, glm::vec3(-9.54f, -1.35f, -0.19f));
		modelBodoque = glm::rotate(modelBodoque, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		// matrices que serviran para mover el dardo localmente en la mano
		glm::mat4 dardoLocalOffset = glm::mat4(1.0f);
		dardoLocalOffset = glm::translate(dardoLocalOffset, glm::vec3(-0.1f, 0.07f, -0.12f)); 
		 dardoLocalOffset = glm::rotate(dardoLocalOffset, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		// Logica de lanzamiento
		if (animBodoqueActive && animBodoqueTime >= tiempoLanzamiento && estadoDardo == EN_MANO) {
			estadoDardo = EN_VUELO;
			tiempoActualVuelo = 0.0f;

			glm::mat4 handTransform = BodoqueDardo.GetBoneTransform("bodoque2_RightHand");
			glm::mat4 finalHandMat = modelBodoque * handTransform * dardoLocalOffset;
			dardoPos = glm::vec3(finalHandMat[3]);

			// Tiro parabolico
			dardoVel.x = (dardoTarget.x - dardoPos.x) / tiempoVueloDardo;
			dardoVel.z = (dardoTarget.z - dardoPos.z) / tiempoVueloDardo;
			dardoVel.y = (dardoTarget.y - dardoPos.y - 0.5f * dardoGravedad * tiempoVueloDardo * tiempoVueloDardo) / tiempoVueloDardo;
		}

		// Calculamos la matriz del dardo
		glm::mat4 modelDardo = glm::mat4(1.0f);
		if (estadoDardo == EN_MANO) {
			glm::mat4 handTransform = BodoqueDardo.GetBoneTransform("bodoque2_RightHand");
			modelDardo = modelBodoque * handTransform * dardoLocalOffset;
		}
		else if (estadoDardo == EN_VUELO) {
			tiempoActualVuelo += deltaTime;
			if (tiempoActualVuelo >= tiempoVueloDardo) {
				estadoDardo = INACTIVO;
				dardoPos = dardoTarget;
			}
			else {
				dardoPos.x += dardoVel.x * deltaTime;
				dardoPos.y += dardoVel.y * deltaTime + 0.5f * dardoGravedad * deltaTime * deltaTime;
				dardoPos.z += dardoVel.z * deltaTime;
				dardoVel.y += dardoGravedad * deltaTime;
			}
			modelDardo = glm::translate(glm::mat4(1.0f), dardoPos);
		}
		else if (estadoDardo == INACTIVO && animBodoqueActive) {
			modelDardo = glm::translate(glm::mat4(1.0f), dardoPos);
		}

		// Solo dibujamos el dardo si ya se presiono el 5
			glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelDardo));
			dardo.Draw(lightingShader);
		


		//Traslucidez

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUniform1i(glGetUniformLocation(lightingShader.Program, "activetrans"), 1);
		glUniform4f(glGetUniformLocation(lightingShader.Program, "coloralpha"), 0.0, 1.0, 1.0, 0.35);
		//Parte superior de cristal
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		GDUP.Draw(lightingShader);
		//Animacion puerta de cristal abriendose/cerrandose
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-14.62f, -0.72f, 13.40f));
		model = glm::rotate(model, glm::radians(rotp), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(14.62f, 0.72f, -13.40f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		GDLEF.Draw(lightingShader);
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-14.71f, -0.63f, 12.12f));
		model = glm::rotate(model, glm::radians(-rotp), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(14.71f, 0.63f, -12.12f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		GDRIG.Draw(lightingShader);
		glUniform4f(glGetUniformLocation(lightingShader.Program, "coloralpha"), 1.0, 1.0, 1.0, 0.35);
		glUniform1i(glGetUniformLocation(lightingShader.Program, "activetrans"), 0);
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glBindVertexArray(0);
		/*_______________________________Personaje Animado___________________________*/
		// Encendemos la logica de huesos en el vertex shader
		glUniform1i(glGetUniformLocation(lightingShader.Program, "isAnimatedModel"), 1);
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelBodoque));
		BodoqueDardo.Draw(lightingShader, animBodoqueTime);
		glUniform1i(glGetUniformLocation(lightingShader.Program, "isAnimatedModel"), 0);
		glBindVertexArray(0);
		// Draw skybox as last
		glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
		SkyBoxshader.Use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
		glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // Set depth function back to default




		// Swap the screen buffers
		glfwSwapBuffers(window);
	}




	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();




	return 0;
}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	


	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}

	if (keys[GLFW_KEY_SPACE] && GLFW_PRESS == action)
	{
		active = !active;
		if (active)
		{
			LightEnable = glm::vec3(1.0f);
		}
		else
		{
			LightEnable = glm::vec3(0.0f);//Cuado es solo un valor en los 3 vectores pueden dejar solo una componente
		}
	}

	if (keys[GLFW_KEY_Z] && GLFW_PRESS == action)
	{
		active = !active;
		if (active)
		{
			LightEnable = glm::vec3(1.0f);
		}
		else
		{
			LightEnable = glm::vec3(0.0f);//Cuado es solo un valor en los 3 vectores pueden dejar solo una componente
		}
	}

	if (keys[GLFW_KEY_2] && GLFW_PRESS == action)
	{
		if (anim1 == false) {
			anim1 = true;
		}
	}
	if (keys[GLFW_KEY_3] && GLFW_PRESS == action)
	{
		if (anim2 == false) {
			anim2 = true;
		}
	}
	if (keys[GLFW_KEY_4] && GLFW_PRESS == action)
	{
		if (anim3 == false) {
			anim3 = true;
		}
	}
	if (key == GLFW_KEY_5 && action == GLFW_PRESS)
	{
		// Al presionar 5, reiniciamos la animacion y el dardo vuelve a la mano
		animBodoqueActive = true;
		animBodoqueStartTime = glfwGetTime();
		animBodoqueTime = 0.0f;
		estadoDardo = EN_MANO;
	}
	if (key == GLFW_KEY_6 && action == GLFW_PRESS)
	{
		moverAuto = !moverAuto; // Alterna entre encendido y apagado
	}
	if (key == GLFW_KEY_7 && action == GLFW_PRESS)
	{
		tramAnim = !tramAnim; // Alterna entre encendido y apagado
	}
}

void MouseCallback(GLFWwindow *window, double xPos, double yPos)
{

	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

// Moves/alters the camera positions based on user input
void DoMovement()
{

	if (keys[GLFW_KEY_1])
	{
		
		movCamera = 0.01f;//Manda una velocidad de 0.01 a la camara automatica

	}

	//lightpoint 
	if (keys[GLFW_KEY_T])
	{
		pointLightPositions[0].x += 0.1f;
	}
	if (keys[GLFW_KEY_G])
	{
		pointLightPositions[0].x -= 0.1f;
	}

	if (keys[GLFW_KEY_Y])
	{
		pointLightPositions[0].y += 0.1f;
	}

	if (keys[GLFW_KEY_H])
	{
		pointLightPositions[0].y -= 0.1f;
	}
	if (keys[GLFW_KEY_U])
	{
		pointLightPositions[0].z -= 0.1f;
	}
	if (keys[GLFW_KEY_J])
	{
		pointLightPositions[0].z += 0.1f;
	}
	





	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);

	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);


	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);


	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
	float velocidad = 20;
	//animpuerta cristal (anim1)


	if (anim1) {
		if (!modeanim1) {
			rotp += velocidad * deltaTime;
			if (rotp >= 77) {
				anim1 = false;
				modeanim1 = true;
			}
		}
		else {
			rotp -= velocidad * deltaTime;
			if (rotp <= 0) {
				anim1 = false;
				modeanim1 = false;
			}
		}

	}
	//animpuerta cristal (anim1)


	if (anim2) {
		if (!modeanim2) {
			rotp2 += velocidad * deltaTime;
			if (rotp2 >= 90) {
				anim2 = false;
				modeanim2 = true;
			}
		}
		else {
			rotp2 -= velocidad * deltaTime;
			if (rotp2 <= 0) {
				anim2 = false;
				modeanim2 = false;
			}
		}

	}
	if (anim3) {
		if (!modeanim3) {
			rotC += velocidad * deltaTime;
			if (rotC >= 32.6) {
				anim3 = false;
				modeanim3 = true;
			}
		}
		else {
			rotC -= velocidad * deltaTime;
			if (rotC <= 0) {
				anim3 = false;
				modeanim3 = false;
			}
		}

	}
	velocidad *= 8;
	if (tramAnim) {
		if (tramAniml) {
			rottl -= velocidad * deltaTime;
			if (rottl <= -35) {
				
				tramAniml = false;
			}
		}
		else {
			rottl += velocidad * deltaTime;
			if (rottl >= 21.5) {
				tramAniml = true;
				
			}
		}
		if (tramAnimr) {
			rottr += velocidad * deltaTime;
			if (rottr >= 21.5) {
				tramAnimr = false;

			}

		}
		else {
			rottr -= velocidad * deltaTime;
			if (rottr <= -34) {

				tramAnimr = true;
			}
		}

	}

}
void RenderLoadingScreen(GLFWwindow* window, Shader& shader, GLuint textureID) {
	static GLuint loadVAO = 0, loadVBO;

	// Solo se genera la geometria en la primera llamada
	if (loadVAO == 0) {
		// Cuadrado que cubre toda la pantalla en Clip Space
		GLfloat vertices[] = {
			-1.0f,  1.0f, 0.0f,  0.0f, 0.0f,
			-1.0f, -1.0f, 0.0f,  0.0f, 1.0f,
			 1.0f,  1.0f, 0.0f,  1.0f, 0.0f,
			 1.0f, -1.0f, 0.0f,  1.0f, 1.0f,
		};
		glGenVertexArrays(1, &loadVAO);
		glGenBuffers(1, &loadVBO);
		glBindVertexArray(loadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, loadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Puntero de Posicion
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		// Puntero de Coordenadas UV
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}

	// Limpieza de buffers
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Activacion del shader y la textura
	shader.Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glUniform1i(glGetUniformLocation(shader.Program, "screenTexture"), 0);

	// Dibujado optimizado usando Triangle Strip
	glBindVertexArray(loadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	// Intercambio inmediato para forzar la actualizacion visual
	glfwSwapBuffers(window);
}