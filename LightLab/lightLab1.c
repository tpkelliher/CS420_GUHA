/* lightlab1.c --- Experimentation with lighting in OpenGL.
 * Tom Kelliher 11/17/2022.
 *
 * See the comments in the Make file for building and running this program.
 *
 * Movement is via x/X, y/Y, and z/Z keys.  Use esc key to exit.
 * Mouse buttons rotate the right cube.
 *
 * Things to try:
 *    1) Run program as is.  Move around.  Note lack of shadows and
 *       differences between left and right cubes.
 *    2) Comment out the DIRTY define and verify that the cubes look
 *       the same.  Un-comment out the define when finished.
 *    3) In display(), swap the gluLookAt() and glLightfv() calls.
 *       Can see any effect of doing this?  Re-swap the calls when
 *       finished.
 *    4) Change the shading model from smooth to flat and observe the
 *       difference.  Change the shading model back to smooth shading.
 *    5) Try the light source at a few different positions.  Can you
 *       see any difference as a directional vs. positional source?
 *    6) Un-comment out the GET_VALUES define and expeiment with
 *       material and light values.
 *    7) Try to find material and light values that provide
 *       realistic looking results.  (Hint: you may want to
 *       augment the code by changing the constant, linear, and
 *       quadratic distance attenuation coefficients.  See the online
 *       documentation for glLight().)
 *    8) Add another light source.
 *    9) Augment the code so that each light and each object has
 *       independent properties.
 */

#define X 0
#define Y 1
#define Z 2
#define ESC 27

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>


// Vertices for right cube.
GLfloat vertices[][3] = {{-1.0,-1.0,-1.0},{1.0,-1.0,-1.0},
{1.0,1.0,-1.0}, {-1.0,1.0,-1.0}, {-1.0,-1.0,1.0}, 
{1.0,-1.0,1.0}, {1.0,1.0,1.0}, {-1.0,1.0,1.0}};


// Un-comment to be queried for light and material properties.
//#define GET_VALUES

// Un-comment to use "quick and dirty normals" for right cube.
#define DIRTY


#ifdef DIRTY

// These are the "quick and dirty" normals for the right cube,
// 1 per vertex.
GLfloat normals[][3] = {{-1.0,-1.0,-1.0},{1.0,-1.0,-1.0},
{1.0,1.0,-1.0}, {-1.0,1.0,-1.0}, {-1.0,-1.0,1.0}, 
{1.0,-1.0,1.0}, {1.0,1.0,1.0}, {-1.0,1.0,1.0}};

#else

// These are the real normals for the right cube, 1 per face.
GLfloat normals[][3] = {{0.0,0.0,-1.0},{0.0,1.0,0.0},
{-1.0,0.0,0.0},{1.0,0.0,0.0},{0.0,0.0,1.0},{0.0,-1.0,0.0}};

#endif


// The right cube can be rotated.  Whoopee.
static GLfloat theta[] = {0.0,0.0,0.0};
static GLint axis = 2;

/* initial viewer location */
static GLdouble viewer[]= {0.0, 0.0, 5.0};


// Guess what this one is.
// Fourth parameter: 0.0 --- directional light source
//                   1.0 --- positional light source.
GLfloat light_position[] = { 0.0, 5.0, 5.0, 0.0 };


#ifdef DIRTY

// Draw a polygon with its normals.
void polygon(int a, int b, int c , int d)
{
   glBegin(GL_POLYGON);
      glNormal3fv(normals[a]);
      glVertex3fv(vertices[a]);
      glNormal3fv(normals[b]);
      glVertex3fv(vertices[b]);
      glNormal3fv(normals[c]);
      glVertex3fv(vertices[c]);
      glNormal3fv(normals[d]);
      glVertex3fv(vertices[d]);
   glEnd();
}

// Draw a cube with lighting normals.
void colorcube()
{
   polygon(0,3,2,1);
   polygon(2,3,7,6);
   polygon(0,4,7,3);
   polygon(1,2,6,5);
   polygon(4,5,6,7);
   polygon(0,1,5,4);
}

#else
// !DIRTY

void polygon(int a, int b, int c , int d, GLfloat normal[])
{
   glBegin(GL_POLYGON);
      glNormal3fv(normal);
      glVertex3fv(vertices[a]);
      glVertex3fv(vertices[b]);
      glVertex3fv(vertices[c]);
      glVertex3fv(vertices[d]);
   glEnd();
}

void colorcube()
{
	polygon(0,3,2,1,normals[0]);
	polygon(2,3,7,6,normals[1]);
	polygon(0,4,7,3,normals[2]);
	polygon(1,2,6,5,normals[3]);
	polygon(4,5,6,7,normals[4]);
	polygon(0,1,5,4,normals[5]);
}

#endif


// Paint the scene.
void display(void)
{

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   /* Update viewer position in modelview matrix */
   glLoadIdentity();
   gluLookAt(viewer[X],viewer[Y],viewer[Z],
             0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

   // Position the light.
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);

   glPushMatrix();

   // Left cube with "correct normals."
   glTranslatef(-2.0, 0.0, 0.0);
   glutSolidCube(2.0);

   // Sphere centered at origin.
   glTranslatef(2.0, 0.0, 0.0);
   glutSolidSphere(1.0, 32, 32);

   // Right cube with normals determined by DIRTY.
   glTranslatef(2.0, 0.0, 0.0);
   // Rotate cube.
   glRotatef(theta[X], 1.0, 0.0, 0.0);
   glRotatef(theta[Y], 0.0, 1.0, 0.0);
   glRotatef(theta[Z], 0.0, 0.0, 1.0);
   colorcube();

   /* Now that I'm thoroughly confused, let's re-establish
    * the origin.
    */
   glPopMatrix();

   // Up y-axis by 1.
   glTranslatef(0.0, 1.0, 0.0);
   // Rotate z-axis up to y-axis.
   glRotatef(-90.0, 1.0, 0.0, 0.0);
   glutSolidCone(1.0, 2.0, 32, 32);
   // Translate up the cone.
   glTranslatef(0.0, 0.0, 1.0);
   glutSolidTorus(0.25, 1.0, 32, 32);

   glFlush();
   glutSwapBuffers();
}


/* Rotate the right cube.  Looks kinda neat when it cuts
 * into the sphere.
 */

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


// Set material and light values.
void init(void) 
{
   // Default values for material and light properties.
   GLfloat mat_ambient[] = { 0.5, 0.0, 0.0, 1.0 };
   GLfloat mat_diffuse[] = { 1.0, 0.5, 0.5, 1.0 };
   GLfloat mat_specular[] = { 1.0, 0.5, 0.5, 1.0 };
   GLfloat mat_shininess[] = { 50.0 };

   GLfloat light_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };

#ifdef GET_VALUES
   printf("Enter RGB float values for:\n");
   printf("   Material ambient properties: ");
   scanf("%f %f %f", mat_ambient, mat_ambient+1, mat_ambient+2);
   printf("   Material diffuse properties: ");
   scanf("%f %f %f", mat_diffuse, mat_diffuse+1, mat_diffuse+2);
   printf("   Material specular properties: ");
   scanf("%f %f %f", mat_specular, mat_specular+1, mat_specular+2);
   printf("\nEnter shininess float value: ");
   scanf("%f", mat_shininess);

   printf("\nEnter RGB float values for:\n");
   printf("   Light ambient properties: ");
   scanf("%f %f %f", light_ambient, light_ambient+1,
         light_ambient+2);
   printf("   Light diffuse properties: ");
   scanf("%f %f %f", light_diffuse, light_diffuse+1,
         light_diffuse+2);
   printf("   Light specular properties: ");
   scanf("%f %f %f", light_specular, light_specular+1,
         light_specular+2);
#endif

   glClearColor (0.2f, 0.3f, 0.4f, 1.0);
   glShadeModel (GL_SMOOTH);

   glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

   glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
   glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);

   /* The "quick and dirty" normals haven't been normalized. */
   glEnable(GL_NORMALIZE);
}


int
main(int argc, char **argv)
{
   setvbuf(stdout, (char *)NULL, _IONBF, 0);
   setvbuf(stderr, (char *)NULL, _IONBF, 0);

   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize(500, 500);
   glutCreateWindow("Light Lab");
   glutReshapeFunc(myReshape);
   glutDisplayFunc(display);
   glutMouseFunc(mouse);
   glutKeyboardFunc(keys);
   init();
   glutMainLoop();

   return 0;
}
