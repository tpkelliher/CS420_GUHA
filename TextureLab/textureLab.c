/* texturelab.c --- Experimentation with lighting and textures in
 * OpenGL.
 * Tom Kelliher 11/22/22.
 *
 * This version uses the  moving light and spotlight anchored to the
 * viewer.  Use l to turn the spot off and L to turn it back on.
 * The spotlight is now actually a point source, so that we can
 * see better --- textures attenuate the lights somewhat.
 *
 * Four different textures are read and applied.  The torus is not
 * textured, so as to demonstrate how to enable/disable texturing.
 * 
 * Automatic texture coordinate generation is used.  Note that it
 * does not work well for cubes --- it is used for the left cube.
 * Manual texture coordinates are used for the right cube.  This
 * illustrates how to disable and enable automatic texture
 * coordinate generation.
 *
 * Movement is via x/X, y/Y, and z/Z keys.  Use the esc key to exit.
 * Mouse buttons rotate the right cube.
 *
 * The four texture files, imgfile[1-4].rgb should be in the main
 * project directory.
 *
 *
 * Experiments:
 *
 *    Experiment with BREAK_TEXTURE and OBJECT_COORDS.
 *
 *    In textureInit(), vary the use of TEX_IMAGE and MIP_MAP.
 *    Can you see any difference?
 *
 *    Re-configure the viewer-anchored light to be a spotlight,
 *    rather than a point light source.  Notice much of a
 *    difference?
 *
 *    Fix the texture coordinate generation problems of the left
 *    cube.
 *
 *    Create a fifth texture and apply it to the torus.
 *
 */

#define X 0
#define Y 1
#define Z 2
#define ESC 27

#define TIMER_INTERVAL 25

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
// The IRIS RGB "library."  gltx.c must be added to the project.
#include "gltx.h"


// Uncomment the following to see the right cube's texture
// applied in a manner similar to the left cube.  If you study
// polygon(), below, you will get a hint as to what's going on.
//#define BREAK_TEXTURE


// Comment-out the following to have automatic texture coordinates
// generated in eye-coordinates rather than object coordinates.
#define OBJECT_COORDS


#define TEX_IMAGE 0
#define MIP_MAP 1


// Base of the display lists.
GLuint lists;


// The right cube can be rotated.  Whoopee.
static GLfloat theta[] = {45.0,0.0,0.0};
static GLint axis = 2;
/* initial viewer location */
static GLdouble viewer[]= {20.0, 6.0, 6.0};
//static GLdouble viewer[]= {0.0, 0.0, 10.0};


// Light 0 is the moving light.  Light 1 is the spotlight that moves
// with us.  Note that these are positional lights.
GLfloat light0_position[] = { 0.0, 0.0, 5.0, 1.0 };
GLfloat light1_position[] = { 0.0, 0.0, 0.0, 1.0 };

// Angle of the moving light.
GLint light_angle = 0;


// For the textures.
GLuint texNames[4];


// Vertices for right cube.
GLfloat vertices[][3] = {{-1.0,-1.0,-1.0},{1.0,-1.0,-1.0},
                         {1.0,1.0,-1.0}, {-1.0,1.0,-1.0}, {-1.0,-1.0,1.0}, 
                         {1.0,-1.0,1.0}, {1.0,1.0,1.0}, {-1.0,1.0,1.0}};


// These are the real normals for the right cube, 1 per face.
GLfloat normals[][3] = {{0.0,0.0,-1.0},{0.0,1.0,0.0},
                        {-1.0,0.0,0.0},{1.0,0.0,0.0},{0.0,0.0,1.0},{0.0,-1.0,0.0}};


// Code for rendering the right cube.
void polygon(int a, int b, int c , int d, GLfloat normal[])
{
    glBegin(GL_POLYGON);
    glNormal3fv(normal);
    glTexCoord2d(0.0, 0.0);
    glVertex3fv(vertices[a]);
    glTexCoord2d(0.0, 1.0);
    glVertex3fv(vertices[b]);
#ifndef BREAK_TEXTURE
    glTexCoord2d(1.0, 1.0);
#endif
    glVertex3fv(vertices[c]);
#ifndef BREAK_TEXTURE
    glTexCoord2d(1.0, 0.0);
#else
    glTexCoord2d(0.0, 0.0);
#endif
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


// Using gltx, read the IRIS file and bind the image to a texture.

void getTexture(int *tName, char *fName, int mode)
{
	GLTXimage *image = gltxReadRGB(fName);

	if (!image)
        {
            printf("Problems with the image file: %s.\n",
                   fName);
            exit(1);
        }

    // Similar to glGenLists().
    glGenTextures(1, tName);

    // Similar to glNewList(), but there's nothing corresponding to
    // glEndList().
    glBindTexture(GL_TEXTURE_2D, *tName);

    // Tile the 2-D texture in both dimenstions.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Use quick and dirty magnification and minification.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_NEAREST);

    // glTexImage2D() only generates a single version of the texture.
    // gluBuild2dMipmaps() generates several scaled versions of the
    // texture.  It's preferable because it has fewer aliasing
    // problems.  On the other hand, it uses more texture memory.

    if (mode == TEX_IMAGE)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width,
                     image->height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                     image->data);
    else if (mode == MIP_MAP)
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image->width,
                          image->height, GL_RGB, GL_UNSIGNED_BYTE,
                          image->data);
    else
        {
            printf("Invalid texture mode.\n");
            exit(1);
        }

    gltxDelete(image);
}


void textureInit(void)
{
    /* Set texel storage format. */
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    /* Get the textures. */
    getTexture(texNames, "imgfile1.rgb", TEX_IMAGE);
    getTexture(texNames + 1, "imgfile2.rgb", MIP_MAP);
    getTexture(texNames + 2, "imgfile3.rgb", MIP_MAP);
    getTexture(texNames + 3, "imgfile4.rgb", MIP_MAP);

    /* Use modulated application to work with lighting, enable
     * automatic texture coordinate generation, and enable textures.
     */
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
#ifdef OBJECT_COORDS
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
#endif
    glEnable(GL_TEXTURE_2D);
}


// Paint the scene.
void display(void)
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_TEXTURE_2D);

    /* Update viewer position in modelview matrix */
    glLoadIdentity();

    gluLookAt(viewer[X],viewer[Y],viewer[Z], 0.0, 0.0, 0.0, 0.0, 1.0,
              0.0);

    // Position the moving light.  It will be positioned in world
    // coordinates.
    glPushMatrix();
    glRotatef(light_angle, 1.0, 0.0, 0.0);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

    // Render a solid sphere indicating the light's position.  Notice
    // how it's neither lit nor textured.

    glTranslatef(light0_position[X], light0_position[Y],
                 light0_position[Z]);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glCallList(lists);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    // Notice how we indicate which texture to use.  This texture
    // will be in use until we specify another or disable 2-D
    // textures.

    glBindTexture(GL_TEXTURE_2D, texNames[1]);

    glPushMatrix();

    // Left cube.
    glTranslatef(-2.0, 0.0, 0.0);
    glCallList(lists + 1);

    glBindTexture(GL_TEXTURE_2D, texNames[0]);

    // Sphere centered at origin.
    glTranslatef(2.0, 0.0, 0.0);
    glCallList(lists + 2);

    glBindTexture(GL_TEXTURE_2D, texNames[3]);

    // Disable automatic texture coordinate generation.
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);

    // Right cube.
    glTranslatef(2.0, 0.0, 0.0);
    // Rotate cube.
    glRotatef(theta[X], 1.0, 0.0, 0.0);
    glRotatef(theta[Y], 0.0, 1.0, 0.0);
    glRotatef(theta[Z], 0.0, 0.0, 1.0);
    colorcube();

    // Re-enable automatic texture coordinate generation.
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);

    glBindTexture(GL_TEXTURE_2D, texNames[2]);

    // Now that I'm thoroughly confused, let's re-establish the
    // origin.
    glPopMatrix();

    // The cone.
    // Up y-axis by 1.
    glTranslatef(0.0, 1.0, 0.0);
    // Rotate z-axis up to y-axis.
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    glCallList(lists + 3);

    glDisable(GL_TEXTURE_2D);

    // The torus.
    // Translate up the cone.
    glTranslatef(0.0, 0.0, 1.0);
    glCallList(lists + 4);

    glutSwapBuffers();
}


// Rotate the right cube.  Looks kinda neat when it cuts into the
// sphere.
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
    // No ambient light whatsoever.
    GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };

    GLfloat light1_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light1_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light1_specular[] = { 1.0, 1.0, 1.0, 1.0 };

    //glClearColor (0.2f, 0.3f, 0.4f, 1.0);
    glClearColor (0.0f, 0.0f, 0.0f, 1.0);
    glShadeModel (GL_SMOOTH);

    // Note use of linear attenuation values, the spot cutoff
    // half-angle, and the spot exponent (intensity dropoff
    // within the cone).

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    //glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.1f);

    // Specifying the light position here causes the position to be
    // interpreted in eye coordinates.  The default direction of the
    // spotlight is (0, 0, -1), which is also interpreted in eye
    // coordinates in this case.

    glLoadIdentity();
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

    glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
    //glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.1f);
    //glLighti(GL_LIGHT1, GL_SPOT_CUTOFF, 5);
    //glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 0.2f);

    // Declare the viewer to be local.

    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_DEPTH_TEST);

    textureInit();
}


int
main(int argc, char **argv)
{
    // Material properties for the torus.
    GLfloat mat_ambient[] = { 0.5, 0.0, 0.0, 1.0 };
    GLfloat mat_diffuse[] = { 1.0, 0.5, 0.5, 1.0 };
    GLfloat mat_specular[] = { 1.0, 0.5, 0.5, 1.0 };
    GLfloat mat_shininess[] = { 100.0 };

    setvbuf(stdout, (char *)NULL, _IONBF, 0);
    setvbuf(stderr, (char *)NULL, _IONBF, 0);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Textures Lab");
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

    // Default material values are used with the texture mapped
    // objects since that seems to look best.

    // For the left cube.
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

    // The torus.  Note how we restore and save the default material
    // values.

    glNewList(lists + 4, GL_COMPILE);
    glPushAttrib(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glutSolidTorus(0.25, 1.0, 32, 32);
    glPopAttrib();
    glEndList();

    glutMainLoop();

    return 0;
}
