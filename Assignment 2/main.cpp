/*
Subject: CP2060
Lecturer: Lindsay Ward
Group Members: Christopher Trott, Ashley Sexton and Aleesha Torkington
Assignment 2.cpp
Heliaac
Created: 28/08/09
Last Modified: 27/10/09 @ 02:45

Some of this code is taken from animlightpos.cpp on the LearnJCU resources page
Some code to do with lighting was gained from the URL: http://www.falloutsoftware.com/tutorials/gl/gl8.htm
Some code to do with text on screen gained from Lighthouse 3D @ URL: http://www.lighthouse3d.com/opengl/glut/index.php?bmpfontortho
Bitmap.h and Bitmap.cpp found at: http://www.gamedev.net/reference/articles/article1966.asp

/////----Part A Marks-----\\\\\
Ground Object - drawGround() (line 1085)
Helicopter - drawHeli()(line 611), drawHeliBody()(line 648), drawHeliRotor() (line 1051)
Takeoff and landing - startHeli in idle (line 2299) and keyboard (line 1463) functions
Flying Helicopter - moveHeliForward(), moveHeliBack(), moveHeliUp(), moveHeliDown() (line 1754 - 1791)
Obstacles - checkHeliCollisions() (line 1344)
Wireframe - wireframeOn() (line 1586)
Fully Shaded - shadingOn() (line 1579)
Lighting - in the init function (line 238)
Dashboard - displayDashboard() (line 1935)
Stop Helicopter - checkHeliCollisions() (line 1344)
Goals - Checkpoints, landingPadB, Time
Correct Commands - in the keyboard (line 1463) and special (line 1605) key functions
Right Click Menu - in the main function (line 2579)
Mouse Drag - in the mouse (line 1733) and mouseMotion (line 1720) functions
Other controls and functions - spacebar/b accelerate/brake in the keyboard (line 1463) and special (line 1605) key functions
Help Screen - displayHelp() (line 1868)
Texture mapping - texture mapping on helicopter(switchable), ground, sky and buildings (load texture bmp - line 413)

/////----Part B Marks-----\\\\\
More Detailed Model - drawHeliBody() (line 648)
More Realistic Movement - in the special (line 1605) key function
Keyframed landing/takeoff - startHeli in idle (line 2299) and keyboard (line 1463) functions
Random Creation of environment - Loads random map file
GLSL Shaders - commented out for use on Lab computers (uncomment to use on other computers) (setupShaders - line 366)
Extra Functionality - random map loading, sky box, zoomin/out, different preset camera views

*/

#include<math.h>
#include<string>
#include<iostream>
#include<fstream>
#include<GL/glew.h>
#include<GL/freeglut.h>
#include<time.h>
#include"Bitmap.h"

using namespace std;

struct vertex
{
    float x;
    float y;
    float z;
};

struct objectBox
{
    float xPos;			// x position
    float yPos;			// y position
    float zPos;			// z position
	float rotX;			// rotation angle in x axis
    float rotY;			// rotation angle of direction (y axis)
	float rotZ;			// rotation angle in z axis
    float xSize;		// x radius (half-length) of bounding box
    float ySize;		// y radius of bounding box
    float zSize;		// z radius of bounding box
};

struct checkPoint
{
	int checkpoint;
	float xSize;
	float ySize;
	float zSize;
	float xPos;
	float yPos;
	float zPos;
	float rotY;
	bool activated;
};

void heliLight(void);
void drawHeli(void);
void drawHeliBody(void);
void drawHeliRotor(void);
void drawGround(void);
void moveHeliForward(float speed, bool checkCol);
void moveHeliBack(float speed, bool checkCol);
void moveHeliDown(float speed, bool checkCol);
void moveHeliUp(float speed, bool checkCol);
void checkBounds(void);
bool checkBoxCollision(objectBox object1, objectBox object2);
void checkHeliCollisions(void);
void checkHeliLanding(void);
void drawBuilding(objectBox building, int textureNum);
void updateFPS(void);
void updateGameTime(void);
void displayText(void);
void resetPerspectiveProjection(void);
void setOrthographicProjection(void);
void renderBitmapString(float x, float y, void *font,char *string);
void displayHelp(void);
int cameraView = 1;
float cosDeg(float degRot);
float sinDeg(float degRot);
GLuint loadTextureBMP(char * filename, int wrap, int width, int height);
void displayDashboard(void);
void drawFinishScreen(void);
void enableFog(void);
char* getTimeString(int time);
void setupShaders(void);
char* readShaderFile(char* fileName);
void setPoint(int pointNum, float xWidth, float yWidth, float zWidth, float xPosition, float yPosition, float zPosition, float rotateY);
void loadMaps(void);
void selectMap(void);
bool readMapFile(const char* fileName);
void writeTime(const char* fileName, int time);

float cameraDistance = 6.0;
float cameraZoom = 2.0;
objectBox heli = {0, 2, 0, 0, 0, 0, 1.25, 0.75, 0.5};
float windscreenRot = 0.0;
float doorRot = 0.0;
const float MARGIN = 5.0;

int maxBuildings = 0;
objectBox eye = {cameraDistance, heli.yPos, cameraDistance, 0, 135, 0, 0, 0, 0};
objectBox *buildings;
objectBox landingPadA;
objectBox landingPadB;

bool movingForward = false;
bool movingBack = false;
bool movingUp = false;
bool movingDown = false;
bool turningLeft = false;
bool turningRight = false;

bool stopHeli = false;
bool startHeli = false;

bool gameFinished = false;
bool timeUpdated = false;

int font = (int)GLUT_BITMAP_HELVETICA_18;
int fontTitle = (int)GLUT_BITMAP_TIMES_ROMAN_24;
int textX = 20;
int textY = 20;

bool light0 = true;
bool light1 = true;
bool lighting = true;
bool helicopterOn = false;

//skyHeight global to stop heli from breaching sky top
float skyHeight = 20.0;

GLfloat light0_position[] = { 1, 20, 1, 0 };

GLuint heliBodyList;
GLuint heliRotorList;
GLuint groundList;

double rotor = 0;
const double MAX_ROTOR_SPEED = 10;
double rotorSpeed = 0.0;
float groundSize = 20.0;
int groundHeight = 0;

int windowWidth = 600;
int windowHeight = 600;

int windowPosWidth = 40;
int windowPosHeight = 40;

int frames = 0;
int timePassed = 0;
int timeBase = 0;
float fps = 50.0;
char* strFps = new char[4];			// FPS string for display on-screen
char* strGameTime = new char[8];	// Game time string
int bestTime = 0;
char* strBestTime = new char[8];
int gameTime = 0;
int gameTimeBase = 0;
int penaltyTime = 0;

const int ROTATE_SPEED = 180;
const int HELI_SPEED = 12;
const int MAX_SPEED = 50;
const int MIN_SPEED = 2;
const float LEAN_FACTOR = 15.0;
float heliLeanFront = 0.0;
float heliLeanSide = 0.0;
int rotSpeed = ROTATE_SPEED / fps;
int currentSpeed = HELI_SPEED;
float heliSpeed = currentSpeed / fps;

bool pause = true;
bool wire = false;

int last_mouse_x = 0;
int last_mouse_y = 0;
bool leftMouseDown = false;

bool fogOn = true;
const float PI = 3.1415926535897932384626433832795;

const int MAX_TEXTURES = 10;
GLuint textures[MAX_TEXTURES];

int maxCheckpoints = 0;		// Cannot exceed 100. Only set as high as is required.
checkPoint *points;
int checkpointNum = 0;

bool heliTextures = true;
bool shaderOn = false;
bool useOldOGL = true;

int numMaps = 0;
string* maps;
const char* strCurrentMap;

const GLfloat heliShininess = 50.0;			// Shininess of helicopeter
const GLfloat genShininess = 10.0;			// Genereal shininess

GLhandleARB vertexShader;
GLhandleARB fragmentShader;
GLhandleARB shaderProgram;

// Initialise the OpenGL properties
void init(void)
{
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
    glShadeModel(GL_SMOOTH);

	if (fogOn)
	{
		enableFog();
	}

	// Load maps and choose a map for this run
	srand(time(NULL));
	loadMaps();
	selectMap();
	readMapFile(strCurrentMap);

    // Make object materials equal to glColor*() properties
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

	heli.yPos = groundHeight + heli.ySize;

    // Define the heliBody display list
    heliBodyList = glGenLists(1);
    glNewList(heliBodyList, GL_COMPILE);
    drawHeliBody();
    glEndList();
   
    // Define the heliRotor display list
    heliRotorList = glGenLists(1);
    glNewList(heliRotorList, GL_COMPILE);
    drawHeliRotor();
    glEndList();

    glClearColor(0.95, 0.95, 1.0, 0.0);

	heliLight();
   
    // Set light0 position
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

    // Create light0 components
    GLfloat ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat diffuseLight[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    GLfloat specularLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };

    // Assign light0 components
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

	glMaterialf(GL_FRONT, GL_SHININESS, genShininess);

    timeBase = glutGet(GLUT_ELAPSED_TIME);

	textures[0] = loadTextureBMP( "Textures/ground.bmp", true, 256, 256 );
	textures[1] = loadTextureBMP( "Textures/building.bmp", true, 256, 256 );
	textures[2] = loadTextureBMP( "Textures/heliTex.bmp", true, 256, 256 );
	textures[3] = loadTextureBMP( "Textures/heliPadA.bmp", true, 256, 256 );
	textures[4] = loadTextureBMP( "Textures/heliPadB.bmp", true, 256, 256 );
	textures[5] = loadTextureBMP( "Textures/sky.bmp", true, 256, 256);
}

void loadMaps()
{
	ifstream fin;
	fin.open("Maps/maps.txt");

	while ( !fin.eof() )
	{
		fin.ignore(1000, '\n');
		numMaps++;
	}

	fin.seekg(0, ios::beg);
	cout << "There are " << numMaps << " maps available.\n";
	maps = new string[numMaps];

	for (int line = 0; line < numMaps; line++)
	{
		getline(fin, maps[line], '\n');
	}

	fin.close();
}

void selectMap()
{
	int randomNumber = rand() % numMaps;
	strCurrentMap = maps[randomNumber].c_str();
}

// These functions have been adapted from Lighthouse 3D GLSL Examples
// http://www.lighthouse3d.com/opengl/glsl/index.php?oglexample1 
char* readShaderFile(char* fileName)
{
	FILE *fp;
	char *content = NULL;

	int count=0;

	if (fileName != NULL) 
	{
		fp = fopen(fileName,"rt");

		if (fp != NULL) 
		{
			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);

			if (count > 0) 
			{
				content = (char *)malloc(sizeof(char) * (count+1));
				count = fread(content,sizeof(char),count,fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	return content;
}

void setupShaders()
{
	char *vs;
	char *fs;

	vertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	fragmentShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

	vs = readShaderFile("Shaders/dirLightAmbDiffSpecPix.vert");
	fs = readShaderFile("Shaders/dirLightAmbDiffSpecPix.frag");

	const char * vv = vs;
	const char * ff = fs;

	glShaderSourceARB(vertexShader, 1, &vv, NULL);
	glShaderSourceARB(fragmentShader, 1, &ff, NULL);

	free(vs);
	free(fs);

	glCompileShaderARB(vertexShader);
	glCompileShaderARB(fragmentShader);

	shaderProgram = glCreateProgramObjectARB();
	
	glAttachObjectARB(shaderProgram,vertexShader);
	glAttachObjectARB(shaderProgram,fragmentShader);

	glLinkProgramARB(shaderProgram);
	glUseProgramObjectARB(shaderProgram);
}
// End GLSL adapted code

void enableFog(void)
{
	GLfloat fogColor[4] = {1.0, 1.0, 1.0, 1.0};
	GLfloat density = 0.04;

	glEnable(GL_DEPTH_TEST); //enable the depth testing
	glEnable(GL_FOG); //enable the fog
	glFogi(GL_FOG_MODE, GL_EXP2); //set the fog mode to GL_EXP2

	glFogfv(GL_FOG_COLOR, fogColor); //set the fog color to our color chosen above
	glFogf(GL_FOG_DENSITY, density); //set the density to the value above
	glHint(GL_FOG_HINT, GL_DONT_CARE); // set the fog to look the nicest, may slow down on older cards
}

GLuint loadTextureBMP( char * filename, int wrap, int width, int height )
{
	Bitmap *image;
    GLuint texture;

	image = new Bitmap();

	glEnable(GL_TEXTURE_2D);

	if (image == NULL) {
		cout << "Could not create Bitmap class.\n";
		return -1;
	}

	if (image->loadBMP(filename))
	{
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

		// if wrap is true, the texture wraps over at the edges (repeat)
		//       ... false, the texture ends at the edges (clamp)
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap ? GL_REPEAT : GL_CLAMP );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap ? GL_REPEAT : GL_CLAMP );

		glTexImage2D(GL_TEXTURE_2D, 0, 3, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data);

		glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	}
	else 
	{
		cout << "Could not load bitmap " << filename << ".\n";
		return -1;
	}

	if (image)
	{
		delete image;
	}

    return texture;
}


// Draw sky
void drawSky()
{
	float skyz = 0.0;
	float skyzb = 0.0;
	float skyx = 0.0;
	float skyxb = 0.0;
	float skyDist = 20.0;

	if (heli.zPos + skyDist < groundSize)
	{
		skyz = heli.zPos + skyDist;
	}
	else skyz = groundSize;

	if (heli.zPos - skyDist > -groundSize)
	{
		skyzb = heli.zPos - skyDist;
	}
	else skyzb = -groundSize;

	if (heli.xPos + skyDist < groundSize)
	{
		skyx = heli.xPos + skyDist;
	}
	else skyx = groundSize;

	if (heli.xPos - skyDist > -groundSize)
	{
		skyxb = heli.xPos - skyDist;
	}
	else skyxb = -groundSize;

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[5]);			// Select The Sky Texture
	glColor3f(0.33, 0.64, 1);
	glBegin(GL_QUADS);							// Begin Drawing Quads
	//top sky
		glTexCoord2f(0.0f,1.0f); glVertex3f(groundSize, skyHeight, groundSize);	// Top Left
		glTexCoord2f(1.0f,1.0f); glVertex3f(-groundSize, skyHeight, groundSize);	// Top Right
		glTexCoord2f(1.0f,0.0f); glVertex3f(-groundSize, skyHeight, -groundSize);	// Bottom Right
		glTexCoord2f(0.0f,0.0f); glVertex3f(groundSize, skyHeight, -groundSize);	// Bottom Left
	//right sky
		glTexCoord2f(0.0f,1.0f); glVertex3f(-groundSize, skyHeight, skyz);	// Top Left
		glTexCoord2f(1.0f,1.0f); glVertex3f(groundSize, skyHeight, skyz);	// Top Right
		glTexCoord2f(1.0f,0.0f); glVertex3f(groundSize, 0, skyz);	// Bottom Right
		glTexCoord2f(0.0f,0.0f); glVertex3f(-groundSize, 0, skyz);	// Bottom Left
	//front sky
		glTexCoord2f(1.0f,1.0f); glVertex3f(skyx, skyHeight, groundSize);	// Top Right
		glTexCoord2f(0.0f,1.0f); glVertex3f(skyx, skyHeight, -groundSize);	// Top Left
		glTexCoord2f(0.0f,0.0f); glVertex3f(skyx, 0, -groundSize);	// Bottom Left
		glTexCoord2f(1.0f,0.0f); glVertex3f(skyx, 0, groundSize);	// Bottom Right
	// left sky
		glTexCoord2f(1.5f,1.0f); glVertex3f(groundSize, skyHeight, skyzb);		// Top Right
		glTexCoord2f(0.5f,1.0f); glVertex3f(-groundSize, skyHeight, skyzb);		// Top Left
		glTexCoord2f(0.5f,0.0f); glVertex3f(-groundSize, 0.0, skyzb);		// Bottom Left
		glTexCoord2f(1.5f,0.0f); glVertex3f(groundSize, 0.0, skyzb);		// Bottom Right
	// back sky
		// making sky roll toward viewer
		glTexCoord2f(1.0f,1.0f); glVertex3f(skyxb,skyHeight,-groundSize);	// Top Right
		glTexCoord2f(0.0f,1.0f); glVertex3f(skyxb,skyHeight,groundSize);	// Top Left
		glTexCoord2f(0.0f,0.0f); glVertex3f(skyxb,0.0,groundSize);	// Bottom Left
		glTexCoord2f(1.0f,0.0f); glVertex3f(skyxb,0.0,-groundSize);	// Bottom Right

	glEnd();	// Done Drawing Quads
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);

}

void heliLight()
{
	// Set light1 position
	GLfloat light1_position[] = { heli.xSize + heli.xSize, heli.ySize - heli.ySize, heli.zSize, 1.0 };
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

	// Create light1 components
	//GLfloat ambientLight1[] = { 0.6f, 0.6f, 0.6f, 1.0f };
	GLfloat diffuseLight1[] = { 0.9f, 0.9f, 0.9f, 1.0f };
	GLfloat specularLight1[] = { 0.7f, 0.7f, 0.7f, 1.0f };

	// Assign light1 components
	//glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specularLight1);
}

void drawBuilding(objectBox building, int textureNum)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[textureNum]);
//	   -------
//   /       /|
//	v2-----v3 |
//	|       | |
//	|       | |
//	|       | |
//	|       | |
//	|       |/
//	v0-----v1

	vertex v0 = {-building.xSize, 0.0, building.zSize};
	vertex v1 = {building.xSize, 0.0, building.zSize};
	vertex v2 = {-building.xSize, building.ySize, building.zSize};
	vertex v3 = {building.xSize, building.ySize, building.zSize};

	glTranslatef(building.xPos, building.yPos, building.zPos);
	glRotatef(building.rotY, 0.0, 1.0, 0.0);

	glBegin(GL_QUADS);
	// Draw wall
	glTexCoord2f(0.0, 0.0);							glVertex3f(v0.x, v0.y, v0.z);
	glTexCoord2f(building.xSize, 0.0);				glVertex3f(v1.x, v1.y, v1.z);
	glTexCoord2f(building.xSize, building.ySize);	glVertex3f(v3.x, v3.y, v3.z);
	glTexCoord2f(0.0, building.ySize);				glVertex3f(v2.x, v2.y, v2.z);
	// Draw wall
	glTexCoord2f(0.0,0.0);							glVertex3f(v0.x, v0.y, -v0.z);
	glTexCoord2f(0.0,building.ySize);				glVertex3f(v2.x, v2.y, -v2.z);
	glTexCoord2f(building.xSize,building.ySize);	glVertex3f(v3.x, v3.y, -v3.z);
	glTexCoord2f(building.xSize,0.0);				glVertex3f(v1.x, v1.y, -v1.z);
	// Draw wall
	glTexCoord2f(0.0, building.ySize);				glVertex3f(v2.x, v2.y, -v2.z);
	glTexCoord2f(0.0, 0.0);							glVertex3f(v0.x, v0.y, -v0.z);
	glTexCoord2f(building.xSize, 0.0);				glVertex3f(v0.x, v0.y, v0.z);
	glTexCoord2f(building.xSize, building.ySize);	glVertex3f(v2.x, v2.y, v2.z);
	// Draw wall
	glTexCoord2f(0.0, building.ySize);				glVertex3f(v3.x, v3.y, v3.z);
	glTexCoord2f(0.0, 0.0);							glVertex3f(v1.x, v1.y, v1.z);
	glTexCoord2f(building.xSize, 0.0);				glVertex3f(v1.x, v1.y, -v1.z);
	glTexCoord2f(building.xSize, building.ySize);	glVertex3f(v3.x, v3.y, -v3.z);
	// Draw floor
	glVertex3f(v0.x, v0.y, v0.z);
	glVertex3f(v1.x, v1.y, v1.z);
	glVertex3f(v1.x, v1.y, -v1.z);
	glVertex3f(v0.x, v0.y, -v0.z);
	// Draw roof
	glVertex3f(v2.x, v2.y, v2.z);
	glVertex3f(v3.x, v3.y, v3.z);
	glVertex3f(v3.x, v3.y, -v3.z);
	glVertex3f(v2.x, v2.y, -v2.z);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);
}

void drawHeli()
{
	glMaterialf(GL_FRONT, GL_SHININESS, heliShininess);
    glPushMatrix();
    // Go to the heli position before drawing the heli
    glTranslatef(heli.xPos, heli.yPos, heli.zPos);
    glRotatef(heli.rotY, 0.0, 1.0, 0.0);
    glRotatef(heliLeanFront, 0.0, 0.0, 1.0);
    glRotatef(heliLeanSide, 1.0, 0.0, 0.0);
	
	// Animate rotor
	glPushMatrix();
    glTranslatef(0.0, 0.6, 0.0);
    glRotatef(rotor, 0.0, 1.0, 0.0);

    // Draw rotor
    glCallList(heliRotorList);
	glPopMatrix();

	glPushMatrix();
	glRotatef(90, 1.0, 0.0, 0.0);
	glTranslatef(-1.35, 0.24, -0.25);
    glRotatef(rotor, 0.0, 1.0, 0.0);
	glScalef(0.4, 0.4, 0.4);

    // Draw tail rotor
    glCallList(heliRotorList);
	glPopMatrix();

    // Draw body
	drawHeliBody();

    glPopMatrix();
	glMaterialf(GL_FRONT, GL_SHININESS, genShininess);
}

// Draw the body
void drawHeliBody()
{
	if (heliTextures)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textures[2]);
	}

	glPushMatrix();
	glScalef(0.5, 0.5, 0.5);
	//
	//     v12           v0-------------------v1 \
	//     | \             \                  |   \
    //     |  v8------------v9                |    v4
    //     |   |            |  \             v5    |
    //    v13-v11-----------v14-v10           |    v6
    //                             \          |   /
	//                              \v2-------v3 /
	//

	// Body vertices
	vertex v0 = { -1.3, 1.0, 1.0 };
	vertex v1 = { 0.9, 1.0, 1.0 };
	vertex v2 = { 0.3, -1.0, 1.0 };
	vertex v3 = { 2.5, -1.0, 1.0 };
	vertex v4 = { 2.1, 0.8, 0.5 };
	vertex v5 = { 1.7, 0.0, 1.0 };
	vertex v6 = { 2.7, 0.0, 0.5 };
	// Body normals
	vertex n0 = { 0.0, 1.0, 1.0 };
	vertex n1 = { 0.0, 1.0, 1.0 };
	vertex n2 = { -1.0, -1.0, 1.0 };
	vertex n3 = { 0.0, -1.0, 1.0 };
	vertex n4 = { 1.0, 1.0, 1.0 };
	vertex n5 = { 1.0, 0.0, 1.0 };
	vertex n6 = { 1.0, -1.0, 1.0 };
	// Tail vertices
	vertex v8 = { -2.5, 0.4, 0.4 };
	vertex v9 = { -0.8, 0.4, 0.4 };
	vertex v10 = { -0.185, -0.4, 0.4 };
	vertex v11 = { -2.5, -0.4, 0.4 };
	vertex v14 = { -0.8, -0.4, 0.4 };
	// Tail Normals
	vertex n8 = { 1.0, 1.0, 1.0 };
	vertex n9 = { -1.0, 1.0, 1.0 };
	vertex n10 = { -1.0, -1.0, 1.0 };
	vertex n11 = { 0.0, -1.0, 1.0 };
	vertex n14 = { 0.0, -1.0, 1.0 };
	// Tip of tail
	vertex v12 = {-2.8, 1.0, 0.4};
	vertex v13 = {-2.8, -0.4, 0.4};
	// Tip normals
	vertex n12 = { 0.0, 1.0, 1.0 };
	vertex n13 = {-1.0, -1.0, 1.0};

	// Draw main body
	glColor4f(1.0, 0.4, 0.4, 1.0);	// Light Red

	glPushMatrix();

	// Rotate right door
	glTranslatef(v1.x, v1.y, v1.z);			// Translate v1
	glRotatef(-doorRot, 1.0, 0.0, 0.0);		// Rotate
	glTranslatef(-v1.x, -v1.y, -v1.z);		// Translate -v1

	// Right Side Panel
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(n0.x, n0.y, n0.z);
	glTexCoord2f(0.0, 0.0);		glVertex3f(v0.x, v0.y, v0.z);
	glNormal3f(n1.x, n1.y, n1.z);
	glTexCoord2f(0.0, 1.0);		glVertex3f(v1.x, v1.y, v1.z);
	glNormal3f(n2.x, n2.y, n2.z);
	glTexCoord2f(1.0, 1.0);		glVertex3f(v2.x, v2.y, v2.z);
	glNormal3f(n3.x, n3.y, n3.z);
	glTexCoord2f(1.0, 0.0);		glVertex3f(v3.x, v3.y, v3.z);
	glEnd();

	glPopMatrix();

	// Top Panel
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(n0.x, n0.y, n0.z);
	glTexCoord2f(0.0, 0.0);		glVertex3f(v0.x, v0.y, v0.z);
	glNormal3f(n1.x, n1.y, n1.z);
	glTexCoord2f(0.0, 1.0);		glVertex3f(v1.x, v1.y, v1.z);
	glNormal3f(n0.x, n0.y, -n0.z);
	glTexCoord2f(1.0, 1.0);		glVertex3f(v0.x, v0.y, -v0.z);
	glNormal3f(n1.x, n1.y, -n1.z);
	glTexCoord2f(1.0, 0.0);		glVertex3f(v1.x, v1.y, -v1.z);
	glEnd();

	// Bottom Panel
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(n2.x, n2.y, n2.z);
	glTexCoord2f(0.0, 0.0);		glVertex3f(v2.x, v2.y, v2.z);
	glNormal3f(n3.x, n3.y, n3.z);
	glTexCoord2f(0.0, 1.0);		glVertex3f(v3.x, v3.y, v3.z);
	glNormal3f(n2.x, n2.y, -n2.z);
	glTexCoord2f(1.0, 1.0);		glVertex3f(v2.x, v2.y, -v2.z);
	glNormal3f(n3.x, n3.y, -n3.z);
	glTexCoord2f(1.0, 0.0);		glVertex3f(v3.x, v3.y, -v3.z);
	glEnd();

	// Left Panel
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(n0.x, n0.y, -n0.z);
	glTexCoord2f(0.0, 0.0);		glVertex3f(v0.x, v0.y, -v0.z);
	glNormal3f(n1.x, n1.y, -n1.z);
	glTexCoord2f(0.0, 1.0);		glVertex3f(v1.x, v1.y, -v1.z);
	glNormal3f(n2.x, n2.y, -n2.z);
	glTexCoord2f(1.0, 1.0);		glVertex3f(v2.x, v2.y, -v2.z);
	glNormal3f(n3.x, n3.y, -n3.z);
	glTexCoord2f(1.0, 0.0);		glVertex3f(v3.x, v3.y, -v3.z);
	glEnd();

	// Rear Panel
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(n0.x, n0.y, n0.z);
	glTexCoord2f(0.0, 0.0);		glVertex3f(v0.x, v0.y, v0.z);
	glNormal3f(n2.x, n2.y, n2.z);
	glTexCoord2f(0.0, 1.0);		glVertex3f(v2.x, v2.y, v2.z);
	glNormal3f(n0.x, n0.y, -n0.z);
	glTexCoord2f(1.0, 1.0);		glVertex3f(v0.x, v0.y, -v0.z);
	glNormal3f(n2.x, n2.y, -n2.z);
	glTexCoord2f(1.0, 0.0);		glVertex3f(v2.x, v2.y, -v2.z);
	glEnd();

	// Tip of tail (top)
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(n8.x, n8.y, n8.z);
	glTexCoord2f(0.0, 0.0);		glVertex3f(v8.x, v8.y, v8.z);
	glNormal3f(n12.x, n12.y, n12.z);
	glTexCoord2f(0.0, 1.0);		glVertex3f(v12.x, v12.y, v12.z);
	glNormal3f(n8.x, n8.y, -n8.z);
	glTexCoord2f(1.0, 1.0);		glVertex3f(v8.x, v8.y, -v8.z);
	glNormal3f(n12.x, n12.y, -n12.z);
	glTexCoord2f(1.0, 0.0);		glVertex3f(v12.x, v12.y, -v12.z);
	glEnd();

	// Tip of tail (back)
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(n12.x, n12.y, n12.z);
	glTexCoord2f(0.0, 0.0);		glVertex3f(v12.x, v12.y, v12.z);
	glNormal3f(n13.x, n13.y, n13.z);
	glTexCoord2f(0.0, 1.0);		glVertex3f(v13.x, v13.y, v13.z);
	glNormal3f(n12.x, n12.y, -n12.z);
	glTexCoord2f(1.0, 1.0);		glVertex3f(v12.x, v12.y, -v12.z);
	glNormal3f(n13.x, n13.y, -n13.z);
	glTexCoord2f(1.0, 0.0);		glVertex3f(v13.x, v13.y, -v13.z);
	glEnd();

	// Tip of tail (side1)
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(n12.x, n12.y, n12.z);
	glTexCoord2f(0.0, 0.0);		glVertex3f(v12.x, v12.y, v12.z);
	glNormal3f(n8.x, n8.y, n8.z);
	glTexCoord2f(0.0, 1.0);		glVertex3f(v8.x, v8.y, v8.z);
	glNormal3f(n13.x, n13.y, n13.z);
	glTexCoord2f(1.0, 1.0);		glVertex3f(v13.x, v13.y, v13.z);
	glNormal3f(n11.x, n11.y, n11.z);
	glTexCoord2f(1.0, 0.0);		glVertex3f(v11.x, v11.y, v11.z);
	glEnd();

	// Tip of tail (side2)
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(n12.x, n12.y, -n12.z);
	glTexCoord2f(0.0, 0.0);		glVertex3f(v12.x, v12.y, -v12.z);
	glNormal3f(n8.x, n8.y, -n8.z);
	glTexCoord2f(0.0, 1.0);		glVertex3f(v8.x, v8.y, -v8.z);
	glNormal3f(n13.x, n13.y, -n13.z);
	glTexCoord2f(1.0, 1.0);		glVertex3f(v13.x, v13.y, -v13.z);
	glNormal3f(n11.x, n11.y, -n11.z);
	glTexCoord2f(1.0, 0.0);		glVertex3f(v11.x, v11.y, -v11.z);
	glEnd();

	// Tip of tail (bottom)
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(n11.x, n11.y, n11.z);
	glTexCoord2f(0.0, 0.0);		glVertex3f(v11.x, v11.y, v11.z);
	glNormal3f(n13.x, n13.y, n13.z);
	glTexCoord2f(0.0, 1.0);		glVertex3f(v13.x, v13.y, v13.z);
	glNormal3f(n11.x, n11.y, -n11.z);
	glTexCoord2f(1.0, 1.0);		glVertex3f(v11.x, v11.y, -v11.z);
	glNormal3f(n13.x, n13.y, -n13.z);
	glTexCoord2f(1.0, 0.0);		glVertex3f(v13.x, v13.y, -v13.z);
	glEnd();
	
	// Front Heli Nose
	glBegin(GL_TRIANGLE_STRIP);
	glColor4f(1.0, 0.4, 0.4, 1.0);	// Light Red
	glNormal3f(n5.x, n5.y, n5.z);
	glTexCoord2f(0.0, 0.0);		glVertex3f(v5.x, v5.y, v5.z);
	glNormal3f(n3.x, n3.y, n3.z);
	glTexCoord2f(0.0, 1.0);		glVertex3f(v3.x, v3.y, v3.z);
	glNormal3f(n6.x, n6.y, n6.z);
	glTexCoord2f(1.0, 1.0);		glVertex3f(v6.x, v6.y, v6.z);
	glNormal3f(n3.x, n3.y, 0.0);
	glTexCoord2f(1.0, 0.0);		glVertex3f(v3.x, v3.y, 0.0);
	glNormal3f(n6.x, n6.y, -n6.z);
	glTexCoord2f(0.0, 1.0);		glVertex3f(v6.x, v6.y, -v6.z);
	glNormal3f(n3.x, n3.y, -n3.z);
	glTexCoord2f(1.0, 1.0);		glVertex3f(v3.x, v3.y, -v3.z);
	glNormal3f(n5.x, n5.y, -n5.z);
	glTexCoord2f(1.0, 0.0);		glVertex3f(v5.x, v5.y, -v5.z);
	glEnd();

	// Main Heli Tail
	glBegin(GL_TRIANGLE_STRIP);
	// Left
	glNormal3f(n8.x, n8.y, n8.z);
	glTexCoord2f(0.0, 0.0);		glVertex3f(v8.x, v8.y, v8.z);
	glNormal3f(n9.x, n9.y, n9.z);
	glTexCoord2f(0.0, 1.0);		glVertex3f(v9.x, v9.y, v9.z);
	glNormal3f(n14.x, n14.y, n14.z);
	glTexCoord2f(1.0, 1.0);		glVertex3f(v14.x, v14.y, v14.z);
	glNormal3f(n11.x, n11.y, n11.z);
	glTexCoord2f(1.0, 0.0);		glVertex3f(v11.x, v11.y, v11.z);
	glNormal3f(n8.x, n8.y, n8.z);
	glTexCoord2f(0.0, 0.0);		glVertex3f(v8.x, v8.y, v8.z);
	// Top
	glNormal3f(n8.x, n8.y, -n8.z);
	glTexCoord2f(0.0, 0.0);		glVertex3f(v8.x, v8.y, -v8.z);
	glNormal3f(n9.x, n9.y, n9.z);
	glTexCoord2f(0.0, 1.0);		glVertex3f(v9.x, v9.y, v9.z);
	glNormal3f(n9.x, n9.y, -n9.z);
	glTexCoord2f(1.0, 1.0);		glVertex3f(v9.x, v9.y, -v9.z);
	glNormal3f(n8.x, n8.y, -n8.z);
	glTexCoord2f(0.0, 0.0);		glVertex3f(v8.x, v8.y, -v8.z);
	// Right
	glNormal3f(n9.x, n9.y, -n9.z);
	glTexCoord2f(0.0, 1.0);		glVertex3f(v9.x, v9.y, -v9.z);
	glNormal3f(n14.x, n14.y, -n14.z);
	glTexCoord2f(1.0, 1.0);		glVertex3f(v14.x, v14.y, -v14.z);
	glNormal3f(n11.x, n11.y, -n11.z);
	glTexCoord2f(1.0, 0.0);		glVertex3f(v11.x, v11.y, -v11.z);
	glNormal3f(n8.x, n8.y, -n8.z);
	glTexCoord2f(0.0, 0.0);		glVertex3f(v8.x, v8.y, -v8.z);
	glEnd();

	//Bottom
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(n11.x, n11.y, n11.z);
	glTexCoord2f(0.0, 0.0);		glVertex3f(v11.x, v11.y, v11.z);
	glNormal3f(n11.x, n11.y, -n11.z);
	glTexCoord2f(0.0, 1.0);		glVertex3f(v11.x, v11.y, -v11.z);
	glNormal3f(n14.x, n14.y, n14.z);
	glTexCoord2f(1.0, 1.0);		glVertex3f(v14.x, v14.y, v14.z);
	glNormal3f(n14.x, n14.y, -n14.z);
	glTexCoord2f(0.0, 0.0);		glVertex3f(v14.x, v14.y, -v14.z);
	glEnd();

	//Bottom 2
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(n14.x, n14.y, n14.z);
	glTexCoord2f(0.0, 0.0);		glVertex3f(v14.x, v14.y, v14.z);
	glNormal3f(n14.x, n14.y, -n14.z);
	glTexCoord2f(0.0, 1.0);		glVertex3f(v14.x, v14.y, -v14.z);
	glNormal3f(n10.x, n10.y, n10.z);
	glTexCoord2f(1.0, 1.0);		glVertex3f(v10.x, v10.y, v10.z);
	glNormal3f(n10.x, n10.y, -n10.z);
	glTexCoord2f(1.0, 0.0);		glVertex3f(v10.x, v10.y, -v10.z);
	glEnd();

	//Close to body (left)
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(n9.x, n9.y, n9.z);
	glTexCoord2f(0.0, 0.0);		glVertex3f(v9.x, v9.y, v9.z);
	glNormal3f(n14.x, n14.y, n14.z);
	glTexCoord2f(0.0, 1.0);		glVertex3f(v14.x, v14.y, v14.z);
	glNormal3f(n10.x, n10.y, n10.z);
	glTexCoord2f(1.0, 1.0);		glVertex3f(v10.x, v10.y, v10.z);
	glEnd();
	//Close to body (right)
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(n9.x, n9.y, -n9.z);
	glTexCoord2f(0.0, 0.0);		glVertex3f(v9.x, v9.y, -v9.z);
	glNormal3f(n14.x, n14.y, -n14.z);
	glTexCoord2f(0.0, 1.0);		glVertex3f(v14.x, v14.y, -v14.z);
	glNormal3f(n10.x, n10.y, -n10.z);
	glTexCoord2f(1.0, 1.0);		glVertex3f(v10.x, v10.y, -v10.z);
	glEnd();
	
	glPushMatrix();

	glTranslatef(v2.x, v2.y, v2.z);

	// Change texture for chair
	glBindTexture(GL_TEXTURE_2D, textures[4]);

	//Chair
	glPushMatrix();
	glColor3f(1.0, 1.0, 1.0);
	glTranslatef(0.5, 0.9, -1.0);
	glScalef(0.2, 1.5, 1.5);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0, 1.0, 1.0);
	glTranslatef(0.8, 0.25, -1.0);
	glScalef(0.75, 0.2, 1.5);
	glutSolidCube(1.0);
	glPopMatrix();

	// Draw the Skids
	glPushMatrix();
	glColor3f(0.1, 0.1, 0.3);
	glRotatef(90, 1.0, 0.0, 0.0);
	glTranslatef(0.5, -0.5, 0.0);
	glutSolidCylinder(0.125, 0.25, 15.0, 15.0);
	glTranslatef(1.0, 0.0, 0.0);
	glutSolidCylinder(0.125, 0.25, 15.0, 15.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1.0, -0.3, -0.5);
	glScalef(2.5, 0.2, 0.2);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.1, 0.1, 0.3);
	glRotatef(90, 1.0, 0.0, 0.0);
	glTranslatef(0.5, -1.5, 0.0);
	glutSolidCylinder(0.125, 0.25, 15.0, 15.0);
	glTranslatef(1.0, 0.0, 0.0);
	glutSolidCylinder(0.125, 0.25, 15.0, 15.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1.0, -0.3, -1.5);
	glScalef(2.5, 0.2, 0.2);
	glutSolidCube(1.0);
	glPopMatrix();
	// End Skids
	glPopMatrix();

	// Rotate cockpit windscreen
	glTranslatef(v1.x, v1.y, v1.z);					// Translate v1
	glRotatef(windscreenRot, 0.0, 0.0, 1.0);		// Rotate
	glTranslatef(-v1.x, -v1.y, -v1.z);				// Translate -v1

	// Draw cockpit window
	glColor4f(0.0, 0.0, 0.6, 0.5);	// Blue
	// Right Screen
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(n1.x, n1.y, n1.z);
	glVertex3f(v1.x, v1.y, v1.z);
	glNormal3f(n4.x, n4.y, n4.z);
	glVertex3f(v4.x, v4.y, v4.z);
	glNormal3f(n5.x, n5.y, n5.z);
	glVertex3f(v5.x, v5.y, v5.z);
	glNormal3f(n6.x, n6.y, n6.z);
	glVertex3f(v6.x, v6.y, v6.z);
	glEnd();

	// Front top screen
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(n1.x, n1.y, n1.z);
	glVertex3f(v1.x, v1.y, v1.z);
	glNormal3f(n4.x, n4.y, n4.z);
	glVertex3f(v4.x, v4.y, v4.z);
	glNormal3f(n1.x, n1.y, 0.0);
	glVertex3f(v1.x, v1.y, 0.0);
	glNormal3f(n4.x, n4.y, -n4.z);
	glVertex3f(v4.x, v4.y, -v4.z);
	glNormal3f(n1.x, n1.y, -n1.z);
	glVertex3f(v1.x, v1.y, -v1.z);
	glEnd();

	// Front mid screen
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(n4.x, n4.y, n4.z);
	glVertex3f(v4.x, v4.y, v4.z);
	glNormal3f(n4.x, n4.y, -n4.z);
	glVertex3f(v4.x, v4.y, -v4.z);
	glNormal3f(n6.x, n6.y, n6.z);
	glVertex3f(v6.x, v6.y, v6.z);
	glNormal3f(n6.x, n6.y, -n6.z);
	glVertex3f(v6.x, v6.y, -v6.z);
	glEnd();

	// Left Screen
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(n1.x, n1.y, -n1.z);
	glVertex3f(v1.x, v1.y, -v1.z);
	glNormal3f(n4.x, n4.y, -n4.z);
	glVertex3f(v4.x, v4.y, -v4.z);
	glNormal3f(n5.x, n5.y, -n5.z);
	glVertex3f(v5.x, v5.y, -v5.z);
	glNormal3f(n6.x, n6.y, -n6.z);
	glVertex3f(v6.x, v6.y, -v6.z);
	glEnd();

	glPopMatrix();

	if (heliTextures)
	{
		glDisable(GL_TEXTURE_2D);
	}
}

// Draw the rotor blades
void drawHeliRotor()
{
        glPushMatrix();
		glScalef(0.5, 0.5, 0.5);
        glTranslatef(0.0, 0.2, 0.0);

        glPushMatrix();     
        // Make color blue
        glColor3f(0.0f,0.0f,1.0f);
        // Draw rotor axle
        glRotatef(90, 1.0, 0.0, 0.0);
        glutSolidCylinder(0.4, 0.4, 15, 15);
        glPopMatrix();

        glPushMatrix();
        // Make color grey
        glColor3f(0.8, 0.8, 0.8);
        // Draw blades
        glTranslatef(0.0, 0.05, 0.0);
        glScalef(3.0, 0.1, 0.2);
        glutSolidCube(1.0);
        glPopMatrix();

        glPushMatrix();
		glTranslatef(0.0, 0.05, 0.0);
        glRotatef(90, 0.0, 1.0, 0.0);
        glScalef(3.0, 0.1, 0.2);
        glutSolidCube(1.0);
        glPopMatrix();

        glPopMatrix();
}

// Make a yellow ground square with 2 x groundSize width and length
void drawGround(void)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D, textures[0]);

	// Make color yellow
	glColor4f(1.0, 1.0, 1.0, 1.0);

	// Draw the ground
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);						glVertex3f(-groundSize, groundHeight, groundSize);
	glTexCoord2f(groundSize/2, 0.0);			glVertex3f(groundSize, groundHeight, groundSize);
	glTexCoord2f(groundSize/2, groundSize/2);	glVertex3f(groundSize, groundHeight, -groundSize);
	glTexCoord2f(0.0, groundSize/2);			glVertex3f(-groundSize, groundHeight, -groundSize);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);
}

void drawCheckpoint(int checkpoint)
{
	// Draw checkpoint number
	const int POINT_NUM_STR = 2;
	char* pointStr = new char[POINT_NUM_STR];
	sprintf(pointStr, "%.2i", checkpoint + 1);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glRasterPos3f(points[checkpoint].xPos, points[checkpoint].yPos, points[checkpoint].zPos);
	for(int i = 0; i < POINT_NUM_STR; i++)
	{
		glutBitmapCharacter((void *)fontTitle, pointStr[i]);
	}

	glPushMatrix();
	
	if (points[checkpoint].activated)
	{
		// If the checkpoint has been activated, draw green
		glColor4f(0.0, 1.0, 0.0, 0.5);
	}
	else
	{
		// Otherwise draw red
		glColor4f(1.0, 0.0, 0.0, 0.5);
	}

	// Draw the checkpoint
	glTranslatef(points[checkpoint].xPos, points[checkpoint].yPos, points[checkpoint].zPos);
	glRotatef(points[checkpoint].rotY, 0.0, 1.0, 0.0);
	glScalef(points[checkpoint].xSize, points[checkpoint].ySize, points[checkpoint].zSize);
	glutSolidCube(1.0);
	glPopMatrix();
}

void drawLandingPad(objectBox pad, int textureNum)
{
	// Use planes to determine how far the texture is stretched/tiled
	GLfloat texParamsX[4] = {0.4, 0.0, 0.2, 0.0};
	GLfloat texParamsY[4] = {0.0, 0.4, 0.2, 0.0};

	glPushMatrix();
	// Draw a landing pad
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[textureNum]);
	glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_S, GL_OBJECT_PLANE, texParamsX);
	glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, texParamsY);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);

	glColor4f(1.0, 1.0, 1.0, 1.0);
	glTranslatef(pad.xPos, pad.yPos, pad.zPos);
	glRotatef(-90, 1.0, 0.0, 0.0);
	glutSolidCylinder(pad.xSize, pad.ySize, 10, 10);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glPopMatrix();
}

void checkBounds(void)
{
    // If outside of x bounds, move to within
    if (heli.xPos < -groundSize)
    {      
        heli.xPos = -groundSize;
        eye.xPos = -groundSize + cameraDistance;
        }
    else if (heli.xPos > groundSize)
    {
        heli.xPos = groundSize;
        eye.xPos = groundSize + cameraDistance;
    }

    // If outside of z bounds, move to within
    if (heli.zPos < -groundSize)
    {
        heli.zPos = -groundSize;
        eye.zPos = -groundSize + cameraDistance;
    }
    else if (heli.zPos > groundSize)
    {
        heli.zPos = groundSize;
        eye.zPos = groundSize + cameraDistance;
    }
}

bool checkBoxCollision(objectBox object1, objectBox object2)
{
    bool collision = false;
    objectBox diff = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	objectBox object1a = object1;
	objectBox object2a = object2;

	if (object1.rotY != 0)
	{
		// Rotate size in x and z to correspond with rotation of the object
		object1a.xSize = cosDeg(object2a.rotY) * object2a.xSize + sinDeg(object2a.rotY) * object2a.zSize;
		object1a.zSize = -sinDeg(object2a.rotY) * object2a.xSize + cosDeg(object2a.rotY) * object2a.zSize;
	}

	if (object2.rotY != 0)
	{
		// Rotate size in x and z to correspond with rotation of the object
		object2a.xSize = cosDeg(object2a.rotY) * object2a.xSize + sinDeg(object2a.rotY) * object2a.zSize;
		object2a.zSize = -sinDeg(object2a.rotY) * object2a.xSize + cosDeg(object2a.rotY) * object2a.zSize;
	}

    // compute the absolute (positive) distance from object1 to object2
    diff.xPos = abs(object1a.xPos - object2a.xPos);
    diff.yPos = abs(object1a.yPos - object2a.yPos);
    diff.zPos = abs(object1a.zPos - object2a.zPos);
    diff.xSize = (object1a.xSize / 2.0) + object2a.xSize;
    diff.ySize = (object1a.ySize) + object2a.ySize;
    diff.zSize = (object1a.zSize / 2.0) + object2a.zSize;

    // If the distance between each of the three dimensions is within the dimensions of the two objects combined, 
	// there is a collision
    if(diff.xPos < diff.xSize && diff.yPos < diff.ySize && diff.zPos < diff.zSize)
    {
            collision = true;
    }

    return collision;
}

bool checkPointCollision(objectBox object1, checkPoint object2)
{
	bool collision = false;

	objectBox diff = {0, 0, 0, 0};
	objectBox object1a = object1;
	checkPoint object2a = object2;

	// Rotate size in x and z to correspond with rotation of the object
	object1a.xSize = abs( cosDeg(object1a.rotY) * object1a.xSize + -sinDeg(object1a.rotY) * object1a.zSize );
	object1a.zSize = abs( sinDeg(object1a.rotY) * object1a.xSize + cosDeg(object1a.rotY) * object1a.zSize );

	object2a.xSize = abs( cosDeg(object2a.rotY) * object2a.xSize + -sinDeg(object2a.rotY) * object2a.zSize );
	object2a.zSize = abs( sinDeg(object2a.rotY) * object2a.xSize + cosDeg(object2a.rotY) * object2a.zSize );

	// Compute the absolute (positive) distance from object1 to object2
	diff.xPos = abs(object1.xPos - object2.xPos);
	diff.yPos = abs(object1.yPos - object2.yPos);
	diff.zPos = abs(object1.zPos - object2.zPos);
	diff.xSize = object2a.xSize + (object1a.xSize / 2);
	diff.ySize = object2a.ySize + (object1a.ySize / 2);
	diff.zSize = object2a.zSize + (object1a.zSize / 2);

    // If the distance between each of the three dimensions is within the radii combined, there is a collision
    if(diff.xPos < diff.xSize && diff.yPos < diff.ySize && diff.zPos < diff.zSize)
    {
            collision = true;
    }

	return collision;
}

void checkHeliThruCollisions(void)
{
	// for each checkpoint, if it is not activated, check for collision with helicopter
	for (int pointNum = 0; pointNum < maxCheckpoints; pointNum++)
	{
		if ( !points[pointNum].activated )
		{
			if ( checkPointCollision(heli, points[pointNum]) )
			{
				// If the checkpoint entered is not the checkpoint that is next, penalize 10s per gate missed
				if ( checkpointNum != pointNum )
				{
					penaltyTime += (pointNum - checkpointNum) * 5000;
					// Activate all the points leading up to incorrectly activated point
					for (checkpointNum; checkpointNum < pointNum; checkpointNum++)
					{
						points[checkpointNum].activated = true;
					}
				}
				else
				{
					points[pointNum].activated = true;
					checkpointNum = pointNum + 1;
				}
			}
		}
	}
}

void checkHeliLanding(void)
{
	// If landed on landing pad B
	if ( checkBoxCollision(heli, landingPadB) )
	{
		// If engine is off and all checkpoints have been activated
		if ( !helicopterOn && !stopHeli && checkpointNum == maxCheckpoints )
		{
				gameFinished = true;
		}
	}
}

// implementing different camera views
void cameraView1()
{
	cameraView = 1;
	cameraDistance = 6;
	eye.xPos = eye.xPos - 5;
	eye.yPos = eye.yPos - 5;
	eye.zPos = eye.zPos - 5;
}

void cameraView2()
{
	cameraView = 2;
	cameraDistance = 8;
	if (eye.yPos < skyHeight - 1)
	{
		eye.xPos = eye.xPos + 2;
		eye.yPos = eye.yPos + 2;
		eye.zPos = eye.zPos + 2;
	}
}

void cameraView3()
{
	cameraView = 3;
	cameraDistance = 11;
	if (eye.yPos < skyHeight - 1)
	{
		eye.xPos = eye.xPos + 3;
		eye.yPos = eye.yPos + 3;
		eye.zPos = eye.zPos + 3;
	}
}	

void checkHeliCollisions(void)
{
    bool collision = false;

	for ( int buildNum = 0; buildNum < maxBuildings; buildNum++ )
	{
		if ( checkBoxCollision(heli, buildings[buildNum]) )
		{
				collision = true;
		}
	}

    if (collision)
    {
		if (movingUp && movingForward)
        {
            moveHeliBack(heliSpeed, false);
        }
        else if (movingDown)
        {
            moveHeliUp(heliSpeed, false);
        }
		else
		{
			if (movingForward)
			{
				moveHeliBack(heliSpeed, false);
			}
			else if (movingBack)
			{
				moveHeliForward(heliSpeed, false);
			}
		}

		if (stopHeli)
		{
			heli.yPos += 0.02;
			eye.yPos += 0.02;
			stopHeli = false;
			helicopterOn = false;
			rotorSpeed = 0;
		}
    }

	if ( heli.yPos < groundHeight)
	{
		heli.yPos = groundHeight + heli.ySize;
		eye.yPos = groundHeight + heli.ySize;
	}
	else if ( heli.yPos > skyHeight )
	{
		heli.yPos = skyHeight - heli.ySize;
		eye.yPos = groundHeight + heli.ySize;
	}
}

// Converts degrees (in) to radians and returns the cosine (out)
float cosDeg(float degRot)
{
    float radRot = degRot * PI/180;
    return (cos(radRot));
}

// Converts degrees(in) to radians and returns the sine (out)
float sinDeg(float degRot)
{
    float radRot = degRot * PI/180;
	return (sin(radRot));
}

void restartGame()
{
	gameFinished = false;
	timeUpdated = false;

	// Remove old map data
	maxCheckpoints = 0;
	maxBuildings = 0;
	delete points;
	points = NULL;
	delete buildings;
	buildings = NULL;

	// Get and read a new map file
	selectMap();
	readMapFile(strCurrentMap);

	// Reset checkpoints
	checkpointNum = 0;
	for (int num = 0; num < maxCheckpoints; num++)
	{
		points[num].activated = false;
	}

	// Reset heli position
	heli.xPos = 0;
	heli.yPos = groundHeight + heli.ySize;
	heli.zPos = 0;
	heli.rotY = 0;

	// Reset cameraDistance
	cameraDistance = 6;

	// Reset cameraView
	cameraView = 1;

	// Reset eye position
	eye.xPos = cameraDistance;
	eye.yPos = 2;
	eye.zPos = cameraDistance;
	eye.rotY = 135;
	
	// Reset time
	gameTime = 0;
	penaltyTime = 0;
	pause = true;
}

// Catches keyboard key presses
void keyboard(unsigned char key, int mouseX, int mouseY)
{
	if(pause)
	{
		pause = false;
	}
	
	// If game is finished and enter key is pressed
	if (gameFinished && key == 13)
	{
		restartGame();
	}

    switch (key)
    {
        // If ESC key is pressed, exit
        case 27:
            glutLeaveMainLoop();
            break;
		case '1':
			if( windscreenRot > 0 )
				windscreenRot = 0.0;
			else
				windscreenRot = 35.0;
			glutPostRedisplay();
			break;
		case '2':
			if( doorRot > 0 )
				doorRot = 0.0;
			else
				doorRot = 95.0;
			glutPostRedisplay();
			break;
		case '3':
			if (eye.xPos < heli.xPos + 5)
				break;
			else
				//Zoom In
			{
				eye.xPos -= 0.1;
				eye.yPos -= 0.1;
				eye.zPos -= 0.1;
				cameraDistance -= 0.1;
			}
			break;
		case '4':
			if (eye.yPos > skyHeight - 10)
				break;
			else
				//Zoom Out
			{
				eye.xPos += 0.1;
				eye.yPos += 0.1;
				eye.zPos += 0.1;
				cameraDistance += 0.1;
			}
			break;
        case 'a':
            // Start moving the heli up
            movingUp = true;
            break;
        case 'z':
            // Start moving the heli down
            movingDown = true;
            break;
		case 's':
			// Start Blades
			startHeli = true;
			break;
		case 'x':
			//Stop Blades
			heliLeanFront = 0;
			stopHeli = true;
			break;
		case 32:
			if (currentSpeed >= MAX_SPEED)
				break;
			else
				currentSpeed += 2;
				heliSpeed = currentSpeed / fps;
				//acceleration
				break;
		case 'b':
			if (currentSpeed <= MIN_SPEED)
				break;
			else
				currentSpeed -=2;
				heliSpeed = currentSpeed / fps;
				break;
		case 'v':
			if (cameraView == 1)
				cameraView2();
			else if (cameraView == 2)
				cameraView3();
			else if (cameraView == 3)
				cameraView1();

			break;
    }
}

void keyboardUp(unsigned char key, int mouseX, int mouseY)
{
    switch (key)
    {
        case 'a':
                // Stop moving the heli up
                movingUp = false;
                break;
        case 'z':
                // Stop moving the heli down
                movingDown = false;
                break;
    }
}

void shadingOn()
{
	if (wire == true)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	wire = false;
}

void wireFrameOn()
{
	if (wire == false)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	wire = true;

}

void heliTexturesOn()
{
		heliTextures = true;
}

void heliTexturesOff()
{
		heliTextures = false;
}

// Catches special key presses
void special(int key, int mouseX, int mouseY)
{
	// If paused, unpause
	if (pause && key != GLUT_KEY_F1)
	{
		pause = false;
	}

    switch (key)
    {
        case GLUT_KEY_LEFT:
                // rotate heli left
                turningLeft = true;
                break;
        case GLUT_KEY_RIGHT:
                // rotate heli right
                turningRight = true;
                break;
        case GLUT_KEY_UP:
                // move "forward"
                movingForward = true;
                break;
        case GLUT_KEY_DOWN:
                // move "backward"
                movingBack = true;
                break;
		case GLUT_KEY_F1:
				pause = !pause;
				break;
		case GLUT_KEY_F2:
				if (wire == false)
				{
					wireFrameOn();
				}
				else
				{
					shadingOn();
				}
				break;
		case GLUT_KEY_F3:
				if (heliTextures == false)
				{
					heliTexturesOn();
				}
				else
				{
					heliTexturesOff();
				}
				break;
		case GLUT_KEY_F4:
				// turn fog on or off
				fogOn = !fogOn;
				if (fogOn)
				{          
					glEnable(GL_FOG);
                }
                else
                {
                    glDisable(GL_FOG);
                }
				break;
		case GLUT_KEY_F5:
				shaderOn = !shaderOn;
				break;
		case GLUT_KEY_F8:
                // Turn light0 on or off
                light0 = !light0;
				if (light0)
				{                        
					glEnable(GL_LIGHT0);
                }
                else
                {
                    glDisable(GL_LIGHT0);
                }
                break;
		case GLUT_KEY_F9:
			// Turn light1 on or off
			light1 = !light1;
			if (light1)
			{
				glEnable(GL_LIGHT1);
			}
			else
			{
				glDisable(GL_LIGHT1);
			}
			break;
    }
}


void specialUp(int key, int mouseX, int mouseY)
{
    switch(key)
    {
        case GLUT_KEY_UP:
                // move "forward"
                movingForward = false;
                break;
        case GLUT_KEY_DOWN:
                // move "backward"
                movingBack = false;
                break;
        case GLUT_KEY_LEFT:
                // rotate heli left
                turningLeft = false;
                break;
        case GLUT_KEY_RIGHT:
                // rotate heli right
                turningRight = false;
                break;
    }
}

void mouseMotion(int x, int y)
{
    if (leftMouseDown)
    {
		float rotateY = ( (float)x - (float)last_mouse_x) / (float)windowWidth * 360.0;
		float rotateZ = ( (float)y - (float)last_mouse_y) / (float)windowHeight * 360.0;
		eye.rotY += rotateY;
		eye.rotZ += rotateZ;
        last_mouse_x = x;
		last_mouse_y = y;
    }
}

void mouse(int button, int state, int x, int y)
{
    last_mouse_x = x;
	last_mouse_y = y;

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        leftMouseDown = true;
    }
    else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
    {
        eye.rotY = 135;
		eye.rotX = 0;
		eye.rotZ = 0;
    }
    else
    {
        leftMouseDown = false;
    }
}

void moveHeliForward(float speed, bool checkCol)
{
    // Move heli
    heli.xPos += speed * cosDeg(heli.rotY);
    heli.zPos -= speed * sinDeg(heli.rotY);

    // Move camera
    eye.xPos += speed * cosDeg(heli.rotY);
    eye.zPos -= speed * sinDeg(heli.rotY);

    checkHeliCollisions();
	checkHeliThruCollisions();
}

void moveHeliBack(float speed, bool checkCol)
{
    // Move heli
    heli.xPos -= speed * cosDeg(heli.rotY);
    heli.zPos += speed * sinDeg(heli.rotY);

    // Move camera
    eye.xPos -= speed * cosDeg(heli.rotY);
    eye.zPos += speed * sinDeg(heli.rotY);

    checkHeliCollisions();
	checkHeliThruCollisions();
}

void moveHeliUp(float speed, bool checkCol)
{
    heli.yPos += heliSpeed;
    eye.yPos += heliSpeed;

    checkHeliCollisions();
	checkHeliThruCollisions();
}

void moveHeliDown(float speed, bool checkCol)
{
    heli.yPos -= heliSpeed;
    eye.yPos -= heliSpeed;

    checkHeliCollisions();
	checkHeliThruCollisions();
}


void updateGameTime()
{
	if (!pause)
	{
		gameTime += timePassed - gameTimeBase;	// Increment the time spent playing
		int gameTimeMillisec = (gameTime % 1000) / 10;		// Get milliseconds from game time
		int gameTimeSeconds = (gameTime % 60000) / 1000;	// Get seconds from game time
		int gameTimeMinutes = (gameTime % 3600000) / 60000;	// Get minutes from game time
		sprintf(strGameTime, "Time: %.2i:%.2i:%.2i", gameTimeMinutes, gameTimeSeconds, gameTimeMillisec);	
	}

	gameTimeBase = timePassed;
}

void updateFPS()
{
    // Update the FPS every second
    frames++;
    timePassed = glutGet(GLUT_ELAPSED_TIME);

    if (timePassed - timeBase > 1000) // If a second has passed
    {
        fps = frames * 1000.0 / (timePassed - timeBase);              // calculate FPS
        sprintf(strFps, "FPS: %4.2f", fps);		// get the string value of integer FPS
        timeBase = timePassed;        // Set the base time to current time
        frames = 0;     // Reset the frame count
    }
}

void displayText()
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
    // Write text to screen
    glPushMatrix();
    glColor3f(0.2, 0.5, 0.2);
    setOrthographicProjection();
	if (light1)
		glDisable(GL_LIGHT1);
	if (!light0)
		glEnable(GL_LIGHT0);
    glLoadIdentity();
	// Display FPS
    renderBitmapString(textX, textY, (void *)font, strFps);
	// Display map name
	renderBitmapString(windowWidth - windowWidth / 4, textY, (void *)font, (char*)strCurrentMap);

	displayDashboard();

	// If paused, display help/controls/scores
	if (pause)
	{
		displayHelp();
	}

	// Return lighitng to former state
	if (light1)
		glEnable(GL_LIGHT1);
	if (!light0)
		glDisable(GL_LIGHT0);
    resetPerspectiveProjection();
    glPopMatrix();

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
}

void displayHelp()
{
	const int HELP_SPACE = 18;
	const int HELP_YPOS = 30;
	int row = 0;

	glDisable(GL_DEPTH_TEST);

	// Display a backing screen so that the text is readable
	glColor4f(0.5, 0.5, 0.5, 0.8);	// Grey
	glBegin(GL_QUADS);
	glVertex2f(MARGIN, MARGIN);
	glVertex2f(windowWidth - MARGIN, MARGIN);
	glVertex2f(windowWidth - MARGIN, windowHeight - MARGIN);
	glVertex2f(MARGIN, windowHeight - MARGIN);
	glEnd();

	glColor4f(1.0, 0.7, 0.2, 1.0);	// Orange
	
	renderBitmapString(windowWidth / 2.0 - 3 * HELP_SPACE, HELP_YPOS + row++ * HELP_SPACE, (void *)fontTitle, "Heli Rally");
	row++;
	renderBitmapString(HELP_SPACE - 8, HELP_YPOS + row++ * HELP_SPACE, (void *)font, "How To Play - Take off and pass through each of the checkpoints in");
	renderBitmapString(HELP_SPACE - 8, HELP_YPOS + row++ * HELP_SPACE, (void *)font, "sequence and land at point B. The helicopter must pass");
	renderBitmapString(HELP_SPACE - 8, HELP_YPOS + row++ * HELP_SPACE, (void *)font, " more than halfway into the checkpoint for it to be activated.");
	renderBitmapString(HELP_SPACE - 8, HELP_YPOS + row++ * HELP_SPACE, (void *)font, "If you miss a checkpoint, you will be penalised 5 seconds per miss.");
	row++;
	renderBitmapString(HELP_SPACE, HELP_YPOS + row++ * HELP_SPACE, (void *)font, "F1 - Pause and bring up this screen");
	renderBitmapString(HELP_SPACE, HELP_YPOS + row++ * HELP_SPACE, (void *)font, "F2 - Switch between wireframe and solid shapes");
	renderBitmapString(HELP_SPACE, HELP_YPOS + row++ * HELP_SPACE, (void *)font, "F3 - Switch textures on helicopter on/off");
	renderBitmapString(HELP_SPACE, HELP_YPOS + row++ * HELP_SPACE, (void *)font, "F4 - Switch fog on and off");
	renderBitmapString(HELP_SPACE, HELP_YPOS + row++ * HELP_SPACE, (void *)font, "F5 - Switch GLSL shaders on helicopter on/off");
	renderBitmapString(HELP_SPACE, HELP_YPOS + row++ * HELP_SPACE, (void *)font, "F8 - Switch Light 0 on/off");
	renderBitmapString(HELP_SPACE, HELP_YPOS + row++ * HELP_SPACE, (void *)font, "F9 - Switch Light 1 on/off");
	renderBitmapString(HELP_SPACE, HELP_YPOS + row++ * HELP_SPACE, (void *)font, "1 - Open windscreen");
	renderBitmapString(HELP_SPACE, HELP_YPOS + row++ * HELP_SPACE, (void *)font, "2 - Open door");
	renderBitmapString(HELP_SPACE, HELP_YPOS + row++ * HELP_SPACE, (void *)font, "3/4 - Zoom in/out");
	renderBitmapString(HELP_SPACE, HELP_YPOS + row++ * HELP_SPACE, (void *)font, "spacebar/b - Accelerate/brake");
	renderBitmapString(HELP_SPACE, HELP_YPOS + row++ * HELP_SPACE, (void *)font, "v - Switch camera position");
	renderBitmapString(HELP_SPACE, HELP_YPOS + row++ * HELP_SPACE, (void *)font, "a/z - Move helicopter up/down");
	renderBitmapString(HELP_SPACE, HELP_YPOS + row++ * HELP_SPACE, (void *)font, "Directional Arrows - Move forward/backward and Turn left/right");
	renderBitmapString(HELP_SPACE, HELP_YPOS + row++ * HELP_SPACE, (void *)font, "s/x - Start/stop engine");
	renderBitmapString(HELP_SPACE, HELP_YPOS + row++ * HELP_SPACE, (void *)font, "Right Mouse - Brings up game menu");
	renderBitmapString(HELP_SPACE, HELP_YPOS + row++ * HELP_SPACE, (void *)font, "Drag Left Mouse - Rotate camera around helicopter");
	renderBitmapString(HELP_SPACE, HELP_YPOS + row++ * HELP_SPACE, (void *)font, "Middle Mouse - Reset camera to chase position");
	// Leave a line gap
	row++;

	// Display the best time
	strBestTime = getTimeString(bestTime);
	char* strDrawBestTime = new char[30];
	sprintf(strDrawBestTime, "Best Time: %s", strBestTime);
	renderBitmapString(HELP_SPACE, HELP_YPOS + row++ * HELP_SPACE, (void *)font, strDrawBestTime);
	// Leave a gap
	row++;

	// Press any button to continue
	renderBitmapString(windowWidth / 2.0 - 5 * HELP_SPACE, HELP_YPOS + row++ * HELP_SPACE, (void *)font, "Press any key to continue");
	//Leave a gap
	row++;

	// Display who wrote the project and it's purpose
	renderBitmapString(HELP_SPACE, HELP_YPOS + row++ * HELP_SPACE, (void *)font, "Written by Aleesha Torkington, Ashley Sexton and Christopher Trott");
	renderBitmapString(HELP_SPACE, HELP_YPOS + row++ * HELP_SPACE, (void *)font, " for CP2060 Assignment 2 - Semester 2 2009");

	glEnable(GL_DEPTH_TEST);
}

void displayDashboard()
{
	const float DASH_HEIGHT_PC = 0.2;
	const float DASH_WIDTH_PC = 0.0;

	float dashHeight = windowHeight - (DASH_HEIGHT_PC * windowHeight);
	float dashWidth = windowWidth - (DASH_WIDTH_PC * windowWidth);

	// Draw the dash board
	glColor4f(0.5, 0.5, 0.5, 1.0);	// Grey
	glBegin(GL_QUADS);
	glVertex2f(0.0, dashHeight);
	glVertex2f(dashWidth, dashHeight);
	glVertex2f(dashWidth, windowHeight);
	glVertex2f(0.0, windowHeight);
	glEnd();

	glColor4f(0.0, 1.0, 0.5, 1.0);	// Green
	// Display game time
	renderBitmapString(30, dashHeight + 30, (void *)font, strGameTime);

	// Display speed
	char* strHeliSpeed = new char[12];
	if ( movingForward || movingBack )
	{
		sprintf(strHeliSpeed, "Speed: %.2f", heliSpeed);
	}
	else
	{
		sprintf(strHeliSpeed, "Speed: %.2f", 0);
	}
	renderBitmapString(dashWidth / 2.3, dashHeight + 30, (void *)font, strHeliSpeed);

	// Display altitude
	char* strAltitude = new char[14];
	sprintf(strAltitude, "Altitude: %.2f", heli.yPos - heli.ySize);
	renderBitmapString(3 * (dashWidth / 4), dashHeight + 30, (void *)font, strAltitude);

	// Display checkpoint number
	char* strCheckpoint = new char[14];
	sprintf(strCheckpoint, "Checkpoint: %.2i/%.2i", checkpointNum, maxCheckpoints);
	renderBitmapString(3 * (dashWidth / 5), dashHeight + 60, (void *)font, strCheckpoint);

	// Display penalty time
	char* strPenalty = new char[22];
	char* strDrawPenalty = new char[36];
	strPenalty = getTimeString(penaltyTime);
	sprintf(strDrawPenalty, "Penalty Time: %s", strPenalty);
	renderBitmapString(30, dashHeight + 60, (void *)font, strDrawPenalty);

}

char* getTimeString(int time)
{
	char* strTime = new char[10];
	int timeMSec = (time % 1000) / 10;
	int timeSec = (time % 60000) / 1000;
	int timeMin = (time % 3600000) / 60000;
	sprintf( strTime, "%.2i:%.2i:%.2i", timeMin, timeSec, timeMSec );
	return strTime;
}

bool readMapFile(const char* fileName)
{
	bool success = true;
	string strInput = "";
	
	cout << "Loading map " << fileName << " for use.\n";
	// Open file for reading
	ifstream fin;
	fin.open( fileName );

	if (fin.fail())
	{
		cout << "Error reading file " << fileName << ".\n";
		success = false;
	}

	if ( fin.bad() )
	{
		cout << "Fatal error reading file " << fileName << ".\n";
		success = false;
	}
	
	// Read values while not end of file and set their values in the program
	while ( !fin.eof() )
	{
		float fInput = 0.0;
		getline(fin, strInput, '\n');

		if ( strInput.compare("bestTime{") == 0 )
		{
			fin >> fInput;
			bestTime = fInput;
			fin.ignore(100, '\n');
		}
		else if ( strInput.compare("groundSize{") == 0 )
		{
			fin >> fInput;
			groundSize = fInput;
			fin.ignore(100, '\n');
		}
		else if ( strInput.compare("pointA{") == 0 )
		{
			fin >> fInput;
			landingPadA.xPos = fInput;
			fin >> fInput;
			landingPadA.yPos = fInput;
			fin >> fInput;
			landingPadA.zPos = fInput;
			fin >> fInput;
			landingPadA.rotX = fInput;
			fin >> fInput;
			landingPadA.rotY = fInput;
			fin >> fInput;
			landingPadA.rotZ = fInput;
			fin >> fInput;
			landingPadA.xSize = fInput;
			fin >> fInput;
			landingPadA.ySize = fInput;
			fin >> fInput;
			landingPadA.zSize = fInput;
			fin.ignore(100, '\n');
		}
		else if ( strInput.compare("pointB{") == 0 )
		{
			fin >> fInput;
			landingPadB.xPos = fInput;
			fin >> fInput;
			landingPadB.yPos = fInput;
			fin >> fInput;
			landingPadB.zPos = fInput;
			fin >> fInput;
			landingPadB.rotX = fInput;
			fin >> fInput;
			landingPadB.rotY = fInput;
			fin >> fInput;
			landingPadB.rotZ = fInput;
			fin >> fInput;
			landingPadB.xSize = fInput;
			fin >> fInput;
			landingPadB.ySize = fInput;
			fin >> fInput;
			landingPadB.zSize = fInput;
			fin.ignore(100, '\n');
		}
		else if ( strInput.compare("checkpoint{") == 0 )
		{
			fin >> fInput;
			int pointNum = fInput;
			if (points == NULL)
			{
				points = new checkPoint[pointNum + 1];
				maxCheckpoints = pointNum + 1;
			}
			points[pointNum].checkpoint = pointNum;
			points[pointNum].activated = false;
			fin >> fInput;
			points[pointNum].xSize = fInput;
			fin >> fInput;
			points[pointNum].ySize = fInput;
			fin >> fInput;
			points[pointNum].zSize = fInput;
			fin >> fInput;
			points[pointNum].xPos = fInput;
			fin >> fInput;
			points[pointNum].yPos = fInput;
			fin >> fInput;
			points[pointNum].zPos = fInput;
			fin >> fInput;
			points[pointNum].rotY = fInput;
			fin.ignore(100, '\n');
		}
		else if ( strInput.compare("building{") == 0 )
		{
			fin >> fInput;
			int buildNum = fInput;
			if (buildings == NULL)
			{
				buildings = new objectBox[buildNum + 1];
				maxBuildings = buildNum + 1;
			}
			fin >> fInput;
			buildings[buildNum].xPos = fInput;
			fin >> fInput;
			buildings[buildNum].yPos = fInput;
			fin >> fInput;
			buildings[buildNum].zPos = fInput;
			fin >> fInput;
			buildings[buildNum].rotX = fInput;
			fin >> fInput;
			buildings[buildNum].rotY = fInput;
			fin >> fInput;
			buildings[buildNum].rotZ = fInput;
			fin >> fInput;
			buildings[buildNum].xSize = fInput;
			fin >> fInput;
			buildings[buildNum].ySize = fInput;
			fin >> fInput;
			buildings[buildNum].zSize = fInput;
			fin.ignore(100, '\n');
		}
	}

	fin.close();

	return success;
}

// This function replaces the old time with the new time in the map file
void writeTime(const char* fileName, int time)
{
	string line;
	ifstream fin(fileName);

	if( !fin.is_open())
	{
		cout << "Input file failed to open\n";
		return;
	}

	// Now open temp output file
	ofstream fout("temp.txt");

	// Loop to read then write the file.
	while( getline(fin,line) )
	{
		if(line == "bestTime{")
		{
			fout << line << "\n";
			// Output new line
			fout << time << "\n";
			// Get old line so it skips
			getline(fin,line);
		}
		else
		{
			fout << line << "\n";
		}
	}

	fin.close();
	fout.close();

	// Delete the original file
	remove(fileName);
	// Rename old to new
	rename("temp.txt",fileName);
}

void drawFinishScreen()
{
	
	const int X_SPACE = 50;
	const int Y_SPACE = 50;
	int row = 0;

	glDisable(GL_DEPTH_TEST);

	// Display a backing screen so that the text is readable
	glColor4f(0.4, 0.4, 0.4, 1.0);	// Grey
	glBegin(GL_QUADS);
	glVertex2f(MARGIN, MARGIN);
	glVertex2f(windowWidth - MARGIN, MARGIN);
	glVertex2f(windowWidth - MARGIN, windowHeight - MARGIN);
	glVertex2f(MARGIN, windowHeight - MARGIN);
	glEnd();

	glColor4f(0.5, 1.0, 0.5, 1.0);	// Light Green
	char* strFinishTime = new char[30];
	int totalTime = gameTime + penaltyTime;
	char* strTotalTime = getTimeString(totalTime);
	char* strBestTime = getTimeString(bestTime);

	sprintf( strFinishTime, "Your time is: %s", strTotalTime);
	renderBitmapString(X_SPACE, Y_SPACE + (++row * 4*MARGIN), (void *)font, "Race Completed");
	renderBitmapString(X_SPACE, Y_SPACE + (++row * 4*MARGIN), (void *)font, strFinishTime);

	// Do calculations vs best time and display appropriate message
	if ( totalTime < bestTime )
	{
		row++;
		char* strOldBest = new char[30];
		renderBitmapString(X_SPACE, Y_SPACE + (++row * 4*MARGIN), (void *)font, "Congratulations you have a new best time!");
		sprintf(strOldBest, "The old best time was: %s", strBestTime);
		renderBitmapString(X_SPACE, Y_SPACE + (++row * 4*MARGIN), (void *)font, strOldBest);
		
		// Output new time to saved file
		if (!timeUpdated)
		{
			writeTime(strCurrentMap, totalTime);
			timeUpdated = true;
		}
	}
	else
	{
		renderBitmapString(X_SPACE, Y_SPACE + (++row * 4*MARGIN), (void *)font, "You have not beaten the best time.");
		renderBitmapString(X_SPACE, Y_SPACE + (++row * 4*MARGIN), (void *)font, "Would you like to try again?");
	}

	row++;

	// If you would like to play again, press Enter
	renderBitmapString(X_SPACE, Y_SPACE + (++row * 4*MARGIN), (void *)font, "If you would like to play again, press Enter.");

	// Otherwise, press ESC to quit
	renderBitmapString(X_SPACE, Y_SPACE + (++row * 4*MARGIN), (void *)font, "Otherwise press ESC to quit.");

	glEnable(GL_DEPTH_TEST);
}

// These next three functions are taken from Lighthouse 3D tutorials:
// http://www.lighthouse3d.com/opengl/glut/index.php?bmpfontortho
void setOrthographicProjection()
{
    // switch to projection mode
    glMatrixMode(GL_PROJECTION);
	if (light1)
		glDisable(GL_LIGHT1);
	if (!light0)
		glEnable(GL_LIGHT0);
    // save previous matrix which contains the
    //settings for the perspective projection
    glPushMatrix();
    // reset matrix
    glLoadIdentity();
    // set a 2D orthographic projection
    gluOrtho2D(0, windowWidth, 0, windowHeight);
    // invert the y axis, down is positive
    glScalef(1, -1, 1);
    // move the origin from the bottom left corner
    // to the upper left corner
    glTranslatef(0, -windowHeight, 0);
	if (light1)
		glEnable(GL_LIGHT1);
	if(!light0)
		glDisable(GL_LIGHT0);
    glMatrixMode(GL_MODELVIEW);
}

// Set projection back to it's starting point
void resetPerspectiveProjection()
{
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// Draw the string on the screen
void renderBitmapString(float x, float y, void *font,char *string)
{
    char *c;
    // Set the draw co-ordinates
    glRasterPos2f(x, y);

    for (c=string; *c != '\0'; c++)
    {
        // Display each character in the array
        glutBitmapCharacter(font, *c);
    }
}
// End lighthouse 3D code

// When there's nothing else to do, update animation
void idle(void)

{
	if (!helicopterOn)
	{
		checkHeliLanding();
	}

	if (stopHeli)
	{
		if (rotorSpeed > 0)
		{
			rotorSpeed = rotorSpeed - 0.2; 
		}

		if (heli.yPos > groundHeight + heli.ySize)
		{

			heli.yPos -= 0.02;
			eye.yPos -= 0.02;
			checkHeliCollisions();

			if (heli.yPos < groundHeight + heli.ySize)
			{
				heli.yPos = groundHeight + heli.ySize;
			}
		}
		else if (rotorSpeed <= 0)
		{
			stopHeli = false;
		}

		if (rotorSpeed < MAX_ROTOR_SPEED) 
		{
			helicopterOn = false;
		}
	}
	else if (startHeli)
	{
		if (rotorSpeed < MAX_ROTOR_SPEED)
		{
			rotorSpeed = rotorSpeed + 0.2; 
			heli.yPos += 0.02; 
			eye.yPos += 0.02; 
		} 
		else
		{
			helicopterOn = true;
			startHeli = false;
		}
	}

	if(!pause && !gameFinished)
	{
		if (helicopterOn == true)
		{
			// Update speed
			heliSpeed = currentSpeed / fps;
			if (movingForward)
			{
				// Move forward
				moveHeliForward(heliSpeed, true);
				heliLeanFront = -LEAN_FACTOR;
			
			}
			else if (movingBack)
			{
				// Move back
				moveHeliBack(heliSpeed, true);
				heliLeanFront = LEAN_FACTOR;
			}
			else
			{
				// Reset the front lean and straighten up
				heliLeanFront = 0;
			}

			if (turningLeft)
			{
				// Turn left
				heli.rotY += rotSpeed;
				// Adjust the rotor spin to counter heli spin
				rotor += rotorSpeed - rotSpeed;
				heliLeanSide = -LEAN_FACTOR;
			}
			else if (turningRight)
			{
				// Turn right
				heli.rotY -= rotSpeed;
				// Adjust the rotor spin to counter heli spin
				rotor += rotorSpeed + rotSpeed;
				heliLeanSide = LEAN_FACTOR;
			}
			else
			{
				// Turn the rotor normally
				rotor += rotorSpeed;
				// Reset the lean and straighten up
				heliLeanSide = 0;
			}
		}
		else
		{
			// Turn the rotor normally
			rotor += rotorSpeed;
			heliLeanSide = 0;
		}

		if (helicopterOn == true)
		{
			if (movingUp && heli.yPos - heli.ySize < skyHeight - 5 )
			{
				moveHeliUp(heliSpeed, true);
			}
			else if (movingDown)
			{
				if(heli.yPos > groundHeight + heli.ySize)
				{
					moveHeliDown(heliSpeed, true);
				}

				if(heli.yPos < groundHeight + heli.ySize)
				{
					heli.yPos = groundHeight + heli.ySize;
				}
			}
		}

		// Make sure heli is in the level's bounds
        checkBounds();
	}

	glutPostRedisplay();
}

// When the window is reshaped, this function updates the camera and display
void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

	if( w > h)
	{
		gluPerspective(45, w/h, cameraZoom, 60.0);
	}
	else
	{
		gluPerspective(45, h/w, cameraZoom, 60.0);
	}
    glMatrixMode(GL_MODELVIEW);

	windowWidth = w;
	windowHeight = h;
}

double up = 1.0;

// Display the scene and it's components
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

	/*if (shaderOn)
	{
		glUseProgramObjectARB(shaderProgram);
	}
	else
	{
		glUseProgramObjectARB(0);
	}*/

	// Update the heliLight position
	heliLight();

	if ( !gameFinished )
	{
		gluLookAt(eye.xPos, eye.yPos, eye.zPos, heli.xPos, heli.yPos, heli.zPos, 0.0, up, 0.0);

		// Rotate camera so that it is always behind the heli
		glPushMatrix();
		glTranslatef(heli.xPos, heli.yPos, heli.zPos);
		glRotatef(-heli.rotY + eye.rotY, 0.0, 1.0, 0.0);
		glRotatef(eye.rotZ, 0.0, 0.0, 1.0);
		glTranslatef(-heli.xPos, -heli.yPos, -heli.zPos);

		//glUseProgramObjectARB(0);

		// Draw Sky
		glPushMatrix;
		drawSky();
		glPopMatrix;

		// Draw ground
		glPushMatrix();
		glTranslatef(0, groundHeight, 0);
		drawGround();
		glPopMatrix();

		// Draw building
		for(int i = 0; i < maxBuildings; i++)
		{
			glPushMatrix();
			drawBuilding(buildings[i], 1);
			glPopMatrix();
		}

		// Draw the helicopter
		drawHeli();

		// Draw landing pads A and B
		drawLandingPad(landingPadA, 3);
		drawLandingPad(landingPadB, 4);

		// Draw the checkpoints
		for (int checkPoint = maxCheckpoints - 1; checkPoint > -1; checkPoint--)
		{
			glPushMatrix();
			drawCheckpoint( points[checkPoint].checkpoint );
			glPopMatrix();
		}
		
		updateFPS();
		updateGameTime();
		displayText();
		displayDashboard();

		glPopMatrix();
	}
	else
	{
		setOrthographicProjection();
		if (light1)
			glDisable(GL_LIGHT1);	// Disable light1
		if (!light0)
			glEnable(GL_LIGHT0);	// Enable light 2
		drawFinishScreen();
		// Return lighting to former state
		if (light1)
			glEnable(GL_LIGHT1);
		if (!light0)
			glDisable(GL_LIGHT0);
		resetPerspectiveProjection();
	}
    glutSwapBuffers();
}

// Creating menu
void mymenu(int choice)
{

	switch (choice) {
	case 1: shadingOn();	   // Full Shading
		break;
	case 2: wireFrameOn();     // Wireframe
		break;
	case 3: heliTexturesOn();  // enable texture mapping
		break;
	case 4: heliTexturesOff(); // disable texture mapping
		break;
	case 5: pause = !pause;    // Help/Credits/Scores
		break;
	case 6: exit(0);           // exit program
		break;
	default: break;
	}
}

/*void cleanUpShaders()
{
	glDetachObjectARB(shaderProgram, vertexShader);
	glDetachObjectARB(shaderProgram, fragmentShader);

	glDeleteObjectARB(vertexShader);
	glDeleteObjectARB(fragmentShader);

	glDeleteObjectARB(shaderProgram);
}*/

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(windowPosWidth, windowPosHeight);
    glutCreateWindow("CP2060 Assignment 2 - Heliaac");
	glewInit();

	/*if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
	{
		printf("Ready for GLSL\n");
	}
	else 
	{
		printf("Not totally ready :( \n");
		exit(1);
	}*/

    init();
    glutDisplayFunc(display);
    glutSpecialFunc(special);
    glutSpecialUpFunc(specialUp);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);

	//setupShaders();

	// create sub menu 1
	int subMenu1 = glutCreateMenu(mymenu);
	glutAddMenuEntry("Full Shading", 1);
	glutAddMenuEntry("Wire Frame", 2);

	// create sub menu 2 
	int subMenu2 = glutCreateMenu(mymenu);
	glutAddMenuEntry("Enable", 3);
	glutAddMenuEntry("Disable", 4);

	//create sub menu 3
	int subMenu3 = glutCreateMenu(mymenu);
	glutAddMenuEntry("Yes", 6);
	glutAddMenuEntry("No", 0);

	// create main menu (no ID needed)
	glutCreateMenu(mymenu);
	glutAddSubMenu("Shading", subMenu1);
	glutAddSubMenu("Helicopter Textures", subMenu2);
	glutAddMenuEntry("Help/Credits/Scores", 5);
	glutAddSubMenu("Exit Program", subMenu3);
	// set the menu to the right button
	glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutMainLoop();

	//cleanUpShaders();
    return 0;
}
