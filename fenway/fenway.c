/*
Evan Kerns
CSCI: 4229
HW4
Fenway Park's Green Monster
*/

/*#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif*/
#include "CSCIx229.h"

int axes=0;       //  Display axes
int mode=0;       //  Projection mode
int move=1;       //  Move light
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
int fp=0;         //  First person
int light=1;      //  Lighting
double asp=1;     //  Aspect ratio
double dim=5.0;   //  Size of world

// Light values
int one       =   1;  // Unit value
int distance  =   5;  // Light distance
int inc       =  10;  // Ball increment
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
int ambient   =  30;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =   0;  // Elevation of light

int rot = 0.0; // FP Rotation
double Fx = 0;
double Fy = 0;
double Fz = 5;
double Cx = 0;
double Cz = 0;

typedef struct {
   double x;
   double y;
   double z;
} points_t;

/*
 *  Draw vertex in polar coordinates with normal
 */
static void Vertex(double th,double ph)
{
   double x = Sin(th)*Cos(ph);
   double y = Cos(th)*Cos(ph);
   double z =         Sin(ph);
   //  For a sphere at the origin, the position
   //  and normal vectors are the same
   glNormal3d(x,y,z);
   glVertex3d(x,y,z);
}

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 *
#define LEN 8192  //  Maximum length of text string
void Print(const char* format , ...)
{
   char    buf[LEN];
   char*   ch=buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}*/

/*
 *  Set projection
 */
static void eProject()
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   // First Person transformation
   // First Person code based off Github user: taylorjandrews
   if (fp)
   {
     gluPerspective(fov,asp,dim/4,4*dim);
   }
   else
   {
     //  Perspective transformation
     if (mode)
        gluPerspective(fov,asp,dim/4,4*dim);
     //  Orthogonal projection
     else
        glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
   }
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

/*
 *  Draw vertex in polar coordinates
 *
static void Vertex(double th,double ph)
{
   glColor3f(Cos(th)*Cos(th) , Sin(ph)*Sin(ph) , Sin(th)*Sin(th));
   glVertex3d(Sin(th)*Cos(ph) , Sin(ph) , Cos(th)*Cos(ph));
}*/

/*
 *  Draw a sphere (version 1)
 *     at (x,y,z)
 *     radius (r)
 */
static void sphere1(double x,double y,double z,double r)
{
   const int d=5;
   int th,ph;

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r,r,r);

   //  South pole cap
   glBegin(GL_TRIANGLE_FAN);
   Vertex(0,-90);
   for (th=0;th<=360;th+=d)
   {
      Vertex(th,d-90);
   }
   glEnd();

   //  Latitude bands
   for (ph=d-90;ph<=90-2*d;ph+=d)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=d)
      {
         Vertex(th,ph);
         Vertex(th,ph+d);
      }
      glEnd();
   }

   //  North pole cap
   glBegin(GL_TRIANGLE_FAN);
   Vertex(0,90);
   for (th=0;th<=360;th+=d)
   {
      Vertex(th,90-d);
   }
   glEnd();

   //  Undo transformations
   glPopMatrix();
}

/*
 *  Draw a ball
 *     at (x,y,z)
 *     radius (r)
 */
static void ball(double x,double y,double z,double r)
{
   int th,ph;
   float yellow[] = {1.0,1.0,0.0,1.0};
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //  White ball

   glMaterialf(GL_FRONT,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
   //  Bands of latitude
   for (ph=-90;ph<90;ph+=inc)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=2*inc)
      {
         Vertex(th,ph);
         Vertex(th,ph+inc);
      }
      glEnd();
   }
   //  Undo transofrmations
   glPopMatrix();
}

static void drawMonster(double x,double y,double z,
                        double dx,double dy,double dz,
                        double th)
{
  glPushMatrix();

  // Translations
  glTranslated(x, y, z);
  glRotated(th, 0, 1, 0);
  glScaled(dx, dy, dz);

  // Green Monster Sides
  glBegin(GL_QUADS);
  // Front
  glColor3ub(74,119,75);
  glVertex3f(-4, 0, 1);
  glVertex3f(-4, 3, 1);
  glVertex3f(7, 3, 1);
  glVertex3f(7, 0, 1);
  //  Back
  glVertex3f(-4,0,-1);
  glVertex3f(-4,3,-1);
  glVertex3f(7,3,-1);
  glVertex3f(7,0,-1);
  //  Right
  glVertex3f(7,3,1);
  glVertex3f(7,3,-1);
  glVertex3f(7,0,-1);
  glVertex3f(7,0,1);
  //  Left
  glVertex3f(-4,3,1);
  glVertex3f(-4,3,-1);
  glVertex3f(-4,0,-1);
  glVertex3f(-4,0,1);
  //  Top
  glColor3ub(75,75,75);
  glVertex3f(7,3,1);
  glVertex3f(-4,3,1);
  glVertex3f(-4,3,-1);
  glVertex3f(7,3,-1);
  //  Bottom
  glVertex3f(7,0,1);
  glVertex3f(-4,0,1);
  glVertex3f(-4,0,-1);
  glVertex3f(7,0,-1);
  glEnd();

  // Monster Pole
  glBegin(GL_QUADS);
  // Front
  glColor3ub(250,250,10);
  glVertex3f(4,1.4,1.1);
  glVertex3f(4.1,1.4,1.1);
  glVertex3f(4.1,3,1.1);
  glVertex3f(4,3,1.1);
  // Right
  glVertex3f(4.1,1.4,1.1);
  glVertex3f(4.1,1.4,1);
  glVertex3f(4.1,3,1);
  glVertex3f(4.1,3,1.1);
  // Left
  glVertex3f(4,1.4,1);
  glVertex3f(4,1.4,1.1);
  glVertex3f(4,3,1.1);
  glVertex3f(4,3,1);
  // Top
  glVertex3f(4, 3, 1.1);
  glVertex3f(4.1, 3, 1.1);
  glVertex3f(4.1, 3, 1);
  glVertex3f(4, 3, 1);
  // Bottom
  glVertex3f(4, 1.4, 1.1);
  glVertex3f(4.1, 1.4, 1.1);
  glVertex3f(4.1, 1.4, 1);
  glVertex3f(4, 1.4, 1);
  glEnd();


  // Seats
  glBegin(GL_QUADS);
  // Front (Seats)
  glColor3ub(75,75,75);
  glVertex3f(4,3,1);
  glVertex3f(4,4,-1);
  glVertex3f(-4,4,-1);
  glVertex3f(-4,3,1);
  // Back
  glColor3ub(74,119,75);
  glVertex3f(4,3,-1);
  glVertex3f(4,4,-1);
  glVertex3f(-4,4,-1);
  glVertex3f(-4,3,-1);
  // Bottom
  glVertex3f(4,3,1);
  glVertex3f(4,3,-1);
  glVertex3f(-4,3,-1);
  glVertex3f(-4,3,1);

  // Seat Wall
  // Front
  glColor3ub(74,119,75);
  glVertex3f(4,3.4,1);
  glVertex3f(4,3,1);
  glVertex3f(-4,3,1);
  glVertex3f(-4,3.4,1);
  // Right
  glVertex3f(4,3.4,1);
  glVertex3f(4,3,1);
  glVertex3f(4,3,.9);
  glVertex3f(4,3.4,.9);
  // Left
  glVertex3f(-4,3.4,1);
  glVertex3f(-4,3,1);
  glVertex3f(-4,3,.9);
  glVertex3f(-4,3.4,.9);
  // Back
  glColor3ub(50,50,50); ////////// change to dark green
  glVertex3f(4,3.4,.9);
  glVertex3f(4,3,.9);
  glVertex3f(-4,3,.9);
  glVertex3f(-4,3.4,.9);
  // Top
  glColor3ub(150,150,150); ///////// change to yellow
  glVertex3f(4,3.4,1);
  glVertex3f(4,3.4,.9);
  glVertex3f(-4,3.4,.9);
  glVertex3f(-4,3.4,1);
  // Bottom
  glColor3ub(74,119,75);
  glVertex3f(4,3,1);
  glVertex3f(4,3,.9);
  glVertex3f(-4,3,.9);
  glVertex3f(-4,3,1);
  glEnd();

  // Seats Sides
  glBegin(GL_TRIANGLES);
  // Right
  glColor3ub(130,130,130);
  glVertex3f(4,3,1);
  glVertex3f(4,3,-1);
  glVertex3f(4,4,-1);
  // Left
  glColor3ub(230,230,230);
  glVertex3f(-4,3,1);
  glVertex3f(-4,3,-1);
  glVertex3f(-4,4,-1);
  glEnd();

  // Foul Pole
  glBegin(GL_QUADS);
  // Front
  glColor3ub(250,250,10);
  glVertex3f(-4,0,1.2);
  glVertex3f(-4.2,0,1.2);
  glVertex3f(-4.2,4.5,1.2);
  glVertex3f(-4,4.5,1.2);
  // Right
  glVertex3f(-4,0,1.2);
  glVertex3f(-4,0,1);
  glVertex3f(-4,4.5,1);
  glVertex3f(-4,4.5,1.2);
  // Left
  glVertex3f(-4.2,0,1);
  glVertex3f(-4.2,0,1.2);
  glVertex3f(-4.2,4.5,1.2);
  glVertex3f(-4.2,4.5,1);
  // Back
  glVertex3f(-4,0,1);
  glVertex3f(-4.2,0,1);
  glVertex3f(-4.2,4.5,1);
  glVertex3f(-4,4.5,1);
  // Top
  glVertex3f(-4,4.5,1.2);
  glVertex3f(-4.2,4.5,1.2);
  glVertex3f(-4.2,4.5,1);
  glVertex3f(-4,4.5,1);
  // Bottom
  glVertex3f(-4,0,1.2);
  glVertex3f(-4.2,0,1.2);
  glVertex3f(-4.2,0,1);
  glVertex3f(-4,0,1);
  glEnd();

  glPopMatrix();
}

static void drawOutfield(double x,double y,double z,
                        double dx,double dy,double dz,
                        double th)
{
  glPushMatrix();

  // Translations
  glTranslated(x, y, z);
  glRotated(th, 0, 1, 0);
  glScaled(dx, dy, dz);


  // Center Field Wall
  glBegin(GL_QUADS);
  // Center Field Wall
  glColor3ub(74,119,75);
  glVertex3f(4, 0, 1);
  glVertex3f(4, 1.4, 1);
  glVertex3f(9, 1.4, 3);
  glVertex3f(9, 0, 3);
  //  Center Field Wall Extended
  glVertex3f(9,0,3);
  glVertex3f(9,1.4,3);
  glVertex3f(11,1.4,3.8);
  glVertex3f(11,0,3.8);
  glEnd();

  // Center Field Seats
  glBegin(GL_TRIANGLES);
  // Top
  glColor3ub(130,130,130);
  glVertex3f(4,1.4,1);
  glVertex3f(11,1.4,3.8);
  glVertex3f(11,3,1);
  // Bottom
  glColor3ub(230,230,230);
  glVertex3f(4,0,1);
  glVertex3f(11,0,3.8);
  glVertex3f(11,0,1);
  // Lower Center Seats
  // Top
  glColor3ub(130,130,130);
  glVertex3f(9,.6,3);
  glVertex3f(10,.6,6.5);
  glVertex3f(11,1.4,3.8);
  // Bottom
  glColor3ub(230,230,230);
  glVertex3f(9,0,3);
  glVertex3f(10,0,6.5);
  glVertex3f(11,0,3.8);
  //
  glEnd();

  // Center Field Wall
  glBegin(GL_QUADS);
  //  Center divider
  glColor3ub(153,100,0);
  glVertex3f(11,0,3.8);
  glVertex3f(11,1.4,3.8);
  glVertex3f(11,3,1);
  glVertex3f(11,0,1);
  //  Monster Extension
  glColor3ub(74,119,75);
  glVertex3f(7,0,1);
  glVertex3f(7,3,1);
  glVertex3f(11,3,1);
  glVertex3f(11,0,1);
  //  Triangle Wall
  glVertex3f(9,0,3);
  glVertex3f(9,.6,3);
  glVertex3f(10,.6,6.5);
  glVertex3f(10,0,6.5);
  //  Right Field Wall
  glVertex3f(10,0,6.5);
  glVertex3f(10,.6,6.5);
  glVertex3f(14,.6,12);
  glVertex3f(14,0,12);
  // Corner Wall
  glVertex3f(14,0,12);
  glVertex3f(14,.6,12);
  glVertex3f(13,.6,14);
  glVertex3f(13,0,14);
  // Right Field Lower Seats
  glColor3ub(230,230,230);
  glVertex3f(11,1.4,3.8);
  glVertex3f(10,.6,6.5);
  glVertex3f(14,.6,12);
  glVertex3f(15,1.4,9.3);
  // Right Field Upper Seats
  glColor3ub(30,30,30);
  glVertex3f(11,1.4,3.8);
  glVertex3f(11,3,1);
  glVertex3f(16,3,6.6);
  glVertex3f(15,1.4,9.3);
  // Back
  glColor3ub(74,119,75);
  glVertex3f(11,0,1);
  glVertex3f(11,3,1);
  glVertex3f(16,3,6.6);
  glVertex3f(16,0,6.6);
  // Upper Side
  glColor3ub(130,130,130);
  glVertex3f(15,0,9.3);
  glVertex3f(15,1.4,9.3);
  glVertex3f(16,3,6.6);
  glVertex3f(16,0,6.6);
  // Lower Side
  glColor3ub(190,130,30);
  glVertex3f(15,0,9.3);
  glVertex3f(15,1.4,9.3);
  glVertex3f(14,.6,12);
  glVertex3f(14,0,12);
  // Right Field Lower Seats Floor
  glColor3ub(230,30,230);
  glVertex3f(11,0,3.8);
  glVertex3f(10,0,6.5);
  glVertex3f(14,0,12);
  glVertex3f(15,0,9.3);
  // Right Field Upper Seats Floor
  glColor3ub(30,230,30);
  glVertex3f(11,0,3.8);
  glVertex3f(11,0,1);
  glVertex3f(16,0,6.6);
  glVertex3f(15,0,9.3);
  glEnd();

  // Triangle Pole
  glBegin(GL_QUADS);
  // Front
  glColor3ub(250,250,10);
  glVertex3f(9, 1.4, 3.1);
  glVertex3f(9.1, 1.4, 3.1);
  glVertex3f(9.1, .6, 3.1);
  glVertex3f(9, .6, 3.1);
  // Right
  glVertex3f(9.1, 1.4, 3.1);
  glVertex3f(9.1, 1.4, 3);
  glVertex3f(9.1, .6, 3);
  glVertex3f(9.1, .6, 3.1);
  // Left
  glVertex3f(9, 1.4, 3);
  glVertex3f(9, 1.4, 3.1);
  glVertex3f(9, .6, 3.1);
  glVertex3f(9, .6, 3);
  // Bottom
  glVertex3f(9, .6, 3.1);
  glVertex3f(9.1, .6, 3.1);
  glVertex3f(9.1, .6, 3);
  glVertex3f(9, .6, 3);
  // Top
  glVertex3f(9, 1.4, 3.1);
  glVertex3f(9.1, 1.4, 3.1);
  glVertex3f(9.1, 1.4, 3);
  glVertex3f(9, 1.4, 3);
  glEnd();

  // Pesky Pole
  glBegin(GL_QUADS);
  // Front
  glColor3ub(250,250,10);
  glVertex3f(13,0,14);
  glVertex3f(13.2,0,14);
  glVertex3f(13.2,4.5,14);
  glVertex3f(13,4.5,14);
  // Right
  glColor3ub(150,250,10);
  glVertex3f(13,0,14.2);
  glVertex3f(13,0,14);
  glVertex3f(13,4.5,14);
  glVertex3f(13,4.5,14.2);
  // Left
  glColor3ub(250,150,10);
  glVertex3f(13.2,0,14);
  glVertex3f(13.2,0,14.2);
  glVertex3f(13.2,4.5,14.2);
  glVertex3f(13.2,4.5,14);
  // Back
  glColor3ub(250,250,190);
  glVertex3f(13,0,14);
  glVertex3f(13.2,0,14);
  glVertex3f(13.2,4.5,14);
  glVertex3f(13,4.5,14);
  // Top
  glColor3ub(50,250,30);
  glVertex3f(13,4.5,14.2);
  glVertex3f(13.2,4.5,14.2);
  glVertex3f(13.2,4.5,14);
  glVertex3f(13,4.5,14);
  // Bottom
  glColor3ub(250,40,210);
  glVertex3f(13,0,14.2);
  glVertex3f(13.2,0,14.2);
  glVertex3f(13.2,0,14);
  glVertex3f(13,0,14);
  glEnd();

  glPopMatrix();
}

static void drawField(double x,double y,double z,
                        double dx,double dy,double dz,
                        double th)
{
  glPushMatrix();

  // Translations
  glTranslated(x, y, z);
  glRotated(th, 0, 1, 0);
  glScaled(dx, dy, dz);

  // Warning Track
  glBegin(GL_QUADS);
  // Monster Track
  glColor3ub(120,72,0);
  glVertex3f(-4, 0, 1);
  glVertex3f(4, 0, 1);
  glVertex3f(4, 0, 2);
  glVertex3f(-4, 0, 2);
  //  Center Track
  glVertex3f(4, 0, 1);
  glVertex3f(4, 0, 2);
  glVertex3f(9, 0, 4);
  glVertex3f(9, 0, 3);
  // Triangle Track
  glVertex3f(9,0,3);
  glVertex3f(8,0,3);
  glVertex3f(9,0,6.5);
  glVertex3f(10,0,6.5);
  //  Right Field Track
  glVertex3f(10,0,6.5);
  glVertex3f(9,0,6.5);
  glVertex3f(13,0,12);
  glVertex3f(14,0,12);
  // Corner Track
  glVertex3f(14,0,12);
  glVertex3f(13,0,12);
  glVertex3f(12,0,14);
  glVertex3f(13,0,14);
  glEnd();

  // Foul Lines
  glBegin(GL_QUADS);
  // Left Field
  glColor3ub(240,240,240);
  glVertex3f(-4, 0, 1);
  glVertex3f(-4.2, 0, 1);
  glVertex3f(-4.2, 0, 14);
  glVertex3f(-4, 0, 14);
  //  Right Field
  glVertex3f(13, 0, 14);
  glVertex3f(-4.2, 0, 14);
  glVertex3f(-4.2, 0, 14.2);
  glVertex3f(13, 0, 14.2);
  glEnd();

  // Grass
  glBegin(GL_TRIANGLES);
  // Monster - Home
  glColor3ub(0,123,12);
  glVertex3f(-4,0,2);
  glVertex3f(4,0,2);
  glVertex3f(-4,0,14);
  // Center - Home
  glVertex3f(4,0,2);
  glVertex3f(9,0,4);
  glVertex3f(-4,0,14);
  // Triangle - Home
  glVertex3f(9,0,6.5);
  glVertex3f(8,0,3);
  glVertex3f(-4,0,14);
  // Right Field - Home
  glVertex3f(9,0,6.5);
  glVertex3f(13,0,12);
  glVertex3f(-4,0,14);
  // Corner - Home
  glVertex3f(13,0,12);
  glVertex3f(12,0,14);
  glVertex3f(-4,0,14);
  glEnd();

  glPopMatrix();
}
/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   const double len=1.5;  //  Length of axes
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   glLoadIdentity();
   if (fp)
   {
     Cx = +.5*dim*Sin(rot);
     Cz = -.5*dim*Cos(rot);

     gluLookAt(Fx,Fy,Fz,Cx+Fx,Fy,Cz+Fz, 0,1,0);
   }
   else
   {
     //  Perspective - set eye position
     if (mode)
     {
        double Ex = -2*dim*Sin(th)*Cos(ph);
        double Ey = +2*dim        *Sin(ph);
        double Ez = +2*dim*Cos(th)*Cos(ph);
        gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
     }
     //  Orthogonal - set world orientation
     else
     {
        glRotatef(ph,1,0,0);
        glRotatef(th,0,1,0);
     }
   }

   //  Flat or smooth shading
   glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);

   //  Light switch
   if (light)
   {
        //  Translate intensity to color vectors
        float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
        float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
        float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
        //  Light position
        float Position[]  = {distance*Cos(zh),ylight,distance*Sin(zh),1.0};
        //  Draw light position as ball (still no lighting here)
        glColor3f(1,1,1);
        ball(Position[0],Position[1],Position[2] , 0.1);
        //  OpenGL should normalize normal vectors
        glEnable(GL_NORMALIZE);
        //  Enable lighting
        glEnable(GL_LIGHTING);
        //  Location of viewer for specular calculations
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
        //  glColor sets ambient and diffuse color materials
        glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);
        //  Enable light 0
        glEnable(GL_LIGHT0);
        //  Set ambient, diffuse, specular components and position of light 0
        glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
        glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
        glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
        glLightfv(GL_LIGHT0,GL_POSITION,Position);
   }
   else
     glDisable(GL_LIGHTING);

   //  Decide what to draw
   drawMonster(0,-.25,0, .25,.25,.25, 0);
   drawOutfield(0,-.25,0, .25,.25,.25, 0);
   drawField(0,-.25,0, .25,.25,.25, 0);
   sphere1(0,2,2,.1);

   //  Draw axes - no lighting from here on
   glDisable(GL_LIGHTING);
   //  White
   glColor3f(1,1,1);
   //  Draw axes
   if (axes)
   {
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();
      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
   }
  //  Display parameters
   glWindowPos2i(5,5);
   Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s Light=%s",
     th,ph,dim,fov,mode?"Perpective":"Orthogonal",light?"On":"Off");
   if (light)
   {
      glWindowPos2i(5,45);
      Print("Model=%s LocalViewer=%s Distance=%d Elevation=%.1f",smooth?"Smooth":"Flat",local?"On":"Off",distance,ylight);
      glWindowPos2i(5,25);
      Print("Ambient=%d  Diffuse=%d Specular=%d Emission=%d Shininess=%.0f",ambient,diffuse,specular,emission,shiny);
   }

   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
  if (!fp)
  {
    //  Right arrow key - increase angle by 5 degrees
    if (key == GLUT_KEY_RIGHT)
       th += 5;
    //  Left arrow key - decrease angle by 5 degrees
    else if (key == GLUT_KEY_LEFT)
       th -= 5;
    //  Up arrow key - increase elevation by 5 degrees
    else if (key == GLUT_KEY_UP)
       ph += 5;
    //  Down arrow key - decrease elevation by 5 degrees
    else if (key == GLUT_KEY_DOWN)
       ph -= 5;
       //  PageUp key - increase dim
       else if (key == GLUT_KEY_PAGE_UP)
          dim += 0.1;
       //  PageDown key - decrease dim
       else if (key == GLUT_KEY_PAGE_DOWN && dim>1)
          dim -= 0.1;
    //  Keep angles to +/-360 degrees
    th %= 360;
    ph %= 360;
  }
   //  Update projection
   eProject();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void idle()
{
   //  Elapsed time in seconds
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*t,360.0);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}


/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
  //  Exit on ESC
  if (ch == 27)
     exit(0);
  //  Toggle axes
  else if (ch == 'a' || ch == 'A')
     axes = 1-axes;
  // Toggle first person
  else if (ch == 'f' || ch == 'F')
  {
    fp = 1-fp;
  }

  if (fp)
  {
    double dt = 0.05;
    if (ch == 'w' || ch == 'W')
    {
      Fx += Cx*dt;
      Fz += Cz*dt;
    }
    else if (ch == 'a' || ch == 'A')
    {
      rot -= 2;
    }
    else if (ch == 's' || ch == 'S')
    {
      Fx -= Cx*dt;
      Fz -= Cz*dt;
    }
    else if (ch == 'd' || ch == 'D')
    {
      rot += 2;
    }

    rot %= 360;
  }

  else
  {
    //  Reset view angle
    if (ch == '0')
       th = ph = 0;
    //  Switch display mode
    else if (ch == 'm' || ch == 'M')
        mode = 1-mode;
    //  Change field of view angle
    else if (ch == '-' && ch>1)
       fov--;
    else if (ch == '+' && ch<179)
       fov++;
  }

  //  Reproject
  eProject();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
  //  Ratio of the width to the height of the window
  asp = (height>0) ? (double)width/height : 1;
  //  Set the viewport to the entire window
  glViewport(0,0, width,height);
  //  Set projection
  eProject();
  /* const double dim=2.5;
   //  Ratio of the width to the height of the window
   double w2h = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Orthogonal projection
   glOrtho(-w2h*dim,+w2h*dim, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();*/
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitWindowSize(600,600);
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   //  Create the window
   glutCreateWindow("Evan Kerns - HW6");
   //  Tell GLUT to call "display" when the scene should be drawn
   glutDisplayFunc(display);
   //  Tell GLUT to call "reshape" when the window is resized
   glutReshapeFunc(reshape);
   //  Tell GLUT to call "special" when an arrow key is pressed
   glutSpecialFunc(special);
   //  Tell GLUT to call "key" when a key is pressed
   glutKeyboardFunc(key);
   glutIdleFunc(idle);
   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   return 0;
}
