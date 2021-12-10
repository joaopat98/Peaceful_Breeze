#include <iostream>
#include <map>

#include "engine.h"
#include "fly_camera.h"
#include "input.h"
#include "SSAO.h"
#include "post_processing.h"
#include "bloom.h"
#include "hdr.h"
#include "dof.h"
#include "dither.h"
#include "fxaa.h"
#include "light.h"
#include "shadowbuffer.h"
#include "cubeMap.h"
#include "screenshot.h"
#include "texture.h"

#include <FreeImage.h>
#include <direct.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

Program* texProg;

OrthoProjection ortho = OrthoProjection(-1, 1, -1, 1, 0, 100);
PerspectiveProjection persp = PerspectiveProjection(90, 1, 0.01f, 100);
FlyCamera* cam;

Scene scene;

DeferredRenderer* defRenderer;
SSAORenderer* ssaoRenderer;

PostProcessingStack* pps;
BloomPass* bloomPass;
HDRToneMappingPass* hdrPass;
DOFPass* dofPass;
DitherPass* ditherPass;
FXAAPass* fxaaPass;

Light* light;
ShadowBuffer* shadowBuffer;

double FPS;

unsigned int width = 1280, height = 720;

bool useOcclusion = true;
bool showSSAO = false;

Engine* engine;

bool focused = true;

SceneNode* sun;

float minBias = 0.001f, maxBias = 0.015f;

CubeMap* cubeMap, * cubeMapSun;

SceneNode* skyboxSun;

float sunRadius = 10;

vec2 sunRotation = vec2(334.021f, 45);

bool takeScreenShot = false;

//Grass
float noiseScale = 0.35f;
vec3 windDir = AXIS3_Z;
float windSpeed = 3;
float windStrengthGrass = 0.3f;
float windStrengthLeaves = 1.0f;
int leafTesselation[] = { 6,6,6,10,10,6 };

struct CubeDrawable : IDrawable {
	Mesh* mesh;

	CubeDrawable(Mesh* mesh) {
		this->mesh = mesh;
	}

	void draw(Program* program, string drawMode) {
		mesh->draw(true);
	}
};

struct ShadowDrawable : IDrawable {
	Mesh* mesh;
	vec3 color;
	Light* light;
	Program* shadowProg, * defferedProg, * renderProg;
	bool useTexture;
	GLuint texture;

	ShadowDrawable(Mesh* mesh, vec3 color, Light* light, bool useTexture = false, GLuint texture = 0) {
		this->mesh = mesh;
		this->color = color;
		this->light = light;
		this->useTexture = useTexture;
		this->texture = texture;
		programs.push_back(shadowProg = Program::getOrCompile("shadowshader_vs.glsl", "shadowShader_fs.glsl"));
		programs.push_back(defferedProg = Program::getOrCompile("gbuffer_vert.glsl", "gbuffer_frag.glsl"));
		programs.push_back(renderProg = Program::getOrCompile("toonshader_vs.glsl", "toonshader_fs.glsl"));
	}

	void draw(string drawMode) {
		if (drawMode == DEFERRED_DRAW_MODE) {
			defferedProg->setUniform1i("useTexture", useTexture);
			if (useTexture) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture);
				defferedProg->setUniform1i("Texture", 0);
			}
			defferedProg->use();
			mesh->draw();
		}
		else if (drawMode == SHADOW_DRAW_MODE) {
			shadowProg->use();
			shadowProg->setUniform1i("useTexture", useTexture);
			if (useTexture) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture);
				shadowProg->setUniform1i("Texture", 0);
			}
			shadowProg->setUniformMat4("LightSpaceMatrix", light->camera.getProjectionMatrix() * light->camera.getViewMatrix());
			mesh->draw();
		}
		else {
			renderProg->use();
			renderProg->setUniformv3("Color", color);
			renderProg->setUniform1f("minBias", minBias);
			renderProg->setUniform1f("maxBias", maxBias);
			renderProg->setUniformv3("LightColor", light->color);
			renderProg->setUniformv3("LightDir", (light->position - light->directionPoint).normalize());
			renderProg->setUniformv3("cameraPos", cam->eye);
			renderProg->setUniformMat4("LightSpaceMatrix", light->camera.getProjectionMatrix() * light->camera.getViewMatrix());
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, shadowBuffer->depthMap);
			renderProg->setUniform1i("shadowMap", 0);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, ssaoRenderer->getSSAOTex());
			renderProg->setUniform1i("occlusion", 1);
			renderProg->setUniform1i("useOcclusion", useOcclusion);
			renderProg->setUniform1i("useTexture", useTexture);
			if (useTexture) {
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, texture);
				renderProg->setUniform1i("Texture", 2);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
			mesh->draw();
			glDisable(GL_BLEND);
		}
	}
};

struct GrassDrawable : IDrawable {
	vector<vec3> verts;
	vector<vec2> texcoords;
	vector<vec3> normals;

	Mesh* mesh;
	vec3 color;
	Light* light;
	vec3 bladeSize;
	vec3 noisePos;
	float windStrength;
	bool useTexture;
	GLuint texture;
	int tesselation = 10;
	/*/
	float offsetValue = 0.5f;
	float sinFactor = 50.0f; // accelerate/desaccelerate sin frequency
	float kAmpValue = 30.0f;
	bool kAmp = false;
	float fTimePassed = 15.0f;
	float prev_tCur = 0.0f;
	/**/
	Program* shadowProg, * defferedProg, * renderProg;

	GrassDrawable(Mesh* mesh, vec3 bladeSize, vec3 color, Light* light, vector<vec3> meshVerts, vector<vec2> meshTexCoords, vector<vec3> meshNormals, bool useTexture = false, GLuint texture = 0) {
		this->mesh = mesh;
		this->bladeSize = bladeSize;
		this->color = color;
		this->light = light;
		this->useTexture = useTexture;
		this->texture = texture;
		verts = meshVerts;
		texcoords = meshTexCoords;
		normals = meshNormals;
		programs.push_back(shadowProg = Program::getOrCompile("grass_vert.glsl", "shadowShader_fs.glsl", "grass_tcs.glsl", "grass_tes.glsl", "grass_geom_shadow.glsl"));
		programs.push_back(defferedProg = Program::getOrCompile("grass_vert.glsl", "gbuffer_frag.glsl", "grass_tcs.glsl", "grass_tes.glsl", "grass_geom_deffered.glsl"));
		programs.push_back(renderProg = Program::getOrCompile("grass_vert.glsl", "toonshader_fs.glsl", "grass_tcs.glsl", "grass_tes.glsl", "grass_geom.glsl"));
	}

	void update(double seconds) {
		noisePos +=  windDir * windSpeed * (float)seconds * noiseScale;
	}

	void draw(string drawMode) {
		if (drawMode == DEFERRED_DRAW_MODE) {
			defferedProg->use();
			defferedProg->setUniform1i("tesselation", tesselation);
			defferedProg->setUniform1i("useTexture", useTexture);
			if (useTexture) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture);
				defferedProg->setUniform1i("Texture", 0);
			}
			defferedProg->setUniformv3("grassVertices", (int)verts.size(), &verts[0]);
			defferedProg->setUniformv2("grassTexCoords", (int)verts.size(), &texcoords[0]);
			defferedProg->setUniformv3("grassNormals", (int)verts.size(), &normals[0]);
			defferedProg->setUniform1i("grassVerticesSize", (int)verts.size());
			defferedProg->setUniformv3("bladeSize", bladeSize);

			defferedProg->setUniformv3("noisePos", noisePos);
			defferedProg->setUniform1f("noiseScale", noiseScale);
			defferedProg->setUniformv3("windDir", windDir);
			defferedProg->setUniform1f("windStrength", windStrength);

			mesh->draw(true);
		}
		if (drawMode == SHADOW_DRAW_MODE) {
			shadowProg->use();
			shadowProg->setUniform1i("tesselation", tesselation);
			shadowProg->setUniformv3("grassVertices", (int)verts.size(), &verts[0]);
			shadowProg->setUniformv2("grassTexCoords", (int)verts.size(), &texcoords[0]);
			shadowProg->setUniform1i("grassVerticesSize", (int)verts.size());
			shadowProg->setUniformv3("bladeSize", bladeSize);

			shadowProg->setUniform1i("useTexture", useTexture);
			if (useTexture) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture);
				shadowProg->setUniform1i("Texture", 0);
			}

			shadowProg->setUniformv3("noisePos", noisePos);
			shadowProg->setUniform1f("noiseScale", noiseScale);
			shadowProg->setUniformv3("windDir", windDir);
			shadowProg->setUniform1f("windStrength", windStrength);

			shadowProg->setUniformMat4("LightSpaceMatrix", light->camera.getProjectionMatrix() * light->camera.getViewMatrix());

			mesh->draw(true);
		}
		if (drawMode == DEFAULT_DRAW_MODE) {
			renderProg->setUniform1i("tesselation", tesselation);
			renderProg->setUniformv3("grassVertices", (int)verts.size(), &verts[0]);
			renderProg->setUniformv2("grassTexCoords", (int)verts.size(), &texcoords[0]);
			renderProg->setUniformv3("grassNormals", (int)verts.size(), &normals[0]);
			renderProg->setUniform1i("grassVerticesSize", (int)verts.size());
			renderProg->setUniformv3("bladeSize", bladeSize);
			/*/
			clock_t tCur = clock();
			fTimePassed += (tCur - prev_tCur) / float(CLOCKS_PER_SEC);
			fTimePassed = fmod(fTimePassed, 360.0f);
			prev_tCur = tCur;
			/**/
			renderProg->setUniformv3("noisePos", noisePos);
			renderProg->setUniform1f("noiseScale", noiseScale);
			renderProg->setUniformv3("windDir", windDir);
			renderProg->setUniform1f("windStrength", windStrength);
			renderProg->setUniformv3("Color", color);
			renderProg->setUniform1f("minBias", minBias);
			renderProg->setUniform1f("maxBias", maxBias);
			renderProg->setUniformv3("LightColor", light->color);
			renderProg->setUniformv3("LightDir", (light->position - light->directionPoint).normalize());
			renderProg->setUniformv3("cameraPos", cam->eye);
			renderProg->setUniformMat4("LightSpaceMatrix", light->camera.getProjectionMatrix() * light->camera.getViewMatrix());
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, shadowBuffer->depthMap);
			renderProg->setUniform1i("shadowMap", 0);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, ssaoRenderer->getSSAOTex());
			renderProg->setUniform1i("occlusion", 1);
			renderProg->setUniform1i("useOcclusion", useOcclusion);
			renderProg->setUniform1i("useTexture", useTexture);
			if (useTexture) {
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, texture);
				renderProg->setUniform1i("Texture", 2);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
			renderProg->use();
			mesh->draw(true);
			glDisable(GL_BLEND);
		}
	}
};

GrassDrawable* grassDrawable;
vector<GrassDrawable*> leafDrawables;
vector<Mesh*> leafMeshes;
vector<SceneNode*> leafNodes;

struct SkyBoxDrawable : IDrawable {
	Mesh* mesh;
	CubeMap* cubeMap;
	Program* renderProg;
	float luminosity;
	bool useLightColorAbsolute;

	SkyBoxDrawable(Mesh* mesh, CubeMap* cubeMap, float luminosity, bool useLightColorAbsolute = false) {
		this->mesh = mesh;
		this->cubeMap = cubeMap;
		this->luminosity = luminosity;
		this->useLightColorAbsolute = useLightColorAbsolute;
		programs.push_back(renderProg = Program::getOrCompile("skybox_vertex.glsl", "skybox_fragment.glsl"));
	}
	void draw(string drawMode) {
		if (drawMode == DEFAULT_DRAW_MODE) {
			glCullFace(GL_FRONT);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap->texture);
			renderProg->setUniform1f("luminosity", luminosity);
			renderProg->setUniformv3("lightColor", light->color);
			renderProg->setUniform1i("useLightColorAbsolute", useLightColorAbsolute);
			renderProg->use();
			mesh->draw();
			glDisable(GL_BLEND);
			glCullFace(GL_BACK);
		}
	}
};

/////////////////////////////////////////////////////////////////////// SHADERs

void createShaderPrograms()
{
	texProg = Program::getOrCompile("tex_vert.glsl", "tex_frag.glsl");
}

void destroyShaderPrograms()
{
	texProg->destroy();
	glUseProgram(0);
}

/////////////////////////////////////////////////////////////////////// SCENE


void drawUI() {
	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	{
		ostringstream ss;
		ss << "FPS: " << FPS;
		ImGui::Text(ss.str().c_str());
		if (ImGui::Button("Take ScreenShot"))
			takeScreenShot = true;
		if (ImGui::TreeNode("Sun")) {
			ImGui::ColorPicker3("Color", &light->color.x);
			ImGui::Text("Position");
			ImGui::SliderFloat("X Rotation", &sunRotation.x, 0, 360);
			ImGui::SliderFloat("Y Rotation", &sunRotation.y, -90, 90);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Light")) {
			const char* resolutionNames[] = { "256x256", "512x512", "1024x1024", "2048x2048", "4096x4096", "8192x8192", "16384x16384" };
			static const char* current_item = "2048x2048";
			const GLuint resolutions[] = { 256, 512, 1024, 2048, 4096, 8192, 16384 };
			int resolution = 2048;

			if (ImGui::BeginCombo("Shadow Resolution", current_item))
			{
				for (int n = 0; n < IM_ARRAYSIZE(resolutionNames); n++)
				{
					bool is_selected = (current_item == resolutionNames[n]);
					if (ImGui::Selectable(resolutionNames[n], is_selected)) {
						current_item = resolutionNames[n];
						resolution = resolutions[n];
						shadowBuffer->SHADOW_WIDTH = resolution;
						shadowBuffer->SHADOW_HEIGHT = resolution;
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus(); 
				}
				ImGui::EndCombo();
			}

			ImGui::SliderFloat("Near Plane", &light->near_plane, 0, 20);
			ImGui::SliderFloat("Far Plane", &light->far_plane, 0, 20);
			ImGui::SliderFloat("Min Bias", &minBias, 0, maxBias);
			ImGui::SliderFloat("Max Bias", &maxBias, 0, 0.02f);
			ImGui::SliderFloat2("Box Dimensions", &light->box_width, 0, 20);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Tesselation")) {
			ImGui::SliderInt("Grass Tesselation", &grassDrawable->tesselation, 1, 64);
			if (ImGui::TreeNode("Leaf Tesselation")) {
				for (int i = 0; i < leafDrawables.size(); i++)
				{
					ostringstream ss;
					ss << "Leaf " << i;
					ImGui::SliderInt(ss.str().c_str(), &leafDrawables[i]->tesselation, 1, 64);
				}
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("HDR")) {
			ImGui::Checkbox("Active", &hdrPass->enabled);
			ImGui::Checkbox("Auto-Expose", &hdrPass->autoExpose);
			if (hdrPass->autoExpose) {
				ImGui::SliderFloat("Adaptation Speed", &hdrPass->adaptationSpeed, 0, 10);
				ImGui::SliderFloat("Exposure Multiplier", &hdrPass->autoExposureMultiplier, 0, 10);
			}
			else
				ImGui::SliderFloat("Exposure", &hdrPass->exposure, 0, 4);
			ImGui::SliderFloat("Gamma", &hdrPass->gamma, 0, 10);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Bloom")) {
			ImGui::Checkbox("Active", &bloomPass->enabled);
			ImGui::SliderFloat("Intensity", &bloomPass->intensity, 0, 1);
			ImGui::Checkbox("Progressive Upsampling", &bloomPass->progressiveUpsampling);
			ImGui::SliderInt("Subdivisions", &bloomPass->numDivisions, 0, 9);
			ImGui::SliderInt("Blur Passes", &bloomPass->numBlurPasses, 1, 100);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("SSAO")) {
			ImGui::Checkbox("Active", &useOcclusion);
			ImGui::Checkbox("Show", &showSSAO);
			ImGui::SliderInt("Samples", &ssaoRenderer->numSamples, 1, 1024);
			ImGui::SliderFloat("Sample Radius", &ssaoRenderer->sampleRadius, 0.000001f, 1);
			ImGui::SliderFloat("Bias", &ssaoRenderer->ssaoBias, 0, 0.1f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Wind")) {
			if (ImGui::TreeNode("Wind Strength")) {
				ImGui::SliderFloat("Grass", &windStrengthGrass, 0, 2.0f);
				ImGui::SliderFloat("Leaves", &windStrengthLeaves, 0, 2.0f);
				ImGui::TreePop();
			}
			ImGui::SliderFloat3("Wind Direction", &windDir.x, -1.0f, 1.0f);
			windDir = windDir.normalize();
			ImGui::SliderFloat("Wind Speed", &windSpeed, 0.0f, 10.0f);
			ImGui::SliderFloat("Noise Scale", &noiseScale, 0, 10);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Depth of Field")) {
			ImGui::Checkbox("Active", &dofPass->enabled);
			ImGui::SliderFloat("Aperture", &dofPass->aperture, 0, 50);
			ImGui::SliderFloat("Focus Distance", &dofPass->focusDistance, 0, 20);
			ImGui::SliderFloat("Image Distance", &dofPass->imageDistance, 0, 2);
			ImGui::SliderFloat("Max Depth Delta", &dofPass->maxDepthDelta, 0, 20);
			ImGui::SliderInt("Number of Rings", &dofPass->numRings, 0, 64);
			ImGui::Checkbox("Use Dynamic Ring Segmentation", &dofPass->dynamicRingSegments);
			if (dofPass->dynamicRingSegments)
				ImGui::SliderFloat("RingRatio", &dofPass->ringRatio, 0, 20);
			else
				ImGui::SliderInt("Number of Segments", &dofPass->numSegments, 0, 128);
			ImGui::Checkbox("Show COC", &dofPass->showCoc);
			ImGui::TreePop();
		}
		ImGui::Checkbox("Dither", &ditherPass->enabled);
		ImGui::Checkbox("FXAA", &fxaaPass->enabled);
	}
	ImGui::End();
}

void drawScene()
{
	//SSAO
	defRenderer->draw(&scene);
	ssaoRenderer->draw(&scene);

	//Shadows
	shadowBuffer->render(light, &scene);
	
	//PostProcessing
	pps->beginPostProcessing();
	scene.draw();
	pps->endPostProcessing();
	pps->drawToFrameBuffer(0);

	//SSAO Debug
	if (showSSAO) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ssaoRenderer->getSSAOTex());
		texProg->use();
		texProg->setUniform1i("Texture", 0);
		Mesh::getScreenQuad()->draw();
	}

	//ScreenShot
	if (takeScreenShot) {
		takeScreenShot = false;
		ScreenShot::takeScreenShot();
	}

	//UI
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	drawUI();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

///////////////////////////////////////////////////////////////////// SKYBOX

void createSkyBox()
{
	std::vector<std::string> faces
	{
		"Models/Skybox/tropical/right.jpg",
		"Models/Skybox/tropical/left.jpg",
		"Models/Skybox/tropical/top.jpg",
		"Models/Skybox/tropical/bottom.jpg",
		"Models/Skybox/tropical/front.jpg",
		"Models/Skybox/tropical/back.jpg"
	};
	cubeMap = new CubeMap(faces);
}

void createSkyBoxTransparent()
{
	std::vector<std::string> faces
	{
		"Models/Skybox/right_transparent.png",
		"Models/Skybox/left_transparent.png",
		"Models/Skybox/top_transparent.png",
		"Models/Skybox/bottom_transparent.png",
		"Models/Skybox/front_transparent.png",
		"Models/Skybox/back_transparent.png"
	};
	cubeMapSun = new CubeMap(faces);
}

///////////////////////////////////////////////////////////////////// CALLBACKS

void close_callback(GLFWwindow* win)
{
	destroyShaderPrograms();
}

void resize_callback(GLFWwindow* win, int winx, int winy)
{
	glViewport(0, 0, winx, winy);
	persp.aspect = (float)winx / winy;
	ortho = OrthoProjection(-(winx / (float)winy), (winx / (float)winy), -1, 1, 0, 100);
	defRenderer->resize(winx, winy);
	ssaoRenderer->resize(winx, winy);
	pps->resize(winx, winy);
}

////////////////////////////////////////////////////////////////////////// RUN

void update(double seconds) {
	FPS = 1 / seconds;
	if (focused)
		cam->update(seconds);
	if (Input::GetKeyDown(GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(engine->win, GL_TRUE);
	}
	if (Input::GetKeyDown(GLFW_KEY_LEFT_SHIFT)) {
		focused = !focused;
		glfwSetInputMode(engine->win, GLFW_CURSOR, focused ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
		glfwSetInputMode(engine->win, GLFW_CURSOR, focused ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	}
	light->position = sunRadius * vec3(vec3(cos(deg2rad(sunRotation.y)) * cos(deg2rad(sunRotation.x)), sin(deg2rad(sunRotation.y)), cos(deg2rad(sunRotation.y)) * sin(deg2rad(sunRotation.x))));
	light->updateCamera();
	skyboxSun->transform.rotation = quaternion::fromAxisAngle(-sunRotation.x - 90, AXIS3_Y) * quaternion::fromAxisAngle(sunRotation.y + 180, AXIS3_X);

	grassDrawable->update(seconds);
	grassDrawable->windStrength = windStrengthGrass;
	for (auto leaf : leafDrawables) {
		leaf->update(seconds);
		leaf->windStrength = windStrengthLeaves;
	}
}

////////////////////////////////////////////////////////////////////////// MAIN

int main(int argc, char* argv[])
{
	int gl_major = 4, gl_minor = 3;
	int is_fullscreen = 0;
	int is_vsync = 0;
	cam = new FlyCamera(vec3(-0.960214f, 1.31593f, 2.90204f), vec2(-0.75256f, 0.139469f), &persp, 2, 0.1f);
	engine = new Engine(gl_major, gl_minor, width, height, "WASD ctrl space - movement, Left Shift - toggle mouse focus, Esc - close application", is_fullscreen, is_vsync);
	engine->setDisplayFn(drawScene);
	engine->setUpdateFn(update);
	engine->setWindowCloseCallback(close_callback);
	engine->setWindowSizeCallback(resize_callback);
	createShaderPrograms();

	Input::Init(engine);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui_ImplGlfw_InitForOpenGL(engine->win, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	ImGui::StyleColorsDark();

	defRenderer = new DeferredRenderer(width, height);
	ssaoRenderer = new SSAORenderer(width, height, defRenderer);

	pps = new PostProcessingStack(width, height);

	bloomPass = new BloomPass(width, height);
	hdrPass = new HDRToneMappingPass(engine, width, height);
	dofPass = new DOFPass(defRenderer);
	ditherPass = new DitherPass();
	fxaaPass = new FXAAPass();
	pps->passes.push_back(fxaaPass);
	pps->passes.push_back(dofPass);
	pps->passes.push_back(bloomPass);
	pps->passes.push_back(hdrPass);
	pps->passes.push_back(ditherPass);

	resize_callback(engine->win, width, height);

	shadowBuffer = new ShadowBuffer();
	light = new Light(vec3(0, 4, 0) + 5 * vec3((float)cos(0), 0, (float)sin(0)), vec3(0, 0, 0));
	light->startCamera();
	light->updateCamera();

	createSkyBox();
	createSkyBoxTransparent();

	Mesh* sphereMesh = Mesh::fromObj("Models/sphere.obj");
	Mesh* cubeMesh = Mesh::fromObj("Models/cube.obj");
	Mesh* grassMesh = Mesh::fromObj("Models/floatislandtop.obj");
	Mesh* floorMesh = Mesh::fromObj("Models/floatisland.obj");
	Mesh* treeMesh = Mesh::fromObj("Models/treestump.obj");

	scene.cam = cam;

	SceneNode* tree = scene.createNode(new ShadowDrawable(treeMesh, vec3(0.46f, 0.36f, 0.28f), light));
	tree->transform.scale = vec3(0.3f);

	vector<vec3> verts = {
		{-0.5f, 0, 0}, {0.5f, 0, 0}, {0.5f, 1, 0},
		{-0.5f, 0, 0}, {0.5f, 1, 0}, {-0.5f, 1, 0},
		{-0.5f, 0, 0}, {0.5f, 1, 0}, {0.5f, 0, 0},
		{-0.5f, 0, 0}, {-0.5f, 1, 0}, {0.5f, 1, 0},
	};
	vector<vec2> texcoords = {
		{0, 0}, {1, 0}, {1, 1},
		{0, 0}, {1, 1}, {0, 1},
		{0, 0}, {1, 1}, {1, 0},
		{0, 0}, {0, 1}, {1, 1}
	};
	vector<vec3> normals = {
		{0, 0, 1}, {0, 0, 1}, {0, 0, 1},
		{0, 0, 1}, {0, 0, 1}, {0, 0, 1},
		{0, 0, -1}, {0, 0, -1}, {0, 0, -1},
		{0, 0, -1}, {0, 0, -1}, {0, 0, -1},
	};

	Texture* leafTex = new Texture("Models/leaf.png");
	for (int i = 0; i < 6; i++)
	{
		ostringstream ss;
		ss << "Models/Leaves/hightreeleaf" << i + 1 << ".obj";
		Mesh* leafMesh = Mesh::fromObj(ss.str());
		leafMeshes.push_back(leafMesh);
		GrassDrawable* leaf = new GrassDrawable(leafMeshes[i], vec3(0.4f), vec3(), light, verts, texcoords, normals, true, leafTex->id);
		leaf->tesselation = leafTesselation[i];
		leafDrawables.push_back(leaf);
		SceneNode* leafNode = scene.createNode(leafDrawables[i]);
		leafNode->transform.scale = vec3(0.3f);
		leafNodes.push_back(leafNode);
	}

	Texture *floorTex = new Texture("Models/islandtext.png", GL_NEAREST);
	SceneNode* floor = scene.createNode(new ShadowDrawable(floorMesh, vec3(0.1f, 0.79f, 0.06f), light, true, floorTex->id));
	floor->transform.scale = vec3(1);
	floor->transform.position = vec3(0, -0.45f, 0);

	verts = {
		{-0.5f, 0, -0.5f}, {0, 1, 0}, {0.5f, 0, -0.5f},
		{0.5f, 0, -0.5f}, {0, 1, 0}, {0.5f, 0, 0.5f},
		{0.5f, 0, 0.5f},  {0, 1, 0}, {-0.5f, 0, 0.5f},
		{-0.5f, 0, 0.5f}, {0, 1, 0}, {-0.5f, 0, -0.5f}
	};
	texcoords = {
		{0, 0}, {1, 0}, {1, 1},
		{1, 1}, {0, 1}, {0, 0},
		{1, 0}, {0, 0}, {1, 1},
		{0, 1}, {1, 1}, {0, 0}
	};
	normals = {
		{0, 0.45f, -0.89f}, {0, 0.45f, -0.89f}, {0, 0.45f, -0.89f},
		{0.89f, 0.45f, 0}, {0.89f, 0.45f, 0}, {0.89f, 0.45f, 0},
		{0, 0.45f, 0.89f}, {0, 0.45f, 0.89f}, {0, 0.45f, 0.89f},
		{-0.89f, 0.45f, 0}, {-0.89f, 0.45f, 0}, {-0.89f, 0.45f, 0},
	};

	grassDrawable = new GrassDrawable(grassMesh, vec3(0.05f, 0.75f, 0.15f), vec3(0.25f, 1, 0.25f), light, verts, texcoords, normals);
	SceneNode* grass = scene.createNode(grassDrawable);
	grass->transform.scale = vec3(0.95f);
	grass->transform.position = vec3(0, -0.45f, 0);

	SceneNode* skybox = scene.createNode(new SkyBoxDrawable(cubeMesh, cubeMap, 1.2f), { vec3(), vec3(100), quaternion() });
	skyboxSun = scene.createNode(new SkyBoxDrawable(cubeMesh, cubeMapSun, 10), { vec3(), vec3(99), quaternion() });

	glfwSetInputMode(engine->win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	engine->run();

	delete defRenderer;
	delete ssaoRenderer;

	delete pps;

	delete hdrPass;
	delete bloomPass;
	delete dofPass;
	delete ditherPass;
	delete fxaaPass;

	exit(EXIT_SUCCESS);
}

/////////////////////////////////////////////////////////////////////////// END