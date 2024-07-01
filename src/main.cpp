#include "main.h"

/* Terrain LOD
* The basic framework is adapted from the one provided in the assignments from the course, especially the shader loading class.
* Code was refactored to adhere to Cpp Core Guidelines available at https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md/
* Create by kb on 2022-05-05
*/

// CLASSES, STRUCTS, ENUMS
// ----------------------------------------------------------------------------

// FUNCTIONS 
// ----------------------------------------------------------------------------
// buffer object functions
unsigned int create_array_buffer(const std::vector<float>& array);
unsigned int create_element_array_buffer(const std::vector<unsigned int>& array);
unsigned int create_vertex_array(const std::vector<float>& positions, const std::vector<float>& colors, const std::vector<unsigned int>& indices);

// scene object functions
void setup();
void gen_terrain();

// matrix functions
glm::mat4 get_view_projection_matrix();

// glfw and input functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void cursor_input_callback(GLFWwindow* window, double pos_x, double pos_y);
void button_input_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_input_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_input_callback(GLFWwindow* window, int button, int other, int action, int mods);

void process_input(GLFWwindow* window);

// gui functions
void draw_gui();

// ----------------------------------------------------------------------------

// CONSTANTS AND VARIABLES
// ----------------------------------------------------------------------------
// screen settings
unsigned int scr_width = 1000;
unsigned int scr_height = 1000;
float fov = 80.0f;

// terrain settings
unsigned int tex_w = 8192, tex_h = 8192, patch_res = 128;

// shaders
Shader* terrain_shader;
ComputeShader* generator_shader;

// camera
Camera* camera;

// scene objects
Terrain* terrain;
std::vector<SceneObject*> objects;

NoiseSettings* noise;

// global control variables
bool pause = true, toggle_wireframe = false;
float last_frame = 0.0f, delta_time = 0.0f;
glm::vec3 click_start(0.0f), click_end(0.0f);
// ----------------------------------------------------------------------------

int main()
{
    // standard setup as per class exercises

    // glfw window creation and setup
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
    #endif

    GLFWwindow* window = glfwCreateWindow(scr_width, scr_height, "terrain_lod", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_input_callback);
    glfwSetMouseButtonCallback(window, button_input_callback);
    glfwSetScrollCallback(window, scroll_input_callback);
    glfwSetKeyCallback(window, key_input_callback);

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // load glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // init imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430 core");

    // z-buffer
    glDepthRange(-1, 1);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // prepare scene
    camera = new Camera(glm::vec3(0.0f, 11.0f, 0.0f));
    setup();

    while (!glfwWindowShouldClose(window))
    {
        float current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;
        process_input(window);
        glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if(toggle_wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe mode
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        if (terrain) {
            terrain->set_uniforms(camera, get_view_projection_matrix());
            terrain->draw();
        }
        /*for (auto obj : objects) {
            obj->draw();
        }*/

        if (pause)
            draw_gui();
        glfwSwapBuffers(window);
        glfwPollEvents(); 
    }

    delete terrain;
    delete terrain_shader;
    delete generator_shader;
    delete noise;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}


void draw_gui() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    {
        ImGui::Begin("Terrain LOD");
        ImGui::Text("Keyboard shortcuts:");
        ImGui::Text("    WASD for movement");
        ImGui::Text("    SPACE for pause/unpause");
        ImGui::Separator();

        ImGui::Text("Height settings: ");
        ImGui::SliderFloat("Height scale", (float*)&terrain->height_scale, 1.0f, 256.0f);
        ImGui::SliderFloat("Height shift", (float*)&terrain->height_shift, 1.0f, 256.0f);

        ImGui::Text("Generation settings: ");
        ImGui::InputInt("Width", (int*)&tex_w, 16, 128);
        ImGui::InputInt("Height", (int*)&tex_h, 16, 128);
        ImGui::InputInt("Patch resolution", (int*)&patch_res, 1, 5);
        ImGui::DragFloat3("Offset", (float*)&noise->offset, .1f, -10, 10);
        ImGui::SliderFloat("Frequency", (float*)&noise->frequency, 0.0001f, 0.005f, "%.4f");
        ImGui::DragInt("Octaves", (int*)&noise->octaves, 1.0f, 1.0f, 16.0f);
        ImGui::SliderFloat("Amplitude", (float*)&noise->amplitude, 1.0f, 10.0f);
        ImGui::SliderFloat("Lacunarity", (float*)&noise->lacunarity, 0.1f, 4.0f);
        ImGui::SliderFloat("Gain", (float*)&noise->gain, 0.1f, 2.0f);
        ImGui::SliderFloat("Range", (float*)&noise->range, 0.1f, 10.0f);
        if (ImGui::Button("Regenerate terrain"))
            gen_terrain();
        ImGui::Separator();

        ImGui::Text("Tessellation settings: ");
        ImGui::InputInt("Min tessellation level", (int*)&terrain->min_tess_level, 1, 5);
        ImGui::InputInt("Max tessellation level", (int*)&terrain->max_tess_level, 1, 5);
        ImGui::InputFloat("Min distance", (float*)&terrain->min_distance, 1.0f, 10.0f);
        ImGui::InputFloat("Max distance", (float*)&terrain->max_distance, 1.0f, 10.0f);
        ImGui::Separator();

        ImGui::Text("Visualization: ");
        ImGui::Checkbox("Toggle wireframe", &toggle_wireframe);
      
        ImGui::Separator();

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Separator();

        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void setup() {
    // initialize shaders
    terrain_shader = new Shader("shaders/shader.vert", "shaders/shader.frag", nullptr, "shaders/terrain_lod.tesc", "shaders/terrain_lod.tese");
    generator_shader = new ComputeShader("shaders/terrain_gen.comp");
    noise = new NoiseSettings(glm::vec3(0), 0.0025f, 8, 4.0f, 2.0f, 0.575f, 0.65f);
    gen_terrain();
}

void gen_terrain() {
    if (terrain != nullptr)
        delete terrain;
    terrain = new Terrain(tex_w, tex_h, patch_res, terrain_shader, generator_shader, *noise);
}

glm::mat4 get_view_projection_matrix() {
        auto eye = glm::vec3(0, 0, 1);
        auto fwd = glm::vec3(0, 0, -1);
        auto ctr = eye + fwd;

        glm::mat4 view = camera->get_view_matrix();
        glm::mat4 projection = glm::perspectiveFov(glm::radians(fov), (float)scr_width, (float)scr_height, 0.01f, 4000.f);

        return projection * view;
}

unsigned int create_vertex_array(const std::vector<float>& positions, const std::vector<float>& colors, const std::vector<unsigned int>& indices) {
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    // bind vertex array object
    glBindVertexArray(VAO);

    // set vertex shader attribute "pos"
    create_array_buffer(positions); // creates and bind the VBO
    int posAttributeLocation = glGetAttribLocation(terrain_shader->ID, "pos");
    glEnableVertexAttribArray(posAttributeLocation);
    glVertexAttribPointer(posAttributeLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // set vertex shader attribute "color"
    create_array_buffer(colors); // creates and bind the VBO
    int colorAttributeLocation = glGetAttribLocation(terrain_shader->ID, "color");
    glEnableVertexAttribArray(colorAttributeLocation);
    glVertexAttribPointer(colorAttributeLocation, 4, GL_FLOAT, GL_FALSE, 0, 0);

    // creates and bind the EBO
    create_element_array_buffer(indices);

    return VAO;
}


unsigned int create_array_buffer(const std::vector<float>& array) {
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, array.size() * sizeof(GLfloat), &array[0], GL_STATIC_DRAW);

    return VBO;
}


unsigned int create_element_array_buffer(const std::vector<unsigned int>& array) {
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, array.size() * sizeof(unsigned int), &array[0], GL_STATIC_DRAW);

    return EBO;
}

void cursor_input_callback(GLFWwindow* window, double pos_x, double pos_y) {
    static auto last_coord = glm::vec2(pos_x, pos_y);
    auto offset = glm::vec2(pos_x - last_coord.x, last_coord.y - pos_y);
    last_coord = glm::vec2(pos_x, pos_y);
    if (pause)
        return;
    camera->process_cursor_input(offset.x, offset.y);
}

void button_input_callback(GLFWwindow* window, int button, int action, int mods) {
}

void scroll_input_callback(GLFWwindow* window, double xoffset, double yoffset) {
 
    //if (camera->position.z >= 2.0f)
    //    camera->position.z -= yoffset/10;
    //if (camera->position.z < 2.0f)
    //    camera->position.z = 2.0f;
}

void key_input_callback(GLFWwindow* window, int button, int other, int action, int mods) {
    if (button == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (button == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        pause = !pause;
        glfwSetInputMode(window, GLFW_CURSOR, pause ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }
}

void process_input(GLFWwindow* window) {
    if (pause)
        return;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera->process_key_input(Camera::FORWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera->process_key_input(Camera::BACKWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera->process_key_input(Camera::LEFT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera->process_key_input(Camera::RIGHT, delta_time);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    scr_width = width;
    scr_height = height;
}