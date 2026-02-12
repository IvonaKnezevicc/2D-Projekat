#include "../Header/Util.h"

#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <glm/glm.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "../Header/stb_image.h"

// Autor: Nedeljko Tesanovic
// Opis: pomocne funkcije za zaustavljanje programa, ucitavanje sejdera, tekstura i kursora
// Smeju se koristiti tokom izrade projekta

int endProgram(std::string message) {
    std::cout << message << std::endl;
    glfwTerminate();
    return -1;
}

unsigned int compileShader(GLenum type, const char* source)
{
    //Uzima kod u fajlu na putanji "source", kompajlira ga i vraca sejder tipa "type"
    //Citanje izvornog koda iz fajla
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Uspjesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
    }
    std::string temp = ss.str();
    const char* sourceCode = temp.c_str(); //Izvorni kod sejdera koji citamo iz fajla na putanji "source"

    int shader = glCreateShader(type); //Napravimo prazan sejder odredjenog tipa (vertex ili fragment)

    int success; //Da li je kompajliranje bilo uspjesno (1 - da)
    char infoLog[512]; //Poruka o gresci (Objasnjava sta je puklo unutar sejdera)
    glShaderSource(shader, 1, &sourceCode, NULL); //Postavi izvorni kod sejdera
    glCompileShader(shader); //Kompajliraj sejder

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success); //Provjeri da li je sejder uspjesno kompajliran
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog); //Pribavi poruku o gresci
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" sejder ima gresku! Greska: \n");
        printf(infoLog);
    }
    return shader;
}
unsigned int createShader(const char* vsSource, const char* fsSource)
{
    //Pravi objedinjeni sejder program koji se sastoji od Vertex sejdera ciji je kod na putanji vsSource

    unsigned int program; //Objedinjeni sejder
    unsigned int vertexShader; //Verteks sejder (za prostorne podatke)
    unsigned int fragmentShader; //Fragment sejder (za boje, teksture itd)

    program = glCreateProgram(); //Napravi prazan objedinjeni sejder program

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource); //Napravi i kompajliraj vertex sejder
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource); //Napravi i kompajliraj fragment sejder

    //Zakaci verteks i fragment sejdere za objedinjeni program
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program); //Povezi ih u jedan objedinjeni sejder program
    glValidateProgram(program); //Izvrsi provjeru novopecenog programa

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success); //Slicno kao za sejdere
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }

    //Posto su kodovi sejdera u objedinjenom sejderu, oni pojedinacni programi nam ne trebaju, pa ih brisemo zarad ustede na memoriji
    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}

unsigned loadImageToTexture(const char* filePath) {
    int TextureWidth;
    int TextureHeight;
    int TextureChannels;
    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
    if (ImageData != NULL)
    {
        //Slike se osnovno ucitavaju naopako pa se moraju ispraviti da budu uspravne
        stbi__vertical_flip(ImageData, TextureWidth, TextureHeight, TextureChannels);

        // Provjerava koji je format boja ucitane slike
        GLint InternalFormat = -1;
        switch (TextureChannels) {
        case 1: InternalFormat = GL_RED; break;
        case 2: InternalFormat = GL_RG; break;
        case 3: InternalFormat = GL_RGB; break;
        case 4: InternalFormat = GL_RGBA; break;
        default: InternalFormat = GL_RGB; break;
        }

        unsigned int Texture;
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, TextureWidth, TextureHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, ImageData);
        glBindTexture(GL_TEXTURE_2D, 0);
        // oslobadjanje memorije zauzete sa stbi_load posto vise nije potrebna
        stbi_image_free(ImageData);
        return Texture;
    }
    else
    {
        std::cout << "Textura nije ucitana! Putanja texture: " << filePath << std::endl;
        stbi_image_free(ImageData);
        return 0;
    }
}

GLFWcursor* createCameraCursor() {
    const int width = 48;
    const int height = 48;
    unsigned char pixels[width * height * 4];
    
    for (int i = 0; i < width * height * 4; i += 4) {
        pixels[i] = 0;
        pixels[i + 1] = 0;
        pixels[i + 2] = 0;
        pixels[i + 3] = 0;
    }
    
    int centerX = width / 2;
    int centerY = height / 2;
    
    int reelRadius1 = 6;
    int reelX1 = centerX - 8;
    int reelY1 = 8;
    for (int y = reelY1 - reelRadius1; y <= reelY1 + reelRadius1; y++) {
        for (int x = reelX1 - reelRadius1; x <= reelX1 + reelRadius1; x++) {
            int dx = x - reelX1;
            int dy = y - reelY1;
            float dist = sqrt(dx * dx + dy * dy);
            if (dist <= reelRadius1 && dist >= reelRadius1 - 2.5f) {
                if (x >= 0 && x < width && y >= 0 && y < height) {
                    int idx = (y * width + x) * 4;
                    pixels[idx] = 0;
                    pixels[idx + 1] = 0;
                    pixels[idx + 2] = 0;
                    pixels[idx + 3] = 255;
                }
            }
        }
    }
    
    int reelRadius2 = 7;
    int reelX2 = centerX + 8;
    int reelY2 = 8;
    for (int y = reelY2 - reelRadius2; y <= reelY2 + reelRadius2; y++) {
        for (int x = reelX2 - reelRadius2; x <= reelX2 + reelRadius2; x++) {
            int dx = x - reelX2;
            int dy = y - reelY2;
            float dist = sqrt(dx * dx + dy * dy);
            if (dist <= reelRadius2 && dist >= reelRadius2 - 2.5f) {
                if (x >= 0 && x < width && y >= 0 && y < height) {
                    int idx = (y * width + x) * 4;
                    pixels[idx] = 0;
                    pixels[idx + 1] = 0;
                    pixels[idx + 2] = 0;
                    pixels[idx + 3] = 255;
                }
            }
        }
    }
    
    for (int y = reelY1; y <= reelY1 + 5; y++) {
        for (int x = reelX1 + 3; x <= reelX2 - 3; x++) {
            if (x >= 0 && x < width && y >= 0 && y < height) {
                int idx = (y * width + x) * 4;
                pixels[idx] = 0;
                pixels[idx + 1] = 0;
                pixels[idx + 2] = 0;
                pixels[idx + 3] = 255;
            }
        }
    }
    
    for (int y = centerY - 7; y < centerY + 7; y++) {
        for (int x = centerX - 11; x < centerX + 11; x++) {
            if (x >= 0 && x < width && y >= 0 && y < height) {
                int idx = (y * width + x) * 4;
                bool isEdge = (x == centerX - 11 || x == centerX + 10 || y == centerY - 7 || y == centerY + 6);
                bool isTopCorner = (y == centerY - 7 && (x >= centerX - 11 && x <= centerX + 10));
                bool isBottomCorner = (y == centerY + 6 && (x >= centerX - 11 && x <= centerX + 10));
                bool isLeftCorner = (x == centerX - 11 && (y >= centerY - 7 && y <= centerY + 6));
                bool isRightCorner = (x == centerX + 10 && (y >= centerY - 7 && y <= centerY + 6));
                
                if (isEdge || isTopCorner || isBottomCorner || isLeftCorner || isRightCorner) {
                    pixels[idx] = 0;
                    pixels[idx + 1] = 0;
                    pixels[idx + 2] = 0;
                    pixels[idx + 3] = 255;
                }
            }
        }
    }
    
    for (int y = centerY - 4; y < centerY + 4; y++) {
        for (int x = centerX - 6; x < centerX + 6; x++) {
            if (x >= 0 && x < width && y >= 0 && y < height) {
                int idx = (y * width + x) * 4;
                if (x == centerX - 6 || x == centerX + 5 || y == centerY - 4 || y == centerY + 3) {
                    pixels[idx] = 0;
                    pixels[idx + 1] = 0;
                    pixels[idx + 2] = 0;
                    pixels[idx + 3] = 255;
                }
            }
        }
    }
    
    int lensStartX = centerX + 11;
    int lensStartY = centerY;
    int lensWidth = 10;
    int lensHeight = 16;
    
    for (int y = lensStartY - lensHeight/2; y <= lensStartY + lensHeight/2; y++) {
        for (int x = lensStartX; x < lensStartX + lensWidth; x++) {
            if (x >= 0 && x < width && y >= 0 && y < height) {
                int idx = (y * width + x) * 4;
                int relY = y - lensStartY;
                int relX = x - lensStartX;
                
                bool isLeftEdge = (relX == 0);
                bool isRightEdge = (relX == lensWidth - 1);
                bool isTopEdge = (relY == -lensHeight/2 && relX >= 0 && relX < lensWidth);
                bool isBottomEdge = (relY == lensHeight/2 && relX >= 0 && relX < lensWidth);
                bool isTopSlope = (relX < lensWidth/2 && relY == -relX - lensHeight/2 + lensWidth/2);
                bool isBottomSlope = (relX < lensWidth/2 && relY == relX + lensHeight/2 - lensWidth/2);
                
                if (isLeftEdge || isRightEdge || isTopEdge || isBottomEdge || isTopSlope || isBottomSlope) {
                    pixels[idx] = 0;
                    pixels[idx + 1] = 0;
                    pixels[idx + 2] = 0;
                    pixels[idx + 3] = 255;
                }
            }
        }
    }
    
    GLFWimage image;
    image.width = width;
    image.height = height;
    image.pixels = pixels;
    
    int hotspotX = lensStartX + lensWidth / 2;
    int hotspotY = centerY;
    
    return glfwCreateCursor(&image, hotspotX, hotspotY);
}

bool loadOBJModel(const char* filePath, ModelData& outModel) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cout << "Greska: Ne mogu da otvorim OBJ fajl: " << filePath << std::endl;
        return false;
    }
    
    std::vector<glm::vec3> tempVertices;
    std::vector<glm::vec2> tempUVs;
    std::vector<glm::vec3> tempNormals;
    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    
    std::string line;
    std::string mtlPath;
    
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;
        
        if (prefix == "v") {
            glm::vec3 vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            tempVertices.push_back(vertex);
        }
        else if (prefix == "vt") {
            glm::vec2 uv;
            iss >> uv.x >> uv.y;
            tempUVs.push_back(uv);
        }
        else if (prefix == "vn") {
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            tempNormals.push_back(normal);
        }
        else if (prefix == "f") {
            std::vector<std::string> faceVertices;
            std::string vertex;
            while (iss >> vertex) {
                faceVertices.push_back(vertex);
            }
            
            if (faceVertices.size() < 3) continue;
            
            auto processVertex = [&](const std::string& vertexStr) {
                std::stringstream vss(vertexStr);
                std::string v, vt, vn;
                std::getline(vss, v, '/');
                std::getline(vss, vt, '/');
                std::getline(vss, vn, '/');
                
                int vi = std::stoi(v) - 1;
                vertexIndices.push_back(vi);
                
                if (!vt.empty()) {
                    int vti = std::stoi(vt) - 1;
                    uvIndices.push_back(vti);
                }
                
                if (!vn.empty()) {
                    int vni = std::stoi(vn) - 1;
                    normalIndices.push_back(vni);
                }
            };
            
            // Triangulacija: ako je quad, podeli na 2 trougla
            processVertex(faceVertices[0]);
            processVertex(faceVertices[1]);
            processVertex(faceVertices[2]);
            
            if (faceVertices.size() == 4) {
                processVertex(faceVertices[0]);
                processVertex(faceVertices[2]);
                processVertex(faceVertices[3]);
            }
        }
        else if (prefix == "mtllib") {
            iss >> mtlPath;
        }
    }
    
    file.close();
    
    bool hasUVs = !tempUVs.empty() && !uvIndices.empty();
    bool hasNormals = !tempNormals.empty() && !normalIndices.empty();
    
    outModel.vertices.clear();
    outModel.indices.clear();
    
    std::vector<unsigned int> uniqueIndices;
    std::vector<glm::vec3> uniqueVertices;
    std::vector<glm::vec2> uniqueUVs;
    std::vector<glm::vec3> uniqueNormals;
    
    for (size_t i = 0; i < vertexIndices.size(); i++) {
        unsigned int vi = vertexIndices[i];
        
        if (vi >= tempVertices.size()) continue;
        
        glm::vec3 vertex = tempVertices[vi];
        glm::vec2 uv = hasUVs && i < uvIndices.size() && uvIndices[i] < tempUVs.size() 
                       ? tempUVs[uvIndices[i]] : glm::vec2(0.0f, 0.0f);
        glm::vec3 normal = hasNormals && i < normalIndices.size() && normalIndices[i] < tempNormals.size()
                          ? tempNormals[normalIndices[i]] : glm::vec3(0.0f, 1.0f, 0.0f);
        
        bool found = false;
        unsigned int index = 0;
        
        for (size_t j = 0; j < uniqueVertices.size(); j++) {
            if (uniqueVertices[j] == vertex && 
                (!hasUVs || uniqueUVs[j] == uv) &&
                (!hasNormals || uniqueNormals[j] == normal)) {
                found = true;
                index = (unsigned int)j;
                break;
            }
        }
        
        if (!found) {
            uniqueVertices.push_back(vertex);
            uniqueUVs.push_back(uv);
            uniqueNormals.push_back(normal);
            index = (unsigned int)uniqueVertices.size() - 1;
        }
        
        outModel.indices.push_back(index);
    }
    
    for (size_t i = 0; i < uniqueVertices.size(); i++) {
        outModel.vertices.push_back(uniqueVertices[i].x);
        outModel.vertices.push_back(uniqueVertices[i].y);
        outModel.vertices.push_back(uniqueVertices[i].z);
        
        if (hasNormals) {
            outModel.vertices.push_back(uniqueNormals[i].x);
            outModel.vertices.push_back(uniqueNormals[i].y);
            outModel.vertices.push_back(uniqueNormals[i].z);
        } else {
            outModel.vertices.push_back(0.0f);
            outModel.vertices.push_back(1.0f);
            outModel.vertices.push_back(0.0f);
        }
        
        if (hasUVs) {
            outModel.vertices.push_back(uniqueUVs[i].x);
            outModel.vertices.push_back(uniqueUVs[i].y);
        } else {
            outModel.vertices.push_back(0.0f);
            outModel.vertices.push_back(0.0f);
        }
    }
    
    if (!mtlPath.empty()) {
        std::string objDir = std::string(filePath);
        size_t lastSlash = objDir.find_last_of("/\\");
        if (lastSlash != std::string::npos) {
            objDir = objDir.substr(0, lastSlash + 1);
        } else {
            objDir = "";
        }
        outModel.texturePath = objDir + mtlPath;
        
        std::ifstream mtlFile(outModel.texturePath);
        if (mtlFile.is_open()) {
            std::string mtlLine;
            while (std::getline(mtlFile, mtlLine)) {
                std::istringstream mtlIss(mtlLine);
                std::string mtlPrefix;
                mtlIss >> mtlPrefix;
                
                if (mtlPrefix == "map_Kd" || mtlPrefix == "map_Ka") {
                    std::string texPath;
                    mtlIss >> texPath;
                    size_t mtlLastSlash = outModel.texturePath.find_last_of("/\\");
                    std::string mtlDir = mtlLastSlash != std::string::npos 
                                        ? outModel.texturePath.substr(0, mtlLastSlash + 1) 
                                        : "";
                    outModel.texturePath = mtlDir + texPath;
                    break;
                }
            }
            mtlFile.close();
        }
    }
    
    std::cout << "Uspesno ucitano " << outModel.vertices.size() / 8 << " verteksa, " 
              << outModel.indices.size() / 3 << " trouglova iz " << filePath << std::endl;
    std::cout << "Total vertices array size: " << outModel.vertices.size() << ", Total indices: " << outModel.indices.size() << std::endl;
    std::cout << "Has UVs: " << (hasUVs ? "YES" : "NO") << ", Has Normals: " << (hasNormals ? "YES" : "NO") << std::endl;
    
    if (outModel.vertices.empty() || outModel.indices.empty()) {
        std::cout << "ERROR: Model data is empty!" << std::endl;
        return false;
    }
    
    return true;
}