#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader/shader_m.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <iostream>
#include <shader/camera.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
glm::mat4 ShearTransform(glm::mat4 model, float SaX, float SaY, float SaZ);
glm::mat4 Mirror(glm::mat4 model, float aXY, float aXZ, float aYZ);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.f, 1.f, -5.f);

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CG Cube", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("Shaders/cube3d.vs", "Shaders/cube3d.fs");
    Shader lightCubeShader("Shaders/2.2.light_cube.vs", "Shaders/2.2.light_cube.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        //positions         //texture coords // color surfaces //normals
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,   1.f, 0.f, 0.f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,   1.f, 0.f, 0.f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   1.f, 0.f, 0.f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   1.f, 0.f, 0.f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   1.f, 0.f, 0.f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,   1.f, 0.f, 0.f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   0.f, 1.f, 0.f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   0.f, 1.f, 0.f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,   0.f, 1.f, 0.f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,   0.f, 1.f, 0.f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,   0.f, 1.f, 0.f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   0.f, 1.f, 0.f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   0.f, 0.f, 1.f,  -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   0.f, 0.f, 1.f,  -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   0.f, 0.f, 1.f,  -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   0.f, 0.f, 1.f,  -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   0.f, 0.f, 1.f,  -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   0.f, 0.f, 1.f,  -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   1.f, 1.f, 0.f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   1.f, 1.f, 0.f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   1.f, 1.f, 0.f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   1.f, 1.f, 0.f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   1.f, 1.f, 0.f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   1.f, 1.f, 0.f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   1.f, 0.f, 1.f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,   1.f, 0.f, 1.f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   1.f, 0.f, 1.f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   1.f, 0.f, 1.f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   1.f, 0.f, 1.f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   1.f, 0.f, 1.f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   0.f, 1.f, 1.f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   0.f, 1.f, 1.f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   0.f, 1.f, 1.f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   0.f, 1.f, 1.f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,   0.f, 1.f, 1.f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   0.f, 1.f, 1.f,  0.0f,  1.0f,  0.0f
    };
    //first configure the cube's VAO and VBO
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // color surface attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // normal attribute
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // load and create a texture 
    // -------------------------
    unsigned int texture1;
    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load("textures/matrix.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
   

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    ourShader.use();
    ourShader.setInt("texture1", 0);
    
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsClassic();
    //ImGui::StyleColorsLight();
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    
    ImVec4 clear_color = ImVec4(1.f, 0.1f, 0.2f, 1.00f);


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        
        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window
        
        //current value holders
        static float TRANSLATE_X = 0.0f;
        static float TRANSLATE_Y = 0.0f;
        static float TRANSLATE_Z = 0.0f;

        static float ROTATE_X = 0.0f;
        static float ROTATE_Y = 0.0f;
        static float ROTATE_Z = 0.0f;
        static float RP_X = 0.0f;
        static float RP_Y = 0.0f;
        static float RP_Z = 0.0f;

        static float SCALE_X = 1.0f;
        static float SCALE_Y = 1.0f;
        static float SCALE_Z = 1.0f;
        static float Sk = 1.0f;

        static bool  ASPECT_RATIO = true;
        static bool ROTATE_ENABLE = false;
        static bool TRANSLATE_ENABLE = false;
        static bool SCALE_ENABLE = false;
        static bool PERSPECTIVE_ENABLE = true;
        static bool WIREFRAME = false;
        static bool TEX_ENABLE = true;
        static bool SHEAR_ENABLE = false;
        static bool MIRROR_ENABLE = false;

        static bool SaX_ENABLE = false;
        static bool SaY_ENABLE = false;
        static bool SaZ_ENABLE = false;

        static bool mXY_ENABLE = false;
        static bool mXZ_ENABLE = false;
        static bool mYZ_ENABLE = false;

        //past value holders
        static float tra_x = 0.f;
        static float tra_y = 0.f;
        static float tra_z = 0.f;

        static float rot_x = 0.f;
        static float rot_y = 0.f;
        static float rot_z = 0.f;

        static float rp_x = 0.0f;
        static float rp_y = 0.0f;
        static float rp_z = 0.0f;

        static float sca_x = 1.f;
        static float sca_y = 1.f;
        static float sca_z = 1.f;

        static float past_sax = 0.f;
        static float past_say = 0.f;
        static float past_saz = 0.f;

        static float past_mxy = 0.f;
        static float past_mxz = 0.f;
        static float past_myz = 0.f;
        
        static float Lx = 0.0f;
        static float Ly = 0.0f;
        static float Lz = -5.f;

        if (WIREFRAME) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
        ImGui::Begin("Options");                          // Create a window called "Hello, world!" and append into it.
        

        // TRANSLATE
        ImGui::Text("Translation");
        ImGui::SetNextItemWidth(100);
        ImGui::InputFloat("Tx", &TRANSLATE_X, .1f, 1.0f, "%.3f");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        ImGui::InputFloat("Ty", &TRANSLATE_Y, .1f, 1.0f, "%.3f");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        ImGui::InputFloat("Tz", &TRANSLATE_Z, .1f, 1.0f, "%.3f");
        ImGui::SameLine();

        if (ImGui::Button("Translate"))
            TRANSLATE_ENABLE = true;
        
        // ROTATE by Angle
        ImGui::Text("Rotation");
        ImGui::Text("Angle"); ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        ImGui::InputFloat("Rx", &ROTATE_X, 1.f, 1.0f, "%.3f");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        ImGui::InputFloat("Ry", &ROTATE_Y, 1.f, 1.0f, "%.3f");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        ImGui::InputFloat("Rz", &ROTATE_Z, 1.f, 1.0f, "%.3f");

        //ROTATE by point
        ImGui::Text("Point"); ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        ImGui::InputFloat("PRx", &RP_X, 1.f, 1.0f, "%.3f");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        ImGui::InputFloat("PRy", &RP_Y, 1.f, 1.0f, "%.3f");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        ImGui::InputFloat("PRz", &RP_Z, 1.f, 1.0f, "%.3f");
        ImGui::SameLine();

        if (ImGui::Button("Rotate"))
            ROTATE_ENABLE = true;

        // SCALE
        if (ASPECT_RATIO) {
            ImGui::Text("Scaling");
            ImGui::SetNextItemWidth(100);
            ImGui::InputFloat("Sx", &Sk, 0.1f, 1.0f, "%.3f");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(100);
            ImGui::InputFloat("Sy", &Sk, 0.1f, 1.0f, "%.3f");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(100);
            ImGui::InputFloat("Sz", &Sk, .1f, 1.0f, "%.3f");
            ImGui::SameLine();
        }

        else {
            ImGui::Text("Scaling");
            ImGui::SetNextItemWidth(100);
            ImGui::InputFloat("Sx", &SCALE_X, .1f, 1.0f, "%.3f");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(100);
            ImGui::InputFloat("Sy", &SCALE_Y, .1f, 1.0f, "%.3f");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(100);
            ImGui::InputFloat("Sz", &SCALE_Z, .1f, 1.0f, "%.3f");
            ImGui::SameLine();
        }

        if (ImGui::Button("Scale"))
            SCALE_ENABLE = true;

        if (ImGui::Button("Reset"))
        {
             TRANSLATE_X = 0.0f;
             TRANSLATE_Y = 0.0f;
             TRANSLATE_Z = 0.0f;

             ROTATE_X = 0.0f;
             ROTATE_Y = 0.0f;
             ROTATE_Z = 0.0f;
             RP_X = 0.0f;
             RP_Y = 0.0f;
             RP_Z = 0.0f;

             SCALE_X = 1.0f;
             SCALE_Y = 1.0f;
             SCALE_Z = 1.0f;
             Sk = 1.0f;

             ASPECT_RATIO = true;
             ROTATE_ENABLE = false;
             TRANSLATE_ENABLE = false;
             SCALE_ENABLE = false;
             PERSPECTIVE_ENABLE = true;
             WIREFRAME = false;
             TEX_ENABLE = true;
             SHEAR_ENABLE = false;
             MIRROR_ENABLE = false;
             
             SaX_ENABLE = false;
             SaY_ENABLE = false;
             SaZ_ENABLE = false;

             mXY_ENABLE = false;
             mXZ_ENABLE = false;
             mYZ_ENABLE = false;
            //past value holders
             tra_x = 0.f;
             tra_y = 0.f;
             tra_z = 0.f;

             rot_x = 0.f;
             rot_y = 0.f;
             rot_z = 0.f;

             rp_x = 0.f;
             rp_y = 0.f;
             rp_z = 0.f;

             sca_x = 1.f;
             sca_y = 1.f;
             sca_z = 1.f;

             past_sax = 0.f;
             past_say = 0.f;
             past_saz = 0.f;

             past_mxy = 0.f;
             past_mxz = 0.f;
             past_myz = 0.f;
        }

        ImGui::Checkbox("Keep Aspect Ratio", &ASPECT_RATIO);
        ImGui::Checkbox("Perspective/Ortho", &PERSPECTIVE_ENABLE);
        ImGui::Checkbox("Fill/Wireframe", &WIREFRAME);
        ImGui::Checkbox("Texture ON/OFF", &TEX_ENABLE);

        if (ImGui::Button("Shear")) {
            SHEAR_ENABLE = true;
        }
        ImGui::SameLine();
        ImGui::Checkbox("X", &SaX_ENABLE); ImGui::SameLine();
        ImGui::Checkbox("Y", &SaY_ENABLE); ImGui::SameLine();
        ImGui::Checkbox("Z", &SaZ_ENABLE); 

        if (ImGui::Button("Mirror")) {
            MIRROR_ENABLE = true;
        }
        ImGui::SameLine();
        ImGui::Checkbox("XY", &mXY_ENABLE); ImGui::SameLine();
        ImGui::Checkbox("XZ", &mXZ_ENABLE); ImGui::SameLine();
        ImGui::Checkbox("YZ", &mYZ_ENABLE);

        //Light bulb position
        ImGui::Text("Bulb"); ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        ImGui::InputFloat("Lx", &Lx, 1.f, 1.0f, "%.3f");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        ImGui::InputFloat("Ly", &Ly, 1.f, 1.0f, "%.3f");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        ImGui::InputFloat("Lz", &Lz, 1.f, 1.0f, "%.3f");
        ImGui::SameLine();

        ImGui::End();

        // render
        // ------
        
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!
        

        // bind textures on corresponding texture units
        if (TEX_ENABLE) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture1);
        }
        else {
            glActiveTexture(NULL);
            glBindTexture(GL_TEXTURE_2D, NULL);
        }

        // activate shader
        ourShader.use();

        // create transformations
        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        

        
        //Translate
        if (TRANSLATE_ENABLE) {
            model = glm::translate(model, glm::vec3(TRANSLATE_X, TRANSLATE_Y, TRANSLATE_Z-3.f));

            tra_x = TRANSLATE_X;
            tra_y = TRANSLATE_Y;
            tra_z = TRANSLATE_Z;

            TRANSLATE_ENABLE = false;
        }

        else 
        {
            model = glm::translate(model, glm::vec3(tra_x, tra_y, tra_z-3.f));
        }
        
        // Rotate by angle
        if (ROTATE_ENABLE) {
            model = glm::translate(model, glm::vec3(RP_X, RP_Y, RP_Z));
            model = glm::rotate(model, glm::radians(ROTATE_X), glm::vec3(1.f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(ROTATE_Y), glm::vec3(0.f, 1.f, 0.0f));
            model = glm::rotate(model, glm::radians(ROTATE_Z), glm::vec3(0.f, 0.0f, 1.f));
            model = glm::translate(model, glm::vec3(-RP_X, -RP_Y, -RP_Z));
            
            rot_x = ROTATE_X;
            rot_y = ROTATE_Y;
            rot_z = ROTATE_Z;

            rp_x = RP_X;
            rp_y = RP_Y;
            rp_z = RP_Z;

            ROTATE_ENABLE = false;
        }

        else
        {
            model = glm::translate(model, glm::vec3(rp_x, rp_y, rp_z));
            model = glm::rotate(model, glm::radians(rot_x), glm::vec3(1.f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(rot_y), glm::vec3(0.f, 1.f, 0.0f));
            model = glm::rotate(model, glm::radians(rot_z), glm::vec3(0.f, 0.0f, 1.f));
            model = glm::translate(model, glm::vec3(-rp_x, -rp_y, -rp_z));

        }
        
        
        //SCALE
        if (SCALE_ENABLE && SCALE_X >= 0 && SCALE_Y >= 0 && SCALE_Z >= 0 && Sk >= 0) {
            model = glm::scale(model, glm::vec3(SCALE_X, SCALE_Y, SCALE_Z));

            sca_x = SCALE_X;
            sca_y = SCALE_Y;
            sca_z = SCALE_Z;

            if (ASPECT_RATIO) {
                model = glm::scale(model, glm::vec3(Sk*1.f, Sk*1.f, Sk*1.f));
                sca_x = Sk;
                sca_y = Sk;
                sca_z = Sk;
            }

            if (!PERSPECTIVE_ENABLE) {
                sca_x = sca_x * 500.f;
                sca_y = sca_y * 500.f;
                sca_z = sca_z * 500.f;
            }
            
            SCALE_ENABLE = false;
        }

        else
        {
            model = glm::scale(model, glm::vec3(sca_x, sca_y, sca_z));
        }

        if (SHEAR_ENABLE) {

            if (SaX_ENABLE) {
                model = ShearTransform(model, 1.f, 0.f, 0.f);
                past_sax = 1.f;
                past_say = 0.f;
                past_saz = 0.f;
                SaY_ENABLE = false;
                SaZ_ENABLE = false;
            }
            else if (SaY_ENABLE) {
                model = ShearTransform(model, 0.f, 1.f, 0.f);
                past_sax = 0.f;
                past_say = 1.f;
                past_saz = 0.f;
                SaX_ENABLE = false;
                SaZ_ENABLE = false;
            }
            else if (SaZ_ENABLE) {
                model = ShearTransform(model, 0.f, 0.f, 1.f);
                past_sax = 0.f;
                past_say = 0.f;
                past_saz = 1.f;
                SaX_ENABLE = false;
                SaY_ENABLE = false;
            }
            else {
                past_sax = 0.f;
                past_say = 0.f;
                past_saz = 0.f;
            }
            

            SHEAR_ENABLE = false;
        }

        else {
            model = ShearTransform(model, past_sax, past_say, past_saz);
        }

        if (MIRROR_ENABLE) {
            if (mXY_ENABLE) {
                past_mxy = 1.f;
                past_mxz = 0.f;
                past_myz = 0.f;
                model = Mirror(model, past_mxy, 0.f, 0.f);

                mYZ_ENABLE = false;
                mXZ_ENABLE = false;
            }
            else if (mXZ_ENABLE) {
                past_mxz = 1.f;
                past_mxy = 0.f;
                past_myz = 0.f;
                model = Mirror(model, 0.f, past_mxz, 0.f);

                mXY_ENABLE = false;
                mYZ_ENABLE = false;
            }
            else if (mYZ_ENABLE) {
                past_myz = 1.f;
                past_mxy = 0.f;
                past_mxz = 0.f;
                model = Mirror(model, 0.f, 0.f, past_myz);

                mXY_ENABLE = false;
                mXZ_ENABLE = false;
            }
            else {
                past_mxy = 0.f;
                past_mxz = 0.f;
                past_myz = 0.f;
            }

            MIRROR_ENABLE = false;
        }

        else {
            model = Mirror(model, past_mxy, past_mxz, past_myz);
            
        }

      

        // CHANGE Projection Mode Perspective/Ortho
        if (PERSPECTIVE_ENABLE) {
            projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, .001f, 100.0f);
        }
        else
        {
            projection = glm::ortho(
                -static_cast<float>(SCR_WIDTH / 2.f),
                static_cast<float>(SCR_WIDTH / 2.f),
                -static_cast<float>(SCR_HEIGHT /2.f),
                static_cast<float>(SCR_HEIGHT / 2.f),
                0.1f,
                10000.0f
            );
            
        }
        
        glm::vec3 lightPos = glm::vec3(Lx, Ly, Lz);
        // retrieve the matrix uniform locations
        unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
        //unsigned int viewLoc = glGetUniformLocation(ourShader.ID, "view");
        // pass them to the shaders (3 different ways)
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        //glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
        ourShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("lightPos", lightPos);
        ourShader.setVec3("viewPos", camera.Position);
        ourShader.setMat4("projection", projection);
        
        ourShader.setBool("TEX_ENABLE", TEX_ENABLE);

        // render box
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // also draw the lamp object
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        if (!PERSPECTIVE_ENABLE) {
            model = glm::scale(model, glm::vec3(0.1f * 500.f)); // a smaller cube
        }
        else {
            model = glm::scale(model, glm::vec3(0.1f)); // a smaller cube
        }
        lightCubeShader.setMat4("model", model);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &lightCubeVAO);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
glm::mat4 ShearTransform(glm::mat4 model, float SaX, float SaY, float SaZ) {
    glm::mat4 shear = glm::mat4x4(
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f);

    model = glm::translate(model, glm::vec3(0.f, 0.f, 0.f));
    // X Shear
    if (SaX) {
        shear[0][1] = 1.f;
        shear[0][2] = 1.f;

    }
    //Y Shear
    else if (SaY) {
        shear[1][0] = 1.f;
        shear[1][2] = 1.f;
    }
    //Z Shear
    else if (SaZ) {
        shear[2][0] = 1.f;
        shear[2][1] = 1.f;
    }
    model = shear * model;

    return model;
}

glm::mat4 Mirror(glm::mat4 model, float aXY, float aXZ, float aYZ) {
    glm::mat4 reflect = glm::mat4x4(
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f);

    if (aXY) {
        reflect[2][2] = -1.f;
    }
    else if (aXZ) {
        reflect[1][1] = -1.f;
    }
    else if (aYZ) {
        reflect[0][0] = -1.f;
    }

    model = reflect * model;

    return model;
}