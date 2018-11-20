#include "pch.h"
#include "Renderer.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {

	camera = new Camera();
	//light = new Light(Vector3(RAW_WIDTH * HEIGHTMAP_X / 2.0f, 200.0f, RAW_WIDTH * HEIGHTMAP_X / 2.0f), Vector4(1, 1, 1, 1), 5500.0f);
	//light = new Light(Vector3(-2000, 5000.0f, 0), Vector4(1, 1, 1, 1), 55000.0f);
	light = new Light(Vector3(RAW_WIDTH * HEIGHTMAP_X / 2.0f, 4000.0f, RAW_WIDTH * HEIGHTMAP_X / 2.0f), Vector4(1, 1, 1, 1), 55000.0f);
	
	quad = Mesh::GenerateQuad();
	quad2 = Mesh::GenerateQuad();

	sphere = new OBJMesh();

	if(!sphere->LoadOBJMesh(SUNDIR "sphere.obj")) return;
	sphere->SetTexture(SOIL_load_OGL_texture("../Assets/sol/2k_sun.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	
	root = new SceneNode();
	//root->SetMesh(sphere);
	//root->SetTransform(Matrix4::Translation(Vector3(RAW_WIDTH * HEIGHTMAP_X / 2.0f, 0.0f, RAW_WIDTH * HEIGHTMAP_X / 2.0f)));
	//root->SetModelScale(Vector3(0.01f, 0.01f, 0.01f));
	//root->SetBoundingRadius(10000.0f);
	
	sun = new SceneNode(sphere);
	sun->SetMesh(sphere);
	sun->SetTransform(Matrix4::Translation(Vector3(-10000.0f, 10000.0f, 0.0f)));
	sun->SetModelScale(Vector3(50.0f, 50.0f, 50.0f));
	sun->SetBoundingRadius(10000.0f);
	
	root->AddChild(sun);


	//BuildNodeLists(root);

	camera->SetPosition(Vector3(RAW_WIDTH * HEIGHTMAP_X / 2.0f, 550.0f, RAW_WIDTH * HEIGHTMAP_X / 2.0f));


	hellData = new MD5FileData(MESHDIR"hellknight.md5mesh");
	hellNode = new MD5Node(*hellData);
	hellNode->SetTransform(Matrix4::Translation(Vector3((RAW_WIDTH * HEIGHTMAP_X / 2.0f), 350.0f, (RAW_WIDTH * HEIGHTMAP_X / 2.0f))));
	hellNode->SetModelScale(Vector3(3.0f, 3.0f, 3.0f));

	//light = new Light((RAW_WIDTH * HEIGHTMAP_X / 2.0f), 0.0f, (RAW_WIDTH * HEIGHTMAP_X / 2.0f)-2000.0f)
	//light->SetPosition(Vector3(light->GetPosition().x, 500.0f, light->GetPosition().z));
	//light = new Light(Vector3(-450.0f, 200.0f, 280.0f), Vector4(1, 1, 1, 1), 5500.0f);
	nodeShader = new Shader(SHADERDIR "SceneVertex.glsl", SHADERDIR "SceneFragment.glsl");
	reflectShader = new Shader(SHADERDIR "PerPixelVertex.glsl", SHADERDIR "ReflectFragment.glsl");
	skyboxShader = new Shader(SHADERDIR "SkyboxVertex.glsl", SHADERDIR "SkyboxFragment.glsl");
	lightShader = new Shader(SHADERDIR "PerPixelVertex.glsl", SHADERDIR "PerPixelFragment.glsl");

#ifdef MD5_USE_HARDWARE_SKINNING
	hellNodeShader = new Shader(SHADERDIR"skeletonVertexSimple.glsl", SHADERDIR"TexturedFragment.glsl");
#else
	hellNodeShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
#endif


	sceneShader = new Shader(SHADERDIR "ShadowSceneVertex.glsl", SHADERDIR "ShadowSceneFragment.glsl");
	shadowShader = new Shader(SHADERDIR "ShadowVertex.glsl", SHADERDIR "ShadowFragment.glsl");
	

	if (!nodeShader->LinkProgram() || !sceneShader->LinkProgram() || !shadowShader->LinkProgram() || !reflectShader->LinkProgram() || !lightShader->LinkProgram() || !skyboxShader->LinkProgram() || !hellNodeShader->LinkProgram()) {
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
		TEXTUREDIR "mp_ayden/ayden_ft.tga", TEXTUREDIR "mp_ayden/ayden_bk.tga",
		TEXTUREDIR "mp_ayden/ayden_up.tga", TEXTUREDIR "mp_ayden/ayden_dn.tga",
		TEXTUREDIR "mp_ayden/ayden_rt.tga", TEXTUREDIR "mp_ayden/ayden_lf.tga",
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID, 0
	);

	cubeMaps.push_back(cubeMap);

	cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR "sor_sea/sea_ft.jpg", TEXTUREDIR "sor_sea/sea_bk.jpg",
		TEXTUREDIR "sor_sea/sea_up.jpg", TEXTUREDIR "sor_sea/sea_dn.jpg",
		TEXTUREDIR "sor_sea/sea_rt.jpg", TEXTUREDIR "sor_sea/sea_lf.jpg",
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID, 0
	);

	cubeMaps.push_back(cubeMap);

	

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

	//Scene 0 heightmap.
	heightMap = new HeightMap(TEXTUREDIR "terrain5.raw", 0);
	heightMap->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR "Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR "Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMaps.push_back(heightMap);
	SetTextureRepeating(heightMap->GetTexture(), true);
	SetTextureRepeating(heightMap->GetBumpMap(), true);

	//Scene 1 heightMap.
	heightMap = new HeightMap(TEXTUREDIR "terrain6.raw", 1);
	heightMap->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR "Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR "Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	SetTextureRepeating(heightMap->GetTexture(), true);
	SetTextureRepeating(heightMap->GetBumpMap(), true);
	heightMaps.push_back(heightMap);
	
	if (!cubeMaps[0] || !cubeMaps[1] || !quad->GetTexture() || !heightMaps[0]->GetTexture() || !heightMaps[0]->GetBumpMap() || !heightMaps[1]->GetTexture() || !heightMaps[1]->GetBumpMap()) {
		return;
	}


	root2 = new SceneNode();
	ico = new OBJMesh();

	if (!ico->LoadOBJMesh(SUNDIR "sphere.obj")) return;
	ico->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR "whiteTex.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	//ico->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR "Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	SetTextureRepeating(ico->GetTexture(), true);
	//SetTextureRepeating(ico->GetBumpMap(), true);
	for (int x = 0; x < 7; x++)
	{
		for (int z = 0; z < 7; z++)
		{
			SceneNode* ico2 = new SceneNode(ico);
			ico2->SetModelScale(Vector3(15.0f, 15.0f, 15.0f));
			ico2->SetBoundingRadius(100000.0f);
			ico2->SetTransform(Matrix4::Translation(Vector3(500.0f * (x+1), 150.0f, 500.0f * (z+1))));
			root2->AddChild(ico2);
		}
	}

	// Need to make an empty constructor for the Light class ...
	pointLights = new Light[LIGHTNUM * LIGHTNUM];
	for (int x = 0; x < LIGHTNUM; ++x) {
		for (int z = 0; z < LIGHTNUM; ++z) {
			Light & l = pointLights[(x * LIGHTNUM) + z];

			float xPos = (RAW_WIDTH * HEIGHTMAP_X / (LIGHTNUM - 1)) * x;
			float zPos = (RAW_HEIGHT * HEIGHTMAP_Z / (LIGHTNUM - 1)) * z;
			l.SetPosition(Vector3(xPos, 100.0f, zPos));

			float r = 0.5f + (float)(rand() % 129) / 128.0f;
			float g = 0.5f + (float)(rand() % 129) / 128.0f;
			float b = 0.5f + (float)(rand() % 129) / 128.0f;
			l.SetColour(Vector4(r, g, b, 1.0f));

			float radius = (RAW_WIDTH * HEIGHTMAP_X / LIGHTNUM);
			l.SetRadius(radius);

		}

	}

	sphere2 = new OBJMesh();
	if (!sphere2->LoadOBJMesh(MESHDIR "ico.obj")) {
		return;
	}

	sceneShader2 = new Shader(SHADERDIR "BumpVertex.glsl", SHADERDIR "BufferFragment.glsl");
	if (!sceneShader2->LinkProgram()) { return; }

	combineShader = new Shader(SHADERDIR "CombineVertex.glsl", SHADERDIR "CombineFragment.glsl");
	if (!combineShader->LinkProgram()) {
		return;
	}

	pointlightShader = new Shader(SHADERDIR "PointLightVertex.glsl", SHADERDIR "PointLightFragment.glsl");
	if (!pointlightShader->LinkProgram()) {
		return;
	}

	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &pointLightFBO);



	GLenum buffers[2];
	buffers[0] = GL_COLOR_ATTACHMENT0;
	buffers[1] = GL_COLOR_ATTACHMENT1;

	// Generate our scene depth texture ...
	GenerateScreenTexture(bufferDepthTex, true);
	GenerateScreenTexture(bufferColourTex);
	GenerateScreenTexture(bufferNormalTex);
	GenerateScreenTexture(lightEmissiveTex);
	GenerateScreenTexture(lightSpecularTex);

	// And now attach them to our FBOs
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;

	}

	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightEmissiveTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lightSpecularTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);



	init = true;
	waterRotate = 0.0f;

	projMatrix = Matrix4::Perspective(1.0f, 150000000.0f, (float)width / (float)height, 45.0f);

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

	delete sphere;
	delete root;
	delete nodeShader;

	delete root2;
	delete sphere2;
	delete sceneShader2;
	delete combineShader;
	delete pointlightShader;
	delete quad2;

	delete[] pointLights;

	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &lightEmissiveTex);
	glDeleteTextures(1, &lightSpecularTex);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &pointLightFBO);
	

	currentShader = NULL;
}

void Renderer::UpdateScene(float msec) {
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_NUMPAD8))
	{
		Matrix4 newPosition = hellNode->GetTransform();
		newPosition.values[12] += -5.0f;
		
		hellNode->SetTransform(newPosition);
		UpdateShaderMatrices();
		hellNode->PlayAnim(MESHDIR"walk7.md5anim");
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_NUMPAD2))
	{
		Matrix4 newPosition = hellNode->GetTransform();
		newPosition.values[12] += 5.0f;

		hellNode->SetTransform(newPosition);
		UpdateShaderMatrices();
		hellNode->PlayAnim(MESHDIR"walk7.md5anim");
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_NUMPAD4))
	{
		Matrix4 newRotation = Matrix4::Rotation(10.0f, Vector3(0.0f, 1.0f, 0.0f));
		Matrix4 temp = hellNode->GetTransform() * Matrix4::Rotation(10.0f, Vector3(0.0f, 1.0f, 0.0f));;

		hellNode->SetTransform(temp);
		UpdateShaderMatrices();
		hellNode->PlayAnim(MESHDIR"idle2.md5anim");
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_NUMPAD6))
	{
		Matrix4 newRotation = Matrix4::Rotation(10.0f, Vector3(0.0f, 1.0f, 0.0f));
		Matrix4 temp = hellNode->GetTransform() * Matrix4::Rotation(-10.0f, Vector3(0.0f, 1.0f, 0.0f));;
		hellNode->SetTransform(temp);
		UpdateShaderMatrices();
		hellNode->PlayAnim(MESHDIR"idle2.md5anim");
	}

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_RIGHT) && sceneNumber < 1)
	{
		sceneNumber += 1;
		cout << "sceneNumber is: " << sceneNumber << endl;
	}

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_LEFT) && sceneNumber > 0)
	{
		sceneNumber -= 1;
		cout << "sceneNumber is: " << sceneNumber << endl;
	}
	
	if (sceneNumber == 0)
	{
		hellNode->Update(msec);
		sun->SetTransform(root->GetWorldTransform() * Matrix4::Rotation(msec/100.0f, Vector3(0, 1, 0)) * sun->GetTransform());
		
		light = new Light(Vector3(sun->GetWorldTransform().GetPositionVector().x, 4000.0f, sun->GetWorldTransform().GetPositionVector().z), Vector4(1, 1, 1, 1), 55000.0f);
		root->Update(msec);
		waterRotate += msec / 1000.0f;

	}
	if (sceneNumber == 1)
	{
		root2->Update(msec);
		rotation = msec * 0.01f;
	}
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	//frameFrustum.FromMatrix(projMatrix * viewMatrix);
	


}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	projMatrix = Matrix4::Perspective(1.0f, 150000000.0f, (float)width / (float)height, 45.0f);

	if (sceneNumber == 0)
	{
		DrawSkybox();
		BuildNodeLists(root);
		SortNodeLists();
		
		SetCurrentShader(nodeShader);
		UpdateShaderMatrices();
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
		glDepthMask(GL_FALSE);

		DrawNodes();
		glDepthMask(GL_TRUE);

		DrawShadowScene();
		DrawCombinedScene();

		ClearNodeLists();
	}
	else if (sceneNumber == 1)
	{
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		BuildNodeLists(root2);
		SortNodeLists();

		SetCurrentShader(nodeShader);
		UpdateShaderMatrices();
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
		//glDepthMask(GL_FALSE);
		
		//glDepthMask(GL_TRUE);
		
		FillBuffers();
		DrawPointLights();
		CombineBuffers();
		
		ClearNodeLists();
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
	}
	SwapBuffers();


}

void Renderer::ClearNodeLists() {
	//transparentNodeList.clear();
	nodeList.clear();
}

void Renderer::BuildNodeLists(SceneNode* from) {
	if (frameFrustum.InsideFrustum(*from)) {
		Vector3 dir = from->GetWorldTransform().GetPositionVector() -
			camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));

		/*if (from->GetColour().w < 1.0f) {
			transparentNodeList.push_back(from);
		}
		else {*/
			nodeList.push_back(from);
		//}
	}

	for (vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i) {
		BuildNodeLists((*i));
	}

}

void Renderer::SortNodeLists() {
	//std::sort(transparentNodeList.begin(), transparentNodeList.end(), SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);
}

void Renderer::DrawNodes() {
	
	for (vector<SceneNode*>::const_iterator i = nodeList.begin(); i != nodeList.end(); ++i) {
		DrawNode((*i));
	}
	/*for (vector<SceneNode*>::const_reverse_iterator i = transparentNodeList.rbegin(); i != transparentNodeList.rend(); ++i) {
		DrawNode((*i));
	}*/
}

void Renderer::DrawNode(SceneNode* n) {
	if (n->GetMesh()) {
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)&(n->GetWorldTransform()* Matrix4::Scale(n->GetModelScale())));
		glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "nodeColour"), 1, (float*)& n->GetColour());
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"), (int)n->GetMesh()->GetTexture());
		n->Draw(*this);
	}
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

	DrawHeightmap();
	SetCurrentShader(sceneShader);
	if (sceneNumber == 0)
	{
		DrawHellNode();
		DrawWater();
	}
	SetCurrentShader(sceneShader);


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

	if (sceneNumber == 0)
	{
		DrawHellNode();
		SetCurrentShader(sceneShader);
	}
	DrawHeightmap();
	if (sceneNumber == 0) DrawWater();
	SetCurrentShader(sceneShader);

	glUseProgram(0);

}

void Renderer::DrawHellNode()
{
	SetCurrentShader(hellNodeShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	modelMatrix = hellNode->GetWorldTransform();
	UpdateShaderMatrices();
	hellNode->Draw(*this);

	glUseProgram(0);
}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	SetCurrentShader(skyboxShader);

	UpdateShaderMatrices();

	glActiveTexture(GL_TEXTURE2);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMaps[sceneNumber]);

	quad->Draw();

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glUseProgram(0);

}

void Renderer::DrawHeightmap() {
	modelMatrix = Matrix4::Rotation(90, Vector3(1, 0, 0)) * Matrix4::Scale(Vector3(450, 450, 1));
	Matrix4 tempMatrix = shadowMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "shadowMatrix"), 1, false, *& tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *& modelMatrix.values);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	UpdateShaderMatrices();

	heightMaps[sceneNumber]->Draw();

	glUseProgram(0);

}

void Renderer::DrawWater() {

	SetCurrentShader(reflectShader);
	SetShaderLight(*light);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float *)& camera->GetPosition());

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 2);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMaps[sceneNumber]);

	float heightX = (RAW_WIDTH * HEIGHTMAP_X / 2.0f);

	float heightY = 256 * HEIGHTMAP_Y / 3.0f;

	float heightZ = (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f);

	modelMatrix = Matrix4::Translation(Vector3(heightX, heightY, heightZ)) * Matrix4::Scale(Vector3(heightX, 1, heightZ)) * Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));

	textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) * Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f));

	UpdateShaderMatrices();

	quad->Draw();

	glUseProgram(0);

}

void Renderer::GenerateScreenTexture(GLuint & into, bool depth) {
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8, width, height, 0, depth ? GL_DEPTH_COMPONENT : GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);

}

void Renderer::FillBuffers() {
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	DrawSkybox();

	SetCurrentShader(sceneShader2);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	modelMatrix.ToIdentity();
	UpdateShaderMatrices();

	heightMaps[1]->Draw();
	DrawNodes();

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Renderer::DrawPointLights() {
	SetCurrentShader(pointlightShader);

	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glBlendFunc(GL_ONE, GL_ONE);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "depthTex"), 3);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "normTex"), 4);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float *)& camera->GetPosition());

	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);

	Vector3 translate = Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 500, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f));

	Matrix4 pushMatrix = Matrix4::Translation(translate);
	Matrix4 popMatrix = Matrix4::Translation(-translate);

	for (int x = 0; x < LIGHTNUM; ++x) {
		for (int z = 0; z < LIGHTNUM; ++z) {
			Light & l = pointLights[(x * LIGHTNUM) + z];
			float radius = l.GetRadius();

			modelMatrix = pushMatrix * Matrix4::Rotation(rotation, Vector3(0, 1, 0)) * popMatrix * Matrix4::Translation(l.GetPosition()) * Matrix4::Scale(Vector3(radius, radius, radius));
			//modelMatrix = Matrix4::Translation(Vector3(100.0 * x, 0, 100.0 * z));

			l.SetPosition(modelMatrix.GetPositionVector());

			SetShaderLight(l);

			UpdateShaderMatrices();

			float dist = (l.GetPosition() - camera->GetPosition()).Length();
			if (dist < radius) {// camera is inside the light volume !
				glCullFace(GL_FRONT);

			}
			else {
				glCullFace(GL_BACK);

			}

			sphere2->Draw();

		}

	}

	glCullFace(GL_BACK);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.2f, 0.2f, 0.2f, 1);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

}


void Renderer::CombineBuffers() {
	SetCurrentShader(combineShader);

	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 2);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "emissiveTex"), 3);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "specularTex"), 4);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "depthTex"), 5);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "normalTex"), 6);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, lightEmissiveTex);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

	quad2->Draw();

	glUseProgram(0);

}

