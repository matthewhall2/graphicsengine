#include "Zeppelin.h"
#include <stdio.h>
#include <iostream>
#include <chrono>
#include "MatrixManager.h"
#include "Camera.h"
#include "ShaderManager.h"
#include <GL/freeglut.h>
#include <random>

using mm = MatrixManager;


#define M_PI 3.14159265358979323846

const int vWidth = 1000;    // Viewport width in pixels
const int vHeight = (1000 * 10) / 16;    // Viewport height in pixels

std::mt19937 rng(std::random_device{}());
bool firstPerson = false;


// Define the distribution for the desired range


// Mouse button
int currentButton;

void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void mouseMotionHandler(int xMouse, int yMouse);
void keyboard(unsigned char key, int x, int y);
void functionKeys(int key, int x, int y);
void fkeysUp(int key, int x, int y);
void animationHandler(int param);
void keyup(unsigned char key, int x, int y);
void setupPlayerZeppelin();
void setupEnemyZeppelin();
void mouseMotionHandler3D(int, int);
void mouseButtonHandler3D(int button, int state, int x, int y);
void handleWheel(int, int, int, int);
void checkBullets();

void enemyControl(long t);


Zeppelin* playerZeppelin;
Zeppelin* enemyZeppelin;
World* world;

GLdouble viewRadius = 7;
GLdouble viewTheta = 0.5 * M_PI;
GLdouble viewPhi = 2 * M_PI;

//DrawTest* d;
float cameraAngle = 0;
std::chrono::high_resolution_clock::time_point lastCallTime = std::chrono::high_resolution_clock::now();

bool isMoving = false;
unsigned short moveDirection = 0;
unsigned short enemyDirection = NONE;


int main(int argc, char** argv) {
	// Initialize GLUT
	MatrixManager m();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(vWidth, vHeight);
	glutInitWindowPosition(200, 30);
	glutCreateWindow("Matthew Hall CPS511 A1");

	// Initialize GL
	initOpenGL(vWidth, vHeight);

	// Register callback functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotionHandler);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(functionKeys);
	glutKeyboardUpFunc(keyup);
	glutSpecialUpFunc(fkeysUp);
	glutMotionFunc(mouseMotionHandler3D);
	glutMouseFunc(mouseButtonHandler3D);
	glutMouseWheelFunc(handleWheel);

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stdout, "Glew error: %s\n", glewGetErrorString(err));
	}

	setupPlayerZeppelin();
	setupEnemyZeppelin();
	world = new World();

	//d = new DrawTest();

	// Start event loop, never returns
	glutMainLoop();

	return 0;
}

// Set up OpenGL. For viewport and projection setup see reshape(). 
void initOpenGL(int w, int h)
{
    // Set up and enable lighting
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

    glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);   // This second light is currently off

    // Other OpenGL setup
    glEnable(GL_DEPTH_TEST);   // Remove hidded surfaces
    glShadeModel(GL_SMOOTH);   // Use smooth shading, makes boundaries between polygons harder to see 
    glClearColor(0.4F, 0.4F, 0.4F, 0.0F);  // Color and depth for glClear
    glClearDepth(1.0f);
    glEnable(GL_NORMALIZE);    // Renormalize normal vectors 
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   // Nicer perspective
	Camera::setProj(125, (GLdouble)w / h, 0.1, 40.0);
	Camera::lookAt(0, 15, 8, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	MatrixManager::loadIdentity();
	std::cout << glGetString(GL_VERSION);
	
}

// Callback, called whenever GLUT determines that the window should be redisplayed
// or glutPostRedisplay() has been called.
void display(void)
{
	static bool done = false;
	if (isMoving) {
		playerZeppelin->start(moveDirection);
	}
	else {
		playerZeppelin->stop();
	}

	//enemyZeppelin->start(0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	MatrixManager::loadIdentity();
	if (firstPerson) {
		firstPerson = true;
		glm::vec3 p(playerZeppelin->x, playerZeppelin->y - 2, playerZeppelin->z);
		float fdvx = -glm::cos(glm::radians(playerZeppelin->yawAngle));
		float fdvz = glm::sin(glm::radians(playerZeppelin->yawAngle));
		glm::vec3 norm = glm::normalize(glm::vec3(fdvx, 0, fdvz));
		norm = p + norm;
		Camera::lookAt(playerZeppelin->x, playerZeppelin->y - 2, playerZeppelin->z, norm.x, norm.y, norm.z, 0, 1, 0);
	}
	else {
		Camera::lookAt(0, 15, 8, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	}
	// Create Viewing Matrix V
	// Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
//	gluLookAt(22.0*sin(cameraAngle), 0, 22.0 * cos(cameraAngle), 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	//Camera::lookAt(cos(viewPhi) * sin(viewTheta) * viewRadius, cos(viewTheta) * viewRadius, sin(viewPhi) * sin(viewTheta) * viewRadius, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	auto currentTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> timeElapsed = currentTime - lastCallTime;
	auto millisElapsed = std::chrono::duration_cast<std::chrono::microseconds>(timeElapsed);
	auto x = static_cast<long>(millisElapsed.count());
	//std::cout << x;
	//glRotatef(90, 1, 0, 0);
	if (playerZeppelin->health > 0) {
		enemyControl(x);
	}
	checkBullets();
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotLowerBody_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotLowerBody_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotLowerBody_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotLowerBody_mat_shininess);
	//glTranslatef(0, 0, -1);
	//glScalef(3, 3, 3);
	//glRotatef(90, 0, 0, 1);
	//glRotatef(-90, 1,0, 0);
	
	//MatrixManager::rotate(0.01, 0,1,1);
	//MatrixManager::scale(0.25, 0.25, 0.25);
	done = true;

	//d->draw(0);
	
	world->draw();
	mm::push();

		playerZeppelin->draw(x, moveDirection);

	
	mm::pop();

	mm::push();
	
		enemyZeppelin->draw(x, enemyDirection);
	
	mm::pop();
	
	glutSwapBuffers();   // Double buffering, swap buffers
	glutPostRedisplay();
	lastCallTime = currentTime;
}



// Callback, called at initialization and whenever user resizes the window.
void reshape(int w, int h)
{
	// Set up viewport, projection, then change to modelview matrix mode - 
	// display function will then set up camera and do modeling transforms.
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	Camera::setProj(90, (GLdouble)w / h, 0.1, 40.0);
	gluPerspective(90, (GLdouble)w / h, 0.1, 40.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	MatrixManager::loadIdentity();

	// Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
	//gluLookAt(22.0*sin(cameraAngle), 0, 22.0 * cos(cameraAngle), 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	//Camera::lookAt(0, 0, 8, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

bool stop = false;
// Callback, handles input from the keyboard, non-arrow keys
void keyboard(unsigned char key, int x, int y)
{
//	bool wOn = false;
	switch (key)
	{
	case 't':

		break;
	case 'r':
		playerZeppelin->x = 0;
		playerZeppelin->y = 10;
		playerZeppelin->z = 0;
		playerZeppelin->health = 100;
		playerZeppelin->hit = false;

		enemyZeppelin->x = -15;
		enemyZeppelin->y = 10;
		enemyZeppelin->z = 0;
		enemyZeppelin->health = 100;
		enemyZeppelin->hit = false;

		playerZeppelin->props[0]->hit = false;
		playerZeppelin->props[1]->hit = false;


		enemyZeppelin->props[0]->hit = false;
		enemyZeppelin->props[1]->hit = false;

		break;
	case 'R':
		
		break;
	case 'a':
		
		break;
	case 'A':
		
		break;
	case 'g':
		
		break;
	case 'G':
	
		break;
	case 's':
		playerZeppelin->fire();
		break;
	case 'S':
		stop = true;
	case 'w':
		isMoving = true;
		break;
		//break;
	case 'W':
		isMoving = true;
		break;
	case 'f':
		if (!firstPerson) {
			firstPerson = true;
		}
		else {
			firstPerson = false;
		}
	}
	
	//std::cout << isMoving;
}


void animationHandler(int param)
{
	if (!stop)
	{
		//shoulderAngle += 1.0;
		//cubeAngle += 2.0;
		glutPostRedisplay();
		glutTimerFunc(10, animationHandler, 0);
	}
}



// Callback, handles input from the keyboard, function and arrow keys
void functionKeys(int key, int x, int y)
{
	// Help key
	if (key == GLUT_KEY_F1)
	{

	}

	if (key == GLUT_KEY_LEFT) {
		moveDirection = moveDirection | ZEP_LEFT;
	//	std::cout << "left: " << moveDirection << "\n";
	}

	if (key == GLUT_KEY_RIGHT) {
		moveDirection = moveDirection | ZEP_RIGHT;
	}

	if (key == GLUT_KEY_UP){
		moveDirection = moveDirection | ZEP_UP;
	}

	if (key == GLUT_KEY_DOWN) {
		moveDirection = moveDirection | ZEP_DOWN;
	}
	// Do transformations with arrow keys
	//else if (...)   // GLUT_KEY_DOWN, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_LEFT
	//{
	//}

	//glutPostRedisplay();   // Trigger a window redisplay
}

void fkeysUp(int key, int x, int y) {
	if (key == GLUT_KEY_LEFT) {
		moveDirection = moveDirection ^ ZEP_LEFT;
	}

	if (key == GLUT_KEY_RIGHT) {
		moveDirection = moveDirection ^ ZEP_RIGHT;
	}

	if (key == GLUT_KEY_UP) {
		moveDirection = moveDirection ^ ZEP_UP;
	}

	if (key == GLUT_KEY_DOWN) {
		moveDirection = moveDirection ^ ZEP_DOWN;
	}

	
}


// Mouse button callback - use only if you want to 
void mouse(int button, int state, int x, int y)
{
	currentButton = button;

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			;

		}
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{
			;
		}
		break;
	default:
		break;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}


// Mouse motion callback - use only if you want to 
void mouseMotionHandler(int xMouse, int yMouse)
{
	if (currentButton == GLUT_LEFT_BUTTON)
	{
		;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}

void keyup(unsigned char key, int x, int y) {
	switch (key)
	{
	case 't':

		break;
	case 'r':
		cameraAngle += 2.0 * (3.141592 / 180);
		break;
	case 'R':

		break;
	case 'a':

		break;
	case 'A':

		break;
	case 'g':

		break;
	case 'G':

		break;
	case 's':

		break;
	case 'S':
		stop = true;
		break;
	case 'w':
		isMoving = false;
		break;
	case 'W':
		isMoving = false;
	}
}

GLfloat robotLowerBody1_mat_ambient[] = { 0.9f, 0.9f, 0.9f, 1.0f };
GLfloat robotLowerBody1_mat_diffuse[] = { 0.9f, 0.9f, 0.9f, 1.0f };
GLfloat robotLowerBody1_mat_specular[] = { 0.3, 0.3, 0.3, 0.3 };
GLfloat robotLowerBody1_mat_shininess[] = { 76.8F };
// Light properties


void setupPlayerZeppelin() {
	playerZeppelin = new Zeppelin(0, 10, -4, 0.5, 0.8, 0.5);
	playerZeppelin->meshes[0].meshFilename = (char*)"meshes/zep_body.obj";
	playerZeppelin->meshes[0].texture.textureFolder = (char*)"textures/player_body";

	playerZeppelin->meshes[1].texture.textureFolder = (char*)"textures/player_rudders";
	playerZeppelin->meshes[2].texture.textureFolder = (char*)"textures/car_1";
	playerZeppelin->meshes[3].texture.textureFolder = (char*)"textures/car_1";

	playerZeppelin->meshes[0].vertFileName = (char*)"a3.vert";
	playerZeppelin->meshes[0].fragFileName = (char*)"a3.frag";
	playerZeppelin->sameShaderForAll();

	playerZeppelin->assignShaders();

	playerZeppelin->meshes[1].meshFilename = (char*)"meshes/zep_rudder.obj";
	playerZeppelin->meshes[2].meshFilename = (char*)"meshes/zep_car.obj";
	playerZeppelin->meshes[3].meshFilename = (char*)"meshes/zep_engine_rod.obj";
	playerZeppelin->loadMeshes();

}

void setupEnemyZeppelin() {
	enemyZeppelin = new Zeppelin(-10, 10, 0, 0.5, 0.8, 0.5);

	enemyZeppelin->meshes[0].meshFilename = (char*)"meshes/zep_body.obj";
	enemyZeppelin->meshes[0].texture.textureFolder = (char*)"textures/enemy_rudders";
	
	enemyZeppelin->meshes[1].texture.textureFolder = (char*)"textures/enemy_rudders";
	enemyZeppelin->meshes[2].texture.textureFolder = (char*)"textures/car_1";
	enemyZeppelin->meshes[3].texture.textureFolder = (char*)"textures/car_1";

	enemyZeppelin->meshes[0].vertFileName = (char*)"a3.vert";
	enemyZeppelin->meshes[0].fragFileName = (char*)"a3.frag";
	enemyZeppelin->sameShaderForAll();

	enemyZeppelin->assignShaders();

	enemyZeppelin->meshes[1].meshFilename = (char*)"meshes/zep_rudder.obj";
	enemyZeppelin->meshes[2].meshFilename = (char*)"meshes/zep_car.obj";
	enemyZeppelin->meshes[2].meshFilename = (char*)"meshes/zep_car.obj";
	enemyZeppelin->meshes[3].meshFilename = (char*)"meshes/zep_engine_rod.obj";
	enemyZeppelin->loadMeshes();

}


static int lastMouseX;
static int lastMouseY;;

void mouseButtonHandler3D(int button, int state, int x, int y)
{
	currentButton = button;
	lastMouseX = x;
	lastMouseY = y;
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{

		}
		break;
	default:
		break;
	}
	glutPostRedisplay();
}


void mouseMotionHandler3D(int x, int y)
{
	
	//
	////printf("dx: %d, dy: %d\n", dx, dy);
	//// REQUIREMENT 3 code here
	//if (currentButton == GLUT_LEFT_BUTTON)
	//{
	//	int dx = x - lastMouseX;
	//	int dy = y - lastMouseY;
	//	viewPhi += dx * 0.001;
	//	lastMouseX = x;
	//	lastMouseY = y;
	//	Camera::rotate(dx, 0, 1, 0);

	//	if (glutGetModifiers() == GLUT_ACTIVE_SHIFT)
	//	{
	//		viewTheta += -dy * 0.01;
	//		Camera::rotate(dy, 1, 0, 0);
	//		if (viewTheta < 0.01) {
	//			viewTheta = 0.01;
	//		}
	//		else if (viewTheta > M_PI / 1.3) {
	//			viewTheta = M_PI / 1.3;
	//		}
	//	}
	//	else
	//	{

	//	}
	//}
}


void handleWheel(int button, int dir, int x, int y) {
	/*if (dir > 0) {
		viewRadius -= 0.2;
		if (viewRadius < 8) {
			viewRadius = 8;
		}
	}
	else {
		viewRadius += 0.2;
		if (viewRadius > 25) {
			viewRadius = 25;
		}
	}
	Camera::zoom(-dir * 0.15f);*/
}

typedef enum {
	LOOK = 0,
	TURN,
	FIND_ANGLE,
	FIND_PLAYER,
	IDLE
} STATE;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<double> dist(0, 45);
void enemyControl(long t) {
	static STATE  state = LOOK;
	static float yaw = 0;
	static double r = 0;
	double distance = 0;
	float angle = 0;
	float pz;
	float px;
	float ez;
	float ex;
	static float goalAngle = 0;
	static bool angleOK = false;
	static bool coolDown = false;
	static auto time = std::chrono::high_resolution_clock::now();
	static ZEP_DIRECTION current = NONE;
	printf("state: %d\n", state);
	distance = std::sqrt(std::pow(enemyZeppelin->x - playerZeppelin->x, 2) + std::pow(enemyZeppelin->y - playerZeppelin->y, 2) + std::pow(enemyZeppelin->z - playerZeppelin->z, 2));

	std::chrono::duration<double> timeElapsed = std::chrono::high_resolution_clock::now() - time;
	auto millisElapsed = std::chrono::duration_cast<std::chrono::microseconds>(timeElapsed);
	auto x = static_cast<long>(millisElapsed.count());
	if (x > 3000000) {
		coolDown = false;
	}
	switch (state) {
		
	case(LOOK):
		enemyZeppelin->start(0);
		if (distance < 5) {
			state = FIND_ANGLE;
		}
		if (enemyZeppelin->x < -18 || enemyZeppelin->x > 18 || enemyZeppelin->z < -18 || enemyZeppelin->z > 8) {
			if ((enemyZeppelin->x < -18)) enemyZeppelin->x = -17.18;
			if ((enemyZeppelin->x > 18)) enemyZeppelin->x = 17.18;
			if ((enemyZeppelin->z < -18)) enemyZeppelin->z = -17.18;
			if ((enemyZeppelin->z > 8)) enemyZeppelin->z = 7.9;
			enemyZeppelin->stop();
			state = TURN;
			yaw = enemyZeppelin->yawAngle + 180 + dist(gen);
			while (yaw > 360) {
				yaw -= 360;
			}
			
		}
		break;
	case(TURN):
		if (distance < 5) {
			state = FIND_ANGLE;
		}
		
		enemyDirection = ZEP_LEFT;
		enemyZeppelin->stop();
		if (yaw > enemyZeppelin->yawAngle) {
			enemyDirection = ZEP_LEFT;
		}
		if (yaw < enemyZeppelin->yawAngle) {
			enemyDirection = ZEP_RIGHT;
		}
		if (abs(yaw - enemyZeppelin->yawAngle) < 2) {
			enemyDirection = NONE;
			state = LOOK;
		}
		
		break;
	case(FIND_ANGLE):
		//enemyDirection = ZEP_LEFT;
		if (distance > 5) {
			state = LOOK;
			enemyDirection = NONE;
			return;
		}
		enemyZeppelin->stop();
		 pz = playerZeppelin->z;
		 px = playerZeppelin->x;
		 ez = enemyZeppelin->z;
		 ex = enemyZeppelin->x;
		 glm::vec3 n = glm::vec3(playerZeppelin->x, playerZeppelin->y, playerZeppelin->z) - glm::vec3(enemyZeppelin->x, enemyZeppelin->y, enemyZeppelin->z);
		 angle = glm::degrees(glm::atan(-n.z, n.x));
		if (pz > ez && px > ex) {
			printf("player greater en\n");
			angle += 180;
	}
		else if ((pz < ez && px > ex)) {
			printf("player z  less en\n");
			angle += 180;
		}
		else if ((pz > ez && px < ex)) {
			printf("player x  less en\n");
			angle += 180;
		}
		else if ((pz < ez && px < ex)) {
			printf("player  less en\n");
			angle += 180;
		}
		
		goalAngle = angle;
		while (goalAngle > 360) {
			goalAngle -= 360;
		}

		while (goalAngle < 0) {
			goalAngle += 360;
		}

		state = FIND_PLAYER;
		

		//enemyZeppelin->yawAngle = angle;
		//enemyDirection = 0;
		break;
	case(FIND_PLAYER):
		if (distance > 5) {
			state = LOOK;
			enemyDirection = NONE;
			return;
		}
		if (goalAngle > enemyZeppelin->yawAngle) {
			//printf("left\n");
			state = FIND_ANGLE;
			printf("LEFT: en angle: %2.2f, goal: %2.2f, diff  = %2.2f\n", enemyZeppelin->yawAngle, goalAngle, goalAngle - enemyZeppelin->yawAngle);
			enemyDirection = ZEP_LEFT | current;
		//	current = ZEP_LEFT;
			if (goalAngle - enemyZeppelin->yawAngle < 5) {
				angleOK = true;
				
				enemyDirection = enemyDirection ^ ZEP_LEFT;
				//goalAngle = 0;
			}
		}
		else if (goalAngle < enemyZeppelin->yawAngle) {
		//	printf("right\n");
			state = FIND_ANGLE;
			printf("RIGHT: en angle: %2.2f, goal: %2.2f, diff  = %2.2f\n", enemyZeppelin->yawAngle, goalAngle, enemyZeppelin->yawAngle - goalAngle);
			enemyDirection = ZEP_RIGHT | current;
			//current = ZEP_RIGHT;
			if (enemyZeppelin->yawAngle - goalAngle < 5) {
				enemyDirection = enemyDirection ^ ZEP_RIGHT;
				//goalAngle = 0;
				
				angleOK = true;
			}
		}

		if ((enemyZeppelin->y - playerZeppelin->y > 0.25) && (enemyZeppelin->y - playerZeppelin->y < 1.5) && !angleOK) {
			enemyDirection = enemyDirection ^ current;
			state = FIND_ANGLE;
			current = NONE;
		}

		if ((enemyZeppelin->y - playerZeppelin->y > 0.25) && (enemyZeppelin->y - playerZeppelin->y < 1.5) && angleOK) {
			if (!playerZeppelin->hit) {
				if (!coolDown) {
					enemyZeppelin->fire();
					time = std::chrono::high_resolution_clock::now();
					coolDown = true;
				}
			}
			enemyDirection = enemyDirection ^ current;
			current = NONE;
			state = IDLE;
			angleOK = false;
		}else if (enemyZeppelin->y - playerZeppelin->y < 0.25) {
			state = FIND_ANGLE;
			enemyDirection = enemyDirection ^ ZEP_UP ^ current;
			current = ZEP_UP;
		}else  if (enemyZeppelin->y - playerZeppelin->y > 1.5) {
			state = FIND_ANGLE;
			enemyDirection = enemyDirection ^ ZEP_DOWN ^ current;
			current = ZEP_DOWN;
		}
		break;
	case(IDLE):
		enemyDirection = NONE;
		//enemyZeppelin->fire();
		if (glm::abs(goalAngle - enemyZeppelin->yawAngle) > 15 || abs(playerZeppelin->y - enemyZeppelin->y) > 1.6) {
			state = FIND_ANGLE;
		}
		else if (distance > 3) {
			state = LOOK;
			angleOK = false;
		}
		
		break;
	default:
		break;
	}



}

void checkBullets() {
	if (!playerZeppelin->fired && !enemyZeppelin->fired) {
		return;
	}

	if (playerZeppelin->fired) {
		glm::vec3 loc = playerZeppelin->missileLoc;
		double distance = std::sqrt(std::pow(enemyZeppelin->x - loc.x, 2) + std::pow(enemyZeppelin->y - loc.y, 2) + std::pow(enemyZeppelin->z - loc.z, 2));
		if (distance < 2) {
			enemyZeppelin->hit = true;
			playerZeppelin->fired = false;
			//enemyZeppelin->stop();
		}

	}

	if (enemyZeppelin->fired) {
		glm::vec3 loc = enemyZeppelin->missileLoc;
		double distance = std::sqrt(std::pow(playerZeppelin->x - loc.x, 2) + std::pow(playerZeppelin->y - loc.y, 2) + std::pow(playerZeppelin->z - loc.z, 2));
		if (distance < 2) {
			playerZeppelin->hit = true;
			enemyZeppelin->fired = false;
			//playerZeppelin->stop();
		}

	}
}
