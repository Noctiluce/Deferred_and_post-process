#include "Helper.hpp"



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
int initWindow(GLFWwindow*& window, bool& retflag);
int getCurrentWorkingDirectory(std::string& currentPath, bool& retflag);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// swap
bool requestInverseSwap = false;
bool swapInterval = true;

// freeze
bool requestMove = false;
bool move = true;
 
// Imgui 
bool requestSSAO = false;
bool ssao = true;
float ssaoRadius = 0.5;
int currentEffect = 0;


// Point
float seuil = 10;
float transition = 3.0f;
float ambient = 0.3f;


int invertSwap(bool& retflag) {
    retflag = true;
    swapInterval = !swapInterval;
    glfwSwapInterval(swapInterval);
    retflag = false;
    return {};
}


void setCallbacks() {
    glfwSetFramebufferSizeCallback(glfwGetCurrentContext(), framebuffer_size_callback);
    glfwSetCursorPosCallback(glfwGetCurrentContext(), mouse_callback);
    glfwSetScrollCallback(glfwGetCurrentContext(), scroll_callback);
}

int main()
{
    // Local variables
    bool retflag;
    int retval;
    GLFWwindow* window;
    std::string currentPath;


    // Current Working Directory 
    {
        retval = getCurrentWorkingDirectory(currentPath, retflag); if (retflag) return retval;
        std::cout << "Current working directory is " << currentPath << std::endl;
    }
    // Initialize Window
    {
        retval = initWindow(window, SCR_WIDTH, SCR_HEIGHT, retflag); if (retflag) return retval;
        setIcon(currentPath + "assets/textures/bunny.png");
        setCallbacks();
    }

    // Configure global opengl state
    {
        glEnable(GL_DEPTH_TEST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    // build and compile our shader programs
    Shader shaderGeometryPass((currentPath + "assets/shaders/ssao_geometry.vs.glsl").c_str(), (currentPath + "assets/shaders/ssao_geometry.fs.glsl").c_str());
    Shader shaderLightingPass((currentPath + "assets/shaders/ssao.vs.glsl").c_str(), (currentPath + "assets/shaders/ssao_lighting.fs.glsl").c_str());
    Shader shaderSSAO((currentPath + "assets/shaders/ssao.vs.glsl").c_str(), (currentPath + "assets/shaders/ssao.fs.glsl").c_str());
    Shader shaderSSAOBlur((currentPath + "assets/shaders/ssao.vs.glsl").c_str(), (currentPath + "assets/shaders/ssao_blur.fs.glsl").c_str());
    Shader finalShader((currentPath + "assets/shaders/final.vs.glsl").c_str(), (currentPath + "assets/shaders/final.fs.glsl").c_str());

    // shaders configuration
    {
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
    }

    // Load Models
    Model modelToRender = Model(currentPath + "assets/models/sponza", "sponza.dae");


    // User Interface
    UserInterface ui = UserInterface();
    {
        ui.init();
        ui.SCR_HEIGHT = SCR_HEIGHT;
        ui.SCR_WIDTH = SCR_WIDTH;
        ui._ssao = &ssao;
        ui._ssaoRadius = &ssaoRadius;
        ui._currentEffect = &currentEffect;
        ui._ambientStrength = &ambient;
        retval = invertSwap(retflag); if (retflag) return retval;
    }

    // buffers
    unsigned int decompositionFBO, recompositionFBO, ssaoFBO, ssaoBlurFBO;
    glGenFramebuffers(1, &decompositionFBO); glGenFramebuffers(1, &recompositionFBO);
    glGenFramebuffers(1, &ssaoFBO);  glGenFramebuffers(1, &ssaoBlurFBO);
    
    // Textures
    
    unsigned int TexPosition, TexNormal, TexAlbedoSpec, prePostProcess;
    unsigned int ssaoColorBuffer, ssaoColorBufferBlur;
    unsigned int noiseTexture;

    unsigned int rboDepth;
    
    {
        {
            // DECOMPOSITION FBO 
            glBindFramebuffer(GL_FRAMEBUFFER, decompositionFBO);
            retval = createTexture2D(retflag, TexPosition, SCR_WIDTH, SCR_HEIGHT, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_NEAREST, 0); if (retflag) return retval;
            retval = createTexture2D(retflag, TexNormal, SCR_WIDTH, SCR_HEIGHT, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_NEAREST, 1); if (retflag) return retval;
            retval = createTexture2D(retflag, TexAlbedoSpec, SCR_WIDTH, SCR_HEIGHT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST, 2); if (retflag) return retval;

            unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
            glDrawBuffers(3, attachments);
            
            glGenRenderbuffers(1, &rboDepth);
            glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) std::cout << "Framebuffer not complete!" << std::endl;
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        
        {
            // RECOMPOSITION FBO
            glBindFramebuffer(GL_FRAMEBUFFER, recompositionFBO);
            retval = createTexture2D(retflag, prePostProcess, SCR_WIDTH, SCR_HEIGHT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST, 0); if (retflag) return retval;

            unsigned int attachments2[1] = { GL_COLOR_ATTACHMENT0 };
            glDrawBuffers(1, attachments2);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

        }

        // SSAO based on https://learnopengl.com/Advanced-Lighting/SSAO
        {
            // SSAO FBO
            glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
            retval = createTexture2D(retflag, ssaoColorBuffer, SCR_WIDTH, SCR_HEIGHT, GL_RED, GL_RED, GL_FLOAT, GL_NEAREST, 0); if (retflag) return retval;
            
            unsigned int attachments3[1] = { GL_COLOR_ATTACHMENT0 }; glDrawBuffers(1, attachments3);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        {
            // SSAO BLUR FBO
            glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
            retval = createTexture2D(retflag, ssaoColorBufferBlur, SCR_WIDTH, SCR_HEIGHT, GL_RED, GL_RED, GL_FLOAT, GL_NEAREST, 0); if (retflag) return retval;
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }

    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    std::vector<glm::vec3> ssaoNoise;
    std::vector<glm::vec3> ssaoKernel;

    {
        for (unsigned int i = 0; i < 16; i++) {
            glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
            ssaoNoise.push_back(noise);
        }
        glGenTextures(1, &noiseTexture);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    {
        // generate sample kernel
        for (unsigned int i = 0; i < 64; ++i)
        {
            glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
            sample = glm::normalize(sample);
            sample *= randomFloats(generator);
            float scale = float(i) / 64.0;

            // scale samples s.t. they're more aligned to center of kernel
            scale = lerp(0.1f, 1.0f, scale * scale);
            sample *= scale;
            ssaoKernel.push_back(sample);
        }
    }


    // generate Lights
    int nr_lights = 32;
    std::vector<glm::vec4> lights_positions;
    float y = 3.0;
    float x = -60.0;

    for (int i = 0; i < nr_lights; i++) {
        x += 15;
        if (x > 60) {
            x = -60.0;
            y += 15;
        }
        lights_positions.emplace_back(glm::vec4(x, y , 0.0, 1.0));
    }

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // input
        processInput(window);

        // render
        glClearColor(0.2f, 0.2f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1. geometry pass
        // write in TexPosition, TexNormal, TexAlbedoSpec
        {
            glBindFramebuffer(GL_FRAMEBUFFER, decompositionFBO);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glm::mat4 model = glm::mat4(1.0f);
            shaderGeometryPass.use();
            shaderGeometryPass.setMat4("projection", projection);
            shaderGeometryPass.setMat4("view", view);
            model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(0.05f));
            shaderGeometryPass.setMat4("model", model);
            modelToRender.draw(shaderGeometryPass, camera, model);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        // 2. compute ssao
        // write in ssaoColorBuffer
        {   glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
            glClear(GL_COLOR_BUFFER_BIT);
            shaderSSAO.use();
            for (unsigned int i = 0; i < 64; ++i)
                shaderSSAO.setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
            shaderSSAO.setMat4("projection", projection);
            shaderSSAO.setFloat("radius", ssaoRadius);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, TexPosition);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, TexNormal);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, noiseTexture);
            renderScreenTriangle();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        
        // 3. remove noise from ssaoColorBuffer
        // write in ssaoColorBufferBlur
        {
            glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
            glClear(GL_COLOR_BUFFER_BIT);
            shaderSSAOBlur.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
            renderScreenTriangle();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        // 4. recompose image
        // write in prePostProcess
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glBindFramebuffer(GL_FRAMEBUFFER, recompositionFBO);
            shaderLightingPass.use();
            // send light relevant uniforms
            // Update attenuation parameters
            const float linear = 0.09;
            const float quadratic = 0.032;
            for (int i = 0; i < nr_lights; i++) {
                glm::vec3 lightPosView = glm::vec3(camera.GetViewMatrix() * lights_positions[i]);
                shaderLightingPass.setVec3("u_lights[" + std::to_string(i) + "].Position", lightPosView);
                shaderLightingPass.setVec3("u_lights[" + std::to_string(i) + "].Color", glm::vec3(1.0));
                shaderLightingPass.setFloat("u_lights[" + std::to_string(i) + "].Linear", linear);
                shaderLightingPass.setFloat("u_lights[" + std::to_string(i) + "].Quadratic", quadratic);
                shaderLightingPass.setFloat("u_lights[" + std::to_string(i) + "].seuil", seuil);
                shaderLightingPass.setFloat("u_lights[" + std::to_string(i) + "].transition", transition);
            }
            shaderLightingPass.setFloat("u_ambient", ambient);
            shaderLightingPass.setBool("u_useSSAO", ssao);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, TexPosition);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, TexNormal);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, TexAlbedoSpec);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
            renderScreenTriangle();
        }

        // 5. Post process treatment 
        // write in viewport
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, prePostProcess);
            finalShader.use();
            finalShader.setInt("currentEffect", currentEffect);
            finalShader.setFloat("u_time", glfwGetTime());
            renderScreenTriangle();
        }

        ui.update();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}



// ***************************** CALLBACKS ***************************** //

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
    // seuil
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        seuil += 0.1;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        seuil -= 0.1;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        transition += 0.01;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        transition -= 0.01;
        if (transition < 0.0) transition = 0.0;
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
        float yoffset = lastY - ypos;

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
