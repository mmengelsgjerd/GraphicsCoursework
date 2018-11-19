#pragma once


#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/MD5Mesh.h"
#include "../nclgl/MD5Node.h"
#include "../nclgl/OBJMesh.h"
#include "../nclgl/Frustrum.h"
#include <algorithm>

#define SHADOWSIZE 2048 // New !

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

protected:
	void BuildNodeLists(SceneNode * from);
	void SortNodeLists();
	void DrawNode(SceneNode * n);
	void DrawNodes();
	void ClearNodeLists();

	void DrawHeightmap();
	void DrawWater();
	void DrawSkybox();
	void DrawHellNode();
	void DrawShadowScene();
	void DrawCombinedScene();

	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	Shader* hellNodeShader;
	Shader* sceneShader;
	Shader* shadowShader;
	Shader* nodeShader;



	OBJMesh* sphere;
	SceneNode* root;
	SceneNode* sun;

	GLuint shadowTex;
	GLuint shadowFBO;

	MD5FileData* hellData;
	MD5Node*	hellNode;

	HeightMap* heightMap;
	Mesh* quad;

	Frustum frameFrustum;
	vector<SceneNode*> nodeList;

	Light* light;
	Camera* camera;

	GLuint cubeMap;

	float waterRotate;


};
