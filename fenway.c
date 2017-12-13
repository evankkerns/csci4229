/*
Evan Kerns
CSCI: 4229
Final Project
Fenway Park
*/

#include "CSCIx229.h"

int axes=0;       //  Display axes
int mode=0;       //  Projection mode
int move=1;       //  Move light
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
int fp=1;         //  First person
int EvansSeats=0; //  Shows where Evan has sat at Fenway
int light=0;      //  Lighting
double asp=1;     //  Aspect ratio
double dim=5.0;   //  Size of world

// Light values
int one       =   1;  // Unit value
int distance  =   5;  // Light distance
int inc       =   1;  // Ball increment
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

unsigned int texture[12];

int rot = 0.0; // FP Rotation
double Fx = 0;
double Fy = 0;
double Fz = 5;
double Cx = 0;
double Cy = 0;
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

   glColor3ub(250,250,250);
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

  //  Enable textures
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
  //glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
   glColor3f(1,1,1);
  glBindTexture(GL_TEXTURE_2D,texture[0]);


  // Fenway Textures [0]
  glBegin(GL_QUADS);
  // Front - Green Monster
  glNormal3f(0, 0, 1);
  glColor3ub(74,119,75);
  glTexCoord2f(0, 0); glVertex3f(-7.5, 0, 1);
  glTexCoord2f(1, 0); glVertex3f(-7.5, 3, 1);
  glTexCoord2f(1, 1); glVertex3f(7, 3, 1);
  glTexCoord2f(0, 1); glVertex3f(7, 0, 1);

  // Front - Seat Wall
  glTexCoord2f(0, 0); glVertex3f(4,3.4,1);
  glTexCoord2f(1, 0); glVertex3f(4,3,1);
  glTexCoord2f(1, 1); glVertex3f(-4,3,1);
  glTexCoord2f(0, 1); glVertex3f(-4,3.4,1);

  glEnd();
  glDisable(GL_TEXTURE_2D);

  // Monster Scoreboard [9]
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,texture[9]);
  glBegin(GL_QUADS);
  // Scoreboard
  glNormal3f(0, 0, 1);
  //glColor3ub(74,119,75);
  glTexCoord2f(0, 0); glVertex3f(-2.5, .2, 1.05);
  glTexCoord2f(0, 1); glVertex3f(-2.5, 1.5, 1.05);
  glTexCoord2f(1, 1); glVertex3f(2.5, 1.5, 1.05);
  glTexCoord2f(1, 0); glVertex3f(2.5, .2, 1.05);

  glEnd();
  glDisable(GL_TEXTURE_2D);


  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,texture[1]);
  // Seat Textures [1]
  glBegin(GL_QUADS);
  // Front (Seats)
  glNormal3f(0, .5, .75);
  glColor3ub(75,75,75);
  glTexCoord2f(0, 0); glVertex3f(4,3,1);
  glTexCoord2f(0, 1); glVertex3f(4,4,-1);
  glTexCoord2f(1, 1); glVertex3f(-4,4,-1);
  glTexCoord2f(1, 0); glVertex3f(-4,3,1);

  glEnd();
  glDisable(GL_TEXTURE_2D);

  // Monster Pole
  glBegin(GL_QUADS);
  // Front
  glNormal3f(0, 0, 1);
  glColor3ub(250,250,10);
  glVertex3f(4,1.4,1.1);
  glVertex3f(4.1,1.4,1.1);
  glVertex3f(4.1,3,1.1);
  glVertex3f(4,3,1.1);
  // Right
  glNormal3f(-1, 0, 0);
  glVertex3f(4.1,1.4,1.1);
  glVertex3f(4.1,1.4,1);
  glVertex3f(4.1,3,1);
  glVertex3f(4.1,3,1.1);
  // Left
  glNormal3f(1, 0, 0);
  glVertex3f(4,1.4,1);
  glVertex3f(4,1.4,1.1);
  glVertex3f(4,3,1.1);
  glVertex3f(4,3,1);
  // Top
  glNormal3f(0, 1, 0);
  glVertex3f(4, 3, 1.1);
  glVertex3f(4.1, 3, 1.1);
  glVertex3f(4.1, 3, 1);
  glVertex3f(4, 3, 1);
  // Bottom
  glNormal3f(0, -1, 0);
  glVertex3f(4, 1.4, 1.1);
  glVertex3f(4.1, 1.4, 1.1);
  glVertex3f(4.1, 1.4, 1);
  glVertex3f(4, 1.4, 1);
  glEnd();


  // Seats
  glBegin(GL_QUADS);

  // Back
  glNormal3f(0, 0, -1);
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
  glColor3ub(74,119,75);
  // Right
  glNormal3f(-1, 0, 0);
  glVertex3f(4,3.4,1);
  glVertex3f(4,3,1);
  glVertex3f(4,3,.9);
  glVertex3f(4,3.4,.9);
  // Left
  glNormal3f(1, 0, 0);
  glVertex3f(-4,3.4,1);
  glVertex3f(-4,3,1);
  glVertex3f(-4,3,.9);
  glVertex3f(-4,3.4,.9);
  // Back
  glNormal3f(0, 0, -1);
  glVertex3f(4,3.4,.9);
  glVertex3f(4,3,.9);
  glVertex3f(-4,3,.9);
  glVertex3f(-4,3.4,.9);
  // Top
  glNormal3f(0, 1, 0);
  glColor3ub(250,250,10);
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
  glColor3ub(130,130,130);
  // Right
  glNormal3f(-1, 0, 0);
  glColor3ub(130,130,130);
  glVertex3f(4,3,1);
  glVertex3f(4,3,-1);
  glVertex3f(4,4,-1);
  // Left
  glNormal3f(1, 0, 0);
  glVertex3f(-4,3,1);
  glVertex3f(-4,3,-1);
  glVertex3f(-4,4,-1);
  glEnd();

  // Foul Pole
  glBegin(GL_QUADS);
  // Front
  glColor3ub(250,250,10);
  glNormal3f(0, 0, 1);
  glVertex3f(-4,0,1.2);
  glVertex3f(-4.2,0,1.2);
  glVertex3f(-4.2,4.5,1.2);
  glVertex3f(-4,4.5,1.2);
  // Right
  glNormal3f(-1, 0, 0);
  glVertex3f(-4,0,1.2);
  glVertex3f(-4,0,1);
  glVertex3f(-4,4.5,1);
  glVertex3f(-4,4.5,1.2);
  // Left
  glNormal3f(1, 0, 0);
  glVertex3f(-4.2,0,1);
  glVertex3f(-4.2,0,1.2);
  glVertex3f(-4.2,4.5,1.2);
  glVertex3f(-4.2,4.5,1);
  // Back
  glNormal3f(0, 0, -1);
  glVertex3f(-4,0,1);
  glVertex3f(-4.2,0,1);
  glVertex3f(-4.2,4.5,1);
  glVertex3f(-4,4.5,1);
  // Top
  glNormal3f(0, 1, 0);
  glVertex3f(-4,4.5,1.2);
  glVertex3f(-4.2,4.5,1.2);
  glVertex3f(-4.2,4.5,1);
  glVertex3f(-4,4.5,1);
  // Bottom
  glNormal3f(0, -1, 0);
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

  //  Enable textures
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
  //glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
   glColor3f(1,1,1);
  glBindTexture(GL_TEXTURE_2D,texture[0]);

  // Fenway Textures [0]
  glBegin(GL_QUADS);
  glColor3ub(74,119,75);
  // Center Field Wall
  glNormal3f(.25, 0, .75);
  glTexCoord2f(0, 0); glVertex3f(4, 0, 1);
  glTexCoord2f(1, 0); glVertex3f(4, 1.4, 1);
  glTexCoord2f(1, 1); glVertex3f(9, 1.4, 3);
  glTexCoord2f(0, 1); glVertex3f(9, 0, 3);
  //  Center Field Wall Extended
  glNormal3f(.25, 0, .75);
  glTexCoord2f(0, 0); glVertex3f(9,0,3);
  glTexCoord2f(1, 0); glVertex3f(9,1.4,3);
  glTexCoord2f(1, 1); glVertex3f(11,1.4,3.8);
  glTexCoord2f(0, 1); glVertex3f(11,0,3.8);
  //  Center divider
  glTexCoord2f(0, 0); glVertex3f(11,0,3.8);
  glTexCoord2f(1, 0); glVertex3f(11,1.4,3.8);
  glTexCoord2f(1, 1); glVertex3f(11,3,1);
  glTexCoord2f(0, 1); glVertex3f(11,0,1);
  //  Monster Extension
  glNormal3f(0, 0, 1);
  glTexCoord2f(0, 0); glVertex3f(7,0,1);
  glTexCoord2f(1, 0); glVertex3f(7,3,1);
  glTexCoord2f(1, 1); glVertex3f(11,3,1);
  glTexCoord2f(0, 1); glVertex3f(11,0,1);
  //  Triangle Wall
  glNormal3f(.9, 0, .1);
  glTexCoord2f(0, 0); glVertex3f(9,0,3);
  glTexCoord2f(1, 0); glVertex3f(9,.6,3);
  glTexCoord2f(1, 1); glVertex3f(10,.6,6.5);
  glTexCoord2f(0, 1); glVertex3f(10,0,6.5);
  // Right Field Wall
  glNormal3f(.5, 0, .5);
  glTexCoord2f(0, 0); glVertex3f(10,0,6.5);
  glTexCoord2f(1, 0); glVertex3f(10,.6,6.5);
  glTexCoord2f(1, 1); glVertex3f(13.5,.6,13);
  glTexCoord2f(0, 1); glVertex3f(13.5,0,13);
  // Corner Wall
  glNormal3f(.5, 0, -.5);
  glTexCoord2f(0, 0); glVertex3f(13.5,0,13);
  glTexCoord2f(1, 0); glVertex3f(13.5,.6,13);
  glTexCoord2f(1, 1); glVertex3f(13,.6,14);
  glTexCoord2f(0, 1); glVertex3f(13,0,14);

  glEnd();
  glDisable(GL_TEXTURE_2D);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,texture[1]);

  // Center Field Wall
  glBegin(GL_QUADS);

  // Right Field Lower Seats
  glColor3ub(75,75,75);
  glNormal3f(.3, .8, 1);
  glTexCoord2f(0, 0); glVertex3f(11,1.4,3.8);
  glTexCoord2f(0, 1); glVertex3f(10,.6,6.5);
  glTexCoord2f(1, 1); glVertex3f(13.5,.6,13);
  glTexCoord2f(1, 0); glVertex3f(15,1.4,9.3);
  // Right Field Lower Seats - Extended
  glTexCoord2f(0, 0); glVertex3f(13.5,.6,13);
  glTexCoord2f(1, 0); glVertex3f(15,1.4,9.3);
  glTexCoord2f(1, 1); glVertex3f(15,1.4,9.3);
  glTexCoord2f(0, 1); glVertex3f(15,1.4,12);
  // Right Field Lower Seats - Corner
  glTexCoord2f(0, 0); glVertex3f(14,.6,12);
  glTexCoord2f(0, 1); glVertex3f(15,1.4,12);
  glTexCoord2f(1, 1); glVertex3f(15,1.4,14);
  glTexCoord2f(1, 0); glVertex3f(13,.6,14);
  // Right Field Upper Seats
  glTexCoord2f(0, 0); glVertex3f(11,1.4,3.8);
  glTexCoord2f(0, 1); glVertex3f(11,3,1);
  glTexCoord2f(1, 1); glVertex3f(16,3,6.6);
  glTexCoord2f(1, 0); glVertex3f(15,1.4,9.3);
  // Right Field Upper Seats - Extended
  glTexCoord2f(0, 0); glVertex3f(16,3,6.6);
  glTexCoord2f(0, 1); glVertex3f(15,1.4,9.3);
  glTexCoord2f(1, 1); glVertex3f(15,1.4,12);
  glTexCoord2f(1, 0); glVertex3f(16,3,12);
  // Right Field Upper Seats - Corner
  glTexCoord2f(0, 0); glVertex3f(16,3,14);
  glTexCoord2f(0, 1); glVertex3f(15,1.4,14);
  glTexCoord2f(1, 1); glVertex3f(15,1.4,12);
  glTexCoord2f(1, 0); glVertex3f(16,3,12);
  // Center Field Seats
  glTexCoord2f(0, 0); glVertex3f(4,1.4,1);
  glTexCoord2f(0, 1); glVertex3f(11,1.4,3.8);
  glTexCoord2f(1, 1); glVertex3f(11,1.4,3.8);
  glTexCoord2f(1, 0); glVertex3f(11,3,1);
  // Lower Center Seats
  glTexCoord2f(0, 0); glVertex3f(9,.6,3);
  glTexCoord2f(0, 1); glVertex3f(10,.6,6.5);
  glTexCoord2f(1, 1); glVertex3f(11,1.4,3.8);
  glTexCoord2f(1, 0); glVertex3f(11,1.4,3.8);

  glEnd();
  glDisable(GL_TEXTURE_2D);

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
  glVertex3f(12.8,0,14);
  glVertex3f(13,0,14);
  glVertex3f(13,4.5,14);
  glVertex3f(12.8,4.5,14);
  // Right
  glVertex3f(12.8,0,14.2);
  glVertex3f(12.8,0,14);
  glVertex3f(12.8,4.5,14);
  glVertex3f(12.8,4.5,14.2);
  // Left
  glVertex3f(13,0,14);
  glVertex3f(13,0,14.2);
  glVertex3f(13,4.5,14.2);
  glVertex3f(13,4.5,14);
  // Back
  glVertex3f(12.8,0,14);
  glVertex3f(13,0,14);
  glVertex3f(13,4.5,14);
  glVertex3f(12.8,4.5,14);
  // Top
  glVertex3f(12.8,4.5,14.2);
  glVertex3f(13,4.5,14.2);
  glVertex3f(13,4.5,14);
  glVertex3f(12.8,4.5,14);
  // Bottom
  glVertex3f(12.8,0,14.2);
  glVertex3f(13,0,14.2);
  glVertex3f(13,0,14);
  glVertex3f(12.8,0,14);
  glEnd();

  glPopMatrix();
}

static void drawFoulWalls(double x,double y,double z,
                        double dx,double dy,double dz,
                        double th)
{
  glPushMatrix();

  // Translations
  glTranslated(x, y, z);
  glRotated(th, 0, 1, 0);
  glScaled(dx, dy, dz);

  //  Enable textures
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
  glColor3f(1,1,1);
  glBindTexture(GL_TEXTURE_2D,texture[0]);

  // Right Field Foul
  glBegin(GL_QUADS);
  glColor3ub(74,119,75);
  // Front
  glNormal3f(0, 0, -1);
  glTexCoord2f(0, 0); glVertex3f(13, 0, 14);
  glTexCoord2f(1, 0); glVertex3f(13, .6, 14);
  glTexCoord2f(1, 1); glVertex3f(7, .6, 17);
  glTexCoord2f(0, 1); glVertex3f(7, 0, 17);
  // Front - 1st Base Dugout
  glTexCoord2f(0, 0); glVertex3f(-3.75, 0, 17);
  glTexCoord2f(1, 0); glVertex3f(-3.75, .6, 17);
  glTexCoord2f(1, 1); glVertex3f(7, .6, 17);
  glTexCoord2f(0, 1); glVertex3f(7, 0, 17);
  // Front - Home Plate
  glNormal3f(1, 0, -1);
  glTexCoord2f(0, 0); glVertex3f(-3.75, 0, 17);
  glTexCoord2f(1, 0); glVertex3f(-3.75, .6, 17);
  glTexCoord2f(1, 1); glVertex3f(-7, .6, 13.75);
  glTexCoord2f(0, 1); glVertex3f(-7, 0, 13.75);
  // Front - 3rd Base Line
  glNormal3f(-1, 0, 0);
  glTexCoord2f(0, 0); glVertex3f(-7, 0, 13.75);
  glTexCoord2f(1, 0); glVertex3f(-7, .6, 13.75);
  glTexCoord2f(1, 1); glVertex3f(-7, .6, 8);
  glTexCoord2f(0, 1); glVertex3f(-7, 0, 8);
  // Angle Wall - 3rd Base Line
  glTexCoord2f(0, 0); glVertex3f(-7, 0, 8);
  glTexCoord2f(1, 0); glVertex3f(-7, .6, 8);
  glTexCoord2f(1, 1); glVertex3f(-4.5, .6, 6);
  glTexCoord2f(0, 1); glVertex3f(-4.5, 0, 6);
  // Corner Wall - 3rd Base Line
  glTexCoord2f(0, 0); glVertex3f(-4.5, 1.4, 1);
  glTexCoord2f(1, 0); glVertex3f(-4.5, 0, 1);
  glTexCoord2f(1, 1); glVertex3f(-4.5, 0, 6);
  glTexCoord2f(0, 1); glVertex3f(-4.5, .6, 6);

  glEnd();
  glDisable(GL_TEXTURE_2D);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,texture[1]);

  glBegin(GL_QUADS);
  // Right Field Foul Seats
  glColor3ub(150,5,5);
  glNormal3f(0, 1, -1);
  glTexCoord2f(0, 0); glVertex3f(15, 1.4, 14);
  glTexCoord2f(0, 1); glVertex3f(13, .6, 14);
  glTexCoord2f(1, 1); glVertex3f(7, .6, 17);
  glTexCoord2f(1, 0); glVertex3f(7, 1.4, 19);
  // 1st Base Line Seats
  glTexCoord2f(0, 0); glVertex3f(-3.75, 1.4, 19);
  glTexCoord2f(0, 1); glVertex3f(-3.75, .6, 17);
  glTexCoord2f(1, 1); glVertex3f(7, .6, 17);
  glTexCoord2f(1, 0); glVertex3f(7, 1.4, 19);
  // Home Plate Seats
  glNormal3f(1, 1, -.1);
  glTexCoord2f(0, 0); glVertex3f(-3.75, .6, 17);
  glTexCoord2f(1, 0); glVertex3f(-7, .6, 13.75);
  glTexCoord2f(1, 1); glVertex3f(-9, 1.4, 13.75);
  glTexCoord2f(0, 1); glVertex3f(-3.75, 1.4, 19);
  // 3rd Base Line Seats
  glNormal3f(-1, 1, 0);
  glTexCoord2f(0, 0); glVertex3f(-9, 1.4, 13.75);
  glTexCoord2f(0, 1); glVertex3f(-7, .6, 13.75);
  glTexCoord2f(1, 1); glVertex3f(-7, .6, 8);
  glTexCoord2f(1, 0); glVertex3f(-9, 1.4, 8);
  // Angle Seats
  glTexCoord2f(0, 0); glVertex3f(-7.5, 1.4, 6);
  glTexCoord2f(0, 1); glVertex3f(-4.5, .6, 6);
  glTexCoord2f(1, 1); glVertex3f(-7, .6, 8);
  glTexCoord2f(1, 0); glVertex3f(-9, 1.4, 8);
  // Corner Seats
  glTexCoord2f(0, 0); glVertex3f(-7.5, 2.2, 1);
  glTexCoord2f(0, 1); glVertex3f(-4.5, 1.4, 1);
  glTexCoord2f(1, 1); glVertex3f(-4.5, .6, 6);
  glTexCoord2f(1, 0); glVertex3f(-7.5, 1.4, 6);

  glEnd();
  glDisable(GL_TEXTURE_2D);

  glPopMatrix();
}

static void drawTopDeck(double x,double y,double z,
                        double dx,double dy,double dz,
                        double th)
{
  glPushMatrix();

  // Translations
  glTranslated(x, y, z);
  glRotated(th, 0, 1, 0);
  glScaled(dx, dy, dz);

  //  Enable textures
  glEnable(GL_TEXTURE_2D);
  //glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
   glColor3f(1,1,1);
  glBindTexture(GL_TEXTURE_2D,texture[0]);

  glBegin(GL_QUADS);
  // TV Booth Center
  // Front
  glColor3ub(74,119,75);
  glNormal3f(-1, 0, -1);
  glTexCoord2f(0, 0); glVertex3f(-4.75, 5, 18);
  glTexCoord2f(1, 0); glVertex3f(-4.75, 3, 18);
  glTexCoord2f(1, 1); glVertex3f(-8, 3, 14.75);
  glTexCoord2f(0, 1); glVertex3f(-8, 5, 14.75);
  // Back
  glNormal3f(1, 0, 1);
  glTexCoord2f(0, 0); glVertex3f(-5.75, 5, 19);
  glTexCoord2f(1, 0); glVertex3f(-5.75, 3, 19);
  glTexCoord2f(1, 1); glVertex3f(-9, 3, 15.75);
  glTexCoord2f(0, 1); glVertex3f(-9, 5, 15.75);
  // Top
  glNormal3f(0, 1, 0);
  glTexCoord2f(0, 0); glVertex3f(-5.75, 5, 19);
  glTexCoord2f(1, 0); glVertex3f(-4.75, 5, 18);
  glTexCoord2f(1, 1); glVertex3f(-8, 5, 14.75);
  glTexCoord2f(0, 1); glVertex3f(-9, 5, 15.75);
  // Bottom
  glNormal3f(0, -1, 0);
  glTexCoord2f(0, 0); glVertex3f(-5.75, 3, 19);
  glTexCoord2f(1, 0); glVertex3f(-4.75, 3, 18);
  glTexCoord2f(1, 1); glVertex3f(-8, 3, 14.75);
  glTexCoord2f(0, 1); glVertex3f(-9, 3, 15.75);
  // TV Booth 1st Base
  // Front
  glNormal3f(0, 0, -1);
  glTexCoord2f(0, 0); glVertex3f(-4.75, 5, 18);
  glTexCoord2f(1, 0); glVertex3f(-4.75, 3, 18);
  glTexCoord2f(1, 1); glVertex3f(-1.75, 3, 18);
  glTexCoord2f(0, 1); glVertex3f(-1.75, 5, 18);
  // Side
  glNormal3f(1, 0, 0);
  glTexCoord2f(0, 0); glVertex3f(-1.75, 5, 18);
  glTexCoord2f(1, 0); glVertex3f(-1.75, 3, 18);
  glTexCoord2f(1, 1); glVertex3f(-1.75, 3, 19);
  glTexCoord2f(0, 1); glVertex3f(-1.75, 5, 19);
  // Back
  glNormal3f(0, 0, 1);
  glTexCoord2f(0, 0); glVertex3f(-5.75, 5, 19);
  glTexCoord2f(1, 0); glVertex3f(-5.75, 3, 19);
  glTexCoord2f(1, 1); glVertex3f(-1.75, 3, 19);
  glTexCoord2f(0, 1); glVertex3f(-1.75, 5, 19);
  // Top
  glNormal3f(0, 1, 0);
  glTexCoord2f(0, 0); glVertex3f(-1.75, 5, 18);
  glTexCoord2f(1, 0); glVertex3f(-1.75, 5, 19);
  glTexCoord2f(1, 1); glVertex3f(-5.75, 5, 19);
  glTexCoord2f(0, 1); glVertex3f(-4.75, 5, 18);
  // Bottom
  glNormal3f(0, -1, 0);
  glTexCoord2f(0, 0); glVertex3f(-1.75, 3, 18);
  glTexCoord2f(1, 0); glVertex3f(-1.75, 3, 19);
  glTexCoord2f(1, 1); glVertex3f(-5.75, 3, 19);
  glTexCoord2f(0, 1); glVertex3f(-4.75, 3, 18);
  // TV Booth 3rd Base
  // Front
  glNormal3f(1, 0, 0);
  glTexCoord2f(0, 0); glVertex3f(-8, 5, 14.75);
  glTexCoord2f(1, 0); glVertex3f(-8, 3, 14.75);
  glTexCoord2f(1, 1); glVertex3f(-8, 3, 11.75);
  glTexCoord2f(0, 1); glVertex3f(-8, 5, 11.75);
  // Side
  glNormal3f(0, 0, -1);
  glTexCoord2f(0, 0); glVertex3f(-9, 5, 11.75);
  glTexCoord2f(1, 0); glVertex3f(-9, 3, 11.75);
  glTexCoord2f(1, 1); glVertex3f(-8, 3, 11.75);
  glTexCoord2f(0, 1); glVertex3f(-8, 5, 11.75);
  // Back
  glNormal3f(-1, 0, 0);
  glTexCoord2f(0, 0); glVertex3f(-9, 5, 15.75);
  glTexCoord2f(1, 0); glVertex3f(-9, 3, 15.75);
  glTexCoord2f(1, 1); glVertex3f(-9, 3, 11.75);
  glTexCoord2f(0, 1); glVertex3f(-9, 5, 11.75);
  // Top
  glNormal3f(0, 1, 0);
  glTexCoord2f(0, 0); glVertex3f(-9, 5, 15.75);
  glTexCoord2f(0, 0); glVertex3f(-8, 5, 14.75);
  glTexCoord2f(0, 1); glVertex3f(-8, 5, 11.75);
  glTexCoord2f(0, 1); glVertex3f(-9, 5, 11.75);
  // Bottom
  glNormal3f(0, 1, 0);
  glTexCoord2f(0, 0); glVertex3f(-9, 3, 15.75);
  glTexCoord2f(0, 0); glVertex3f(-8, 3, 14.75);
  glTexCoord2f(0, 1); glVertex3f(-8, 3, 11.75);
  glTexCoord2f(0, 1); glVertex3f(-9, 3, 11.75);
  // Below Seat Walls
  // TV Booths Seats
  glColor3ub(74,119,75);
  glNormal3f(1, 1, -1);
  glTexCoord2f(0, 0); glVertex3f(-4, 2.5, 17.25);
  glTexCoord2f(0, 1); glVertex3f(-4, 2, 17.25);
  glTexCoord2f(1, 1); glVertex3f(-7.25, 2, 14);
  glTexCoord2f(1, 0); glVertex3f(-7.25, 2.5, 14);
  // 1st Base Line Seats
  glNormal3f(0, 1, -1);
  glTexCoord2f(0, 0); glVertex3f(-4, 2.5, 17.25);
  glTexCoord2f(0, 1); glVertex3f(-4, 2, 17.25);
  glTexCoord2f(1, 1); glVertex3f(13, 2, 17.25);
  glTexCoord2f(1, 0); glVertex3f(13, 2.5, 17.25);
  // Pesky Pole Seats
  glNormal3f(-1, 1, -1);
  glTexCoord2f(0, 0); glVertex3f(13, 2, 17.25);
  glTexCoord2f(0, 1); glVertex3f(13, 2.5, 17.25);
  glTexCoord2f(1, 1); glVertex3f(15, 2.5, 14.5);
  glTexCoord2f(1, 0); glVertex3f(15, 2, 14.5);
  // 3rd Base Line Seats
  glNormal3f(1, 1, 0);
  glTexCoord2f(0, 0); glVertex3f(-7.25, 2.5, 4);
  glTexCoord2f(0, 1); glVertex3f(-7.25, 2, 4);
  glTexCoord2f(1, 1); glVertex3f(-7.25, 2, 14);
  glTexCoord2f(1, 0); glVertex3f(-7.25, 2.5, 14);
  // Pesky Pole Seats Side
  glNormal3f(0, 0, -1);
  glTexCoord2f(0, 0); glVertex3f(16, 2.5, 15);
  glTexCoord2f(0, 1); glVertex3f(16, 3.5, 15);
  glTexCoord2f(1, 1); glVertex3f(15, 2.5, 14.5);
  glTexCoord2f(1, 0); glVertex3f(15, 2, 14.5);
  // 3rd Base Line Seats Side
  glNormal3f(0, 0, -1);
  glTexCoord2f(0, 0); glVertex3f(-7.25, 2.5, 4);
  glTexCoord2f(0, 1); glVertex3f(-7.25, 2, 4);
  glTexCoord2f(1, 1); glVertex3f(-8.25, 2.5, 4);
  glTexCoord2f(1, 0); glVertex3f(-8.25, 3.5, 4);

  glEnd();
  glDisable(GL_TEXTURE_2D);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,texture[1]);
  glBegin(GL_QUADS);
  // TV Booths Seats
  glColor3ub(74,119,75);
  glNormal3f(1, .5, 1);
  glTexCoord2f(0, 0); glVertex3f(-4, 2.5, 17.25);
  glTexCoord2f(0, 1); glVertex3f(-5, 3.5, 18.5);
  glTexCoord2f(1, 1); glVertex3f(-8.25, 3.5, 15);
  glTexCoord2f(1, 0); glVertex3f(-7.25, 2.5, 14);
  // 1st Base Line Seats
  glNormal3f(0, .5, -1);
  glTexCoord2f(0, 0); glVertex3f(-4, 2.5, 17.25);
  glTexCoord2f(0, 1); glVertex3f(-5, 3.5, 18.5);
  glTexCoord2f(1, 1); glVertex3f(14, 3.5, 18.5);
  glTexCoord2f(1, 0); glVertex3f(13, 2.5, 17.25);
  // Pesky Pole Seats
  glNormal3f(-1, .5, -1);
  glTexCoord2f(0, 0); glVertex3f(14, 3.5, 18.5);
  glTexCoord2f(0, 1); glVertex3f(13, 2.5, 17.25);
  glTexCoord2f(1, 1); glVertex3f(15, 2.5, 14.5);
  glTexCoord2f(1, 0); glVertex3f(16, 3.5, 15);
  // 3rd Base Line Seats
  glNormal3f(-1, .5, 0);
  glTexCoord2f(0, 0); glVertex3f(-7.25, 2.5, 4);
  glTexCoord2f(0, 1); glVertex3f(-8.25, 3.5, 4);
  glTexCoord2f(1, 1); glVertex3f(-8.25, 3.5, 15);
  glTexCoord2f(1, 0); glVertex3f(-7.25, 2.5, 14);

  glEnd();
  glDisable(GL_TEXTURE_2D);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,texture[0]);

  glBegin(GL_QUADS);
  // 1st Base Line - Back
  glNormal3f(0, 0, 1);
  glTexCoord2f(0, 0); glVertex3f(-4.75, 2.5, 18.5);
  glTexCoord2f(1, 0); glVertex3f(-4.75, 3.5, 18.5);
  glTexCoord2f(1, 1); glVertex3f(14, 3.5, 18.5);
  glTexCoord2f(0, 1); glVertex3f(14, 2.5, 18.5);
  // Pesky Corner - Back
  glNormal3f(0, 0, 1);
  glTexCoord2f(0, 0); glVertex3f(14, 3.5, 18.5);
  glTexCoord2f(1, 0); glVertex3f(14, 2.5, 18.5);
  glTexCoord2f(1, 1); glVertex3f(16, 2.5, 15);
  glTexCoord2f(0, 1); glVertex3f(16, 3.5, 15);
  // 3rd Base Line - Back
  glNormal3f(-1, 0, 0);
  glTexCoord2f(0, 0); glVertex3f(-8.25, 2.5, 4);
  glTexCoord2f(1, 0); glVertex3f(-8.25, 3.5, 4);
  glTexCoord2f(1, 1); glVertex3f(-8.25, 3.5, 15);
  glTexCoord2f(0, 1); glVertex3f(-8.25, 2.5, 15);
  // Home Plate Seats - Back
  glNormal3f(-1, 0, -1);
  glTexCoord2f(0, 0); glVertex3f(-4.75, 2.5, 18.5);
  glTexCoord2f(1, 0); glVertex3f(-4.75, 3.5, 18.5);
  glTexCoord2f(1, 1); glVertex3f(-8.25, 3.5, 15);
  glTexCoord2f(0, 1); glVertex3f(-8.25, 2.5, 15);

  glColor3ub(75,75,75);
  // 1st Base Line - Bottom
  glNormal3f(0, -1, 0);
  glTexCoord2f(0, 0); glVertex3f(-4.75, 2.5, 18.5);
  glTexCoord2f(1, 0); glVertex3f(-4, 2, 17.25);
  glTexCoord2f(1, 1); glVertex3f(13, 2, 17.25);
  glTexCoord2f(0, 1); glVertex3f(14, 2.5, 18.5);
  // Pesky Corner - Bottom
  glTexCoord2f(0, 0); glVertex3f(13, 2, 17.25);
  glTexCoord2f(1, 0); glVertex3f(14, 2.5, 18.5);
  glTexCoord2f(1, 1); glVertex3f(16, 2.5, 15);
  glTexCoord2f(0, 1); glVertex3f(15, 2, 14.5);
  // 3rd Base Line - Bottom
  glTexCoord2f(0, 0); glVertex3f(-8.25, 2.5, 4);
  glTexCoord2f(1, 0); glVertex3f(-7.25, 2, 4);
  glTexCoord2f(1, 1); glVertex3f(-7.25, 2, 14);
  glTexCoord2f(0, 1); glVertex3f(-8.25, 2.5, 15);
  // Home Plate Seats - Bottom
  glTexCoord2f(0, 0); glVertex3f(-4.75, 2.5, 18.5);
  glTexCoord2f(1, 0); glVertex3f(-4, 2, 17.25);
  glTexCoord2f(1, 1); glVertex3f(-7.25, 2, 14);
  glTexCoord2f(0, 1); glVertex3f(-8.25, 2.5, 15);

  glEnd();
  glDisable(GL_TEXTURE_2D);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,texture[10]);
  glBegin(GL_QUADS);
  // Retired Number 1
  //glColor3ub(74,119,75);
  glNormal3f(1, 0, -1);
  glTexCoord2f(0, 0); glVertex3f(14.9, 2, 14.4);
  glTexCoord2f(0, 1); glVertex3f(14.9, 2.5, 14.4);
  glTexCoord2f(1, 1); glVertex3f(12.9, 2.5, 17.24);
  glTexCoord2f(1, 0); glVertex3f(12.9, 2, 17.24);

  glEnd();
  glDisable(GL_TEXTURE_2D);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,texture[11]);
  glBegin(GL_QUADS);
  // Retired Numbers 2
  glNormal3f(0, 0, -1);
  glTexCoord2f(0, 0); glVertex3f(12.7, 2, 17.24);
  glTexCoord2f(0, 1); glVertex3f(12.7, 2.5, 17.24);
  glTexCoord2f(1, 1); glVertex3f(9.7, 2.5, 17.24);
  glTexCoord2f(1, 0); glVertex3f(9.7, 2, 17.24);

  glEnd();
  glDisable(GL_TEXTURE_2D);

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

  //  Enable textures
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
  glColor3f(1,1,1);
  glBindTexture(GL_TEXTURE_2D,texture[3]);

  // Dirt
  glBegin(GL_QUADS);
  glNormal3f(0, 1, 0);
  glColor3ub(120,72,0);
  // Home Plate Infield Dirt
  glTexCoord2f(0, 0); glVertex3f(-4.8, 0, 14.8);
  glTexCoord2f(1, 0); glVertex3f(-4.8, 0, 13.2);
  glTexCoord2f(1, 1); glVertex3f(-3.2, 0, 13.2);
  glTexCoord2f(0, 1); glVertex3f(-3.2, 0, 14.8);
  // 3rd->2nd Infield Dirt
  glTexCoord2f(0, 0); glVertex3f(-4.8, 0, 9.3);
  glTexCoord2f(1, 0); glVertex3f(-4.8, 0, 8);
  glTexCoord2f(1, 1); glVertex3f(2, 0, 8);
  glTexCoord2f(0, 1); glVertex3f(2, 0, 9.3);
  // 1st->2nd Infield Dirt
  glTexCoord2f(0, 0); glVertex3f(2, 0, 14.8);
  glTexCoord2f(1, 0); glVertex3f(2, 0, 8);
  glTexCoord2f(1, 1); glVertex3f(.7, 0, 8);
  glTexCoord2f(0, 1); glVertex3f(.7, 0, 14.8);
  // 3rd->Home Infield Dirt
  glTexCoord2f(0, 0); glVertex3f(-4.4, 0, 13.2);
  glTexCoord2f(1, 0); glVertex3f(-4.4, 0, 8);
  glTexCoord2f(1, 1); glVertex3f(-3.8, 0, 8);
  glTexCoord2f(0, 1); glVertex3f(-3.8, 0, 13.2);
  // 1st->Home Infield Dirt
  glTexCoord2f(0, 0); glVertex3f(-3.2, 0, 14.4);
  glTexCoord2f(1, 0); glVertex3f(-3.2, 0, 13.8);
  glTexCoord2f(1, 1); glVertex3f(.7, 0, 13.8);
  glTexCoord2f(0, 1); glVertex3f(.7, 0, 14.4);
  // Pitcher's Mound
  glTexCoord2f(0, 0); glVertex3f(-2, 0, 12);
  glTexCoord2f(1, 0); glVertex3f(-2, 0, 11.2);
  glTexCoord2f(1, 1); glVertex3f(-1.2, 0, 11.2);
  glTexCoord2f(0, 1); glVertex3f(-1.2, 0, 12);
  // Monster Track
  glTexCoord2f(0, 0); glVertex3f(-4, 0, 1);
  glTexCoord2f(1, 0); glVertex3f(4, 0, 1);
  glTexCoord2f(1, 1); glVertex3f(4, 0, 2);
  glTexCoord2f(0, 1); glVertex3f(-4, 0, 2);
  //  Center Track
  glTexCoord2f(0, 0); glVertex3f(4, 0, 1);
  glTexCoord2f(1, 0); glVertex3f(4, 0, 2);
  glTexCoord2f(1, 1); glVertex3f(8.25, 0, 3.75);
  glTexCoord2f(0, 1); glVertex3f(9, 0, 3);
  // Triangle Track
  glTexCoord2f(0, 0); glVertex3f(9, 0, 3);
  glTexCoord2f(1, 0); glVertex3f(8.25, 0, 3.75);
  glTexCoord2f(1, 1); glVertex3f(9.25, 0, 7.25);
  glTexCoord2f(0, 1); glVertex3f(10, 0, 6.5);
  //  Right Field Track
  glTexCoord2f(0, 0); glVertex3f(10,0,6.5);
  glTexCoord2f(1, 0); glVertex3f(9.25,0,7.25);
  glTexCoord2f(1, 1); glVertex3f(12.5,0,13);
  glTexCoord2f(0, 1); glVertex3f(13.5,0,13);
  // Corner Track
  glTexCoord2f(0, 0); glVertex3f(13.5,0,13);
  glTexCoord2f(1, 0); glVertex3f(12.5,0,13);
  glTexCoord2f(1, 1); glVertex3f(12,0,14);
  glTexCoord2f(0, 1); glVertex3f(13,0,14);
  // Foul Corner Track
  glTexCoord2f(0, 0); glVertex3f(7,0,17);
  glTexCoord2f(1, 0); glVertex3f(7,0,16);
  glTexCoord2f(1, 1); glVertex3f(12,0,14);
  glTexCoord2f(0, 1); glVertex3f(13,0,14);
  // 1st Base Line Track
  glTexCoord2f(0, 0); glVertex3f(7,0,17);
  glTexCoord2f(1, 0); glVertex3f(7,0,16);
  glTexCoord2f(1, 1); glVertex3f(-3.75,0,16);
  glTexCoord2f(0, 1); glVertex3f(-3.75,0,17);
  // Home Plate Track
  glTexCoord2f(0, 0); glVertex3f(-7,0,13.75);
  glTexCoord2f(1, 0); glVertex3f(-6,0,13.75);
  glTexCoord2f(1, 1); glVertex3f(-3.75,0,16);
  glTexCoord2f(0, 1); glVertex3f(-3.75,0,17);
  // 3rd Base Line Track
  glTexCoord2f(0, 0); glVertex3f(-7,0,13.75);
  glTexCoord2f(1, 0); glVertex3f(-6,0,13.75);
  glTexCoord2f(1, 1); glVertex3f(-6,0,8.75);
  glTexCoord2f(0, 1); glVertex3f(-7,0,8);
  // Angle Track
  glTexCoord2f(0, 0); glVertex3f(-4.5,0,6);
  glTexCoord2f(1, 0); glVertex3f(-3.5,0,6.5);
  glTexCoord2f(1, 1); glVertex3f(-6,0,8.75);
  glTexCoord2f(0, 1); glVertex3f(-7,0,8);
  // Monster Corner Track
  glTexCoord2f(0, 0); glVertex3f(-4.5,0,6);
  glTexCoord2f(1, 0); glVertex3f(-3.5,0,6.5);
  glTexCoord2f(1, 1); glVertex3f(-3.5,0,1);
  glTexCoord2f(0, 1); glVertex3f(-4.5,0,1);

  glEnd();
  glDisable(GL_TEXTURE_2D);

  // Foul Lines
  glBegin(GL_QUADS);
  // Left Field
  glColor3ub(180,180,180);
  glVertex3f(-4, .01, 1);
  glVertex3f(-4.2, .01, 1);
  glVertex3f(-4.2, .01, 14);
  glVertex3f(-4, .01, 14);
  //  Right Field
  glVertex3f(13, .01, 14);
  glVertex3f(-4.2, .01, 14);
  glVertex3f(-4.2, .01, 14.2);
  glVertex3f(13, .01, 14.2);

  glEnd();

  // Grass
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,texture[3]);
  glBegin(GL_QUADS);
  // Outfield
  // Monster -> Infield
  glColor3ub(0,123,12);
  // Monster/Center -> 1st Base Line
  glTexCoord2f(0, 0); glVertex3f(-7,-.05,17);
  glTexCoord2f(1, 0); glVertex3f(-7,-.05,1.5);
  glTexCoord2f(1, 1); glVertex3f(13,-.05,1.5);
  glTexCoord2f(0, 1); glVertex3f(13,-.05,17);

  glEnd();
  glDisable(GL_TEXTURE_2D);

  glPopMatrix();
}

static void drawStadiumWalls(double x,double y,double z,
                        double dx,double dy,double dz,
                        double th)
{
  glPushMatrix();

  // Translations
  glTranslated(x, y, z);
  glRotated(th, 0, 1, 0);
  glScaled(dx, dy, dz);

  //  Enable textures
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
  glColor3f(1,1,1);
  glBindTexture(GL_TEXTURE_2D,texture[0]);

  // Exterior Walls
  glBegin(GL_QUADS);
  glColor3ub(44,89,45);
  // Left Field
  glNormal3f(0, 0, -1);
  glTexCoord2f(0, 0); glVertex3f(-9, -.1, -1);
  glTexCoord2f(1, 0); glVertex3f(-9, 3, -1);
  glTexCoord2f(1, 1); glVertex3f(16, 3, -1);
  glTexCoord2f(0, 1); glVertex3f(16, -.1, -1);
  // Right Field
  glNormal3f(-1, 0, 0);
  glTexCoord2f(0, 0); glVertex3f(16, -.1, 14);
  glTexCoord2f(1, 0); glVertex3f(16, 3, 14);
  glTexCoord2f(1, 1); glVertex3f(16, 3, -1);
  glTexCoord2f(0, 1); glVertex3f(16, -.1, -1);
  // Right Field Corner
  glNormal3f(-1, 0, 0);
  glTexCoord2f(0, 0); glVertex3f(16, -.1, 14);
  glTexCoord2f(1, 0); glVertex3f(16, 1.4, 14);
  glTexCoord2f(1, 1); glVertex3f(16, 1.4, 19);
  glTexCoord2f(0, 1); glVertex3f(16, -.1, 19);
  // 1st Base Line
  glNormal3f(0, 0, 1);
  glTexCoord2f(0, 0); glVertex3f(16, -.1, 19);
  glTexCoord2f(1, 0); glVertex3f(16, 1.4, 19);
  glTexCoord2f(1, 1); glVertex3f(-9, 1.4, 19);
  glTexCoord2f(0, 1); glVertex3f(-9, -.1, 19);
  // 3rd Base Line
  glNormal3f(1, 0, 0);
  glTexCoord2f(0, 0); glVertex3f(-9, -.1, 6);
  glTexCoord2f(1, 0); glVertex3f(-9, 1.4, 6);
  glTexCoord2f(1, 1); glVertex3f(-9, 1.4, 19);
  glTexCoord2f(0, 1); glVertex3f(-9, -.1, 19);
  // 3rd Base Line Corner
  glNormal3f(1, 0, 0);
  glTexCoord2f(0, 0); glVertex3f(-9, -.1, 1);
  glTexCoord2f(1, 0); glVertex3f(-9, 2.2, 1);
  glTexCoord2f(1, 1); glVertex3f(-9, 1.4, 6);
  glTexCoord2f(0, 1); glVertex3f(-9, -.1, 6);
  // 3rd Base Line Corner Corner
  glNormal3f(1, 0, 0);
  glTexCoord2f(0, 0); glVertex3f(-9, -.1, 1);
  glTexCoord2f(1, 0); glVertex3f(-9, 3, 1);
  glTexCoord2f(1, 1); glVertex3f(-9, 3, -1);
  glTexCoord2f(0, 1); glVertex3f(-9, -.1, -1);
  // Bottom
  glColor3ub(75,75,75);
  glNormal3f(0, -1, 0);
  glTexCoord2f(0, 0); glVertex3f(-9, -.1, -1);
  glTexCoord2f(1, 0); glVertex3f(-9, -.1, 19);
  glTexCoord2f(1, 1); glVertex3f(16, -.1, 19);
  glTexCoord2f(0, 1); glVertex3f(16, -.1, -1);
  // Home Plate Back Top
  glNormal3f(0, 1, 0);
  glTexCoord2f(0, 0); glVertex3f(-9, 1.4, 13.75);
  glTexCoord2f(1, 0); glVertex3f(-9, 1.4, 19);
  glTexCoord2f(1, 1); glVertex3f(-3.75, 1.4, 19);
  glTexCoord2f(0, 1); glVertex3f(-9, 1.4, 13.75);
  // Right Field Back Top
  glNormal3f(0, 1, 0);
  glTexCoord2f(0, 0); glVertex3f(15, 1.4, 14);
  glTexCoord2f(1, 0); glVertex3f(7, 1.4, 19);
  glTexCoord2f(1, 1); glVertex3f(16, 1.4, 19);
  glTexCoord2f(0, 1); glVertex3f(16, 1.4, 14);
  // Left Field Angle Back Top
  glNormal3f(0, 1, 0);
  glTexCoord2f(0, 0); glVertex3f(-9, 1.4, 8);
  glTexCoord2f(1, 0); glVertex3f(-9, 1.4, 6);
  glTexCoord2f(1, 1); glVertex3f(-7.5, 1.4, 6);
  glTexCoord2f(0, 1); glVertex3f(-9, 1.4, 8);
  // Left Field Corner Back Top
  glNormal3f(0, 1, 0);
  glTexCoord2f(0, 0); glVertex3f(-9, 2.2, 1);
  glTexCoord2f(1, 0); glVertex3f(-7.5, 2.2, 1);
  glTexCoord2f(1, 1); glVertex3f(-7.5, 1.4, 6);
  glTexCoord2f(0, 1); glVertex3f(-9, 1.4, 6);
  // Left Field Corner Corner Back Top
  glNormal3f(0, 1, 0);
  glTexCoord2f(0, 0); glVertex3f(-9, 3, 1);
  glTexCoord2f(1, 0); glVertex3f(-9, 3, -1);
  glTexCoord2f(1, 1); glVertex3f(-4, 3, -1);
  glTexCoord2f(0, 1); glVertex3f(-4, 3, 1);
  // Extended Monster Top
  glNormal3f(0, 1, 0);
  glTexCoord2f(0, 0); glVertex3f(4, 3, 1);
  glTexCoord2f(1, 0); glVertex3f(4, 3, -1);
  glTexCoord2f(1, 1); glVertex3f(11, 3, -1);
  glTexCoord2f(0, 1); glVertex3f(11, 3, 1);
  // Center Field Back Top
  glNormal3f(0, 1, 0);
  glTexCoord2f(0, 0); glVertex3f(16, 3, 6.6);
  glTexCoord2f(1, 0); glVertex3f(16, 3, -1);
  glTexCoord2f(1, 1); glVertex3f(11, 3, -1);
  glTexCoord2f(0, 1); glVertex3f(11, 3, 1);

  // Left Field Corner Corner Wall
  glColor3ub(74,119,75);
  glNormal3f(0, 0, 1);
  glTexCoord2f(0, 0); glVertex3f(-9, 1.4, 1);
  glTexCoord2f(1, 0); glVertex3f(-9, 3, 1);
  glTexCoord2f(1, 1); glVertex3f(-7.5, 3, 1);
  glTexCoord2f(0, 1); glVertex3f(-7.5, 1.4, 1);

  glEnd();
  glDisable(GL_TEXTURE_2D);

  glPopMatrix();
}

static void drawBoards(double x,double y,double z,
                        double dx,double dy,double dz,
                        double th)
{
  glPushMatrix();

  // Translations
  glTranslated(x, y, z);
  glRotated(th, 0, 1, 0);
  glScaled(dx, dy, dz);

  //  Enable textures
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
  glColor3f(1,1,1);
  glBindTexture(GL_TEXTURE_2D,texture[4]);

  glBegin(GL_QUADS);
  // Big Video Sign
  glNormal3f(1, 0, 1);
  glTexCoord2f(0, 0); glVertex3f(11, 4, 1);
  glTexCoord2f(0, 1); glVertex3f(11, 6, 1);
  glTexCoord2f(1, 1); glVertex3f(16, 6, 6);
  glTexCoord2f(1, 0); glVertex3f(16, 4, 6);

  glEnd();
  glDisable(GL_TEXTURE_2D);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,texture[5]);

  glBegin(GL_QUADS);
  // Wide Video Sign
  glNormal3f(0, 0, 1);
  glTexCoord2f(0, 0); glVertex3f(4.5, 4, 1);
  glTexCoord2f(0, 1); glVertex3f(4.5, 5, 1);
  glTexCoord2f(1, 1); glVertex3f(9.5, 5, 1);
  glTexCoord2f(1, 0); glVertex3f(9.5, 4, 1);

  glEnd();
  glDisable(GL_TEXTURE_2D);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,texture[6]);

  glBegin(GL_QUADS);
  // Fenway Booth Sign
  glNormal3f(-1, 0, -1);
  glTexCoord2f(0, 0); glVertex3f(-5.14, 4.37, 17.41);
  glTexCoord2f(0, 1); glVertex3f(-5.14, 4.8, 17.41);
  glTexCoord2f(1, 1); glVertex3f(-7.41, 4.8, 15.14);
  glTexCoord2f(1, 0); glVertex3f(-7.41, 4.37, 15.14);

  glEnd();
  glDisable(GL_TEXTURE_2D);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,texture[0]);

  glBegin(GL_QUADS);
  glColor3ub(74,119,75);
  // Big Video Sign
  // Back
  glNormal3f(.1, 0, -1);
  glTexCoord2f(0, 0); glVertex3f(12, 4, 0);
  glTexCoord2f(1, 0); glVertex3f(12, 6, 0);
  glTexCoord2f(1, 1); glVertex3f(17, 6, 5);
  glTexCoord2f(0, 1); glVertex3f(17, 4, 5);
  // Front
  glTexCoord2f(0, 0); glVertex3f(11.1, 4, .9);
  glTexCoord2f(1, 0); glVertex3f(11.1, 6, .9);
  glTexCoord2f(1, 1); glVertex3f(16.1, 6, 5.9);
  glTexCoord2f(0, 1); glVertex3f(16.1, 4, 5.9);
  // Top
  glNormal3f(0, 1, 0);
  glTexCoord2f(0, 0); glVertex3f(11.1, 6, .9);
  glTexCoord2f(1, 0); glVertex3f(12, 6, 0);
  glTexCoord2f(1, 1); glVertex3f(17, 6, 5);
  glTexCoord2f(0, 1); glVertex3f(16.1, 6, 5.9);
  // Bottom
  glNormal3f(0, -1, 0);
  glTexCoord2f(0, 0); glVertex3f(11.1, 4, .9);
  glTexCoord2f(1, 0); glVertex3f(12, 4, 0);
  glTexCoord2f(1, 1); glVertex3f(17, 4, 5);
  glTexCoord2f(0, 1); glVertex3f(16.1, 4, 5.9);
  // Left
  glNormal3f(-1, 0, -1);
  glTexCoord2f(0, 0); glVertex3f(11.1, 4, .9);
  glTexCoord2f(1, 0); glVertex3f(11.1, 6, .9);
  glTexCoord2f(1, 1); glVertex3f(12, 6, 0);
  glTexCoord2f(0, 1); glVertex3f(12, 4, 0);
  // Right
  glNormal3f(1, 0, 1);
  glTexCoord2f(0, 0); glVertex3f(17, 4, 5);
  glTexCoord2f(1, 0); glVertex3f(17, 6, 5);
  glTexCoord2f(1, 1); glVertex3f(16.1, 6, 5.9);
  glTexCoord2f(0, 1); glVertex3f(16.1, 4, 5.9);
  // Left Pole
  // Front
  glTexCoord2f(0, 0); glVertex3f(13.5, 3, 1.5);
  glTexCoord2f(1, 0); glVertex3f(13.5, 5, 1.5);
  glTexCoord2f(1, 1); glVertex3f(13.6, 5, 1.6);
  glTexCoord2f(0, 1); glVertex3f(13.6, 3, 1.6);
  // Back
  glTexCoord2f(0, 0); glVertex3f(13.6, 3, 1.4);
  glTexCoord2f(1, 0); glVertex3f(13.6, 5, 1.4);
  glTexCoord2f(1, 1); glVertex3f(13.7, 5, 1.5);
  glTexCoord2f(0, 1); glVertex3f(13.7, 3, 1.5);
  // Left
  glTexCoord2f(0, 0); glVertex3f(13.5, 3, 1.5);
  glTexCoord2f(1, 0); glVertex3f(13.5, 5, 1.5);
  glTexCoord2f(1, 1); glVertex3f(13.6, 5, 1.4);
  glTexCoord2f(0, 1); glVertex3f(13.6, 3, 1.4);
  // Right
  glTexCoord2f(0, 0); glVertex3f(13.7, 3, 1.5);
  glTexCoord2f(1, 0); glVertex3f(13.7, 5, 1.5);
  glTexCoord2f(1, 1); glVertex3f(13.6, 5, 1.6);
  glTexCoord2f(0, 1); glVertex3f(13.6, 3, 1.6);
  // Right Pole
  // Front
  glTexCoord2f(0, 0); glVertex3f(15.5, 3, 3.5);
  glTexCoord2f(1, 0); glVertex3f(15.5, 5, 3.5);
  glTexCoord2f(1, 1); glVertex3f(15.6, 5, 3.6);
  glTexCoord2f(0, 1); glVertex3f(15.6, 3, 3.6);
  // Back
  glTexCoord2f(0, 0); glVertex3f(15.6, 3, 3.4);
  glTexCoord2f(1, 0); glVertex3f(15.6, 5, 3.4);
  glTexCoord2f(1, 1); glVertex3f(15.7, 5, 3.5);
  glTexCoord2f(0, 1); glVertex3f(15.7, 3, 3.5);
  // Left
  glTexCoord2f(0, 0); glVertex3f(15.5, 3, 3.5);
  glTexCoord2f(1, 0); glVertex3f(15.5, 5, 3.5);
  glTexCoord2f(1, 1); glVertex3f(15.6, 5, 3.4);
  glTexCoord2f(0, 1); glVertex3f(15.6, 3, 3.4);
  // Front
  glTexCoord2f(0, 0); glVertex3f(15.7, 3, 3.5);
  glTexCoord2f(1, 0); glVertex3f(15.7, 5, 3.5);
  glTexCoord2f(1, 1); glVertex3f(15.6, 5, 3.6);
  glTexCoord2f(0, 1); glVertex3f(15.6, 3, 3.6);

  // Wide Video Sign
  // Back
  glNormal3f(0, 0, -1);
  glTexCoord2f(0, 0); glVertex3f(4.5, 4, .75);
  glTexCoord2f(1, 0); glVertex3f(4.5, 5, .75);
  glTexCoord2f(1, 1); glVertex3f(9.5, 5, .75);
  glTexCoord2f(0, 1); glVertex3f(9.5, 4, .75);
  // Back
  glTexCoord2f(0, 0); glVertex3f(4.5, 4, .9);
  glTexCoord2f(1, 0); glVertex3f(4.5, 5, .9);
  glTexCoord2f(1, 1); glVertex3f(9.5, 5, .9);
  glTexCoord2f(0, 1); glVertex3f(9.5, 4, .9);
  // Top
  glTexCoord2f(0, 0); glVertex3f(4.5, 5, .9);
  glTexCoord2f(1, 0); glVertex3f(4.5, 5, .75);
  glTexCoord2f(1, 1); glVertex3f(9.5, 5, .75);
  glTexCoord2f(0, 1); glVertex3f(9.5, 5, .9);
  // Bottom
  glTexCoord2f(0, 0); glVertex3f(4.5, 4, .9);
  glTexCoord2f(1, 0); glVertex3f(4.5, 4, .75);
  glTexCoord2f(1, 1); glVertex3f(9.5, 4, .75);
  glTexCoord2f(0, 1); glVertex3f(9.5, 4, .9);
  // Left
  glTexCoord2f(0, 0); glVertex3f(4.5, 4, .9);
  glTexCoord2f(1, 0); glVertex3f(4.5, 5, .9);
  glTexCoord2f(1, 1); glVertex3f(4.5, 5, .75);
  glTexCoord2f(0, 1); glVertex3f(4.5, 4, .75);
  // Right
  glTexCoord2f(0, 0); glVertex3f(9.5, 4, .9);
  glTexCoord2f(1, 0); glVertex3f(9.5, 5, .9);
  glTexCoord2f(1, 1); glVertex3f(9.5, 5, .75);
  glTexCoord2f(0, 1); glVertex3f(9.5, 4, .75);

  // Left Post
  // Left
  glTexCoord2f(0, 0); glVertex3f(4.5, 3, .9);
  glTexCoord2f(1, 0); glVertex3f(4.5, 5, .9);
  glTexCoord2f(1, 1); glVertex3f(4.5, 5, .75);
  glTexCoord2f(0, 1); glVertex3f(4.5, 3, .75);
  // Right
  glTexCoord2f(0, 0); glVertex3f(4.65, 3, .9);
  glTexCoord2f(1, 0); glVertex3f(4.65, 5, .9);
  glTexCoord2f(1, 1); glVertex3f(4.65, 5, .75);
  glTexCoord2f(0, 1); glVertex3f(4.65, 3, .75);
  // Back
  glTexCoord2f(0, 0); glVertex3f(4.5, 3, .75);
  glTexCoord2f(1, 0); glVertex3f(4.5, 5, .75);
  glTexCoord2f(1, 1); glVertex3f(4.65, 5, .75);
  glTexCoord2f(0, 1); glVertex3f(4.65, 3, .75);
  // Front
  glTexCoord2f(0, 0); glVertex3f(4.5, 3, .9);
  glTexCoord2f(1, 0); glVertex3f(4.5, 5, .9);
  glTexCoord2f(1, 1); glVertex3f(4.65, 5, .9);
  glTexCoord2f(0, 1); glVertex3f(4.65, 3, .9);
  // Right Post
  // Right
  glTexCoord2f(0, 0); glVertex3f(9.5, 3, .9);
  glTexCoord2f(1, 0); glVertex3f(9.5, 5, .9);
  glTexCoord2f(1, 1); glVertex3f(9.5, 5, .75);
  glTexCoord2f(0, 1); glVertex3f(9.5, 3, .75);
  // Left
  glTexCoord2f(0, 0); glVertex3f(9.35, 3, .9);
  glTexCoord2f(1, 0); glVertex3f(9.35, 5, .9);
  glTexCoord2f(1, 1); glVertex3f(9.35, 5, .75);
  glTexCoord2f(0, 1); glVertex3f(9.35, 3, .75);
  // Back
  glTexCoord2f(0, 0); glVertex3f(9.5, 3, .75);
  glTexCoord2f(1, 0); glVertex3f(9.5, 5, .75);
  glTexCoord2f(1, 1); glVertex3f(9.35, 5, .75);
  glTexCoord2f(0, 1); glVertex3f(9.35, 3, .75);
  // Front
  glTexCoord2f(0, 0); glVertex3f(9.5, 3, .9);
  glTexCoord2f(1, 0); glVertex3f(9.5, 5, .9);
  glTexCoord2f(1, 1); glVertex3f(9.35, 5, .9);
  glTexCoord2f(0, 11); glVertex3f(9.35, 3, .9);

  glEnd();
  glDisable(GL_TEXTURE_2D);

  glPopMatrix();
}

/*
 *  Draw a cylinder
 *  Some code borrowed my github user taylorjandrews
 */
void drawCylinder(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th, int ekClr) {
   double radius = 1, height = 1;

   glPushMatrix();

   //  Transform cylinder
   glTranslated(x,y,z);
   glRotated(th,0,0,1);
   glScaled(dx,dy,dz);

   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

   glBindTexture(GL_TEXTURE_2D,texture[0]);

   if (EvansSeats && ekClr)
   {
     glColor3ub(144,200,245);
   }
   double j;
   glBegin(GL_QUAD_STRIP);
   for (j = 0; j <= 360; j+=.125) {
      const float tc = (j / (float) 360);
      double x = radius * Cos(j);
      double y = height;
      double z = radius * Sin(j);

      glNormal3d(Cos(j), 0, Sin(j));

      glTexCoord2f(-tc, 0.0); glVertex3d(x, -y, z);
      glTexCoord2f(-tc, 1.0); glVertex3d(x, y, z);
   }
   glEnd();

   glPopMatrix();
   glDisable(GL_TEXTURE_2D);
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
        float Position[]  = {ylight,distance*Cos(zh),distance*Sin(zh),1.0};
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
   drawFoulWalls(0,-.25,0, .25,.25,.25, 0);
   drawTopDeck(0,-.25,0, .25,.25,.25, 0);
   drawStadiumWalls(0,-.25,0, .25,.25,.25, 0);
   drawBoards(0,-.25,0, .25,.25,.25, 0);

   drawCylinder(-1.9375,.1,1.25 , .02,.3,.02 ,0, 0);
   drawCylinder(-1.9375,.1,3.625 , .02,.3,.02 ,0, 0);
   drawCylinder(-1.125,.1,4.4375 , .02,.3,.02 ,0, 0);
   drawCylinder(.5,.1,4.6 , .02,.3,.02 ,0, 0);
   drawCylinder(2,.1,4.5 , .02,.3,.02 ,0, 0);
   drawCylinder(3.5,.1,4.4 , .02,.3,.02 ,0, 0);

   //sphere1(0,1,.5,.05);

   if (EvansSeats)
   {
     drawCylinder(-1.45,1,.35 , .02,1,.02 ,0, 1);
     drawCylinder(3.25,1,.9 , .02,1,.02 ,0, 1);
     drawCylinder(3,1,2 , .02,1,.02 ,0, 1);
     drawCylinder(3.75,1,3 , .02,1,.02 ,0, 1);
     drawCylinder(2.75,1,4 , .02,1,.02 ,0, 1);
     drawCylinder(2.25,1,4.3 , .02,1,.02 ,0, 1);

   }

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
   zh = fmod(35*t,360.0);
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
  // Toggle first person
  else if (ch == 'f' || ch == 'F')
  {
    fp = 1-fp;
  }

  else if (ch == 'k' || ch == 'K')
  {
    EvansSeats = 1-EvansSeats;
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
    else if (ch == 'p' || ch == 'P')
    {
      Fy += .1;
    }
    else if (ch == 'l' || ch == 'L')
    {
      Fy -= .1;
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
   glutCreateWindow("Evan Kerns - Fenway");
   //  Tell GLUT to call "display" when the scene should be drawn
   glutDisplayFunc(display);
   //  Tell GLUT to call "reshape" when the window is resized
   glutReshapeFunc(reshape);
   //  Tell GLUT to call "special" when an arrow key is pressed
   glutSpecialFunc(special);
   //  Tell GLUT to call "key" when a key is pressed
   glutKeyboardFunc(key);
   glutIdleFunc(idle);

   texture[0] = LoadTexBMP("fenway.bmp");
   texture[1] = LoadTexBMP("seats.bmp");
   texture[2] = LoadTexBMP("dirt.bmp");
   texture[3] = LoadTexBMP("grass.bmp");
   texture[4] = LoadTexBMP("bigVideo.bmp");
   texture[5] = LoadTexBMP("wideVideo.bmp");
   texture[6] = LoadTexBMP("booth.bmp");
   // No 7
   // No 8
   texture[9] = LoadTexBMP("monster.bmp");
   texture[10] = LoadTexBMP("numbers1.bmp");
   texture[11] = LoadTexBMP("numbers2.bmp");
   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   return 0;
}
