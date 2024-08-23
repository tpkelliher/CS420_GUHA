/* lightlab2.c --- Experimentation with lighting in OpenGL.
 * Tom Kelliher 11/17/2022.
 *
 * See the comments in the Make file for building and running this program.
 *
 * This version introduces a moving light and a spotlight anchored
 * to the viewer.  Use l to turn the spot off and L to turn it back
 * on.
 *
 * Movement is via x/X, y/Y, and z/Z keys.  Use space bar to exit.
 * Mouse buttons rotate the right cube.
 *
 * Things to try:
 *
 *    1) Change the lighting so that the background is a dark green
 *       rather than the slate blue.
 *
 *    2) Give each object its own material properties.  Make the
 *       rotating light look more realistic.  (Let it self-emanate.)
 *
 *    3) Add the ability to turn the rotating light on and off.  Its
 *       visible representation should disappear when it's off.
 *
 *    4) As you move in close to the objects, you'll notice the
 *       movable spotlight doesn't react realistically.  Correct
 *       this behavior.
 */


#define X 0
#define Y 1
#define Z 2
#define ESC 27

#define TIMER_INTERVAL 25

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>


// Base of the display lists.
GLuint lists;


// The right cube can be rotated.  Whoopee.
static GLfloat theta[] = {45.0,0.0,0.0};
static GLint axis = 2;

/* initial viewer location */
static GLdouble viewer[]= {20.0, 6.0, 6.0};


// Light 0 is the moving light.  Light 1 is the spotlight that moves
// with us.  Note that these are positional lights.
GLfloat light0_position[] = { 0.0, 0.0, 5.0, 1.0 };
GLfloat light1_position[] = { 0.0, 0.0, 0.0, 1.0 };

// Angle of the moving light.
GLint light_angle = 0;


// Paint the scene.
void display(void)
{

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   /* Update viewer position in modelview matrix */
   glLoadIdentity();

   gluLookAt(viewer[X],viewer[Y],viewer[Z], 0.0, 0.0, 0.0,
             0.0, 1.0, 0.0);

   // Position the moving light.  It will be positioned in world
   // coordinates.
   glPushMatrix();
   glRotatef(light_angle, 1.0, 0.0, 0.0);
   glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

   // Render a solid sphere indicating the light's position.  Notice
   // how it's not lit.
   glTranslatef(light0_position[X], light0_position[Y],
	   light0_position[Z]);
   glDisable(GL_LIGHTING);
   glCallList(lists);
   glEnable(GL_LIGHTING);
   glPopMatrix();

   glPushMatrix();

   // Left cube.
   glTranslatef(-2.0, 0.0, 0.0);
   glCallList(lists + 1);

   // Sphere centered at origin.
   glTranslatef(2.0, 0.0, 0.0);
   glCallList(lists + 2);

   // Right cube.
   glTranslatef(2.0, 0.0, 0.0);
   // Rotate cube.
   glRotatef(theta[X], 1.0, 0.0, 0.0);
   glRotatef(theta[Y], 0.0, 1.0, 0.0);
   glRotatef(theta[Z], 0.0, 0.0, 1.0);
   glCallList(lists + 1);

   // Now that I'm thoroughly confused, let's re-establish the
   // origin.
   glPopMatrix();

   // The cone.
   // Up y-axis by 1.
   glTranslatef(0.0, 1.0, 0.0);
   // Rotate z-axis up to y-axis.
   glRotatef(-90.0, 1.0, 0.0, 0.0);
   glCallList(lists + 3);

   // The torus.
   // Translate up the cone.
   glTranslatef(0.0, 0.0, 1.0);
   glCallList(lists + 4);

   glutSwapBuffers();
}


// Rotate the right cube.  Looks kinda neat when it cuts into
// the sphere.
void mouse(int btn, int state, int x, int y)
{
	if(btn==GLUT_LEFT_BUTTON && state == GLUT_DOWN) axis = 0;
	if(btn==GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) axis = 1;
	if(btn==GLUT_RIGHT_BUTTON && state == GLUT_DOWN) axis = 2;
	theta[axis] += 2.0;
	if( theta[axis] > 360.0 ) theta[axis] -= 360.0;
	glutPostRedisplay();
}


void keys(unsigned char key, int x, int y)
{

/* Use x, X, y, Y, z, and Z keys to move viewer */
    switch (key) {

    case ESC:
    case ' ':
        exit(0);

    case 'x':
        viewer[X] -= 1.0;
        break;

    case 'X':
        viewer[X] += 1.0;
        break;
        
    case 'y':
        viewer[Y] -= 1.0;
        break;

    case 'Y':
        viewer[Y] += 1.0;
        break;

    case 'z':
        viewer[Z] -= 1.0;
        break;

    case 'Z':
        viewer[Z] += 1.0;
        break;

    case 'l':
        glDisable(GL_LIGHT1);
        break;

    case 'L':
        glEnable(GL_LIGHT1);
        break;

    default:
        printf("Unrecognized key.\n");
        break;
    }
   
   printf("v[x]: %f, v[y]: %f, v[z]: %f.\n",
          viewer[X], viewer[Y], viewer[Z]);

   glutPostRedisplay();
}


void myReshape(int w, int h)
{
   glViewport(0, 0, w, h);

   /* Use a perspective view */

   glMatrixMode(GL_PROJECTION); 
   glLoadIdentity();
   gluPerspective(35.0, (GLfloat) w/h, 1.0, 100.0);
   glMatrixMode(GL_MODELVIEW);
}


// Rotate the moving light.
void animate(int val)
{
	light_angle = (light_angle + 1) % 360;
    glutTimerFunc(TIMER_INTERVAL, animate, 0);
	glutPostRedisplay();
}


// Set material and light values.
void init(void) 
{
   // Default values for material and light properties.
   GLfloat mat_ambient[] = { 0.5, 0.0, 0.0, 1.0 };
   GLfloat mat_diffuse[] = { 1.0, 0.5, 0.5, 1.0 };
   GLfloat mat_specular[] = { 1.0, 0.5, 0.5, 1.0 };
   GLfloat mat_shininess[] = { 50.0 };

   // No ambient light whatsoever.
   GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
   GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };

   GLfloat light1_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
   GLfloat light1_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat light1_specular[] = { 1.0, 1.0, 1.0, 1.0 };

   glClearColor (0.2f, 0.3f, 0.4f, 1.0);
   glShadeModel (GL_SMOOTH);

   glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

   // Note use of linear attenuation values, the spot cutoff 
   // half-angle, and the spot exponent (intensity dropoff
   // within the cone).

   glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
   glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
   glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.1f);

   // Specifying the light position here causes the position to be
   // interpreted in eye coordinates.  The default direction of the
   // spotlight is (0, 0, -1), which is also interpreted in eye
   // coordinates in this case.

   glLoadIdentity();
   glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

   glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
   glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
   glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
   glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.2f);
   glLighti(GL_LIGHT1, GL_SPOT_CUTOFF, 5);
   glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 0.2f);

   // Declare the viewer to be local.

   glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_LIGHT1);
   glEnable(GL_DEPTH_TEST);
}


int
main(int argc, char **argv)
{
   setvbuf(stdout, (char *)NULL, _IONBF, 0);
   setvbuf(stderr, (char *)NULL, _IONBF, 0);

   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize(500, 500);
   glutCreateWindow("Light Lab Two");
   glutReshapeFunc(myReshape);
   glutDisplayFunc(display);
   glutMouseFunc(mouse);
   glutKeyboardFunc(keys);
   glutTimerFunc(TIMER_INTERVAL, animate, 0);
   init();

   // Get five list handles.
   lists = glGenLists(5);
   
   // For the moving light.
   glNewList(lists, GL_COMPILE);
      glutSolidCube(0.1);
   glEndList();

   // For the two cubes.
   glNewList(lists + 1, GL_COMPILE);
      glutSolidCube(2.0);
   glEndList();

   // The sphere at the origin.
   glNewList(lists + 2, GL_COMPILE);
      glutSolidSphere(1.0, 32, 32);
   glEndList();

   // The cone.
   glNewList(lists + 3, GL_COMPILE);
   glutSolidCone(1.0, 2.0, 32, 32);
   glEndList();

   // The torus.
   glNewList(lists + 4, GL_COMPILE);
   glutSolidTorus(0.25, 1.0, 32, 32);
   glEndList();

   glutMainLoop();

   return 0;
}
