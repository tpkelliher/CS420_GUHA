////////////////////////////////////////////////////////////////
// floweringPlant.cpp
//
// This program shows the animation of a plant with a flower -
// from a wilted state to blooming.
// The stem consists of 4 straight line segments and the flower
// of a hemisphere-like sepal and 6 elliptic petals.
//
// Interaction:
// Press space to toggle between animation on and off.
// Press delete to reset.
// Press the x, X, y, Y, z, Z keys to rotate the scene.
//
// Sumanta Guha.
////////////////////////////////////////////////////////////////

#define _USE_MATH_DEFINES 

#include <cmath>
#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h> 

// Globals.
static float t = 0.0; // Animation parameter.
// Angles to rotate scene.
static float Xangle = 0.0, Yangle = 260.0, Zangle = 0.0;
static int isAnimate = 0; // Animated?
static int animationPeriod = 100; // Time interval between frames.
static unsigned int base; // Display lists base index.

// Routine to draw hemisphere.
void drawHemisphere(float radius, int longSlices, int latSlices)
{
    int  i, j;
    for (j = 0; j < latSlices; j++)
    {
        // One latitudinal triangle strip.
        glBegin(GL_TRIANGLE_STRIP);
        for (i = 0; i <= longSlices; i++)
        {
            glVertex3f(radius * cos((float)(j + 1) / latSlices * M_PI / 2.0)
                       * cos(2.0 * (float)i / longSlices * M_PI),
                radius * sin((float)(j + 1) / latSlices * M_PI / 2.0),
                -radius * cos((float)(j + 1) / latSlices * M_PI / 2.0)
                       * sin(2.0 * (float)i / longSlices * M_PI));
            glVertex3f(radius * cos((float)j / latSlices * M_PI / 2.0)
                       * cos(2.0 * (float)i / longSlices * M_PI),
                radius * sin((float)j / latSlices * M_PI / 2.0),
                -radius * cos((float)j / latSlices * M_PI / 2.0)
                       * sin(2.0 * (float)i / longSlices * M_PI));
        }
        glEnd();
    }
}

// Routine to draw circle.
void drawCircle(float radius, int numVertices)
{
    int i;
    float t;
    glBegin(GL_LINE_LOOP);
    for (i = 0; i < numVertices; ++i)
    {
        t = 2 * M_PI * i / numVertices;
        glVertex3f(radius * cos(t), radius * sin(t), 0.0);
    }
    glEnd();
}

// Initialization routine.
void setup(void)
{
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glEnable(GL_DEPTH_TEST); // Enable depth testing.

    base = glGenLists(3);
    glListBase(base);

    // Stem segment display list.
    glNewList(base, GL_COMPILE);
    glColor3f(0.0, 1.0, 0.0);
    glLineWidth(3.0);
    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(5.0, 0.0, 0.0);
    glEnd();
    glEndList();
}

// Drawing routine.
void drawScene(void)
{
    // Parameters controlling the configuration of the plant and flower as
    // functions of the animation parameter t
    // t is used to linearly interpolate between the start and end value of
    // each configuration parameter.

    // Angle of first plant segment interpolating between 60 and 90.
    float angleFirstSegment = (1 - t)*60.0 + t*80.0;
    // Angle of second plant segment.
    float angleSecondSegment = (1 - t)*-30.0 + t*-20.0;
    // Angle of third plant segment.
    float angleThirdSegment = (1 - t)*-30.0 + t*-20.0;
    // Angle of fourth plant segment.
    float angleFourthSegment = (1 - t)*-30.0 + t*-20.0;
    // Factor scaling the circular base of the hemisphere.
    float hemisphereScaleFactor = (1 - t)*0.1 + t*0.75;
    // Factor scaling petal circle to make it an ellipse.
    float petalAspectRatio = (1 - t)*0.1 + t*1.0;
    // Angle at which the petal is  open.
    float petalOpenAngle = (1 - t)*-10.0 + t*-60.0;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0, -10.0, -30.0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Sepal display list.  
    glNewList(base + 1, GL_COMPILE);
    glColor3f(1.0, 0.0, 0.0);
    glPushMatrix();
    glRotatef(90.0, 0.0, 0.0, 1.0);

    // Hemisphere is scaled to be ellipsoidal.
    glScalef(hemisphereScaleFactor, 1.0, hemisphereScaleFactor);    
    drawHemisphere(2.0, 6, 6);

    glPopMatrix();
    glEndList();

    // Begin petal display list.
    glNewList(base + 2, GL_COMPILE);
    glColor3f(1.0, 0.0, 1.0);
    glPushMatrix();

    // Petal is translated to touch sepal.
    glTranslatef(2.0, 0.0, 2.0*hemisphereScaleFactor);

    // Petal is opened to a given angle.
    glTranslatef(-2.0, 0.0, 0.0);
    glRotatef(petalOpenAngle, 0.0, 1.0, 0.0);
    glTranslatef(2.0, 0.0, 0.0);

    // Circle is scaled to an ellipse.
    glScalef(1.0, petalAspectRatio, 1.0);
    drawCircle(2.0, 10);

    glPopMatrix();
    glEndList();
    // End petal display list.

    // Rotate scene.
    glRotatef(Zangle, 0.0, 0.0, 1.0);
    glRotatef(Yangle, 0.0, 1.0, 0.0);
    glRotatef(Xangle, 1.0, 0.0, 0.0);

    // First stem segment.
    glRotatef(angleFirstSegment, 0.0, 0.0, 1.0);
    glCallList(base);

    // Second stem segment.
    glTranslatef(5.0, 0.0, 0.0);
    glRotatef(angleSecondSegment, 0.0, 0.0, 1.0);
    glCallList(base);

    // Third stem segment.
    glTranslatef(5.0, 0.0, 0.0);
    glRotatef(angleThirdSegment, 0.0, 0.0, 1.0);
    glCallList(base);

    // Fourth stem segment.
    glTranslatef(5.0, 0.0, 0.0);
    glRotatef(angleFourthSegment, 0.0, 0.0, 1.0);
    glCallList(base);

    // Sepal.
    glTranslatef(7.0, 0.0, 0.0);
    glCallList(base + 1);

    // First petal.
    glPushMatrix();
    glRotatef(30.0, 1.0, 0.0, 0.0);
    glCallList(base + 2);
    glPopMatrix();

    // Second petal.
    glPushMatrix();
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glCallList(base + 2);
    glPopMatrix();

    // Third petal.
    glPushMatrix();
    glRotatef(150.0, 1.0, 0.0, 0.0);
    glCallList(base + 2);
    glPopMatrix();

    // Fourth petal.
    glPushMatrix();
    glRotatef(210.0, 1.0, 0.0, 0.0);
    glCallList(base + 2);
    glPopMatrix();

    // Fifth petal.
    glPushMatrix();
    glRotatef(270.0, 1.0, 0.0, 0.0);
    glCallList(base + 2);
    glPopMatrix();

    // Sixth petal.
    glPushMatrix();
    glRotatef(330.0, 1.0, 0.0, 0.0);
    glCallList(base + 2);
    glPopMatrix();

    glutSwapBuffers();
}

// Timer function.
void animate(int value)
{
    if (isAnimate)
    {
        t += 0.01;
        if (t >= 1.0)
            isAnimate = 0;

        glutPostRedisplay();
        glutTimerFunc(animationPeriod, animate, 1);
    }
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0);

    glMatrixMode(GL_MODELVIEW);
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27:
        exit(0);
        break;
    case ' ':
        if (isAnimate)
            isAnimate = 0;
        else
        {
            isAnimate = 1;
            animate(1);
        }
        break;
    case 127:
        if (isAnimate)
            isAnimate = 0;
        t = 0.0;
        glutPostRedisplay();
        break;
    case 'x':
        Xangle += 5.0;
        if (Xangle > 360.0)
            Xangle -= 360.0;
        glutPostRedisplay();
        break;
    case 'X':
        Xangle -= 5.0;
        if (Xangle < 0.0)
            Xangle += 360.0;
        glutPostRedisplay();
        break;
    case 'y':
        Yangle += 5.0;
        if (Yangle > 360.0)
            Yangle -= 360.0;
        glutPostRedisplay();
        break;
    case 'Y':
        Yangle -= 5.0;
        if (Yangle < 0.0)
            Yangle += 360.0;
        glutPostRedisplay();
        break;
    case 'z':
        Zangle += 5.0;
        if (Zangle > 360.0)
            Zangle -= 360.0;
        glutPostRedisplay();
        break;
    case 'Z':
        Zangle -= 5.0;
        if (Zangle < 0.0)
            Zangle += 360.0;
        glutPostRedisplay();
        break;
    default:
        break;
    }
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
    std::cout << "Interaction:" << std::endl;
    std::cout << "Press space to toggle between animation on and off."
              << std::endl
              << "Press delete to reset." << std::endl
              << "Press the x, X, y, Y, z, Z keys to rotate the scene."
              << std::endl;
}

// Main routine.
int main(int argc, char **argv)
{
    printInteraction();
    glutInit(&argc, argv);

    glutInitContextVersion(3, 1);
    glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("floweringPlant.cpp");
    glutDisplayFunc(drawScene);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyInput);

    glewExperimental = GL_TRUE;
    glewInit();

    setup();

    glutMainLoop();
}

