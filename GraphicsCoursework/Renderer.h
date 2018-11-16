#pragma once


#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/MD5Mesh.h"
#include "../nclgl/MD5Node.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

protected:
	void DrawHeightmap();
	void DrawWater();
	void DrawSkybox();
	void DrawHellNode();

	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	Shader* hellNodeShader;

	MD5FileData* hellData;
	MD5Node*	hellNode;

	HeightMap* heightMap;
	Mesh* quad;

	Light* light;
	Camera* camera;

	GLuint cubeMap;

	float waterRotate;};