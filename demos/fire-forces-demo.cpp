#include "Scene.hpp"
#include "Camera.hpp"
#include "Texture.hpp"
#include "Particles.hpp"
#include "Mesh.hpp"
#include "Exception.hpp"

#include <glm.hpp>
#include <GL/glut.h>
#include <gtc/matrix_transform.hpp>

/* Fire Forces Demo
 * This demo applies a force to the fire which varies with time. This is 
 * intended to demonstrate the way the lighting dynamically adapts to the
 * behaviour of the flame.
 */

int init();
void display();
void reshape (int, int);
void keyboard(unsigned char, int, int);

ParticleShader* tShader; // Scrolling texture shader.
ParticleShader* pShader; // Procedural texture shader.
ParticleShader* sShader; // Spark shader.

AdvectParticlesCentroidLights* flame;
AdvectParticles*               sparks;

Mesh* bunny;

Scene* scene;
SHLight* light;

const int k = 5;

int eTime;
int deTime;

float flameIntensity = 0.001f;
float intensityDelta = 0.0001f;

const float delta = 0.4f;

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	eTime = glutGet(GLUT_ELAPSED_TIME);
    glutInitDisplayMode(GLUT_DOUBLE);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Fire External Forces Demo");
    glewInit();
    try
	{
		init();
	}
	catch (Exception& e)
	{
		std::cout << e.msg;
		std::cout << "Press ENTER to quit.\n";
		std::cin.get();
		return 0;
	}
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
    glutMainLoop();

	delete scene;
}

// Called by glutInit().
int init()
{
	/* Ambient Light Properties */
	const float ambIntensity = 0.5f;
	const glm::vec4 ambColor(0.7f, 0.7f, 0.9f, 1.0f); //light blue

	/* Flame Properties */
	const int nFlameParticles = 400;
	const int nFlameLights = 10;
	const int lightClumpSize = 10;
	const int hopInterval = -1; // Never hop. Set to +ve ms value to hop.

	/* Spark Properties */
	const int nSparkParticles = 5;
	const int sparkLifetime = 2000;
	const int sparkVarLifetime = 200;
	const glm::vec4 sparkInitAcn(0.0f, -0.0000004f, 0.0f, 0.0f);
	const float sparkInitVel = 0.0f; 
	const float sparkInitUpVel = 0.0008f;
	const int avgSparkPerturb = 1000;
	const int varSparkPerturb = 100;
	const float sparkPerturbRadius = 0.0004f;
	const float sparkCenterForce = 0.0f;
	const float sparkBaseRadius = 0.2f;
	const float sparkBBHeight = 0.03f;
	const float sparkBBWidth = 0.03f;

	/* Smoke Properties */
	const int nSmokeParticles = 20;

	/* Bunny Properties */
	const float bunnySpecExp = 500.0f;

	glClearColor(ambColor.x, ambColor.y, ambColor.z, ambColor.w);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	scene = new Scene();

	scene->setAmbLight(ambColor * ambIntensity);

	pShader = new ParticleShader(true, false, "ProceduralFire");
	tShader = new ParticleShader(true, true , "ScrollTexFire" );
	sShader = new ParticleShader(true, true , "Sparks");

	Texture* flameAlphaTex = new Texture("flameAlpha.png");
	Texture* flameDecayTex = new Texture("flameDecay.png");

	Texture* sparkAlphaTex = new Texture("sparkAlpha.png");
	Texture* sparkDecayTex = new Texture("sparkDecay.png");

	Texture* smokeAlphaTex = new Texture("smokeAlpha.png");
	Texture* smokeDecayTex = new Texture("smokeDecay.png");

	flame = new AdvectParticlesCentroidLights(
		nFlameParticles, nFlameLights, lightClumpSize, hopInterval,
		pShader, flameAlphaTex, flameDecayTex);

	sparks = new AdvectParticles(
		nSparkParticles, sShader, sparkAlphaTex, sparkDecayTex, 
		false, true);

	sparks->avgLifetime = sparkLifetime;
	sparks->varLifetime = sparkVarLifetime;
	sparks->initAcn = sparkInitAcn;
	sparks->initVel = sparkInitVel;
	sparks->initUpVel = sparkInitUpVel;
	sparks->avgPerturbTime = avgSparkPerturb;
	sparks->varPerturbTime = varSparkPerturb;
	sparks->baseRadius = sparkBaseRadius;
	sparks->bbHeight = sparkBBHeight;
	sparks->bbWidth = sparkBBWidth;

	flame->translate(glm::vec3(0.0f, 0.0f, 1.0f));
	sparks->translate(glm::vec3(0.0f, 0.0f, 1.0f));

	scene->add(flame);
	scene->add(sparks);

	LightShader* bunnyShader = new LightShader(false, "BlinnPhong");

	Texture* bunnyAmbTex = new Texture("ceramic.png");
	Texture* bunnyDiffTex = new Texture("ceramic.png");
	Texture* bunnySpecTex = new Texture("white.png");

	bunny = new Mesh(
		"stanford.obj",
		bunnyAmbTex, bunnyDiffTex, bunnySpecTex,
		bunnySpecExp, bunnyShader);

	bunny->uniformScale(0.2f);

	scene->add(bunny);

	scene->camera->translate(glm::vec3(0.0f, 0.0f, -4.0f));

	return 1;
}

// Perform rendering and updates here.
void display()
{
	deTime = glutGet(GLUT_ELAPSED_TIME) - eTime;
	eTime = glutGet(GLUT_ELAPSED_TIME);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene->update(deTime);
	flame->extForce = glm::vec4(glm::vec3(
		6e-7f * sin(static_cast<float>(eTime) / 1000.0f), 0.0f, 0.0f ), 0.0f);
	sparks->extForce = glm::vec4(glm::vec3(
		6e-7f * sin(static_cast<float>(eTime) / 1000.0f), 0.0f, 0.0f ), 0.0f);
	scene->render();
	glutSwapBuffers();
	glutPostRedisplay();
}

// Called when window size changes.
void reshape (int w, int h)
{
	float aspect = (float) w / (float) h;
	scene->camera->setAspect(aspect);
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}

// Called when a key is pressed.
void keyboard(unsigned char key, int x, int y)
{
	scene->camera->keyboardInput(key, x, y);

    switch (key)
    {
    case 'f':
    	//Switch fire mode.
    	if(flame->getShader() == tShader)
    	{
    		flame->setShader(pShader);
    	}
    	else
    	{
    		flame->setShader(tShader);
    	}
    	break;

	case 't':
		flameIntensity += intensityDelta;
		flame->setLightIntensity(flameIntensity);
		std::cout << flameIntensity << std::endl;
		break;

	case 'g':
		flameIntensity -= intensityDelta;
		flame->setLightIntensity(flameIntensity);
		std::cout << flameIntensity << std::endl;
		break;

    case 27:
        exit(0);
        return;
    }
}
