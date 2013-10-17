#include "Scene.hpp"
#include "Camera.hpp"
#include "Texture.hpp"
#include "Particles.hpp"
#include "PRTMesh.hpp"
#include "SpherePlot.hpp"
#include "Exception.hpp"

#include <glm.hpp>
#include <GL/glut.h>
#include <gtc/matrix_transform.hpp>

#include <fstream>

/* SH Fire Demo
 * Demo displays a textured mesh illuminated by a fire using PRT lighting
 * with centroid lights.
 */

int init();
void display();
void reshape (int, int);
void keyboard(unsigned char, int, int);

ParticleShader* tShader; // Scrolling texture shader.
ParticleShader* pShader; // Procedural texture shader.
ParticleShader* sShader; // Spark shader.

AdvectParticlesCentroidSHLights* flame;
AdvectParticles*                 sparks;
AdvectParticles*                 smoke;

SpherePlot* plot;
bool showPlot = false;

PRTMesh* bunnyU;
PRTMesh* bunnyS;
PRTMode mode = UNSHADOWED;

Scene* scene;
SHLight* light;

const int k = 5;

int eTime;
int deTime;

const float delta = 0.4f;

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	eTime = glutGet(GLUT_ELAPSED_TIME);
    glutInitDisplayMode(GLUT_DOUBLE);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Fire and SH Lighting Demo");
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
	const float flameIntensity = 1.0f;
	const int nFlameLights = 2;
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

	glClearColor(ambColor.x, ambColor.y, ambColor.z, ambColor.w);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	scene = new Scene();
	scene->setAmbLight(ambColor * ambIntensity);

	SHShader* bunnyShader = new SHShader(false, "diffPRT");

	bunnyU = new PRTMesh("stanford.prtu5", bunnyShader);
	bunnyS = new PRTMesh("stanford.prts5", bunnyShader);

	bunnyU->uniformScale(0.2f); bunnyS->uniformScale(0.2f);

	scene->add(bunnyU);

	pShader = new ParticleShader(true, false, "ProceduralFire");
	tShader = new ParticleShader(true, true , "ScrollTexFire" );
	sShader = new ParticleShader(true, true , "Sparks");

	Texture* flameAlphaTex = new Texture("flameAlpha.png");
	Texture* flameDecayTex = new Texture("flameDecay.png");

	Texture* sparkAlphaTex = new Texture("sparkAlpha.png");
	Texture* sparkDecayTex = new Texture("sparkDecay.png");

	Texture* smokeAlphaTex = new Texture("smokeAlpha.png");
	Texture* smokeDecayTex = new Texture("smokeDecay.png");

	flame = new AdvectParticlesCentroidSHLights(
		bunnyU, flameIntensity,
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

	smoke = new AdvectParticles(
		nSmokeParticles, pShader, smokeAlphaTex, smokeDecayTex, true, false);

	flame->translate(glm::vec3(0.0f, 0.0f, 1.0f));
	sparks->translate(glm::vec3(0.0f, 0.0f, 1.0f));
	smoke->translate(glm::vec3(0.0f, 1.0f, 1.0f));

	scene->add(flame);
	scene->add(sparks);
	scene->add(smoke);

	Shader* plotShader = new Shader(false, "SpherePlot");

	plot = new SpherePlot(
		[] (float theta, float phi) -> 
		float {
			std::vector<glm::vec3> allLights(GC::nSHCoeffts);
			std::fill(allLights.begin(), allLights.end(), glm::vec3(0.0f));
			for(size_t l = 0; l < flame->lights.size(); ++l)
				for(size_t c = 0; c < GC::nSHCoeffts; ++c)
					allLights[c] += flame->lights[l]->getCoeffts()[c];
			return SH::evaluate(allLights, theta, phi).x / flame->getIntensity();
			},
		40, plotShader);

	plot->translate(glm::vec3(-1.0f, 0.0f, 0.0f));

	plot->uniformScale(0.08f);

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

	if(showPlot)
	{
		plot->replot(
			[] (float theta, float phi) -> 
			float {
				std::vector<glm::vec3> allLights(GC::nSHCoeffts);
				std::fill(allLights.begin(), allLights.end(), glm::vec3(0.0f));
				for(size_t l = 0; l < flame->lights.size(); ++l)
					for(size_t c = 0; c < GC::nSHCoeffts; ++c)
						allLights[c] += flame->lights[l]->getCoeffts()[c];
				return SH::evaluate(allLights, theta, phi).x / flame->getIntensity();
				},
				40);
	}

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
	case 'p':
		if(showPlot)
		{
			scene->remove(plot);
		}
		else
		{
			scene->add(plot);
		}
		showPlot = !showPlot;
		break;

    case 'f':
    	//Switch fire mode.
    	if(flame->getShader() == tShader)
    	{
    		flame->setShader(pShader);
    		smoke->setShader(pShader);
    	}
    	else
    	{
    		flame->setShader(tShader);
    		smoke->setShader(tShader);
    	}
    	break;

	case 'm':
		if(mode == UNSHADOWED)
		{
			mode = SHADOWED;
			scene->remove(bunnyU);
			scene->add(bunnyS);
			std::cout << "Shadowed PRT enabled." << std::endl;
		}
		else //SHADOWED
		{
			mode = UNSHADOWED;
			scene->remove(bunnyS);
			scene->add(bunnyU);
			std::cout << "Unshadowed PRT enabled." << std::endl;
		}
		break;

    case 27:
        exit(0);
        return;
    }
}
