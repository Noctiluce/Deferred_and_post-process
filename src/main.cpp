#include "Helper.hpp"

#include "Shader.hpp"
#include "Camera.hpp"
#include "OBJ_loader.hpp"
#include "Model.hpp"
#include "UserInterface.hpp"

#include <random>

#include <stdio.h>  /* defines FILENAME_MAX */
#ifdef __linux__
    #include <unistd.h>
    #define GetCurrentDir getcwd
#else
    #include <direct.h>
    #define GetCurrentDir _getcwd
#endif

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

int initWindow(GLFWwindow*& window, bool& retflag);

int getCurrentWorkingDirectory(std::string& currentPath, bool& retflag);

// settings
const unsigned int SCR_WIDTH = 1920*1;
const unsigned int SCR_HEIGHT = 1080*1;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

float ssaoRadius = 0.5;

// sleeping
bool requestInverseSwap = false;
bool requestSSAO = false;
bool ssao = true;
bool requestBloom = false;
bool bloom = false;
bool swapInterval = true;
bool requestMove = false;
bool move = true;
 
bool sobel = false;
bool pencil = false;

int currentEffect = 0;

std::vector<unsigned int> shadersID;


void setIcon(std::string path)
{
    //stb load 

    GLFWimage images[2];
    unsigned char * data = stbi_load(path.c_str(), &images[0].width, &images[0].height, 0, 4);
    images[0].pixels = data;
    images[1].pixels = data;
    glfwSetWindowIcon(glfwGetCurrentContext(), 1, images);

    stbi_image_free(data);
}


/*    TMP FUNCTIONS    */    
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
             1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}
void printWorkGroupsCapabilities() {
    int workgroup_count[3];
    int workgroup_size[3];
    int workgroup_invocations;

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workgroup_count[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workgroup_count[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workgroup_count[2]);

    printf("Taille maximale des workgroups:\n\tx:%u\n\ty:%u\n\tz:%u\n",
        workgroup_size[0], workgroup_size[1], workgroup_size[2]);

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workgroup_size[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workgroup_size[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workgroup_size[2]);

    printf("Nombre maximal d'invocation locale:\n\tx:%u\n\ty:%u\n\tz:%u\n",
        workgroup_size[0], workgroup_size[1], workgroup_size[2]);

    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &workgroup_invocations);
    printf("Nombre maximum d'invocation de workgroups:\n\t%u\n", workgroup_invocations);
}
/***********************/

int initWindow(GLFWwindow*& window, bool& retflag)
{
    retflag = true;
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Starter Window", NULL, NULL);
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    retflag = false;
    return {};
}

int getCurrentWorkingDirectory(std::string& currentPath, bool& retflag)
{
    retflag = true;
    char cCurrentPath[FILENAME_MAX];
    if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath))) { return errno; }
    cCurrentPath[sizeof(cCurrentPath) - 1] = '\0';
    currentPath = std::string(cCurrentPath);
    int found = currentPath.find("build");
    currentPath = currentPath.substr(0, found);
    for (int i = 0; i < (int)currentPath.size(); ++i) { if (currentPath[i] == '\\') { currentPath[i] = '/'; } }
    retflag = false;
    return {};
}


int createTexture2D(bool& retflag, unsigned int & _textureID, unsigned int _sizeWidth, unsigned int _sizeHeight, GLint _internalFormat, GLenum _Format, GLenum _type, GLint _pxFormat, unsigned int _attachment) {
    retflag = true;
    glGenTextures(1, &_textureID);
    glBindTexture(GL_TEXTURE_2D, _textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, _sizeWidth, _sizeHeight, 0, _Format, _type, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _pxFormat);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _pxFormat);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+_attachment, GL_TEXTURE_2D, _textureID, 0);
    retflag = false;
    return {};
}

int invertSwap(bool& retflag) {
    retflag = true;
    swapInterval = !swapInterval;
    glfwSwapInterval(swapInterval);
    retflag = false;
    return {};
}

void recompileShader() {
    for (unsigned int i : shadersID) {
        glCompileShader(i);
    }
}

int main()
{
    // Local variables
    bool retflag;
    int retval;
    GLFWwindow* window;
    std::string currentPath;


    // Current Working Directory 
    retval = getCurrentWorkingDirectory(currentPath, retflag);
    if (retflag) return retval;
    std::cout << "Current working directory is " << currentPath << std::endl;

    // Initialize Window
    retval = initWindow(window, retflag);
    if (retflag) return retval;
    setIcon(currentPath + "assets/textures/bunny.png");
    
    // Configure global opengl state
    glEnable(GL_DEPTH_TEST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


    // TMP todo remove
    printWorkGroupsCapabilities();


    // build and compile our shader programs
    std::cout << 1 << std::endl;
    Shader shaderGeometryPass((currentPath + "assets/shaders/ssao_geometry.vs.glsl").c_str(), (currentPath + "assets/shaders/ssao_geometry.fs.glsl").c_str());
    std::cout << 2 << std::endl;
    Shader shaderLightingPass((currentPath + "assets/shaders/ssao.vs.glsl").c_str(), (currentPath + "assets/shaders/ssao_lighting.fs.glsl").c_str());
    std::cout << 3 << std::endl;
    Shader shaderSSAO((currentPath + "assets/shaders/ssao.vs.glsl").c_str(), (currentPath + "assets/shaders/ssao.fs.glsl").c_str());
    std::cout << 4 << std::endl;
    Shader shaderSSAOBlur((currentPath + "assets/shaders/ssao.vs.glsl").c_str(), (currentPath + "assets/shaders/ssao_blur.fs.glsl").c_str());
    std::cout << 5 << std::endl;
    Shader finalShader((currentPath + "assets/shaders/final.vs.glsl").c_str(), (currentPath + "assets/shaders/final.fs.glsl").c_str());
    std::cout << 6 << std::endl;
    //Shader extractBrightness((currentPath + "assets/shaders/final.vs.glsl").c_str(), (currentPath + "assets/shaders/brightness.fs.glsl").c_str());
    std::cout << 7 << std::endl;




    // shaders configuration
    shaderLightingPass.use();
    shaderLightingPass.setInt("gPosition", 0);
    shaderLightingPass.setInt("gNormal", 1);
    shaderLightingPass.setInt("gAlbedo", 2);
    shaderLightingPass.setInt("ssao", 3);


    shaderSSAO.use();
    shaderSSAO.setInt("gPosition", 0);
    shaderSSAO.setInt("gNormal", 1);
    shaderSSAO.setInt("texNoise", 2);

    shaderSSAOBlur.use();
    shaderSSAOBlur.setInt("ssaoInput", 0);

    shaderGeometryPass.use();

    finalShader.use();
    finalShader.setInt("finalImage", 0);
    finalShader.setInt("albedoImage", 1);


    // Load Models
    //Model modelToRender = Model(currentPath + "assets/models/fireplace_room", "fireplace_room.obj");
    Model modelToRender = Model(currentPath + "assets/models/sponza", "sponza.dae");
    //Model modelToRender = Model(currentPath + "assets/models", "suzanne.dae");
    //Model modelToRender = Model(currentPath + "assets/models", "box2.dae");
     
    // User Interface
    UserInterface ui = UserInterface();
    ui.init();
    ui.SCR_HEIGHT = SCR_HEIGHT;
    ui.SCR_WIDTH = SCR_WIDTH;
    ui._ssao = &ssao;
    ui._ssaoRadius = &ssaoRadius;
    ui._currentEffect = &currentEffect;
    retval = invertSwap(retflag);
    if (retflag) return retval;



    // buffers
    unsigned int gBuffer, finalFBO;
    glGenFramebuffers(1, &gBuffer);
    glGenFramebuffers(1, &finalFBO);
    // Textures
    unsigned int gPosition, gNormal, gAlbedoSpec, prePostProcess, bloomTex;
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    // FIRST FBO 
    // position color buffer
    retval = createTexture2D(retflag, gPosition, SCR_WIDTH, SCR_HEIGHT, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_NEAREST, 0);
    if (retflag) return retval;
    // normal color buffer
    retval = createTexture2D(retflag, gNormal, SCR_WIDTH, SCR_HEIGHT, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_NEAREST, 1);
    if (retflag) return retval;
    // color + specular color buffer
    retval = createTexture2D(retflag, gAlbedoSpec, SCR_WIDTH, SCR_HEIGHT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST, 2);
    if (retflag) return retval;
    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);
    // create and attach depth buffer (renderbuffer)
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // SECOND FBO
    glBindFramebuffer(GL_FRAMEBUFFER, finalFBO);
    // pre-post process texture
    retval = createTexture2D(retflag, prePostProcess, SCR_WIDTH, SCR_HEIGHT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST, 0);
    if (retflag) return retval;
    retval = createTexture2D(retflag, bloomTex, SCR_WIDTH, SCR_HEIGHT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST, 1);
    if (retflag) return retval;
    unsigned int attachments2[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments2);
    // create and attach depth buffer (renderbuffer)
    unsigned int rboDepth2;
    glGenRenderbuffers(1, &rboDepth2);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth2);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth2);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    /************************************************** SSAO */
    unsigned int ssaoFBO, ssaoBlurFBO;
    glGenFramebuffers(1, &ssaoFBO);  glGenFramebuffers(1, &ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    unsigned int ssaoColorBuffer, ssaoColorBufferBlur;
    // SSAO color buffer
    retval = createTexture2D(retflag, ssaoColorBuffer, SCR_WIDTH, SCR_HEIGHT, GL_RED, GL_RED, GL_FLOAT, GL_NEAREST, 0);
    if (retflag) return retval;
    unsigned int attachments3[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments3);

    // and blur stage
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    retval = createTexture2D(retflag, ssaoColorBufferBlur, SCR_WIDTH, SCR_HEIGHT, GL_RED, GL_RED, GL_FLOAT, GL_NEAREST, 0);
    if (retflag) return retval;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;

    // generate noise texture
    std::vector<glm::vec3> ssaoNoise;
    for (unsigned int i = 0; i < 16; i++){
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }
    unsigned int noiseTexture; glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // generate sample kernel
    std::vector<glm::vec3> ssaoKernel;
    for (unsigned int i = 0; i < 64; ++i)
    {   glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / 64.0;

        // scale samples s.t. they're more aligned to center of kernel
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }

    /*********************************************************/

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // render
        glClearColor(0.5f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // 1. geometry pass: render scene's geometry/color data into gbuffer
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        shaderGeometryPass.use();
        shaderGeometryPass.setMat4("projection", projection);
        shaderGeometryPass.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.05f));
        shaderGeometryPass.setMat4("model", model);
        modelToRender.draw(shaderGeometryPass,camera, model);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // ------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
        glClear(GL_COLOR_BUFFER_BIT);
        shaderSSAO.use();
        // Send kernel + rotation 
        for (unsigned int i = 0; i < 64; ++i)
            shaderSSAO.setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
        shaderSSAO.setMat4("projection", projection);
        shaderSSAO.setFloat("radius", ssaoRadius);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        renderScreenTriangle();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 3. blur SSAO texture to remove noise
        // ------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
        glClear(GL_COLOR_BUFFER_BIT);
        shaderSSAOBlur.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
        renderScreenTriangle();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        
        // 2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
 /*       glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, finalFBO); // write to default framebuffer
        // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
        // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
        // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
        glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);*/
        // 4. lighting pass: traditional deferred Blinn-Phong lighting with added screen-space ambient occlusion
       // -----------------------------------------------------------------------------------------------------
                // 2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
        // ----------------------------------------------------------------------------------
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, finalFBO); // write to default framebuffer
        // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
        // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
        // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
        glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, finalFBO);
        shaderLightingPass.use();
        // send light relevant uniforms
        glm::vec3 lightPosView = glm::vec3(camera.GetViewMatrix() * glm::vec4(glm::vec3(0.0,abs(sin(glfwGetTime())*5.0),0.0), 1.0));
        shaderLightingPass.setVec3("light.Position", lightPosView);
        shaderLightingPass.setVec3("light.Color", glm::vec3(1.0));
        // Update attenuation parameters
        const float linear = 0.09;
        const float quadratic = 0.032;
        shaderLightingPass.setFloat("light.Linear", linear);
        shaderLightingPass.setFloat("light.Quadratic", quadratic);
        shaderLightingPass.setBool("useSSAO", ssao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        glActiveTexture(GL_TEXTURE3); // add extra SSAO texture to lighting pass
        glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
        renderScreenTriangle();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, prePostProcess);
        finalShader.use();
        finalShader.setInt("currentEffect", currentEffect);
        finalShader.setFloat("u_time", glfwGetTime());
        renderScreenTriangle();


        ui.update();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
    bool retflag;
    int retval;
    // QUIT
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        requestMove = true;
    }
    if (requestMove && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
        requestMove = false;
        firstMouse = true;
        move = !move;
        if(move) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    }
    // CAMERA DEPLACEMENT
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        camera.ProcessKeyboard(DOWN, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
        camera.ProcessKeyboard(UP, deltaTime);
    }
    // SWAPBUFFER
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        requestInverseSwap = true;
    }
    if (requestInverseSwap && glfwGetKey(window, GLFW_KEY_P) != GLFW_PRESS) {
        requestInverseSwap = false;
        invertSwap(retflag);
    }
    // SSAO
    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) {
        requestSSAO = true;
    }
    if (requestSSAO && glfwGetKey(window, GLFW_KEY_F1) != GLFW_PRESS) {
        requestSSAO = false;
        ssao = !ssao;
    }
    // BLOOM
    if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) {
        requestBloom = true;
    }
    if (requestBloom && glfwGetKey(window, GLFW_KEY_F2) != GLFW_PRESS) {
        requestBloom = false;
        bloom = !bloom;
    }


    
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (move) {
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
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
