#include "CQuad.h"
// Example 4 �}�l
// ���P Example 3 �¤W(Y�b)
// �C�@�� Vertex �W�[ Normal �A�令�~���� CShape�A�@�ֳB�z�������]�w�ݨD


CQuad::CQuad()
{
	_iNumVtx = QUAD_NUM;
	_pPoints = NULL; _pNormals = NULL; _pTex = NULL;

	_pPoints  = new vec4[_iNumVtx];
	_pNormals = new vec3[_iNumVtx];

	_pPoints[0] = vec4( -0.5f, 0.0f,  0.5f, 1.0f);
	_pPoints[1] = vec4(  0.5f, 0.0f,  0.5f, 1.0f);
	_pPoints[2] = vec4(  0.5f, 0.0f, -0.5f, 1.0f);
	_pPoints[3] = vec4( -0.5f, 0.0f,  0.5f, 1.0f);
	_pPoints[4] = vec4(  0.5f, 0.0f, -0.5f, 1.0f);
	_pPoints[5] = vec4( -0.5f, 0.0f, -0.5f, 1.0f);

	_pNormals[0] = vec3(  0, 1.0f, 0);  // Normal Vector �� W �� 0
	_pNormals[1] = vec3(  0, 1.0f, 0);
	_pNormals[2] = vec3(  0, 1.0f, 0);
	_pNormals[3] = vec3(  0, 1.0f, 0);
	_pNormals[4] = vec3(  0, 1.0f, 0);
	_pNormals[5] = vec3(  0, 1.0f, 0);

	// Set shader's name
	setShaderName("vsVtxNormal.glsl", "fsVtxNormal.glsl");

	// Create and initialize a buffer object 
	createBufferObject();

	// �w�]�� -1.0f, �� Normal �M�w�C��
	_fColor[0] = -1.0f; _fColor[1] = -1.0f; _fColor[2] = -1.0f; _fColor[3] = 1;
}

void CQuad::update(float dt)
{
}

void CQuad::draw()
{
	drawingSetShader();
	glDrawArrays( GL_TRIANGLES, 0, QUAD_NUM );
}

void CQuad::drawW()
{
	drawingWithoutSetShader();
	glDrawArrays( GL_TRIANGLES, 0, QUAD_NUM );
}
