#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>

const int Width = 800;
const int Height = 600;

// Definições do labirinto
const int mapWidth = 10;
const int mapHeight = 9;
int labirinto[mapWidth][mapHeight] = {
    {1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0},
    {1,0,1,1,1,0,1,1,0},
    {1,0,1,0,1,0,1,0,0},
    {1,0,1,0,1,0,1,0,0},
    {1,0,1,0,0,0,1,0,0},
    {1,0,1,1,1,0,1,1,0},
    {1,0,0,0,1,0,0,0,0},
    {1,0,1,1,1,1,1,1,1}
};

// Posições e velocidade do círculo
float circleX = 0.0f;
float circleY = 0.0f;
float circleSpeed = 0.001f;

// Dimensões do círculo
const float circleRadius = 0.05f;

// Posições da câmera para seguir o jogador
float cameraOffsetX = 0.0f;
float cameraOffsetY = 0.0f;

// Posições e dimensões da saída
float exitX = 0.9f;
float exitY = -0.9f;
const float exitSize = 0.05f;

// Definição das fases do labirinto
struct Fase {
    int labirinto[mapWidth][mapHeight];
    float exitX;
    float exitY;
};

Fase fases[] = {
    {  // Fase 1
        {
            {1,1,1,1,1,1,1,1,1},
            {1,0,0,0,0,0,0,0,0},
            {1,0,1,1,1,0,1,1,0},
            {1,0,1,0,1,0,1,0,0},
            {1,0,1,0,1,0,1,0,0},
            {1,0,1,0,0,0,1,0,0},
            {1,0,1,1,1,0,1,1,0},
            {1,0,0,0,1,0,0,0,0},
            {1,0,1,1,1,1,1,1,1}
        },
        0.9f, -0.9f
    },
    {  // Fase 2
        {
            {1,1,1,1,1,1,1,1,1},
            {1,0,0,0,0,0,0,0,0},
            {1,0,1,1,0,0,1,1,0},
            {1,0,1,0,0,0,1,0,0},
            {1,0,1,0,1,1,1,0,0},
            {1,0,1,0,0,0,1,0,0},
            {1,0,1,1,1,0,1,1,0},
            {1,0,0,0,0,0,0,0,0},
            {1,0,1,1,1,1,1,1,1}
        },
        -0.8f, -0.8f
    },
    {  // Fase 3
        {
            {1,1,1,1,1,1,1,1,1},
            {1,0,1,1,0,0,1,0,0},
            {1,0,1,0,0,0,1,0,0},
            {1,0,1,0,1,1,1,1,0},
            {1,0,0,0,0,0,0,0,0},
            {1,1,1,1,0,1,1,1,0},
            {1,0,0,0,0,0,0,1,0},
            {1,0,1,1,1,1,0,1,0},
            {1,1,1,1,1,1,1,1,1}
        },
        -0.6f, -0.6f
    }
};

int faseAtual = 0;

// Função para desenhar um círculo (jogador)
void desenhaCirculo(float x, float y, float radius, int numSegments) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y); // Centro do círculo
    for (int i = 0; i <= numSegments; i++) {
        float angle = 2.0f * 3.1415926f * float(i) / float(numSegments);
        float dx = radius * cosf(angle);
        float dy = radius * sinf(angle);
        glVertex2f(x + dx, y + dy);
    }
    glEnd();
}

// Função para desenhar o labirinto baseado na matriz
void desenhaLabirinto() {
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);

    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            if (fases[faseAtual].labirinto[x][y] == 1) {
                float xPos = -1.0f + (2.0f / mapWidth) * x;
                float yPos = -1.0f + (2.0f / mapHeight) * y;
                float width = 2.0f / mapWidth;
                float height = 2.0f / mapHeight;

                glVertex2f(xPos, yPos);
                glVertex2f(xPos + width, yPos);
                glVertex2f(xPos + width, yPos + height);
                glVertex2f(xPos, yPos + height);
            }
        }
    }
    glEnd();
}

// Função para lidar com colisões
bool colisaoComParede(float x, float y) {
    // Impede o jogador de sair da tela e verifica colisões com as paredes

    // Verifica os limites do mapa
    if (x - circleRadius < -1.0f || x + circleRadius > 1.0f || y - circleRadius < -1.0f || y + circleRadius > 1.0f) {
        return true;
    }

    // Verifica colisões com as paredes do labirinto
    int mapX = (x + 1.0f) * (mapWidth / 2.0f);
    int mapY = (y + 1.0f) * (mapHeight / 2.0f);

    if (mapX >= 0 && mapX < mapWidth && mapY >= 0 && mapY < mapHeight) {
        if (fases[faseAtual].labirinto[mapX][mapY] == 1) {
            return true;
        }
    }

    return false;
}

// Função para lidar com eventos de teclado
void processInput(GLFWwindow* window) {
    float nextX = circleX;
    float nextY = circleY;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        nextY += circleSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        nextY -= circleSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        nextX -= circleSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        nextX += circleSpeed;
    }

    // Verificar colisão antes de mover
    if (!colisaoComParede(nextX, nextY)) {
        circleX = nextX;
        circleY = nextY;
        cameraOffsetX = circleX;
        cameraOffsetY = circleY;
    }
}

// Função para criar a névoa ao redor do jogador e escurecer o restante da tela
void criaNévoa() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Desenhar o fundo preto (escurecer a tela)
    glColor3f(0.0f, 0.0f, 0.0f); // Cor preta
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(-1.0f, 1.0f);
    glEnd();

    // Criar círculo de névoa ao redor do jogador
    float fogRadius = 0.4f;
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(1.0f, 1.0f, 1.0f, 0.0f); // Transparente no centro
    glVertex2f(circleX, circleY);
    for (int i = 0; i <= 100; i++) {
        float angle = i * 2.0f * 3.1415926f / 100;
        float x = fogRadius * cosf(angle);
        float y = fogRadius * sinf(angle);
        glColor4f(0.0f, 0.0f, 0.0f, 1.0f); // Preto nas bordas
        glVertex2f(circleX + x, circleY + y);
    }
    glEnd();

    glDisable(GL_BLEND);
}

// Função para desenhar a saída com base na visibilidade
void desenhaSaida() {
    float fogRadius = 0.4f;
    float distanceX = fabs(circleX - fases[faseAtual].exitX);
    float distanceY = fabs(circleY - fases[faseAtual].exitY);
    float distance = sqrt(distanceX * distanceX + distanceY * distanceY);

    if (distance <= fogRadius) { // Se a saída estiver dentro da área da névoa
        glColor3f(0.0f, 1.0f, 0.0f);
        glBegin(GL_QUADS);
        glVertex2f(fases[faseAtual].exitX - exitSize, fases[faseAtual].exitY - exitSize);
        glVertex2f(fases[faseAtual].exitX + exitSize, fases[faseAtual].exitY - exitSize);
        glVertex2f(fases[faseAtual].exitX + exitSize, fases[faseAtual].exitY + exitSize);
        glVertex2f(fases[faseAtual].exitX - exitSize, fases[faseAtual].exitY + exitSize);
        glEnd();
    }
}

// Função para verificar se o jogador atingiu a linha de chegada
bool atingiuLinhaDeChegada() {
    // Verifica a colisão entre o círculo e a área de saída
    float distanceX = fabs(circleX - fases[faseAtual].exitX);
    float distanceY = fabs(circleY - fases[faseAtual].exitY);
    float distance = sqrt(distanceX * distanceX + distanceY * distanceY);

    return distance < (circleRadius + exitSize);
}

// Função principal para o loop do jogo
int main() {
    if (!glfwInit()) {
        std::cerr << "Falha ao inicializar o GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(Width, Height, "Labirinto", NULL, NULL);
    if (!window) {
        std::cerr << "Falha ao criar a janela GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Falha ao inicializar o GLEW" << std::endl;
        return -1;
    }

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        processInput(window);

        criaNévoa();
        desenhaLabirinto();
        desenhaCirculo(circleX, circleY, circleRadius, 100);
        desenhaSaida();

        // Verificar se o jogador atingiu a linha de chegada
        if (atingiuLinhaDeChegada()) {
            faseAtual++;
            if (faseAtual >= sizeof(fases) / sizeof(Fase)) {
                faseAtual = 0; // Reinicia para a primeira fase se todas as fases forem concluídas
            }
            // Opcional: você pode resetar a posição do círculo, se necessário
            circleX = 0.0f;
            circleY = 0.0f;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
