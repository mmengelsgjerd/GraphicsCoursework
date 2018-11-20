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
#define LIGHTNUM 8 // We ’ll generate LIGHTNUM squared lights ...

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
	void DrawFloor();

	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	Shader* hellNodeShader;
	Shader* sceneShader;
	Shader* shadowShader;
	Shader* nodeShader;

	void FillBuffers(); //G- Buffer Fill Render Pass
	void DrawPointLights(); // Lighting Render Pass
	void CombineBuffers(); // Combination Render Pass
	// Make a new texture ...
	void GenerateScreenTexture(GLuint & into, bool depth = false);

	Shader* sceneShader2; // Shader to fill our GBuffers
	Shader* pointlightShader; // Shader to calculate lighting
	Shader* combineShader; // shader to stick it all together

	Light* pointLights; // Array of lighting data
	OBJMesh* sphere2; // Light volume

	float rotation; // How much to increase rotation by

	GLuint bufferFBO; // FBO for our G- Buffer pass
	GLuint bufferColourTex; // Albedo goes here
	GLuint bufferNormalTex; // Normals go here
	GLuint bufferDepthTex; // Depth goes here

	GLuint pointLightFBO; // FBO for our lighting pass
	GLuint lightEmissiveTex; // Store emissive lighting
	GLuint lightSpecularTex; // Store specular lighting



	OBJMesh* sphere;
	OBJMesh* ico;
	SceneNode* root;
	SceneNode* root2;
	SceneNode* sun;

	GLuint shadowTex;
	GLuint shadowFBO;

	MD5FileData* hellData;
	MD5Node*	hellNode;

	HeightMap* heightMap;
	Mesh* quad;
	Mesh* quad2;
	Mesh* floor;

	Frustum frameFrustum;
	vector<SceneNode*> nodeList;

	Light* light;
	Camera* camera;

	GLuint cubeMap;

	float waterRotate;

	int sceneNumber = 0;
	bool justChangedScene = false;
	vector<GLuint> cubeMaps;
	vector<HeightMap*> heightMaps;


};
