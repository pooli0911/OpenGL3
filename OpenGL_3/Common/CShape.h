#ifndef CSHAPE_H
#define CSHAPE_H
#include "../Header/Angel.h"

class CShape 
{
protected:
	// ���V�x�s�X���ƪ�����
	vec4 *_pPoints;
	vec3 *_pNormals;
	vec2 *_pTex;
	int  _iNumVtx;

	// �C�@�� Shape �ۤv���첾��m�A�Y��P���ਤ��

	GLfloat _fColor[4]; // Object's color
	// For shaders' name
	char *_pVXshader, *_pFSshader;

	// For VAO
	GLuint _uiVao;

	// For Shader
	GLuint  _uiModelView, _uiProjection, _uiColor;
	GLuint  _uiProgram ;
	GLuint  _uiBuffer;

	// For Matrices
	mat4    _mxView, _mxProjection, _mxTRS;
	mat4    _mxMVFinal;
	bool    _bProjUpdated, _bViewUpdated, _bTRSUpdated;
	bool    _bColorUpdated;

	void		createBufferObject();
	void		drawingSetShader();
	void		drawingWithoutSetShader();

public:
	CShape();
	virtual ~CShape();
	virtual void draw() = 0;
	virtual void drawW() = 0; // Drawing without setting shaders
	virtual void update(float dt) = 0;	// �������C�@�� frame ���i�H��X��ҫ����ܤ��e

	void setShaderName(const char vxShader[], const char fsShader[]);
	void setShader(mat4 &mxModelView, mat4 &mxProjection, GLuint uiShaderHandle=MAX_UNSIGNED_INT);
	void setShader(GLuint uiShaderHandle = MAX_UNSIGNED_INT);
	void setColor(vec4 vColor);
	void setViewMatrix(mat4 &mat);
	void setProjectionMatrix(mat4 &mat);
	void setTRSMatrix(mat4 &mat);
};

#endif
