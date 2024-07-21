#include "Utils.h"


std::vector<Ray> loadRays(int xRays,int yRays)
{
        int azimuthSteps = xRays; 
        int elevationSteps = yRays; 
        std::vector<Ray> rays;
        double az_degrees ;
        double el_degrees ;
        
        int index = 0;
        for (int az = 0; az < azimuthSteps; az++)
        {
            for (int el = 0; el < elevationSteps; el++)
            {
               az_degrees = (az*360)/azimuthSteps;
               el_degrees = (el*180)/(elevationSteps-1);

                float azimuth = az_degrees * M_PI / 180.0;
                float elevation = el_degrees * M_PI / 180.0;
                
                rays.push_back( {{std::sin(elevation) * std::cos(azimuth),
                                  std::cos(elevation),
                                  std::sin(elevation) * std::sin(azimuth),
                                  0}});
                //rays.push_back({ { 0,1,0,0 } });
                if(index<10)
                std::cout << "ray Vec3(" << std::sin(elevation) * std::cos(azimuth) << ", " << std::cos(elevation) << ", " << std::sin(elevation) * std::sin(azimuth) << ")" << std::endl;

                index++;
            }
        }
    return rays;
}

glm::vec4 computeNormal(const glm::vec4& v0, const glm::vec4& v1, const glm::vec4& v2) {
    glm::vec3 u = glm::vec3(v1) - glm::vec3(v0);
    glm::vec3 v = glm::vec3(v2) - glm::vec3(v0);

    glm::vec3 normal = glm::normalize(glm::cross(u, v));
    return glm::vec4(normal, 0.0f);
}

std::vector<Triangle> readObjFile(const std::string& filename) {
    std::vector<glm::vec4> vertices;
    std::vector<Triangle> triangles;

    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            glm::vec4 vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            vertex.w = 1.0f;  // Set w to 1 for position vectors
            vertices.push_back(vertex);
        }
        else if (prefix == "f") {
            std::string vertex1, vertex2, vertex3;
            iss >> vertex1 >> vertex2 >> vertex3;

            auto getVertexIndex = [](const std::string& vertex) {
                std::istringstream vertexStream(vertex);
                std::string index;
                std::getline(vertexStream, index, '/');
                return std::stoi(index) - 1;  // Convert to 0-based index
                };

            int idx0 = getVertexIndex(vertex1);
            int idx1 = getVertexIndex(vertex2);
            int idx2 = getVertexIndex(vertex3);

            Triangle triangle;
            triangle.v0 = vertices[idx0];
            triangle.v1 = vertices[idx1];
            triangle.v2 = vertices[idx2];
            triangle.norm = computeNormal(triangle.v0, triangle.v1, triangle.v2);
            Triangle demi_triangle;
            demi_triangle.v0 = { 11,15,11,0 };
            demi_triangle.v1 = {-11,15,11,0 };
            demi_triangle.v2 = { 0, 15, -11,0 };
            demi_triangle.norm = { 0,-1,0,0 };
            triangles.push_back(triangle);
        }
    }

    file.close();
    return triangles;
}


std::string readFileIntoCharArray(const char* filename) {
    
    std::ifstream t(filename);
    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    return str.c_str();
}



void originVectorFiller(std::vector<glm::vec4>& originsVector,int numOfSampels)
{
    glm::vec4 temp(0.0, 0.0, 0.0,0);
    for (int i = 0; i < numOfSampels; i++)
    {
        originsVector.push_back(temp);
        temp += glm::vec4(5.0, 5.0, 5.0,0);

    }
}

std::vector<glm::vec4> readRays(const std::string& filename) {
    std::vector<Ray> rays;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Unable to open file " << filename << std::endl;
        return rays;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Remove the parentheses
        line.erase(std::remove(line.begin(), line.end(), '('), line.end());
        line.erase(std::remove(line.begin(), line.end(), ')'), line.end());

        // Split the line by commas
        std::istringstream iss(line);
        std::string x, y, z;
        if (std::getline(iss, x, ',') && std::getline(iss, y, ',') && std::getline(iss, z, ',')) {
            int ix = std::stoi(x);
            int iy = std::stoi(y);
            int iz = std::stoi(z);
            rays.emplace_back(ix, iy, iz);
        }
    }

    file.close();
    return rays;
}




