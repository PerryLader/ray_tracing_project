#include "Dependencies/include/common.h"
#include "Helpers/Renderer.h"
#include "Helpers/Utils.h"
#include "Helpers/Buffers.h"
#include "Helpers/camera.h"

struct Triangle ;
struct Ray;
struct NormTriangle;
GLFWwindow* window;

//--------------------- scene parameters---------------------
// Camera camera;
// float last_x , last_y ;
// bool first_mouse = true;

// float mouse_sensitivity = 0.3;
// float zoom_sensitivity = 0.5;
// float fov_sensitivity = 0.4;
// glm::mat4 model = glm::mat4(1.0f);
// glm::vec3 material_ambient = glm::vec3(0.8,0.8,0.8);
// glm::vec3 material_diffuse = glm::vec3(0.8,0.8,0.8);
// glm::vec3 material_specular = glm::vec3(0.8,0.8,0.8);
// float material_shininess =  32.0;
// glm::vec3 light_ambient = glm::vec3(0.4,0.4,0.4);
// glm::vec3 light_diffuse = glm::vec3(0.7,0.7,0.7);
// glm::vec3 light_specular = glm::vec3(0.9,0.9,0.9);
// glm::vec3 background_color = glm::vec3(0.2,0.7,0.3);

//-------------------compute parameteres----------------------
int window_width=800, window_height=640;
float maxDistRay=1100;
const size_t xRays=21;
const size_t yRays=32;
const size_t numOfRays=xRays*yRays;
int perBatch=128;
const size_t numOfSampels=100;
std::vector<Triangle> triangles;
std::vector<NormTriangle> normtriangles;
std::vector<Ray> rays;
int numOftriangles;
std::vector<glm::vec3> originsVector;





// void mouse_callback(GLFWwindow *window, double x_pos, double y_pos)
// {
//     if (first_mouse)
//     {
//         last_x = x_pos;
//         last_y = y_pos;
//         first_mouse = false;
//     }
//     float x_offset = x_pos - last_x;
//     float y_offset = last_y - y_pos; // reversed since y-coordinates go from bottom to top
//     last_x = x_pos;
//     last_y = y_pos;
//     if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
//     {
//         camera.orbit(x_offset, y_offset);
//     }
// }
// void scroll_callback(GLFWwindow *window, double x_offset, double y_offset)
// {
//     camera.zoom(y_offset);
// }
// void sceneValuesInit()
// {
//     glm::vec3 model_translate_vec = glm::vec3(0.0,-1.5,0.0);
//     glm::vec3 model_scale = glm::vec3(0.05 );
//     float model_rotate_angle = glm::radians(-90.0);
//     glm::vec3 model_rotate_axis = glm::vec3(1.0,0.0,0.0);
//     model = glm::translate(model, model_translate_vec);
//     model = glm::scale(model, model_scale);
//     model = glm::rotate(model, model_rotate_angle, model_rotate_axis);
// }

int init()
{

    if (!glfwInit()) 
    {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }
    // Set GLFW window hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required for Mac
    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(window_width, window_height, "OpenGL Window", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    // glfwSetCursorPosCallback(window, mouse_callback);
    // glfwSetScrollCallback(window, scroll_callback);   


    // camera.setScreenDimensions(window_width,window_height);
    // camera.setSensitivities(mouse_sensitivity, zoom_sensitivity, fov_sensitivity);

    // sceneValuesInit();
    // glfwSwapInterval(1);
    // Make the window's context current
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        glfwTerminate();
        return -1;
    }
    return 1;
   
}
void writeArraysToCSV(const std::string& filename, const std::array<std::array<float, numOfRays>,numOfSampels>& data) {
    std::ofstream outFile(filename);
    if (!outFile) {
        std::cerr << "Error opening file for writing" << std::endl;
        return;
    }

    for (const auto& array : data) {
        for (size_t i = 0; i < array.size(); ++i) {
            outFile << array[i];
            if (i < array.size() - 1) {
                outFile << ",";  // Add comma for all but the last element
            }
        }
        outFile << "\n";  // Newline after each array (row)
    }

    outFile.close();
}

void originVectorFiller()
{
    glm::vec3 temp(0.0,0.0,0.0);
    for(int i=0;i<numOfSampels;i++)
    {
        originsVector.push_back(temp);
        temp+=glm::vec3(1.0,1.0,1.0);

    }

}


int main() {
    
    if(-1==init())
    {
        return -1;
    }
    //compute parameters
    
    //numOfRays=xRays*yRays;
    std::array<std::array<float,(size_t)numOfRays>,(size_t)numOfSampels> distancePerRay;

    loadSTL("res/City1Block1.stl",triangles,normtriangles);
    rays=loadRays(xRays,yRays);
    numOftriangles=triangles.size();
    originVectorFiller();



    GLuint triangleBuffer,rayBuffer, resultBuffer;
    std::string computeShaderSource=readFileIntoCharArray("res/computeshader.glsl");   
    GLuint computeShader = compileShader(GL_COMPUTE_SHADER, computeShaderSource.c_str());
    GLuint Computeprogram = createComputeProgram(computeShader);
    setupComputeBuffers(triangles,rays, triangleBuffer,rayBuffer, resultBuffer, maxDistRay);  
    
    for(int i=0;i<numOfSampels;i++)
    {
        auto start = std::chrono::high_resolution_clock::now();

        performRaycasting(Computeprogram, originsVector[i], numOfRays, numOftriangles,maxDistRay,perBatch ); 
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, resultBuffer);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, numOfRays*sizeof(float),&distancePerRay[i]);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "Compute shader execution time["<<i<<"]: " << elapsed.count() << " seconds" << std::endl;
    }
    auto start = std::chrono::high_resolution_clock::now();
    writeArraysToCSV("data.csv", distancePerRay);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "writing to file execution time: " << elapsed.count() << " seconds" << std::endl;
    std::cout<<"Rays:"<<numOfRays<<std::endl;
    std::cout<<"triangles:"<<numOftriangles<<std::endl;
    

    // for(int i=0;i<numOfRays;i++)
    // {
    //     std::cout<<"distance of ray["<<i<<"]:"<<distancePerRay[i]<<std::endl;
    //     //std::cout<<"cell["<<i<<"]:"<<vertices[i]<<std::endl;

    // }
    //------------------  scene  ------------------------------------------------

//     std::string scenevertexShaderSource=readFileIntoCharArray("res/scenevertexshader.glsl");
//     std::string  scenefragmentShaderSource=readFileIntoCharArray("res/scenefragmentshader.glsl");
//     GLuint shaderSceneProgram = createSceneShaderProgram(scenevertexShaderSource.c_str(), scenefragmentShaderSource.c_str());
//     GLuint SceneVAO, SceneVBO;
//     glUseProgram(shaderSceneProgram);
//     setupSceneBuffers(normtriangles, SceneVAO, SceneVBO);
    
//     setVec3("material.ambient", material_ambient,shaderSceneProgram);
//     setVec3("material.diffuse", material_diffuse,shaderSceneProgram);
//     setVec3("material.specular", material_specular,shaderSceneProgram);    
//     setFloat("material.shininess", material_shininess,shaderSceneProgram);
//     setVec3("light.ambient", light_ambient,shaderSceneProgram);
//     setVec3("light.diffuse", light_diffuse,shaderSceneProgram);
//     setVec3("light.specular", light_specular,shaderSceneProgram);
// // //----------------------------rays----------------------------


//     std::vector<float> vertices;
//     std::vector<float> colors;
//     int i=0;
//     for (const Ray& ray : rays) {
        
//         glm::vec3 end = originsVector[0] + glm::vec3(ray.dir.x,ray.dir.y,ray.dir.z) * distancePerRay[0][i];
//         vertices.push_back(originsVector[0].x);
//         vertices.push_back(originsVector[0].y);
//         vertices.push_back(originsVector[0].z);
//         vertices.push_back(end.x);
//         vertices.push_back(end.y);
//         vertices.push_back(end.z);
        
//         glm::vec3 color = (distancePerRay[0][i] == maxDistRay) ? glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(1.0f, 0.0f, 0.0f);
//         colors.push_back(color.r);
//         colors.push_back(color.g);
//         colors.push_back(color.b);
//         colors.push_back(color.r);
//         colors.push_back(color.g);
//         colors.push_back(color.b);
//         i++;
//     }
//     std::string rayvertexShaderSource=readFileIntoCharArray("res/rayvertexshader.glsl");
//     std::string  rayfragmentShaderSource=readFileIntoCharArray("res/rayfragmentshader.glsl");
//     GLuint shaderRayProgram = createRayShaderProgram(rayvertexShaderSource.c_str(), rayfragmentShaderSource.c_str());
//     GLuint rayVAO;
//     GLuint rayVBO1;
//     GLuint rayVBO2;

    


//     while (!glfwWindowShouldClose(window)) {
//         calculateFPS(window);
        
//         glClearColor(background_color.r, background_color.g,background_color.b, 1.0f);
//         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//         // Use shader program
       

//         // Set up model, view, and projection matrices here   
        
//         glUseProgram(shaderSceneProgram);
//         setupSceneBuffers(normtriangles, SceneVAO, SceneVBO);
//         setMat4("model", model,shaderSceneProgram);
//         setMat4("view", camera.getViewMatrix(),shaderSceneProgram);
//         setMat4("projection", camera.getProjMatrix(),shaderSceneProgram);
//         setVec3("light.position", camera.getPosition(),shaderSceneProgram);
//         setVec3("view_pos", camera.getPosition(),shaderSceneProgram);
//         // Draw the mesh
//         glBindVertexArray(SceneVAO);
//         glDrawArrays(GL_TRIANGLES, 0, triangles.size() * 4);
//        // glBindVertexArray(0);
//         //glBindBuffer(GL_ARRAY_BUFFER, 0); 
//     //----------------------- rays draw ----------------------


//         glUseProgram(shaderRayProgram);
//         setupRaysBuffers(vertices,colors, rayVAO, rayVBO1,rayVBO2);

//         setMat4("model", model,shaderRayProgram);
//         setMat4("view", camera.getViewMatrix(),shaderRayProgram);
//         setMat4("projection", camera.getProjMatrix(),shaderRayProgram);
//         glBindVertexArray(rayVAO);
//         glDrawArrays(GL_LINES, 0, vertices.size() /3);
//         //glBindVertexArray(0);
//         //glBindBuffer(GL_ARRAY_BUFFER, 0); 




//         // Swap buffers
//         glfwSwapBuffers(window);
//         glfwPollEvents();
//     }

    
    // //Cleanup
    glDeleteBuffers(1, &triangleBuffer);
    glDeleteBuffers(1, &resultBuffer);
    glDeleteShader(computeShader);
    glDeleteProgram(Computeprogram);
    // glDeleteVertexArrays(1, &SceneVAO);
    // glDeleteBuffers(1, &SceneVBO);
    // glDeleteBuffers(1, &rayVBO1);
    // glDeleteBuffers(1, &rayVBO2);
    // glDeleteVertexArrays(1, &rayVAO);
    // glDeleteProgram(shaderSceneProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}