#define _USE_MATH_DEFINES
#include <math.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>

#include "point.h"
#include <iostream>

#define WHEEL_UP 3
#define WHEEL_DOWN 4

static size_t windowWidth = 640;
static size_t windowHeight = 480;
static float aspectRatio;


GLint leftMouseButton, rightMouseButton, midMouseButton;  
int mouseX = 0, mouseY = 0;

Point cameraTPR, cameraXYZ;

GLUquadric* quad = NULL;

bool arrowKeyStates[4] = { false };

Point carPosition;
float carTheta;
float carAnimationValue = 0.0f;

int lastTime = 0;

//hàm để sử dụng camera
void recomputeOrientation()
{
    cameraXYZ.x = cameraTPR.z * sinf(cameraTPR.x) * sinf(cameraTPR.y);
    cameraXYZ.z = cameraTPR.z * -cosf(cameraTPR.x) * sinf(cameraTPR.y);
    cameraXYZ.y = cameraTPR.z * -cosf(cameraTPR.y);

    glutPostRedisplay();
}

//thay đổi tỷ lệ khi resize window
void resizeWindow(int w, int h)
{
    windowWidth = w;
    windowHeight = h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 0.1, 100.0f); // Tính tỷ lệ khung hình của cửa sổ
}

//vẽ bề mặt lưới
void drawGrid()
{
    glPushAttrib(GL_LIGHTING_BIT);
    glDisable(GL_LIGHTING);

    for (int k = 0; k < 2; k++)
    {
        for (int i = -10; i < 11; i++)
        {
            glBegin(GL_LINE_STRIP);
            glColor3f(1.0f, 0.0f, 0.0f);
            for (int j = -10; j < 11; j++) {
                glVertex3f(k == 0 ? i : j, 0, k == 0 ? j : i);
                glColor3f(1.0f, 0.0f, 0.0f);
            }
            glEnd();
        }
    }

    glPopAttrib();
}

//vẽ hình hộp
void drawSolidBox(float xSize, float ySize, float zSize)
{
    glPushMatrix();
    glScalef(xSize, ySize, zSize);
    glutSolidCube(1.0f);
    glPopMatrix();
}

//Vẽ vòng tròn rỗng để chứ nan hoa bên trong
void drawCylinder(float outerRadius, float innerRadius, float wheelDepth, int resolution)
{
    gluQuadricOrientation(quad, GLU_OUTSIDE);
    gluCylinder(quad, outerRadius, outerRadius, wheelDepth, resolution, 1);
    glPushMatrix();
    glTranslatef(0, 0, wheelDepth);
    gluDisk(quad, innerRadius, outerRadius, resolution, 1);
    glPopMatrix();

    gluQuadricOrientation(quad, GLU_INSIDE);
    gluCylinder(quad, innerRadius, innerRadius, wheelDepth, resolution, 1);

    gluDisk(quad, innerRadius, outerRadius, resolution, 1);

    gluQuadricOrientation(quad, GLU_OUTSIDE); 
}


//vẽ nan hoa của bánh xe
void veNanHoa()
{
    drawCylinder(0.5f, 0.4f, 0.1f, 16);

    glPushMatrix();
    glTranslatef(0, 0, 0.05f);
    for (int i = 0; i < 8; i++)
    {
        glPushMatrix();
        glRotatef(45 * i, 0, 0, -1);
        glTranslatef(0, 0.15, 0);

        drawSolidBox(0.1, 0.5, 0.1);
        glPopMatrix();
    }
    glPopMatrix();
}

//vẽ hoàn chỉnh xe oto
void drawCar(float x, float y, float z, float carTheta)
{
    glPushMatrix();

    //vẽ gầm xe
    glTranslatef(x, y, z);
    glRotatef(carTheta * 180 / M_PI, 0, -1, 0);

    drawSolidBox(5, 0.5, 2);

    //vẽ buồng lái xe
    glPushMatrix();
    glTranslatef(-0.3, 0.6, 0);
    drawSolidBox(3.5, 0.7, 1.7);

    glPopMatrix();


    float wheelSpeed = 25.0f;
    // bánh xe một bottom right
    glPushMatrix();
    glTranslatef(-2.0, 0, 1);
    glRotatef(carAnimationValue * wheelSpeed, 0, 0, -1);
    veNanHoa();
    glPopMatrix();
    // bánh xe hai top left
    glPushMatrix();
    glTranslatef(1.3, 0, 1);
    glRotatef(carAnimationValue * wheelSpeed, 0, 0, -1);
    veNanHoa();
    glPopMatrix();
    // bánh xe ba bottom left
    glPushMatrix();
    glTranslatef(-2.0, 0, -1.1);
    glRotatef(carAnimationValue * wheelSpeed, 0, 0, -1);
    veNanHoa();
    glPopMatrix();
    // bánh xe thứ tư top right
    glPushMatrix();
    glTranslatef(1.3, 0, -1.1);
    glRotatef(carAnimationValue * wheelSpeed, 0, 0, -1);
    veNanHoa();
    glPopMatrix();
    glPopMatrix();
}

//sử dụng con lăn chuột để zoom khung hình của xe
void mouseCallback(int button, int state, int thisX, int thisY)
{
    if (button == GLUT_LEFT_BUTTON)
        leftMouseButton = state;
    else if (button == GLUT_RIGHT_BUTTON)
        rightMouseButton = state;
    else if (button == GLUT_MIDDLE_BUTTON)
        midMouseButton = state;

    if (button == WHEEL_UP)
    {
        cameraTPR.z -= 0.5;
        recomputeOrientation();
    }

    if (button == WHEEL_DOWN)
    {
        cameraTPR.z += 0.5;
        recomputeOrientation();
    }

    mouseX = thisX;
    mouseY = thisY;
}

//đổi góc nhìn khi dùng chuột trái phải
void mouseMotion(int x, int y)
{
    if (leftMouseButton == GLUT_DOWN)
    {
        cameraTPR.x += (x - mouseX) * 0.005;
        cameraTPR.y += (y - mouseY) * 0.005;

        if (cameraTPR.y <= 0)
            cameraTPR.y = 0 + 0.001;
        if (cameraTPR.y >= M_PI)
            cameraTPR.y = M_PI - 0.001;

        recomputeOrientation();
    }
    else if (rightMouseButton == GLUT_DOWN || midMouseButton == GLUT_DOWN) {
        double totalChangeSq = (x - mouseX) + (y - mouseY);

        cameraTPR.z += totalChangeSq * 0.01;

        if (cameraTPR.z < 1.0)
            cameraTPR.z = 1.0;
        if (cameraTPR.z > 40.0)
            cameraTPR.z = 40.0;

        recomputeOrientation();
    }
    mouseX = x;
    mouseY = y;
}

//hàm chiếu sáng vào xe
void initScene()
{
    glEnable(GL_DEPTH_TEST);

    float lightCol[4] = { 0.3, 0.4123, 0.123, 0.23 };
    float ambientCol[4] = { 0.3, 0.3, 0.3, 1.0 };
    glLightfv(GL_LIGHT7, GL_DIFFUSE, lightCol);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientCol);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);


    glShadeModel(GL_SMOOTH);
}

//sử dụng nút lên xuống trái phải để di chuyển xe
void doUpdateLogic(float secondsElapsed)
{
    float movementConstant = secondsElapsed * 2;
    //std::cout << movementConstant << std::endl;
    if (arrowKeyStates[2])
    {
        float xMovement = cosf(carTheta) * movementConstant;
        float zMovement = sinf(carTheta) * movementConstant;
        carPosition.x += xMovement;
        carPosition.z += zMovement;
    }

    if (arrowKeyStates[3])
    {
        float xMovement = cosf(carTheta) * movementConstant;
        float zMovement = sinf(carTheta) * movementConstant;
        carPosition.x -= xMovement;
        carPosition.z -= zMovement;
    }

    if (arrowKeyStates[0])
        carTheta -= movementConstant;

    if (arrowKeyStates[1])
        carTheta += movementConstant;

    carAnimationValue += (arrowKeyStates[2] ^ arrowKeyStates[3]) ? 0.005f : 0.0f;
}

//render hình ảnh và các trạng thái của xe lên màn hình
void renderScene(void)
{
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    float timeDifference = (currentTime - lastTime) / 1000.0f; 
    //std::cout << currentTime << " - " << timeDifference << " - " << lastTime << std::endl;
    lastTime = currentTime;
    doUpdateLogic(timeDifference);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cameraXYZ.x, cameraXYZ.y, cameraXYZ.z, 
        0, 0, 0,                               
        0.0f, 1.0f, 0.0f);

    float lPosition[4] = { 10, 10, 10, 1 };
    glLightfv(GL_LIGHT0, GL_POSITION, lPosition);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_COLOR_MATERIAL);
    glColor3f(0.137255f, 0.137255f, 0.556863f);
    drawCar(carPosition.x, carPosition.y, carPosition.z, carTheta);
    drawGrid();

    glutSwapBuffers();
}

//bấm esc để thoát
void normalKeys(unsigned char key, int x, int y)
{
    if (key == 27)
        exit(0);
}

//chỉnh sửa trạng thái của các nút di chuyển
//nếu không sửa trạng thái true và false của nút
//true là kích hoạt các nút di chuyển
//false là ngắt kích hoạt
void specialKeys(int key, int x, int y)
{
    if (key == GLUT_KEY_LEFT)
        arrowKeyStates[0] = true;

    if (key == GLUT_KEY_RIGHT)
        arrowKeyStates[1] = true;

    if (key == GLUT_KEY_UP)
        arrowKeyStates[2] = true;

    if (key == GLUT_KEY_DOWN)
        arrowKeyStates[3] = true;

    glutPostRedisplay();
}

void specialKeysUp(int key, int x, int y)
{
    if (key == GLUT_KEY_LEFT)
        arrowKeyStates[0] = false;

    if (key == GLUT_KEY_RIGHT)
        arrowKeyStates[1] = false;

    if (key == GLUT_KEY_UP)
        arrowKeyStates[2] = false;

    if (key == GLUT_KEY_DOWN)
        arrowKeyStates[3] = false;
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - windowWidth) / 2, (glutGet(GLUT_SCREEN_HEIGHT) - windowHeight) / 2);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Mo phong xe oto 3D");

    cameraTPR.z = 25.0f;
    cameraTPR.x = 90.0f;
    cameraTPR.y = 90.0f;
    recomputeOrientation();

    carPosition = Point(0, 0.5, 0);
    carTheta = 0.0f;
    quad = gluNewQuadric();

    glutKeyboardFunc(normalKeys);
    glutSpecialFunc(specialKeys);
    glutSpecialUpFunc(specialKeysUp);
    glutDisplayFunc(renderScene);
    glutIdleFunc(renderScene);
    glutReshapeFunc(resizeWindow);
    glutMouseFunc(mouseCallback);
    glutMotionFunc(mouseMotion);

    initScene();

    glutMainLoop();

    return(0);
}