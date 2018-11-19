#include "pch.h"
#include "Renderer.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {

	camera = new Camera();
	//light = new Light(Vector3(RAW_WIDTH * HEIGHTMAP_X / 2.0f, 200.0f, RAW_WIDTH * HEIGHTMAP_X / 2.0f), Vector4(1, 1, 1, 1), 5500.0f);
	light = new Light(Vector3(0, 2000.0f, 0), Vector4(1, 1, 1, 1), 55000.0f);
	
	quad = Mesh::GenerateQuad();

	camera->SetPosition(Vector3(RAW_WIDTH * HEIGHTMAP_X / 2.0f, 550.0f, RAW_WIDTH * HEIGHTMAP_X / 2.0f));


	hellData = new MD5FileData(MESHDIR"hellknight.md5mesh");
	hellNode = new MD5Node(*hellData);
	hellNode->SetTransform(Matrix4::Translation(Vector3((RAW_WIDTH * HEIGHTMAP_X / 2.0f), 350.0f, (RAW_WIDTH * HEIGHTMAP_X / 2.0f))));
	hellNode->SetModelScale(Vector3(15.0f, 5.0f, 5.0f));

	//light = new Light((RAW_WIDTH * HEIGHTMAP_X / 2.0f), 0.0f, (RAW_WIDTH * HEIGHTMAP_X / 2.0f)-2000.0f)
	//light->SetPosition(Vector3(light->GetPosition().x, 500.0f, light->GetPosition().z));
	//light = new Light(Vector3(-450.0f, 200.0f, 280.0f), Vector4(1, 1, 1, 1), 5500.0f);

	reflectShader = new Shader(SHADERDIR "PerPixelVertex.glsl", SHADERDIR "ReflectFragment.glsl");
	skyboxShader = new Shader(SHADERDIR "SkyboxVertex.glsl", SHADERDIR "SkyboxFragment.glsl");
	lightShader = new Shader(SHADERDIR "PerPixelVertex.glsl", SHADERDIR "PerPixelFragment.glsl");

#ifdef MD5_USE_HARDWARE_SKINNING
	hellNodeShader = new Shader(SHADERDIR"skeletonVertexSimple.glsl", SHADERDIR"TexturedFragment.glsl");
#else
	hellNodeShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
#endif


	sceneShader = new Shader(SHADERDIR "ShadowSceneVertex.glsl",
		SHADERDIR "ShadowSceneFragment.glsl");
	shadowShader = new Shader(SHADERDIR "ShadowVertex.glsl",
		SHADERDIR "ShadowFragment.glsl");
	

	if (!sceneShader->LinkProgram() || !shadowShader->LinkProgram() || !reflectShader->LinkProgram() || !lightShader->LinkProgram() || !skyboxShader->LinkProgram() || !hellNodeShader->LinkProgram()) {
		return;
	}

	hellData->AddAnim(MESHDIR"walk7.md5anim");
	//hellData->AddAnim(MESHDIR"attack2.md5anim");
	hellData->AddAnim(MESHDIR"idle2.md5anim");

	hellNode->PlayAnim(MESHDIR"idle2.md5anim");

	quad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR "water.TGA", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	SetTextureRepeating(quad->GetTexture(), true);
	


	/*cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR "hw_crater/craterlake_ft.tga", TEXTUREDIR "hw_crater/craterlake_bk.tga",
		TEXTUREDIR "hw_crater/craterlake_up.tga", TEXTUREDIR "hw_crater/craterlake_dn.tga",
		TEXTUREDIR "hw_crater/craterlake_rt.tga", TEXTUREDIR "hw_crater/craterlake_lf.tga",
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID, 0
	);*/

	cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR "sb_frozen/ft.tga", TEXTUREDIR "sb_frozen/bk.tga",
		TEXTUREDIR "sb_frozen/up.tga", TEXTUREDIR "sb_frozen/dn.tga",
		TEXTUREDIR "sb_frozen/rt.tga", TEXTUREDIR "sb_frozen/lf.tga",
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID, 0
	);


	

	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &shadowFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	heightMap = new HeightMap(TEXTUREDIR "terrain4.raw");
	heightMap->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR "Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR "Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	if (!cubeMap || !quad->GetTexture() || !heightMap->GetTexture() || !heightMap->GetBumpMap()) {
		return;
	}

	SetTextureRepeating(heightMap->GetTexture(), true);
	SetTextureRepeating(heightMap->GetBumpMap(), true);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	init = true;
	waterRotate = 0.0f;

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

Renderer ::~Renderer(void) {
	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);
	delete camera;

	delete hellData;
	delete hellNode;

	delete heightMap;
	delete quad;
	delete reflectShader;
	delete skyboxShader;
	delete lightShader;
	delete hellNodeShader;
	delete sceneShader;
	delete shadowShader;
	delete light;
	currentShader = NULL;
}

void Renderer::UpdateScene(float msec) {
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_UP))
	{
		Matrix4 newPosition = hellNode->GetTransform();
		newPosition.values[12] += -1.0f;

		hellNode->SetTransform(newPosition);
		UpdateShaderMatrices();
		hellNode->PlayAnim(MESHDIR"walk7.md5anim");
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_LEFT))
	{
		Matrix4 newPosition = hellNode->GetTransform();
		newPosition.values[14] += 1.0f;

		hellNode->SetTransform(newPosition);
		UpdateShaderMatrices();
		hellNode->PlayAnim("walk7.md5anim");
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT))
	{
		Matrix4 newPosition = hellNode->GetTransform();
		newPosition.values[14] -= 1.0f;

		hellNode->SetTransform(newPosition);
		UpdateShaderMatrices();
		hellNode->PlayAnim("walk7.md5anim");
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_DOWN))
	{
		Matrix4 newPosition = hellNode->GetTransform();
		newPosition.values[12] += 1.0f;

		hellNode->SetTransform(newPosition);
		UpdateShaderMatrices();
		hellNode->PlayAnim("walk7.md5anim");
	}
	//hellNode->PlayAnim(MESHDIR"idle2.md5anim");
	camera->UpdateCamera(msec);
	//viewMatrix = camera->BuildViewMatrix();
	hellNode->Update(msec);
	//cout << hellNode->GetModelScale() << endl;
	waterRotate += msec / 1000.0f;
}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	
	//DrawHeightmap();
	//DrawWater();
	//DrawHellNode();
	DrawSkybox();
	//DrawHeightmap();
	//DrawHellNode();
	DrawShadowScene();
	DrawCombinedScene();

	SwapBuffers();

}

void Renderer::DrawShadowScene() {


	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

	glClear(GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	SetCurrentShader(shadowShader);

	viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(), Vector3(RAW_WIDTH * HEIGHTMAP_X, 0, RAW_WIDTH * HEIGHTMAP_Z));
	shadowMatrix = biasMatrix * (projMatrix * viewMatrix);

	UpdateShaderMatrices();

	//DrawSkybox();
	DrawHeightmap();
	SetCurrentShader(sceneShader);
	DrawHellNode();
	DrawWater();
	SetCurrentShader(sceneShader);
	//DrawMesh();

	glUseProgram(0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Renderer::DrawCombinedScene() {
	SetCurrentShader(sceneShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowTex"), 2);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float *)& camera->GetPosition());

	SetShaderLight(*light);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	viewMatrix = camera->BuildViewMatrix();
	UpdateShaderMatrices();

	
	DrawHellNode();
	SetCurrentShader(sceneShader);
	DrawHeightmap();
	DrawWater();
	SetCurrentShader(sceneShader);
	//DrawMesh();

	glUseProgram(0);

}

void Renderer::DrawHellNode()
{
	SetCurrentShader(hellNodeShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	modelMatrix = hellNode->GetTransform();
	UpdateShaderMatrices();
	hellNode->Draw(*this);

	glUseProgram(0);
}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);

	UpdateShaderMatrices();

	//glActiveTexture(GL_TEXTURE2);
	//glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 2);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	quad->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);

}

void Renderer::DrawHeightmap() {
	//SetShaderLight(*light);

	//glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());

	//glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

	//glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);

	modelMatrix = Matrix4::Rotation(90, Vector3(1, 0, 0)) * Matrix4::Scale(Vector3(450, 450, 1));
	Matrix4 tempMatrix = shadowMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "shadowMatrix"), 1, false, *& tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *& modelMatrix.values);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	UpdateShaderMatrices();

	heightMap->Draw();

	glUseProgram(0);

}

void Renderer::DrawWater() {

	SetCurrentShader(reflectShader);
	SetShaderLight(*light);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float *)& camera->GetPosition());

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 2);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	float heightX = (RAW_WIDTH * HEIGHTMAP_X / 2.0f);

	float heightY = 256 * HEIGHTMAP_Y / 3.0f;

	float heightZ = (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f);

	modelMatrix = Matrix4::Translation(Vector3(heightX, heightY, heightZ)) * Matrix4::Scale(Vector3(heightX, 1, heightZ)) * Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));

	textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) * Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f));

	UpdateShaderMatrices();

	quad->Draw();

	glUseProgram(0);

}



