#pragma once
#include "OGLRenderer.h"

enum MeshBuffer {
	VERTEX_BUFFER, COLOUR_BUFFER, TEXTURE_BUFFER,
	NORMAL_BUFFER, TANGENT_BUFFER, INDEX_BUFFER,
	MAX_BUFFER
};

class Mesh {
public:
	Mesh(void);
	~Mesh(void);

	virtual void Draw();
	static Mesh* GenerateTriangle();
	static Mesh* GenerateHouse();

	static Mesh* GenerateScreenQuad(float startX, float startY, float endX, float endY) {
		Mesh* m = new Mesh();
		m->numVertices = 4;
		m->type = GL_TRIANGLE_STRIP;

		m->vertices = new Vector3[m->numVertices];
		m->vertices[0] = Vector3(startX, startY, 0.0f);
		m->vertices[1] = Vector3(startX, endY, 0.0f);
		m->vertices[2] = Vector3(endX, startY, 0.0f);
		m->vertices[3] = Vector3(endX, endY, 0.0f);

		m->textureCoords = new Vector2[m->numVertices];
		m->textureCoords[0] = Vector2(0.0f, 1.0f);
		m->textureCoords[1] = Vector2(0.0f, 0.0f);
		m->textureCoords[2] = Vector2(1.0f, 1.0f);
		m->textureCoords[3] = Vector2(1.0f, 0.0f);

		m->colours = new Vector4[m->numVertices];
		m->normals = new Vector3[m->numVertices];
		m->tangents = new Vector3[m->numVertices];

		for (int i = 0; i < m->numVertices; i++)
		{
			m->colours[i] = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
			m->normals[i] = Vector3(0.0f, 0.0f, -1.0f);
			m->tangents[i] = Vector3(1.0f, 0.0f, 0.0f);
		}

		m->BufferData();
		return m;
	}

	//Tutorial 3
	void SetTexture(GLuint tex) { texture = tex; }
	GLuint GetTexture() { return texture; }

	//Tutorial 4
	static Mesh* GenerateQuad();

	//Tutorial 12
	void SetBumpMap(GLuint tex) { bumpTexture = tex; }
	GLuint GetBumpMap() { return bumpTexture; }

protected:
	void BufferData();

	GLuint arrayObject;
	GLuint bufferObject[MAX_BUFFER];
	GLuint numVertices;
	GLuint type;

	Vector3* vertices;
	Vector4* colours;

	//Tutorial 3
	GLuint texture;
	Vector2* textureCoords;

	//Tutorial 8
	GLuint numIndices;
	unsigned int* indices;

	//Tutorial 11
	void GenerateNormals();
	Vector3* normals;

	//Tutorial 12
	void GenerateTangents();
	Vector3 GenerateTangent(const Vector3 &a, const Vector3 &b,
							const Vector3 &c, const Vector2 & ta,
							const Vector2 & tb, const Vector2 & tc);
	
	Vector3 * tangents;
	GLuint bumpTexture;

};
