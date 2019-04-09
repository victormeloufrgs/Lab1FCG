//     Universidade Federal do Rio Grande do Sul
//             Instituto de Informática
//       Departamento de Informática Aplicada
//
//    INF01047 Fundamentos de Computação Gráfica
//               Prof. Eduardo Gastal
//
//                   LABORATÓRIO 1
//

// Arquivos "headers" padrões de C podem ser incluídos em um
// programa C++, sendo necessário somente adicionar o caractere
// "c" antes de seu nome, e remover o sufixo ".h". Exemplo:
//    #include <stdio.h> // Em C
//  vira
//    #include <cstdio> // Em C++
//
#include <cmath>
#include <cstdio>
#include <cstdlib>

// Headers abaixo são específicos de C++
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
 #include <iostream>

// Headers das bibliotecas OpenGL
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Criação de janelas do sistema operacional

// Headers locais, definidos na pasta "include/"
#include "utils.h"

#define PI 3.14159265358979

int totalZeroVertices = 32;
int totalOneVertices = 6;
int totalVertices = totalZeroVertices + totalOneVertices;

int indicesLength = 0; // +2 pois os dois últimos são pra fechar os triângulos.
int totalV = 0;
int lastSecond = -1;

struct Vertex {
    GLfloat x;
    GLfloat y;
};

// Declaração de várias funções utilizadas em main().  Essas estão definidas
// logo após a definição de main() neste arquivo.
GLuint BuildNumbers(); // Constrói triângulos para renderização
GLuint LoadShader_Vertex(const char* filename);   // Carrega um vertex shader
GLuint LoadShader_Fragment(const char* filename); // Carrega um fragment shader
void LoadShader(const char* filename, GLuint shader_id); // Função utilizada pelas duas acima
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id); // Cria um programa de GPU

// Funções callback para comunicação com o sistema operacional e interação do
// usuário. Veja mais comentários nas definições das mesmas, abaixo.
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Funções para obter círculo
std::vector<Vertex> getZeroVertexArray(Vertex origin, int numberOfVertex, float raioExterno, float raioInterno);
std::vector<Vertex> getOneVertexArray(Vertex origin, float height, float width);
Vertex getZeroVertex(Vertex origin, float angle, float raio);
std::vector<GLubyte> getVerticesForNumber(int number);
std::vector<GLubyte> getVerticesForPattern(bool first, bool second, bool third, bool fourth);
std::vector<GLubyte> getVerticesForOneAt(int position);
std::vector<GLubyte> getVerticesForZeroAt(int position);

std::vector<GLubyte> vertices0, vertices1, vertices2, vertices3, vertices4, vertices5,
                     vertices6, vertices7, vertices8, vertices9, vertices10, vertices11,
                     vertices12, vertices13, vertices14, vertices15;

int main()
{
    // Inicializamos a biblioteca GLFW, utilizada para criar uma janela do
    // sistema operacional, onde poderemos renderizar com OpenGL.
    int success = glfwInit();
    if (!success)
    {
        fprintf(stderr, "ERROR: glfwInit() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos o callback para impressão de erros da GLFW no terminal
    glfwSetErrorCallback(ErrorCallback);

    // Pedimos para utilizar OpenGL versão 3.3 (ou superior)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Pedimos para utilizar o perfil "core", isto é, utilizaremos somente as
    // funções modernas de OpenGL.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Criamos uma janela do sistema operacional, com 500 colunas e 500 linhas
    // de pixels, e com título "INF01047 ...".
    GLFWwindow* window;
    window = glfwCreateWindow(500, 500, "INF01047 - Seu Cartao - Seu Nome", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        fprintf(stderr, "ERROR: glfwCreateWindow() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos a função de callback que será chamada sempre que o usuário
    // pressionar alguma tecla do teclado.
    glfwSetKeyCallback(window, KeyCallback);

    // Definimos a função de callback que será chamada sempre que a janela for
    // redimensionada, por consequência alterando o tamanho do "framebuffer"
    // (região de memória onde são armazenados os pixels da imagem).
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

    // Indicamos que as chamadas OpenGL deverão renderizar nesta janela
    glfwMakeContextCurrent(window);

    // Carregamento de todas funções definidas por OpenGL 3.3, utilizando a
    // biblioteca GLAD.
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    // Imprimimos no terminal informações sobre a GPU do sistema
    const GLubyte *vendor      = glGetString(GL_VENDOR);
    const GLubyte *renderer    = glGetString(GL_RENDERER);
    const GLubyte *glversion   = glGetString(GL_VERSION);
    const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    printf("GPU: %s, %s, OpenGL %s, GLSL %s\n", vendor, renderer, glversion, glslversion);

    // Carregamos os shaders de vértices e de fragmentos que serão utilizados
    // para renderização. Veja slides 217-219 do documento "Aula_03_Rendering_Pipeline_Grafico.pdf".
    //
    // Note que o caminho para os arquivos "shader_vertex.glsl" e
    // "shader_fragment.glsl" estão fixados, sendo que assumimos a existência
    // da seguinte estrutura no sistema de arquivos:
    //
    //    + FCG_Lab_0X/
    //    |
    //    +--+ bin/
    //    |  |
    //    |  +--+ Release/  (ou Debug/ ou Linux/)
    //    |     |
    //    |     o-- main.exe
    //    |
    //    +--+ src/
    //       |
    //       o-- shader_vertex.glsl
    //       |
    //       o-- shader_fragment.glsl
    //       |
    //       o-- ...
    //
    GLuint vertex_shader_id = LoadShader_Vertex("../../src/shader_vertex.glsl");
    GLuint fragment_shader_id = LoadShader_Fragment("../../src/shader_fragment.glsl");

    // Criamos um programa de GPU utilizando os shaders carregados acima
    GLuint program_id = CreateGpuProgram(vertex_shader_id, fragment_shader_id);

    // Construímos a representação de um triângulo
    GLuint vertex_array_object_id = BuildNumbers();

    // Ficamos em um loop infinito, renderizando, até que o usuário feche a janela
    while (!glfwWindowShouldClose(window))
    {

        int seconds = (int)glfwGetTime() % 16;

        if (seconds != lastSecond) {
            lastSecond = seconds;
        }


        // Aqui executamos as operações de renderização

        // Definimos a cor do "fundo" do framebuffer como branco.  Tal cor é
        // definida como coeficientes RGBA: Red, Green, Blue, Alpha; isto é:
        // Vermelho, Verde, Azul, Alpha (valor de transparência).
        // Conversaremos sobre sistemas de cores nas aulas de Modelos de Iluminação.
        //
        //           R     G     B     A
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        // "Pintamos" todos os pixels do framebuffer com a cor definida acima
        glClear(GL_COLOR_BUFFER_BIT);

        // Pedimos para a GPU utilizar o programa de GPU criado acima (contendo
        // os shaders de vértice e fragmentos).
        glUseProgram(program_id);
        
        glScalef(3.0f,1.0f,1.0f);
        

        // "Ligamos" o VAO. Informamos que queremos utilizar os atributos de
        // vértices apontados pelo VAO criado pela função BuildNumbers(). Veja
        // comentários detalhados dentro da definição de BuildNumbers().
        glBindVertexArray(vertex_array_object_id);

        // std::cout << "length: " << indicesLength << std::endl;

        // Pedimos para a GPU rasterizar os vértices apontados pelo VAO como
        // triângulos.
        //
        //                +--- Veja slide 150 do documento "Aula_04_Modelagem_Geometrica_3D.pdf".
        //                |          +--- O array "indices[]" contém 6 índices (veja função BuildNumbers()).
        //                |          |  +--- Os índices são do tipo "GLubyte" (8 bits sem sinal)
        //                |          |  |                 +--- Vértices começam em indices[0] (veja função BuildNumbers()).
        //                |          |  |                 |
        //                V          V  V                 V
        glDrawElements(GL_TRIANGLES, indicesLength, GL_UNSIGNED_BYTE, 0);
        
        // "Desligamos" o VAO, evitando assim que operações posteriores venham a
        // alterar o mesmo. Isso evita bugs.
        glBindVertexArray(0);

        // O framebuffer onde OpenGL executa as operações de renderização não
        // é o mesmo que está sendo mostrado para o usuário, caso contrário
        // seria possível ver artefatos conhecidos como "screen tearing". A
        // chamada abaixo faz a troca dos buffers, mostrando para o usuário
        // tudo que foi renderizado pelas funções acima.
        // Veja o link: https://en.wikipedia.org/w/index.php?title=Multiple_buffering&oldid=793452829#Double_buffering_in_computer_graphics
        glfwSwapBuffers(window);

        // Verificamos com o sistema operacional se houve alguma interação do
        // usuário (teclado, mouse, ...). Caso positivo, as funções de callback
        // definidas anteriormente usando glfwSet*Callback() serão chamadas
        // pela biblioteca GLFW.
        glfwPollEvents();
    }

    // Finalizamos o uso dos recursos do sistema operacional
    glfwTerminate();

    return 0;
}

// Constrói triângulos para futura renderização
GLuint BuildNumbers()
{
    // Primeiro, definimos os atributos de cada vértice.

    // A posição de cada vértice é definida por coeficientes em "normalized
    // device coordinates" (NDC), onde cada coeficiente está entre -1 e 1.
    // (Veja slides 163 e 168 do documento "Aula_03_Rendering_Pipeline_Grafico.pdf").
    // Nas aulas sobre transformações geométrica veremos como transformar
    // coeficientes em outros sistemas de coordenadas para coeficientes NDC.
    //
    // Note que aqui adicionamos um quarto coeficiente W (igual a 1.0).
    // Conversaremos sobre isso quando tratarmos de coordenadas homogêneas.
    //
    // Este vetor "NDC_coefficients" define a GEOMETRIA (veja slide 114 do documento "Aula_04_Modelagem_Geometrica_3D.pdf").
    //

    Vertex originFirstOne = {-0.75,-0.5};
    Vertex originSecondOne = {-0.25,-0.5};
    Vertex originThirdOne = {0.25,-0.5};
    Vertex originFourthOne = {0.75,-0.5};

    Vertex originFirstZero = {-0.75, 0};
    Vertex originSecondZero = {-0.25, 0};
    Vertex originThirdZero = {0.25, 0};
    Vertex originFourthZero = {0.75, 0};

    std::vector<Vertex> firstOne = getOneVertexArray(originFirstOne, 1, 0.2);
    std::vector<Vertex> secondOne = getOneVertexArray(originSecondOne, 1, 0.2);
    std::vector<Vertex> thirdOne = getOneVertexArray(originThirdOne, 1, 0.2);
    std::vector<Vertex> fourthOne = getOneVertexArray(originFourthOne, 1, 0.2);
    
    std::vector<Vertex> vertexArray = firstOne;
    vertexArray.insert(std::end(vertexArray), std::begin(secondOne), std::end(secondOne));
    vertexArray.insert(std::end(vertexArray), std::begin(thirdOne), std::end(thirdOne));
    vertexArray.insert(std::end(vertexArray), std::begin(fourthOne), std::end(fourthOne));

    std::vector<Vertex> firstZero = getZeroVertexArray(originFirstZero, totalZeroVertices, 0.5, 0.4);
    std::vector<Vertex> secondZero = getZeroVertexArray(originSecondZero, totalZeroVertices, 0.5, 0.4);
    std::vector<Vertex> thirdZero = getZeroVertexArray(originThirdZero, totalZeroVertices, 0.5, 0.4);
    std::vector<Vertex> fourthZero = getZeroVertexArray(originFourthZero, totalZeroVertices, 0.5, 0.4);

    vertexArray.insert(std::end(vertexArray), std::begin(firstZero), std::end(firstZero));
    vertexArray.insert(std::end(vertexArray), std::begin(secondZero), std::end(secondZero));
    vertexArray.insert(std::end(vertexArray), std::begin(thirdZero), std::end(thirdZero));
    vertexArray.insert(std::end(vertexArray), std::begin(fourthZero), std::end(fourthZero));

    GLfloat NDC_coefficients [4*(totalVertices*4)];

    for (int i = 0; i < 4*(totalVertices*4); i+= 4) {
        NDC_coefficients[i] = vertexArray[i/4].x;
        NDC_coefficients[i+1] = vertexArray[i/4].y;
        NDC_coefficients[i+2] = 0.0f;
        NDC_coefficients[i+3] = 1.0f;

        // std::cout << "x: " << vertexArray[i/4].x << " y: " << vertexArray[i/4].y << std::endl;
    }

    // GLfloat NDC_coefficients[] = {
    // //    X      Y     Z     W
    //      0.0f,  0.0f, 0.0f, 1.0f,
    //      1.0f,  0.0f, 0.0f, 1.0f,
    //      0.7f, 0.7f, 0.0f, 1.0f,
    //      0.0f,  1.0f, 0.0f, 1.0f,
    //     -0.7f,  0.7f, 0.0f, 1.0f,
    //     -1.0f,  0.0f, 0.0f, 1.0f,
    //     -0.7f, -0.7f, 0.0f, 1.0f,
    //      0.0f,  -1.0f, 0.0f, 1.0f,
    //      0.7f,  -0.7f, 0.0f, 1.0f,
    //      1.0f,   0.0f, 0.0f, 1.0f,
    // };

    // Criamos o identificador (ID) de um Vertex Buffer Object (VBO).  Um VBO é
    // um buffer de memória que irá conter os valores de um certo atributo de
    // um conjunto de vértices; por exemplo: posição, cor, normais, coordenadas
    // de textura. Neste exemplo utilizaremos vários VBOs, um para cada tipo de
    // atributo.  Agora criamos um VBO para armazenarmos um atributo: posição
    // (coeficientes NDC definidos acima).
    GLuint VBO_NDC_coefficients_id;
    glGenBuffers(1, &VBO_NDC_coefficients_id);

    // Criamos o identificador (ID) de um Vertex Array Object (VAO).  Um VAO
    // contém a definição de vários atributos de um certo conjunto de vértices;
    // isto é, um VAO irá conter ponteiros para vários VBOs.
    GLuint vertex_array_object_id;
    glGenVertexArrays(1, &vertex_array_object_id);

    // "Ligamos" o VAO ("bind"). Informamos que iremos atualizar o VAO cujo ID
    // está contido na variável "vertex_array_object_id".
    glBindVertexArray(vertex_array_object_id);

    // "Ligamos" o VBO ("bind"). Informamos que o VBO cujo ID está contido na
    // variável VBO_NDC_coefficients_id será modificado a seguir. A
    // constante "GL_ARRAY_BUFFER" informa que esse buffer é de fato um VBO, e
    // irá conter atributos de vértices.
    glBindBuffer(GL_ARRAY_BUFFER, VBO_NDC_coefficients_id);

    // Alocamos memória para o VBO "ligado" acima. Como queremos armazenar
    // nesse VBO todos os valores contidos no array "NDC_coefficients", pedimos
    // para alocar um número de bytes exatamente igual ao tamanho ("size")
    // desse array. A constante "GL_STATIC_DRAW" dá uma dica para o driver da
    // GPU sobre como utilizaremos os dados do VBO. Neste caso, estamos dizendo
    // que não pretendemos alterar tais dados (são estáticos: "STATIC"), e
    // também dizemos que tais dados serão utilizados para renderizar ou
    // desenhar ("DRAW").  Pense que:
    //
    //            glBufferData()  ==  malloc() do C  ==  new do C++.
    //
    glBufferData(GL_ARRAY_BUFFER, sizeof(NDC_coefficients), NULL, GL_STATIC_DRAW);

    // Finalmente, copiamos os valores do array NDC_coefficients para dentro do
    // VBO "ligado" acima.  Pense que:
    //
    //            glBufferSubData()  ==  memcpy() do C.
    //
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(NDC_coefficients), NDC_coefficients);

    // Precisamos então informar um índice de "local" ("location"), o qual será
    // utilizado no shader "shader_vertex.glsl" para acessar os valores
    // armazenados no VBO "ligado" acima. Também, informamos a dimensão (número de
    // coeficientes) destes atributos. Como em nosso caso são coordenadas NDC
    // homogêneas, temos quatro coeficientes por vértice (X,Y,Z,W). Isto define
    // um tipo de dado chamado de "vec4" em "shader_vertex.glsl": um vetor com
    // quatro coeficientes. Finalmente, informamos que os dados estão em ponto
    // flutuante com 32 bits (GL_FLOAT).
    // Esta função também informa que o VBO "ligado" acima em glBindBuffer()
    // está dentro do VAO "ligado" acima por glBindVertexArray().
    // Veja https://www.khronos.org/opengl/wiki/Vertex_Specification#Vertex_Buffer_Object
    GLuint location = 0; // "(location = 0)" em "shader_vertex.glsl"
    GLint  number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);

    // "Ativamos" os atributos. Informamos que os atributos com índice de local
    // definido acima, na variável "location", deve ser utilizado durante o
    // rendering.
    glEnableVertexAttribArray(location);

    // "Desligamos" o VBO, evitando assim que operações posteriores venham a
    // alterar o mesmo. Isto evita bugs.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Agora repetimos todos os passos acima para atribuir um novo atributo a
    // cada vértice: uma cor (veja slide 115 do documento "Aula_04_Modelagem_Geometrica_3D.pdf").
    // Tal cor é definida como coeficientes RGBA: Red, Green, Blue, Alpha;
    // isto é: Vermelho, Verde, Azul, Alpha (valor de transparência).
    // Conversaremos sobre sistemas de cores nas aulas de Modelos de Iluminação.
 
    GLfloat color_coefficients [4*(totalVertices*4)];

    for (int i = 0; i < 4*(totalVertices*4); i+= 4) {
        color_coefficients[i] = 0.0f;
        color_coefficients[i+1] = 0.0f;
        color_coefficients[i+2] = 0.0f;
        color_coefficients[i+3] = 1.0f;

        if(i < totalOneVertices*16) {
            color_coefficients[i+2] = 1.0f;
        } else {
            color_coefficients[i] = 1.0f;
        }
    }

    // GLfloat color_coefficients[] = {
    // //  R     G     B     A
    //     1.0f, 0.0f, 0.0f, 1.0f,
    //     0.0f, 1.0f, 0.0f, 1.0f,
    //     0.0f, 0.0f, 1.0f, 1.0f,
    //     0.0f, 1.0f, 1.0f, 1.0f
    // };
    
    GLuint VBO_color_coefficients_id;
    glGenBuffers(1, &VBO_color_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_color_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_coefficients), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(color_coefficients), color_coefficients);
    location = 1; // "(location = 1)" em "shader_vertex.glsl"
    number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Vamos então definir dois triângulos utilizando os vértices do array
    // NDC_coefficients. O primeiro triângulo é formado pelos vértices 0,1,2;
    // e o segundo pelos vértices 2,1,3. Note que usaremos o modo de renderização
    // GL_TRIANGLES na chamada glDrawElements() dentro de main(). Veja slide 150 do documento "Aula_04_Modelagem_Geometrica_3D.pdf".
    //
    // Este vetor "indices" define a TOPOLOGIA (veja slide 114 do documento "Aula_04_Modelagem_Geometrica_3D.pdf").
    //

    // GLubyte indices[indicesLength]; // GLubyte: valores entre 0 e 255 (8 bits sem sinal).

    // for(int i = 0; i < indicesLength; i++) {
    //     indices[i] = i;

    //     if (i == indicesLength-2) {
    //         indices[i] = 0;
    //     } else if (i == indicesLength-1) {
    //         indices[i] = 1; // fecha o círculo
    //     }
    // }

    // std::vector<GLubyte> indicesOne1 = getVerticesForOneAt(1);
    // std::vector<GLubyte> indicesOne2 = getVerticesForOneAt(2);
    // std::vector<GLubyte> indicesOne3 = getVerticesForOneAt(3);
    // std::vector<GLubyte> indicesOne4 = getVerticesForOneAt(4);

    // std::vector<GLubyte> indicesZero1 = getVerticesForZeroAt(1);
    // std::vector<GLubyte> indicesZero2 = getVerticesForZeroAt(2);
    // std::vector<GLubyte> indicesZero3 = getVerticesForZeroAt(3);
    // std::vector<GLubyte> indicesZero4 = getVerticesForZeroAt(4);    

    // number 0
    // number 1
    // number 2
    // number 3
    // number 4
    // number 5
    // number 6
    // number 7
    // number 8
    // number 9
    // number 10
    // number 11
    // number 12
    // number 13
    // number 14
    // number 15
    vertices0 = getVerticesForNumber(0);

    // std::vector<GLubyte> indices = indicesOne1;
    // indices.insert(std::end(indices), std::begin(indicesOne1), std::end(indicesOne1));
    // indices.insert(std::end(indices), std::begin(indicesOne2), std::end(indicesOne2));
    // indices.insert(std::end(indices), std::begin(indicesOne3), std::end(indicesOne3));
    // indices.insert(std::end(indices), std::begin(indicesOne4), std::end(indicesOne4));


    // indices.insert(std::end(indices), std::begin(indicesZero1), std::end(indicesZero1));
    // indices.insert(std::end(indices), std::begin(indicesZero2), std::end(indicesZero2));
    // indices.insert(std::end(indices), std::begin(indicesZero3), std::end(indicesZero3));
    // indices.insert(std::end(indices), std::begin(indicesZero4), std::end(indicesZero4));

    // std::vector<GLubyte> indices = { 18,19,20,  18,20,21,  23,22,21,
    //                                 0,1,2,  0,2,3,  5,4,3,  
    //                                  6,7,8,  6,8,9,  11,10,9,  
    //                                  12,13,14,  12,14,15,  17,16,15}; //indicesOne1;
    std::vector<GLubyte> indices = vertices15;

    // std::cout << "indices:" << std::endl;
    // for(int i = 0; i < indices.size(); i++) {
    //     std::cout << int(indices[i]) << std::endl;
    // }

    // std::cout << "-----------------" << std::endl;

    indicesLength = indices.size();

    // Criamos um buffer OpenGL para armazenar os índices acima
    GLuint indices_id;
    glGenBuffers(1, &indices_id);

    // "Ligamos" o buffer. Note que o tipo agora é GL_ELEMENT_ARRAY_BUFFER.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);

    // Alocamos memória para o buffer.
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size(), NULL, GL_STATIC_DRAW);

    // Copiamos os valores do array indices[] para dentro do buffer.
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size(), &indices[0]);

    // NÃO faça a chamada abaixo! Diferente de um VBO (GL_ARRAY_BUFFER), um
    // array de índices (GL_ELEMENT_ARRAY_BUFFER) não pode ser "desligado",
    // caso contrário o VAO irá perder a informação sobre os índices.
    //
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // XXX Errado!
    //

    // "Desligamos" o VAO, evitando assim que operações posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindVertexArray(0);

    // Retornamos o ID do VAO. Isso é tudo que será necessário para renderizar
    // os triângulos definidos acima. Veja a chamada glDrawElements() em main().
    return vertex_array_object_id;
}

// Carrega um Vertex Shader de um arquivo. Veja definição de LoadShader() abaixo.
GLuint LoadShader_Vertex(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo
    // será aplicado nos vértices.
    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);

    // Carregamos e compilamos o shader
    LoadShader(filename, vertex_shader_id);

    // Retorna o ID gerado acima
    return vertex_shader_id;
}

// Carrega um Fragment Shader de um arquivo. Veja definição de LoadShader() abaixo.
GLuint LoadShader_Fragment(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo
    // será aplicado nos fragmentos.
    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    // Carregamos e compilamos o shader
    LoadShader(filename, fragment_shader_id);

    // Retorna o ID gerado acima
    return fragment_shader_id;
}

// Função auxilar, utilizada pelas duas funções acima. Carrega código de GPU de
// um arquivo GLSL e faz sua compilação.
void LoadShader(const char* filename, GLuint shader_id)
{
    // Lemos o arquivo de texto indicado pela variável "filename"
    // e colocamos seu conteúdo em memória, apontado pela variável
    // "shader_string".
    std::ifstream file;
    try {
        file.exceptions(std::ifstream::failbit);
        file.open(filename);
    } catch ( std::exception& e ) {
        fprintf(stderr, "ERROR: Cannot open file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }
    std::stringstream shader;
    shader << file.rdbuf();
    std::string str = shader.str();
    const GLchar* shader_string = str.c_str();
    const GLint   shader_string_length = static_cast<GLint>( str.length() );

    // Define o código do shader GLSL, contido na string "shader_string"
    glShaderSource(shader_id, 1, &shader_string, &shader_string_length);

    // Compila o código do shader GLSL (em tempo de execução)
    glCompileShader(shader_id);

    // Verificamos se ocorreu algum erro ou "warning" durante a compilação
    GLint compiled_ok;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled_ok);

    GLint log_length = 0;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

    // Alocamos memória para guardar o log de compilação.
    // A chamada "new" em C++ é equivalente ao "malloc()" do C.
    GLchar* log = new GLchar[log_length];
    glGetShaderInfoLog(shader_id, log_length, &log_length, log);

    // Imprime no terminal qualquer erro ou "warning" de compilação
    if ( log_length != 0 )
    {
        std::string  output;

        if ( !compiled_ok )
        {
            output += "ERROR: OpenGL compilation of \"";
            output += filename;
            output += "\" failed.\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }
        else
        {
            output += "WARNING: OpenGL compilation of \"";
            output += filename;
            output += "\".\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }

        fprintf(stderr, "%s", output.c_str());
    }

    // A chamada "delete" em C++ é equivalente ao "free()" do C
    delete [] log;
}

// Esta função cria um programa de GPU, o qual contém obrigatoriamente um
// Vertex Shader e um Fragment Shader.
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id)
{
    // Criamos um identificador (ID) para este programa de GPU
    GLuint program_id = glCreateProgram();

    // Definição dos dois shaders GLSL que devem ser executados pelo programa
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);

    // Linkagem dos shaders acima ao programa
    glLinkProgram(program_id);

    // Verificamos se ocorreu algum erro durante a linkagem
    GLint linked_ok = GL_FALSE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &linked_ok);

    // Imprime no terminal qualquer erro de linkagem
    if ( linked_ok == GL_FALSE )
    {
        GLint log_length = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);

        // Alocamos memória para guardar o log de compilação.
        // A chamada "new" em C++ é equivalente ao "malloc()" do C.
        GLchar* log = new GLchar[log_length];

        glGetProgramInfoLog(program_id, log_length, &log_length, log);

        std::string output;

        output += "ERROR: OpenGL linking of program failed.\n";
        output += "== Start of link log\n";
        output += log;
        output += "== End of link log\n";

        // A chamada "delete" em C++ é equivalente ao "free()" do C
        delete [] log;

        fprintf(stderr, "%s", output.c_str());
    }

    // Retorna o ID gerado acima
    return program_id;
}

// Definição da função que será chamada sempre que a janela do sistema
// operacional for redimensionada, por consequência alterando o tamanho do
// "framebuffer" (região de memória onde são armazenados os pixels da imagem).
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // Indicamos que queremos renderizar em toda região do framebuffer. A
    // função "glViewport" define o mapeamento das "normalized device
    // coordinates" (NDC) para "pixel coordinates".  Essa é a operação de
    // "Screen Mapping" ou "Viewport Mapping" vista em aula (slide 183 do documento "Aula_03_Rendering_Pipeline_Grafico.pdf").
    glViewport(0, 0, width, height);
}

// Definição da função que será chamada sempre que o usuário pressionar alguma
// tecla do teclado. Veja http://www.glfw.org/docs/latest/input_guide.html#input_key
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // ==============
    // Não modifique este loop! Ele é utilizando para correção automatizada dos
    // laboratórios. Deve ser sempre o primeiro comando desta função KeyCallback().
    for (int i = 0; i < 10; ++i)
        if (key == GLFW_KEY_0 + i && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT)
            std::exit(100 + i);
    // ==============

    // Se o usuário pressionar a tecla ESC, fechamos a janela.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

// Definimos o callback para impressão de erros da GLFW no terminal
void ErrorCallback(int error, const char* description)
{
    fprintf(stderr, "ERROR: GLFW: %s\n", description);
}

// origin is bottom left vertex
std::vector<Vertex> getOneVertexArray(Vertex origin, float height, float width) {
    
    std::vector<Vertex> oneVertexArray;

    Vertex v0 = origin;
    Vertex v1 = Vertex { origin.x+width*0.6f, origin.y};
    Vertex v2 = Vertex { origin.x+width*0.6f, origin.y+height};
    Vertex v3 = Vertex { origin.x, origin.y+height};
    Vertex v4 = Vertex { origin.x, origin.y+height*0.9f};
    Vertex v5 = Vertex { origin.x-width*0.4f, origin.y+height*0.85f};

    oneVertexArray.push_back(v0);
    oneVertexArray.push_back(v1);
    oneVertexArray.push_back(v2);
    oneVertexArray.push_back(v3);
    oneVertexArray.push_back(v4);
    oneVertexArray.push_back(v5);

    return oneVertexArray;            
}

std::vector<Vertex> getZeroVertexArray(Vertex origin, int numberOfVertex, float raioExterno, float raioInterno) {

    float angleIncrement = float(360.0)/float(numberOfVertex);

    std::vector<Vertex> circleVertexArray;
    circleVertexArray.reserve(numberOfVertex+1);

    Vertex originZero = { 0, 0 };

    int i = 0;
    for(double currentAngle = 0; currentAngle < 360.0; currentAngle+=angleIncrement) {

        Vertex zeroVertex;
        if (i % 2 == 0) {
            zeroVertex = getZeroVertex(originZero, currentAngle, raioExterno);
        zeroVertex.x *= 0.4;
        } else {
            zeroVertex = getZeroVertex(originZero, currentAngle, raioInterno);
        zeroVertex.x *= 0.25;
        }

        std::cout << zeroVertex.x << " " << zeroVertex.y << std::endl; 

        zeroVertex.x += origin.x;
        zeroVertex.y += origin.y;


        circleVertexArray.push_back(zeroVertex);
        i++;
    }

    return  circleVertexArray;
}

Vertex getZeroVertex(Vertex origin, float angle, float raio) {
    Vertex temp;

    temp.x = cos(angle * PI / 180) * raio + origin.x;
    temp.y = sin(angle * PI / 180) * raio + origin.y;

    if (temp.x == -0) {
        temp.x = 0;
    }

    if (temp.y == -0) {
        temp.y = 0; 
    }

    return temp;
}

std::vector<GLubyte> getVerticesForNumber(int number) {

    switch (number)
    {
        case 0:
            return getVerticesForPattern(false, false, false, false);
            break;
        case 1:
            return getVerticesForPattern(false, false, false, true);
            break;
        case 2:
            return getVerticesForPattern(false, false, true, false);
            break;
        case 3:
            return getVerticesForPattern(false, false, true, true);
            break;
        case 4:
            return getVerticesForPattern(false, true, false, false);
            break;
        case 5:
            return getVerticesForPattern(false, true, false, true);
            break;
        case 6:
            return getVerticesForPattern(false, true, true, false);
            break;
        case 7:
            return getVerticesForPattern(false, true, true, true);
            break;
        case 8:
            return getVerticesForPattern(true, false, false, false);
            break;
        case 9:
            return getVerticesForPattern(true, false, false, true);
            break;
        case 10:
            return getVerticesForPattern(true, false, true, false);
            break;
        case 11:
            return getVerticesForPattern(true, false, true, true);
            break;
        case 12:
            return getVerticesForPattern(true, true, false, false);
            break;
        case 13:
            return getVerticesForPattern(true, true, false, true);
            break;
        case 14:
            return getVerticesForPattern(true, true, true, false);
            break;
        case 15:
            return getVerticesForPattern(true, true, true, true);
            break;
        default:
            return {};
            break;
    }
}

std::vector<GLubyte> getVerticesForPattern(bool first, bool second, bool third, bool fourth) {

    std::vector<GLubyte> vertexArray;
    
    if(first) {
        vertexArray = getVerticesForOneAt(1);
    } else {
        vertexArray = getVerticesForZeroAt(1);
    }

    if (second) {
        std::vector<GLubyte> secondOne = getVerticesForOneAt(2);
        vertexArray.insert(std::end(vertexArray), std::begin(secondOne), std::end(secondOne));
    } else {
        std::vector<GLubyte> secondOne = getVerticesForZeroAt(2);
        vertexArray.insert(std::end(vertexArray), std::begin(secondOne), std::end(secondOne));
    }

    if (third) {
        std::vector<GLubyte> thirdOne = getVerticesForOneAt(3);
        vertexArray.insert(std::end(vertexArray), std::begin(thirdOne), std::end(thirdOne));
    } else {
        std::vector<GLubyte> thirdOne = getVerticesForZeroAt(3);
        vertexArray.insert(std::end(vertexArray), std::begin(thirdOne), std::end(thirdOne));
    }

    if (fourth) {
        std::vector<GLubyte> fourthOne = getVerticesForOneAt(4);
        vertexArray.insert(std::end(vertexArray), std::begin(fourthOne), std::end(fourthOne));
    } else {
        std::vector<GLubyte> fourthOne = getVerticesForZeroAt(4);
        vertexArray.insert(std::end(vertexArray), std::begin(fourthOne), std::end(fourthOne));
    }

    return vertexArray;
}

// position 1, 2, 3 or 4, to get the nth one
std::vector<GLubyte> getVerticesForOneAt(int position) {

    GLubyte v0 = totalOneVertices*(position-1);
    GLubyte v1 = v0+1;
    GLubyte v2 = v0+2;
    GLubyte v3 = v0+3;    
    GLubyte v4 = v0+4;
    GLubyte v5 = v0+5;

    return {v0, v1, v2,  v0, v2, v3,  v5, v4, v3};
}

// position 1, 2, 3 or 4, to get the nth zero
std::vector<GLubyte> getVerticesForZeroAt(int position) {

    GLubyte v0 = totalOneVertices*4 + (totalZeroVertices) * (position-1);

    // if (position == 2) { 
    //     v0 += 1;
    // } else if (position == 3) {
    //     v0 += 2;
    // } else if (position == 4) {
    //     v0 += 3;
    // }

    std::vector<GLubyte> vertexList = {};
    for (GLubyte i = 0; i < totalZeroVertices-2; i+= 1) {
        vertexList.push_back(v0+i);
        vertexList.push_back(v0+i+1);
        vertexList.push_back(v0+i+2);
    }

    vertexList.push_back(v0+totalZeroVertices-2);    
    vertexList.push_back(v0+totalZeroVertices-1);
    vertexList.push_back(v0);

    vertexList.push_back(v0+totalZeroVertices-1);    
    vertexList.push_back(v0);
    vertexList.push_back(v0+1);

    std::cout << "Total: " << vertexList.size() << std::endl;

    return vertexList;
}