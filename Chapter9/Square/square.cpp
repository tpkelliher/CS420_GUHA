///////////////////////////////////
// square.cpp
//
// OpenGL program to draw a square.
// 
// Sumanta Guha.
// Added documentation by Tom Kelliher.
///////////////////////////////////

#include <GL/glew.h>
#include <GL/freeglut.h> 

// Drawing routine.
void drawScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Black is our drawing color.
    glColor3f(0.0, 0.0, 0.0);

    // Draw a polygon with specified vertices.
    // Note that the vertices are specified in counter-clockwise order.
    glPolygonMode(GL_FRONT, GL_LINE);
    glPolygonMode(GL_BACK, GL_FILL);
    glBegin(GL_POLYGON);
    glVertex3f(20.0, 20.0, 0.0);
    glVertex3f(80.0, 20.0, 0.0);
    glVertex3f(80.0, 80.0, 0.0);
    glVertex3f(20.0, 80.0, 0.0);
    glEnd();

    // Force all the vertices through the pipeline.  Otherwise, they might
    // loiter.  This is undesirable when using double-buffering.
    glFlush();
}

// Initialization routine.
void setup(void)
{
    // Clear color set to white.
    // The fourth argument is alpha, which specifies opaqueness when it's
    // in use.  It's not in use.
    glClearColor(1.0, 1.0, 1.0, 0.0);
}

// OpenGL window reshape routine.
// w and h are the width and height of the window in pixels.
void resize(int w, int h)
{
    // Use the entire window.  See the documentation for glViewport
    // for the interpretation of the parameters.
    glViewport(0, 0, w, h);

    // Set an orthographic projection.  See the documentation for glOrtho.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 100.0, 0.0, 100.0, -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
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

// Main routine.
int main(int argc, char **argv)
{
    glutInit(&argc, argv);

    // We're using OpenGL version 3.1 in compatibility mode.
    // I.e., the fixed function pipeline.
    glutInitContextVersion(3, 1);
    glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

    // Use single-buffering and RGB color.
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    
    // Create the window and give it a title.
    glutCreateWindow("square.cpp");
    
    // Register callback functions.
    // Called to render the scene.  Do not call the
    // callback directly.  Let GLUT decide when to do so.
    glutDisplayFunc(drawScene);
    // Called when the window is resized.
    glutReshapeFunc(resize);
    // Call upon keyboard input.
    glutKeyboardFunc(keyInput);
    
    glewExperimental = GL_TRUE;
    glewInit();

    setup();

    // GLUT enters its event processing loop.  It will not return.
    glutMainLoop();

    return 0;
}
