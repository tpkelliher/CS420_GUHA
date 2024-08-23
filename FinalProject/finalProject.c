// finalProject.c
// Tom Kelliher, 11/28/22
//
// This program demonstrates viewer movement.  It uses two viewports with
// entirely different views: an overhead view using an orthographic projection
// and an immersed view using perspective projection.
//
// This program models a 20x20 room with two internal dividing walls.


#define ESC 27


#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
// The IRIS RGB "library."  gltx.c must be added to the project.
#include "gltx.h"


// This 1 X 1 X 1 cube is used to render the walls, by appropriate scaling.
// It is centered at the origin in X and Y, with its base sitting on the
// Z = 0 plane.
GLfloat cube[][3] =
{ {-0.5, -0.5, 0.0}, { 0.5, -0.5, 0.0},
  { 0.5,  0.5, 0.0}, {-0.5,  0.5, 0.0},
  {-0.5, -0.5, 1.0}, { 0.5, -0.5, 1.0},
  { 0.5,  0.5, 1.0}, {-0.5,  0.5, 1.0}
};


// Various colors.
GLfloat red[] = {1.0, 0.0, 0.0};
GLfloat green[] = {0.0, 1.0, 0.0};
GLfloat blue[] = {0.0, 0.0, 1.0};
GLfloat magenta[] = {1.0, 0.0, 1.0};
GLfloat white[] = {1.0, 1.0, 1.0};
GLfloat black[] = {0.0, 0.0, 0.0};


// This 1 X 1 square, centered at the origin in X and Y and within the
// Z = 0 plane is used to render the viewer's position, the floor, and the
// ceiling using appropriate scaling.
GLfloat square[][3] =
{ {-0.5, -0.5, 0.0}, { 0.5, -0.5, 0.0},
  { 0.5,  0.5, 0.0}, {-0.5,  0.5, 0.0}
};


// Initial window dimensions.
int width = 500;
int height = 500;


// Constants necessary for rotating the viewer.  The trig functions take
// radian arguments.  (Sigh.)
const float PI = 3.14159f;
const float ROTATE =   0.7853975f;   // 1/8th of a complete revolution in
                                     // radians.


// Viewer state.
GLfloat viewerTheta = 1.570795f;     // View direction.  Initially looking 
                                     // up positive y axis.
GLfloat viewerPosition[] = {0.0, 0.0, 6.0};   // Initial view position.


//
// polygon renders a polygon:
//    polygon: name of the vertex list.
//    a, b, c, d: indices of the vertex list vertices to be rendered.
//    color: the color vector.
//
void polygon(GLfloat polygon[][3], int a, int b, int c , int d,
             GLfloat *color)
{
   glPushAttrib(GL_ALL_ATTRIB_BITS);
   glBegin(GL_POLYGON);
      glColor3fv(color);
      glVertex3fv(polygon[a]);
      glVertex3fv(polygon[b]);
      glVertex3fv(polygon[c]);
      glVertex3fv(polygon[d]);
   glEnd();
   glPopAttrib();
}


//
// colorCube renders a cube.
//    cube: the vertex list specifying the cube.
//    color: the color vector.
//
// Assumptions regarding the vertex list:
// Index   Vertex
//   0     Lower left vertex of back face.
//   1     Lower right vertex of back face.
//   2     Upper right vertex of back face.
//   3     Upper left vertex of back face.
// 4--7    Similar for front face.
// (Assumes we are looking at the origin from the +z axis with the +y axis 
// being "up."
//
void colorCube(GLfloat cube[][3], GLfloat *color)
{
   polygon(cube,0,3,2,1,color);
   polygon(cube,2,3,7,6,color);
   polygon(cube,0,4,7,3,color);
   polygon(cube,1,2,6,5,color);
   polygon(cube,4,5,6,7,color);
   polygon(cube,0,1,5,4,color);
}


//
// room renders the room for me.  Because of the needs of the two views, it 
// renders neither the ceiling nor the viewer.
//
void room()
{
    int i;

    // Render floor.
    glPushMatrix();
    glScalef(24.0, 24.0, 1.0);
    polygon(square, 0, 1, 2, 3, white);
    glPopMatrix();

    // Render middle walls.  Each wall is 2 X 10 X 8.
    glPushMatrix();
    glTranslatef(-4.0, 0.0, 0.0);
    glScalef(2.0, 10.0, 8.0);
    colorCube(cube, red);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(4.0, 0.0, 0.0);
    glScalef(2.0, 10.0, 8.0);
    colorCube(cube, red);
    glPopMatrix();

    // Render walls defining room boundaries, starting with the left wall,
    // then the top wall, etc.  Each wall is 2 X 20 X 8.
    glPushMatrix();
    glTranslatef(-11.0, 0.0, 0.0);
    for (i = 0; i < 4; ++i)
    {
        glPushMatrix();
        glScalef(2.0, 20.0, 8.0);
        colorCube(cube, (i == 1) ? green : blue);
        glPopMatrix();
        glTranslatef(11.0, 11.0, 0.0);
        glRotatef(-90.0, 0.0, 0.0, 1.0);
    }
    glPopMatrix();
}


//
// display is where the work gets done.
//
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set-up to do the overhead view.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-13.0, 13.0, -13.0, 13.0, -10.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
        
    glViewport(0, 0, 100, 100);

    glLoadIdentity();

    // Render viewer.
    glPushMatrix();
    glTranslatef(viewerPosition[0], viewerPosition[1], 10.0);
    polygon(square, 0, 1, 2, 3, black);
    glPopMatrix();

    // Render the room.
    room();

    glFlush();

    // Set-up the do the "immersed" view.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.0, 1.0, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW); 

    glViewport(100, 100, width - 100, height - 100);

    // Position the camera.
    gluLookAt(viewerPosition[0], viewerPosition[1], viewerPosition[2],
        viewerPosition[0] + cos(viewerTheta), 
      viewerPosition[1] + sin(viewerTheta), viewerPosition[2], 0.0, 0.0, 1.0);

    glPushMatrix();

    room();

    glPopMatrix();

    // Render ceiling.
    glPushMatrix();
    glTranslatef(0.0, 0.0, 8.0);
    glScalef(24.0, 24.0, 1.0);
    polygon(square, 0, 1, 2, 3, magenta);
    glPopMatrix();

    glutSwapBuffers();
}


//
// reshape doesn't do too much right now.  It should really make sure we handle
// aspect ratios correctly.
//
void reshape(int w, int h)
{
    width = w;
    height = h;

    glutPostRedisplay();
}


//
// Callback for the non-ASCII keys.  I.e., the arrow keys.
//
void special(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_LEFT:
        --viewerPosition[0];
        break;

    case GLUT_KEY_RIGHT:
        ++viewerPosition[0];
        break;

    case GLUT_KEY_UP:
        ++viewerPosition[1];
        break;

    case GLUT_KEY_DOWN:
        --viewerPosition[1];
        break;

    default:
        return;
    }

    glutPostRedisplay();
}


//
// Callback for the ASCII keys.  "A" is rotate left and "S" is rotate right.
//
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {

    case ESC:
        exit(0);
        break;
        
    case 'a':
    case 'A':
        break;

    case 's':
    case 'S':
        break;

    default:
        return;
    }

    if (viewerTheta >= 2 * PI)
        viewerTheta -= 2 * PI;

    glutPostRedisplay();
}


//
// main just initializes the display modes, creates the window, and registers 
// various callbacks.
//
int main(int argc, char *argv[])
{
   setvbuf(stdout, (char *)NULL, _IONBF, 0);
   setvbuf(stderr, (char *)NULL, _IONBF, 0);
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize(width, height);
   glutCreateWindow("Final Project");
   glutReshapeFunc(reshape);
   glutDisplayFunc(display);
   glutSpecialFunc(special);
   glutKeyboardFunc(keyboard);
   glEnable(GL_DEPTH_TEST);

   glutMainLoop();

   return 0;
}
