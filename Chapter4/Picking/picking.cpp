//////////////////////////////////////////////////////////////////////////      
// picking.cpp
//
// This program illustrates picking with use of the OpenGL pick matrix and 
// selection mechanisms.
//
// Interaction:
// Click the left mouse button to select a triangle.
//
// Sumanta Guha.
//////////////////////////////////////////////////////////////////////////

#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h> 

// Globals.
static unsigned int 
isSelecting = 0, // In selection mode?
    hits,            // Number of entries in hit buffer.
    buffer[1024],    // Hit buffer.
    closestName = 0; // Name of closest hit.
static float highlightColor[] = { 1.0, 1.0, 0.0 }; // Color used to highlight object.

// Drawing routine.
void drawScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Draw triangles, loading names if in selection mode and apply color depending 
    // on if triangle was picked (yellow highlightColor if picked).
    
    // Vertical triangle.
    if (isSelecting) glLoadName(1);
    glBegin(GL_TRIANGLES);
    if (closestName == 1)
        glColor3fv(highlightColor);
    if (closestName != 1) {
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(-3.0, -4.0, -5.3);
    }
    if (closestName != 1) {
        glColor3f(0.0, 1.0, 0.0);
        glVertex3f(-1.0, -4.0, -5.3);
    }
    if (closestName != 1) {
        glColor3f(0.0, 0.0, 1.0);
        glVertex3f(-3.0, 5.0, -5.3);
    }
    glEnd();

    // Angled triangle.
    if (isSelecting) glLoadName(2);
    glBegin(GL_TRIANGLES);
    if (closestName == 2)
        glColor3fv(highlightColor);
    if (closestName != 2) {
        glColor3f(0.0, 0.0, 0.0);
        glVertex3f(4.0, 4.0, -5.2);
    }
    if (closestName != 2) {
        glColor3f(1.0, 1.0, 1.0);
        glVertex3f(2.0, 4.0, -5.2);
    }
    if (closestName != 2) {glColor3f(0.0, 1.0, 1.0);
        glVertex3f(-5.0, -4.0, -5.2);
    }
    glEnd();

    // Horizontal triangle.
    if (isSelecting) glLoadName(3);
    glBegin(GL_TRIANGLES);
    if (closestName == 3)
        glColor3fv(highlightColor);
    if (closestName != 3) {
        glColor3f(0.0, 1.0, 1.0);
        glVertex3f(-4.0, 1.0, -5.1);
    }
    if (closestName != 3) {
        glColor3f(1.0, 1.0, 0.0);
        glVertex3f(-4.0, -1.0, -5.1);
    }
    if (closestName != 3) {
        glColor3f(1.0, 0.0, 1.0);
        glVertex3f(5.0, -1.0, -5.1);
    }
    glEnd();

    glutSwapBuffers();
}

// Process hit buffer to find record with smallest min-z value.
void findClosestHit(int hits, unsigned int buffer[])
{
    unsigned int *ptr, minZ;

    minZ = 0xffffffff; // 2^32 - 1
    ptr = buffer;
    closestName = 0;
    for (int i = 0; i < hits; i++)
        {
            ptr++;
            if (*ptr < minZ)
                {
                    minZ = *ptr;
                    ptr += 2;
                    closestName = *ptr;
                    ptr++;
                }
            else ptr += 3;
        }
}

// Mouse callback routine.
void pickFunction(int button, int state, int x, int y)
{
    int viewport[4]; // Viewport data.

    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return; // Don't react unless left button is pressed.

    glGetIntegerv(GL_VIEWPORT, viewport); // Get viewport data.

    glSelectBuffer(1024, buffer); // Specify buffer to write hit records in selection mode
    (void)glRenderMode(GL_SELECT); // Enter selection mode.

    // Save the viewing volume defined in the resize routine.
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();

    // Define a selection volume corresponding to a 1-pixel region at the cursor.
    glLoadIdentity();
    gluPickMatrix((float)x, (float)(viewport[3] - y), 1.0, 1.0, viewport);
    glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0); // Copied from the reshape routine.

    glMatrixMode(GL_MODELVIEW); // Return to modelview mode before drawing.
    glLoadIdentity();

    glInitNames(); // Initializes the name stack to empty.
    glPushName(0); // Put name 0 on top of stack.

    // Determine hits with drawScene() with name loading.
    isSelecting = 1;
    drawScene();

    hits = glRenderMode(GL_RENDER); // Return to rendering mode, returning number of hits.

    // Restore viewing volume of the resize routine and return to modelview mode.
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    // Determine closest of the hit objects (if any).
    findClosestHit(hits, buffer);

    // Draw triangles without name loading, clicked one will be highlighted.
    isSelecting = 0;
    glutPostRedisplay();
}

// Initialization routine.
void setup(void)
{
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glEnable(GL_DEPTH_TEST); 
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
        default:
            break;
        }
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
    std::cout << "Interaction:" << std::endl;
    std::cout << "Click the left mouse button to select a triangle." << std::endl;
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
    glutCreateWindow("picking.cpp");
    glutDisplayFunc(drawScene);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyInput);
    glutMouseFunc(pickFunction);

    glewExperimental = GL_TRUE;
    glewInit();

    setup();

    glutMainLoop();
}

