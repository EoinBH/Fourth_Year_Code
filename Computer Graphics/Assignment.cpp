#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>

#include <iostream>
#include <math.h>

// Assimp includes
#include <assimp/cimport.h> // scene importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations


using namespace glm;

const float Car_scale = 2.4f;
const float Scene_scale = 2000.0f;
const float Deer_scale = 0.02f;
const float Cube_scale = 0.002f;
const float Target_ratio = 100000.0f;
float Current_ratio = (Scene_scale / Deer_scale);
float adjustment = (Target_ratio / Current_ratio);

float Current_ratio_car = (Scene_scale / Car_scale);
float adjustment_car = (Target_ratio / Current_ratio_car);

float Current_ratio_cube = (Scene_scale / Cube_scale);
float adjustment_cube = (Target_ratio / Current_ratio_cube);



float translation_x_test;
float translation_y_test;
float translation_z_test;
const float deg_to_rad = 0.017444444;

typedef struct ModelData
{
  size_t mPointCount = 0;
  std::vector<vec3> mVertices;
  std::vector<vec3> mNormals;
  std::vector<vec2> mTextureCoords;
  std::vector<int> vert_indices;
} ModelData;

ModelData load_mesh(const char* file_name, float** verticesp);
const int num_deers = 20;
const int num_fences = 10;
const int num_legs = 4;
const int num_cars = 2;
ModelData mesh_data_scene;
ModelData mesh_data_fence;

class ModelMesh {
public:
    ModelData mesh_data;
    float* vertices;
    bool loaded;
    ModelMesh(const char* file_name);
};
ModelMesh::ModelMesh(const char* file_name) {
    vertices = NULL;
    loaded = false;
    if (!loaded) {
		mesh_data = load_mesh(file_name, &vertices);
        loaded = true;
	}
}

class Model {
public:
    ModelMesh* modelMesh;
	GLfloat rotate_x;
	GLfloat rotate_y;
	GLfloat rotate_z;
	GLfloat translation_x;
	GLfloat translation_y;
	GLfloat translation_z;
    GLfloat theta;
	glm::vec3 translation;
	Model(ModelMesh* modelMesh);
};
Model::Model(ModelMesh* m) {
	rotate_x = 0.0f;
	rotate_y = 0.0f;
	rotate_z = 0.0f;
	translation_x = 10.0f;
	translation_y = 25.0f;
	translation_z = 30.0f;
	translation = glm::vec3(translation_x, translation_y, translation_z);
    theta = 0.0f;
    modelMesh = m;
}

void initialise_deer_positions(std::vector<Model>& deer);
void initialise_car_positions(std::vector<Model>& car);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);




// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


// meshes

#define MESH_NAME_FL "Meshes/FL2.dae"
#define MESH_NAME_FR "Meshes/FR2.dae"
#define MESH_NAME_BL "Meshes/BL2.dae"
#define MESH_NAME_BR "Meshes/BR2.dae"
#define MESH_NAME "Meshes/Legless.dae"
//#define MESH_NAME "Meshes/Low_Poly_Car4.dae"
#define MESH_NAME_FENCE "Meshes/fence.dae"
#define MESH_NAME_SCENE "Meshes/road-field_simple.dae"
#define MESH_NAME_CAR "Meshes/Low_Poly_Car4.dae"


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    ModelMesh deerMesh(MESH_NAME);
    std::vector<Model> deer(num_deers, Model{&deerMesh});

    ModelMesh carMesh(MESH_NAME_CAR);
    std::vector<Model> car(num_cars, Model{&carMesh});

    ModelMesh FLMesh(MESH_NAME_FL);
    std::vector<Model> FL(num_deers, Model{&FLMesh});

    ModelMesh FRMesh(MESH_NAME_FR);
    std::vector<Model> FR(num_deers, Model{&FRMesh});

    ModelMesh BLMesh(MESH_NAME_BL);
    std::vector<Model> BL(num_deers, Model{&BLMesh});

    ModelMesh BRMesh(MESH_NAME_BR);
    std::vector<Model> BR(num_deers, Model{&BRMesh});

    float* vertices_scene;
    float* vertices_deer;
    //Scene
    ModelData modelDataScene = load_mesh("Meshes/road-field_simple.dae", &vertices_scene);

    // Load in Legless Deer

    //ModelData modelData = load_mesh("Meshes/Legless.dae", &vertices_deer);
    //initialise_deer_position();



    // float* vertices_deer = new float[modelData.mVertices.size()*(3+3+2)];

    // int j=0;

    // for (int i=0; i<modelData.mVertices.size(); i++) {

    //   vertices_deer[j] = modelData.mVertices[i][0]/200;
    //   vertices_deer[j+1] = modelData.mVertices[i][1]/200;
    //   vertices_deer[j+2] = modelData.mVertices[i][2]/200;
    //   vertices_deer[j+3] = modelData.mNormals[i][0];
    //   vertices_deer[j+4] = modelData.mNormals[i][1];
    //   vertices_deer[j+5] = modelData.mNormals[i][2];
    //   vertices_deer[j+6] = modelData.mTextureCoords[i][0];
    //   vertices_deer[j+7] = modelData.mTextureCoords[i][1];
    //   j += 8;
    // }

    // float* vertices_scene = new float[modelDataScene.mVertices.size()*(3+3+2)];

    // j=0;
    // std::cout << "Size: " << modelDataScene.mVertices.size() << std::endl;
    // for (int i=0; i<modelDataScene.mVertices.size(); i++) {

    //   vertices_scene[j] = modelDataScene.mVertices[i][0]/200;
    //   vertices_scene[j+1] = modelDataScene.mVertices[i][1]/200;
    //   vertices_scene[j+2] = modelDataScene.mVertices[i][2]/200;
    //   vertices_scene[j+3] = modelDataScene.mNormals[i][0];
    //   vertices_scene[j+4] = modelDataScene.mNormals[i][1];
    //   vertices_scene[j+5] = modelDataScene.mNormals[i][2];
    //   vertices_scene[j+6] = modelDataScene.mTextureCoords[i][0];
    //   vertices_scene[j+7] = modelDataScene.mTextureCoords[i][1];
    //   j += 8;
    // }
    /*      
    j=0;

    for (int i=0; i<modelData.mVertices.size(); i++) {

      std::cout << vertices_deer[j] << ", ";
      std::cout << vertices_deer[j+1] << ", ";
      std::cout << vertices_deer[j+2] << ", ";
      std::cout << vertices_deer[j+3] << ", ";
      std::cout << vertices_deer[j+4] << ", ";
      std::cout << vertices_deer[j+5] << ", ";
      std::cout << vertices_deer[j+6] << ", ";
      std::cout << vertices_deer[j+7] << ", ";
      std::cout << std::endl;
      j += 8;
      }*/


    

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader lightingShader("Vertex_Shader.vs", "Fragment_Shader.fs");
    Shader lightCubeShader("Vertex_Shader2.vs", "Fragment_Shader2.vs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // normals           // texture coords

      	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };




    // positions all containers
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    // positions of the point lights
    //std::cout << "adjustment_cube: " << adjustment_cube << std::endl;
    glm::vec3 pointLightPositions[] = {
        glm::vec3((-20.8f / adjustment_cube), (25.5f / adjustment_cube), 0.0f),
        glm::vec3((-21.7f / adjustment_cube), (25.5f / adjustment_cube), 0.0f),
        glm::vec3((22.8f / adjustment_cube), (25.5f / adjustment_cube), 0.0f),
        glm::vec3((23.7f / adjustment_cube), (25.5f / adjustment_cube), 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f)
    };
    glm::vec3 cube_perp[] = {
        glm::vec3((fmodf((pointLightPositions[0][0] + 90.0f), 360.0f)), pointLightPositions[0][1], (fmodf((pointLightPositions[0][2] + 90.0f), 360.0f))),
        glm::vec3((fmodf((pointLightPositions[1][0] + 90.0f), 360.0f)), pointLightPositions[1][1], (fmodf((pointLightPositions[1][2] + 90.0f), 360.0f))),
        glm::vec3((fmodf((pointLightPositions[2][0] - 90.0f), 360.0f)), pointLightPositions[2][1], (fmodf((pointLightPositions[2][2] - 90.0f), 360.0f))),
        glm::vec3((fmodf((pointLightPositions[3][0] - 90.0f), 360.0f)), pointLightPositions[3][1], (fmodf((pointLightPositions[3][2] - 90.0f), 360.0f)))
    };
    // first, configure the model's VAO (and VBO)
    unsigned int modelVBO, modelVAO;

    glGenBuffers(1, &modelVBO);
    glBindBuffer(GL_ARRAY_BUFFER, modelVBO);
    glBufferData(GL_ARRAY_BUFFER, deerMesh.mesh_data.mVertices.size()*(8)*sizeof(float), deerMesh.vertices, GL_STATIC_DRAW);

    // std::cout << "deerMesh.mesh_data.mVertices.size()*(8)*sizeof(float): " << deerMesh.mesh_data.mVertices.size()*(8)*sizeof(float) << std::endl;
    // std::cout << "deerMesh.vertices[0]: " << deerMesh.vertices[0] << std::endl;

    glGenVertexArrays(1, &modelVAO);
    glBindVertexArray(modelVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVBO, lightCubeVAO;

    glGenBuffers(1, &lightCubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, lightCubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);
    /*
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    */
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int sceneVBO, sceneVAO;

    glGenBuffers(1, &sceneVBO);
    glBindBuffer(GL_ARRAY_BUFFER, sceneVBO);
    glBufferData(GL_ARRAY_BUFFER, modelDataScene.mVertices.size()*(8)*sizeof(float), vertices_scene, GL_STATIC_DRAW);

    glGenVertexArrays(1, &sceneVAO);
    glBindVertexArray(sceneVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int carVBO, carVAO;

    glGenBuffers(1, &carVBO);
    glBindBuffer(GL_ARRAY_BUFFER, carVBO);
    glBufferData(GL_ARRAY_BUFFER, carMesh.mesh_data.mVertices.size()*(8)*sizeof(float), carMesh.vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &carVAO);
    glBindVertexArray(carVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int FLVBO, FLVAO;

    glGenBuffers(1, &FLVBO);
    glBindBuffer(GL_ARRAY_BUFFER, FLVBO);
    glBufferData(GL_ARRAY_BUFFER, FLMesh.mesh_data.mVertices.size()*(8)*sizeof(float), FLMesh.vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &FLVAO);
    glBindVertexArray(FLVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int FRVBO, FRVAO;

    glGenBuffers(1, &FRVBO);
    glBindBuffer(GL_ARRAY_BUFFER, FRVBO);
    glBufferData(GL_ARRAY_BUFFER, FRMesh.mesh_data.mVertices.size()*(8)*sizeof(float), FRMesh.vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &FRVAO);
    glBindVertexArray(FRVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int BLVBO, BLVAO;

    glGenBuffers(1, &BLVBO);
    glBindBuffer(GL_ARRAY_BUFFER, BLVBO);
    glBufferData(GL_ARRAY_BUFFER, BLMesh.mesh_data.mVertices.size()*(8)*sizeof(float), BLMesh.vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &BLVAO);
    glBindVertexArray(BLVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int BRVBO, BRVAO;

    glGenBuffers(1, &BRVBO);
    glBindBuffer(GL_ARRAY_BUFFER, BRVBO);
    glBufferData(GL_ARRAY_BUFFER, BRMesh.mesh_data.mVertices.size()*(8)*sizeof(float), BRMesh.vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &BRVAO);
    glBindVertexArray(BRVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // load textures (we now use a utility function to keep the code more organized)
    // -----------------------------------------------------------------------------
    //unsigned int diffuseMap = loadTexture(FileSystem::getPath("resources/textures/container2.png").c_str());
    unsigned int diffuseMap = loadTexture(FileSystem::getPath("resources/textures/fur3.png").c_str());
    unsigned int specularMap = loadTexture(FileSystem::getPath("resources/textures/container2_specular.png").c_str());
    // unsigned int diffuseMap2 = loadTexture(FileSystem::getPath("resources/textures/marble.jpg").c_str());
    unsigned int diffuseMap2 = loadTexture(FileSystem::getPath("resources/textures/test8.png").c_str());
    //unsigned int diffuseMap3 = loadTexture(FileSystem::getPath("resources/textures/bricks2.jpg").c_str());
    unsigned int diffuseMap3 = loadTexture(FileSystem::getPath("resources/textures/Paint1.png").c_str());
    unsigned int diffuseMap4 = loadTexture(FileSystem::getPath("resources/textures/Paint2.png").c_str());
    unsigned int diffuseMap5 = loadTexture(FileSystem::getPath("resources/textures/fur2.png").c_str());

    // shader configuration
    // --------------------
    lightingShader.use();
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);

    float cube_theta0 = 183.5f;
    float cube_theta1 = 183.5f;
    float cube_theta2 = -3.5f;
    float cube_theta3 = -3.5f;
    float cube_rotate_y[4];
    for (int i = 0; i < 4; i++) {
        cube_rotate_y[i] = 0.0f;
    }

    initialise_deer_positions(deer);
    initialise_car_positions(car);

    bool initial = true;
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setFloat("material.shininess", 32.0f);

        /*
           Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index 
           the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
           by defining light types as classes and set their values in there, or by using a more efficient uniform approach
           by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
        */
        // directional light
        lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        // point light 1
        lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[0].constant", 1.0f);
        lightingShader.setFloat("pointLights[0].linear", 0.09f);
        lightingShader.setFloat("pointLights[0].quadratic", 0.032f);
        // point light 2
        lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[1].constant", 1.0f);
        lightingShader.setFloat("pointLights[1].linear", 0.09f);
        lightingShader.setFloat("pointLights[1].quadratic", 0.032f);
        // point light 3
        lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[2].constant", 1.0f);
        lightingShader.setFloat("pointLights[2].linear", 0.09f);
        lightingShader.setFloat("pointLights[2].quadratic", 0.032f);
        // point light 4
        lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[3].constant", 1.0f);
        lightingShader.setFloat("pointLights[3].linear", 0.09f);
        lightingShader.setFloat("pointLights[3].quadratic", 0.032f);

        // lightingShader.setVec3("spotLights[0].position", pointLightPositions[3]);
        // lightingShader.setVec3("spotLights[0].direction", cube_perp[3]);
        // lightingShader.setVec3("spotLights[0].ambient", 0.0f, 0.0f, 0.0f);
        // lightingShader.setVec3("spotLights[0].diffuse", 1.0f, 1.0f, 1.0f);
        // lightingShader.setVec3("spotLights[0].specular", 1.0f, 1.0f, 1.0f);
        // lightingShader.setFloat("spotLights[0].constant", 1.0f);
        // lightingShader.setFloat("spotLights[0].linear", 0.09f);
        // lightingShader.setFloat("spotLights[0].quadratic", 0.032f);
        // lightingShader.setFloat("spotLights[0].cutOff", glm::cos(glm::radians(12.5f)));
        // lightingShader.setFloat("spotLights[0].outerCutOff", glm::cos(glm::radians(15.0f)));
        // spotLight
    lightingShader.setVec3("spotLight.position", camera.Position);
        lightingShader.setVec3("spotLight.direction", camera.Front);
        lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("spotLight.constant", 1.0f);
        lightingShader.setFloat("spotLight.linear", 0.09f);
        lightingShader.setFloat("spotLight.quadratic", 0.032f);
        lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));     


        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);

        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        // render containers
        glBindVertexArray(modelVAO);
        
        //Update Positions:
        if (!initial) {
            float min_x = (3.0f / adjustment);
            float max_x = (22.0f / adjustment);
            float min_z = (28.0f / adjustment);
            float max_z = (47.0f / adjustment);
            //Translate all deers in their facing directions
            for (int i = 0; i < num_deers; i++) {
                deer[i].translation_x += ((0.1f * (sin((deer[i].rotate_y * deg_to_rad)))) / adjustment);
                deer[i].translation_x = fmin(deer[i].translation_x, max_x);
                deer[i].translation_x = fmax(deer[i].translation_x, min_x);
                deer[i].translation_z += ((0.1f * (cos((deer[i].rotate_y * deg_to_rad)))) / adjustment);
                deer[i].translation_z = fmin(deer[i].translation_z, max_z);
                deer[i].translation_z = fmax(deer[i].translation_z, min_z);

                int r = rand() % 10;
                int r2 = rand() % 20;
                int s = 1;
                if (r2 == 3) {
                    s = -s;
                }
                if (r == 3) {
                    deer[i].rotate_y += s * 5.0f;
                }
                deer[i].rotate_y = fmodf(deer[i].rotate_y, 360.0f);
            }

            car[0].theta = fmodf((car[0].theta + 0.5f), 360.0f);
            car[1].theta = fmodf((car[1].theta - 0.7f), 360.0f);
            float radius1 = 21.2;
            float radius2 = 23.2;
            car[0].translation_x = (radius1 * (cos(car[0].theta * deg_to_rad)) / adjustment_car);
            //std::cout << "(radius1 * (cos(car[0].theta) * deg_to_rad)): " << cos(car[0].theta) << std::endl;
            car[0].translation_z = (radius1 * (sin(car[0].theta * deg_to_rad)) / adjustment_car);
            //std::cout << "(radius1 * (sin(car[0].theta) * deg_to_rad)): " << sin(car[0].theta) << std::endl;
            car[1].translation_x = (radius2 * (cos(car[1].theta * deg_to_rad)) / adjustment_car);
            car[1].translation_z = (radius2 * (sin(car[1].theta * deg_to_rad)) / adjustment_car);
            car[0].rotate_y = fmodf((car[0].rotate_y - 0.5f), 360.0f);
            car[1].rotate_y = fmodf((car[1].rotate_y + 0.7f), 360.0f);

            cube_theta0 = fmodf((cube_theta0 + 0.5f), 360.0f);
            cube_theta1 = fmodf((cube_theta1 + 0.5f), 360.0f);
            cube_theta2 = fmodf((cube_theta2 - 0.7f), 360.0f);
            cube_theta3 = fmodf((cube_theta3 - 0.7f), 360.0f);
            float r0 = 20.8;
            float r1 = 21.7;
            float r2 = 22.8;
            float r3 = 23.7;
            pointLightPositions[0][0] = (r0 * (cos(cube_theta0 * deg_to_rad)) / adjustment_cube);
            pointLightPositions[0][2] = (r0 * (sin(cube_theta0 * deg_to_rad)) / adjustment_cube);
            pointLightPositions[1][0] = (r1 * (cos(cube_theta1 * deg_to_rad)) / adjustment_cube);
            pointLightPositions[1][2] = (r1 * (sin(cube_theta1 * deg_to_rad)) / adjustment_cube);
            pointLightPositions[2][0] = (r2 * (cos(cube_theta2 * deg_to_rad)) / adjustment_cube);
            pointLightPositions[2][2] = (r2 * (sin(cube_theta2 * deg_to_rad)) / adjustment_cube);
            pointLightPositions[3][0] = (r3 * (cos(cube_theta3 * deg_to_rad)) / adjustment_cube);
            pointLightPositions[3][2] = (r3 * (sin(cube_theta3 * deg_to_rad)) / adjustment_cube);
            cube_rotate_y[0] = fmodf((cube_rotate_y[0] - 0.5f), 360.0f);
            cube_rotate_y[1] = fmodf((cube_rotate_y[1] - 0.5f), 360.0f);
            cube_rotate_y[2] = fmodf((cube_rotate_y[2] + 0.7f), 360.0f);
            cube_rotate_y[3] = fmodf((cube_rotate_y[3] + 0.7f), 360.0f);

            cube_perp[0] = glm::vec3((fmodf((pointLightPositions[0][0] + 90.0f), 360.0f)), pointLightPositions[0][1], (fmodf((pointLightPositions[0][2] + 90.0f), 360.0f)));
            cube_perp[1] = glm::vec3((fmodf((pointLightPositions[1][0] + 90.0f), 360.0f)), pointLightPositions[1][1], (fmodf((pointLightPositions[1][2] + 90.0f), 360.0f)));
            cube_perp[2] = glm::vec3((fmodf((pointLightPositions[2][0] - 90.0f), 360.0f)), pointLightPositions[2][1], (fmodf((pointLightPositions[2][2] - 90.0f), 360.0f)));
            cube_perp[3] = glm::vec3((fmodf((pointLightPositions[3][0] - 90.0f), 360.0f)), pointLightPositions[3][1], (fmodf((pointLightPositions[3][2] - 90.0f), 360.0f)));

        }
        initial = false;

        for (unsigned int i = 0; i < deer.size(); i++) {
                // calculate the model matrix for each object and pass it to shader before drawing
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
                model = glm::scale(model, glm::vec3(Deer_scale, Deer_scale, Deer_scale));
                //std::cout << "translation_y: " << deer[i].translation_y << std::endl;
                model = glm::translate(model, glm::vec3(deer[i].translation_x, deer[i].translation_y, deer[i].translation_z));
                model = glm::rotate(model, glm::radians(deer[i].rotate_x), glm::vec3(1.0f, 0.0f, 0.0f));
                model = glm::rotate(model, glm::radians(deer[i].rotate_y), glm::vec3(0.0f, 1.0f, 0.0f));
                model = glm::rotate(model, glm::radians(deer[i].rotate_z), glm::vec3(0.0f, 0.0f, 1.0f));
                float angle = 20.0f * i;
                //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
                lightingShader.setMat4("model", model);

                glDrawArrays(GL_TRIANGLES, 0, deer[i].modelMesh->mesh_data.mVertices.size());
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap5);
        glBindVertexArray(FLVAO);
        for (unsigned int i = 0; i < deer.size(); i++) {
                // calculate the model matrix for each object and pass it to shader before drawing
                glm::mat4 child = glm::mat4(1.0f);
                child = glm::translate(child, glm::vec3(0.0f, 0.0f, 0.0f));
                child = glm::scale(child, glm::vec3(Deer_scale, Deer_scale, Deer_scale));
                //std::cout << "translation_y: " << deer[i].translation_y << std::endl;
                child = glm::translate(child, glm::vec3(deer[i].translation_x, deer[i].translation_y, deer[i].translation_z));
                child = glm::rotate(child, glm::radians(deer[i].rotate_x), glm::vec3(1.0f, 0.0f, 0.0f));
                child = glm::rotate(child, glm::radians(deer[i].rotate_y), glm::vec3(0.0f, 1.0f, 0.0f));
                child = glm::rotate(child, glm::radians(deer[i].rotate_z), glm::vec3(0.0f, 0.0f, 1.0f));
                float angle = 20.0f * i;
                //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
                lightingShader.setMat4("model", child);

                glDrawArrays(GL_TRIANGLES, 0, FL[i].modelMesh->mesh_data.mVertices.size());
        }

        glBindVertexArray(FRVAO);
        for (unsigned int i = 0; i < deer.size(); i++) {
                // calculate the model matrix for each object and pass it to shader before drawing
                glm::mat4 child = glm::mat4(1.0f);
                child = glm::translate(child, glm::vec3(0.0f, 0.0f, 0.0f));
                child = glm::scale(child, glm::vec3(Deer_scale, Deer_scale, Deer_scale));
                //std::cout << "translation_y: " << deer[i].translation_y << std::endl;
                child = glm::translate(child, glm::vec3(deer[i].translation_x, deer[i].translation_y, deer[i].translation_z));
                child = glm::rotate(child, glm::radians(deer[i].rotate_x), glm::vec3(1.0f, 0.0f, 0.0f));
                child = glm::rotate(child, glm::radians(deer[i].rotate_y), glm::vec3(0.0f, 1.0f, 0.0f));
                child = glm::rotate(child, glm::radians(deer[i].rotate_z), glm::vec3(0.0f, 0.0f, 1.0f));
                float angle = 20.0f * i;
                //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
                lightingShader.setMat4("model", child);

                glDrawArrays(GL_TRIANGLES, 0, FR[i].modelMesh->mesh_data.mVertices.size());
        }

        glBindVertexArray(BLVAO);
        for (unsigned int i = 0; i < deer.size(); i++) {
                // calculate the model matrix for each object and pass it to shader before drawing
                glm::mat4 child = glm::mat4(1.0f);
                child = glm::translate(child, glm::vec3(0.0f, 0.0f, 0.0f));
                child = glm::scale(child, glm::vec3(Deer_scale, Deer_scale, Deer_scale));
                //std::cout << "translation_y: " << deer[i].translation_y << std::endl;
                child = glm::translate(child, glm::vec3(deer[i].translation_x, deer[i].translation_y, deer[i].translation_z));
                child = glm::rotate(child, glm::radians(deer[i].rotate_x), glm::vec3(1.0f, 0.0f, 0.0f));
                child = glm::rotate(child, glm::radians(deer[i].rotate_y), glm::vec3(0.0f, 1.0f, 0.0f));
                child = glm::rotate(child, glm::radians(deer[i].rotate_z), glm::vec3(0.0f, 0.0f, 1.0f));
                float angle = 20.0f * i;
                //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
                lightingShader.setMat4("model", child);

                glDrawArrays(GL_TRIANGLES, 0, BL[i].modelMesh->mesh_data.mVertices.size());
        }

        glBindVertexArray(BRVAO);
        for (unsigned int i = 0; i < deer.size(); i++) {
                // calculate the model matrix for each object and pass it to shader before drawing
                glm::mat4 child = glm::mat4(1.0f);
                child = glm::translate(child, glm::vec3(0.0f, 0.0f, 0.0f));
                child = glm::scale(child, glm::vec3(Deer_scale, Deer_scale, Deer_scale));
                //std::cout << "translation_y: " << deer[i].translation_y << std::endl;
                child = glm::translate(child, glm::vec3(deer[i].translation_x, deer[i].translation_y, deer[i].translation_z));
                child = glm::rotate(child, glm::radians(deer[i].rotate_x), glm::vec3(1.0f, 0.0f, 0.0f));
                child = glm::rotate(child, glm::radians(deer[i].rotate_y), glm::vec3(0.0f, 1.0f, 0.0f));
                child = glm::rotate(child, glm::radians(deer[i].rotate_z), glm::vec3(0.0f, 0.0f, 1.0f));
                float angle = 20.0f * i;
                //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
                lightingShader.setMat4("model", child);

                glDrawArrays(GL_TRIANGLES, 0, BR[i].modelMesh->mesh_data.mVertices.size());
        }
        /*
        for (int i = 0; i < num_deers; i++) {
            deer[i].model = glm::mat4(1.0f);
            //Model operations
            deer[i].model = glm::scale(deer[i].model, vec3(0.1f, 0.1f, 0.1f));
            deer[i].model = glm::rotate(deer[i].model, glm::radians(deer[i].rotate_x), glm::vec3(1.0f, 0.0f, 0.0f));
            deer[i].model = glm::rotate(deer[i].model, glm::radians(deer[i].rotate_x), glm::vec3(0.0f, 1.0f, 0.0f));
            deer[i].model = glm::rotate(deer[i].model, glm::radians(deer[i].rotate_x), glm::vec3(0.0f, 0.0f, 1.0f));
            deer[i].translation = glm::vec3(deer[i].translation_x, deer[i].translation_y, deer[i].translation_z);
            deer[i].model = glm::translate(deer[i].model, deer[i].translation);

            //Deer model
            glDrawArrays(GL_TRIANGLES, 0, modelData.mVertices.size());

            //deer.Draw(myShader);
                
            //Children of deer model (legs)
            for (int j = 0; j < num_legs; j++) {
                deer[i].model_legs[j] = identity_mat4();
                // Apply the root matrix to the child matrix
                deer[i].model_legs[j] = deer[i].model * deer[i].model_legs[j];
                //deer[0].model_legs[0] = rotate_z_deg(deer[i].model_legs[j], 1.0f);
                // Update the appropriate uniform and draw the mesh again
                glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj.m);
                glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view.m);
                glUniformMatrix4fv(matrix_location, 1, GL_FALSE, deer[i].model_legs[j].m);
                generateObjectBufferMesh(deer[i].mesh_data_legs[j], vao_deer_legs[i][j], vao_deer_legs_n[i][j]);
                glDrawArrays(GL_TRIANGLES, 0, deer[i].mesh_data_legs[j].mPointCount);
            }
        }*/

         // also draw the lamp object(s)
         lightCubeShader.use();
         lightCubeShader.setMat4("projection", projection);
         lightCubeShader.setMat4("view", view);
    
        // we now draw as many light bulbs as we have point lights.
        glBindBuffer(GL_ARRAY_BUFFER, lightCubeVBO);
        glBindVertexArray(lightCubeVAO);
	    for (unsigned int i = 0; i < 5; i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(Cube_scale, Cube_scale, Cube_scale));
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::rotate(model, glm::radians(cube_rotate_y[i]), glm::vec3(0.0f, 1.0f, 0.0f));
            lightCubeShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
	    }

        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setFloat("material.shininess", 32.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap2);

        glBindVertexArray(sceneVAO);
        for (unsigned int i = 0; i < 1; i++)
            {
                // calculate the model matrix for each object and pass it to shader before drawing
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
                model = glm::scale(model, glm::vec3(Scene_scale, Scene_scale, Scene_scale));
                float angle = 20.0f * i;
                model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
                lightingShader.setMat4("model", model);

                glDrawArrays(GL_TRIANGLES, 0, modelDataScene.mVertices.size());
        }

        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, diffuseMap3);
        //std::cout << "car.size(): " << car.size() << std::endl;
        glBindVertexArray(carVAO);
        for (unsigned int i = 0; i < car.size(); i++) {
            if (i == 0) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, diffuseMap3);
            } else {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, diffuseMap4);
            }
                // calculate the model matrix for each object and pass it to shader before drawing
                glm::mat4 model = glm::mat4(1.0f);
		        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
                model = glm::scale(model, glm::vec3(Car_scale, Car_scale, Car_scale));
                //std::cout << "translation_y: " << deer[i].translation_y << std::endl;
                model = glm::translate(model, glm::vec3(car[i].translation_x, car[i].translation_y, car[i].translation_z));
                model = glm::rotate(model, glm::radians(car[i].rotate_x), glm::vec3(1.0f, 0.0f, 0.0f));
                model = glm::rotate(model, glm::radians(car[i].rotate_y), glm::vec3(0.0f, 1.0f, 0.0f));
                model = glm::rotate(model, glm::radians(car[i].rotate_z), glm::vec3(0.0f, 0.0f, 1.0f));
                float angle = 20.0f * i;
                //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
                lightingShader.setMat4("model", model);

                glDrawArrays(GL_TRIANGLES, 0, car[i].modelMesh->mesh_data.mVertices.size());
                //std::cout << "car[i].modelMesh->mesh_data.mVertices.size(): " << car[i].modelMesh->mesh_data.mVertices.size() << std::endl;
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &modelVAO);
    glDeleteBuffers(1, &modelVBO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &lightCubeVBO);
    glDeleteVertexArrays(1, &sceneVAO);
    glDeleteBuffers(1, &sceneVBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}


ModelData load_mesh(const char* file_name, float** verticesp) {
    ModelData modelData;

    /* Use assimp to read the model file, forcing it to be read as    */
    /* triangles. The second flag (aiProcess_PreTransformVertices) is */
    /* relevant if there are multiple meshes in the model file that   */
    /* are offset from the origin. This is pre-transform them so      */
    /* they're in the right position.                                 */
    const aiScene* scene = aiImportFile(
                        file_name, 
                        aiProcess_Triangulate | aiProcess_PreTransformVertices
                        ); 

    if (!scene) {
        fprintf(stderr, "ERROR: reading mesh %s\n", file_name);
        return modelData;
    }

    printf("%s\n", file_name);
    printf("  %i materials\n", scene->mNumMaterials);
    printf("  %i meshes\n", scene->mNumMeshes);
    printf("  %i textures\n", scene->mNumTextures);

    int m_i_skip;
    if (!strcmp(file_name, "Meshes/Legless.dae")) {
        m_i_skip = 1;
    }
    else {
        m_i_skip = 0;
    }
    for (unsigned int m_i = m_i_skip; m_i < scene->mNumMeshes; m_i++) {
        int total_num_indices = 0;
        const aiMesh* mesh = scene->mMeshes[m_i];
        printf("  %i mesh faces\n", mesh->mNumFaces);

        for (unsigned int i3=0;i3<mesh->mNumFaces;++i3) {
            for (unsigned int i4=0; i4<mesh->mFaces[i3].mNumIndices;++i4) {
                modelData.vert_indices.push_back(mesh->mFaces[i3].mIndices[i4]);
            }
            total_num_indices += mesh->mFaces[i3].mNumIndices;
        }

        printf("    %i vertices in mesh\n", mesh->mNumVertices);
        modelData.mPointCount += mesh->mNumVertices;
        for (unsigned int v_i = 0; v_i < mesh->mNumVertices; v_i++) {
            if (mesh->HasPositions()) {
                const aiVector3D* vp = &(mesh->mVertices[v_i]);
                modelData.mVertices.push_back(vec3(vp->x, vp->y, vp->z));
            }
            if (mesh->HasNormals()) {
                const aiVector3D* vn = &(mesh->mNormals[v_i]);
                modelData.mNormals.push_back(vec3(vn->x, vn->y, vn->z));
            }
            if (mesh->HasTextureCoords(0)) {
                if (!strcmp(file_name, "Meshes/road-field_simple.dae")) {
                    //std::cout << "road-field_simple mesh has texture coordinates" << std::endl;
                }
                const aiVector3D* vt = &(mesh->mTextureCoords[0][v_i]);
                modelData.mTextureCoords.push_back(vec2(vt->x, vt->y));
            } else {
                std::cout << "!!!" << std::endl;
                modelData.mTextureCoords.push_back(vec2(0.0f, 0.0f));
            }
            if (mesh->HasTangentsAndBitangents()) {
                /* You can extract tangents and bitangents here              */
                /* Note that you might need to make Assimp generate this     */
                /* data for you. Take a look at the flags that aiImportFile  */
                /* can take.                                                 */
            }
        }
    printf("File name: %s\n", file_name);
    std::cout << "Texture Coordinates size: " << modelData.mTextureCoords.size() << std::endl;
    }

    float* vertices = new float[modelData.mVertices.size()*(3+3+2)];
    int j=0;
    for (int i=0; i < modelData.mVertices.size(); i++) {
      vertices[j] = modelData.mVertices[i][0]/200;
      vertices[j+1] = modelData.mVertices[i][1]/200;
      vertices[j+2] = modelData.mVertices[i][2]/200;
      vertices[j+3] = modelData.mNormals[i][0];
      vertices[j+4] = modelData.mNormals[i][1];
      vertices[j+5] = modelData.mNormals[i][2];
      vertices[j+6] = modelData.mTextureCoords[i][0];
      vertices[j+7] = modelData.mTextureCoords[i][1];
      j += 8;
    }
    j=0;

    for (int i = 0; i < modelData.mVertices.size(); i++) {
        // std::cout << "vertices[j]: " << vertices[j] << " ";
        // std::cout << "vertices[j+1]: " << vertices[j+1] << " ";
        // std::cout << "vertices[j+2]: " << vertices[j+2] << " ";
        // std::cout << "vertices[j+3]: " << vertices[j+3] << " ";
        // std::cout << "vertices[j+4]: " << vertices[j+4] << " ";
        // std::cout << "vertices[j+5]: " << vertices[j+5] << " ";
        // std::cout << "vertices[j+6]: " << vertices[j+6] << " ";
        // std::cout << "vertices[j+7]: " << vertices[j+7] << std::endl;
        j += 8;
    }

    *verticesp = vertices;

    aiReleaseImport(scene);
    return modelData;
}

void initialise_deer_positions(std::vector<Model>& deer) {
	for (int i = 0; i < deer.size(); i++) {
		int r_tx = rand();
		int r_tz = rand();
		int r_ry = rand();
		deer[i].translation_x = ((3.0f + fmodf(r_tx, 19.0f)) / adjustment);
	    deer[i].translation_y = (25.0f / adjustment);
		deer[i].translation_z = ((28.0f + fmodf(r_tz, 19.0f)) / adjustment);
		deer[i].rotate_x = 0.0f;
		deer[i].rotate_y = fmodf(r_ry, 360.0f);
		deer[i].rotate_z = 0.0f;
	}
}

void initialise_car_positions(std::vector<Model>& car) {
	for (int i = 0; i < car.size(); i++) {
		int r_tx = rand();
		int r_tz = rand();
		int r_ry = rand();
		//car[i].translation_x = ((3.0f + fmodf(r_tx, 19.0f)) / adjustment_car);
		car[0].translation_x = (-21.2f / adjustment_car);
        car[1].translation_x = (23.2f / adjustment_car);
		//car[i].translation_y = (25.0f / adjustment_car);
		car[i].translation_y = (25.0f / adjustment_car);
		//car[i].translation_z = ((28.0f + fmodf(r_tz, 19.0f)) / adjustment_car);
		car[i].translation_z = 0.0f;
		car[i].rotate_x = 0.0f;
		//car[i].rotate_y = fmodf(r_ry, 360.0f);
        car[i].rotate_y = 180.0f;
		car[i].rotate_z = 0.0f;
        // translation_x_test = ((3.0f + fmodf(r_tx, 19.0f)) / adjustment);
        // translation_y_test = (25.0f / adjustment);
        // translation_z_test = ((28.0f + fmodf(r_tz, 19.0f)) / adjustment);
        car[0].theta = 180.0f;
        car[1].theta = 0.0f;

	}
}
