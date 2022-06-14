#ifndef CSHAPE_H
#define CSHAPE_H
#include "../Header/Angel.h"

class CShape 
{
protected:
	// 指向儲存幾何資料的指標
	vec4 *_pPoints;
	vec3 *_pNormals;
	vec2 *_pTex;
	int  _iNumVtx;

	// 每一個 Shape 自己的位移位置，縮放與旋轉角度

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
	virtual void update(float dt) = 0;	// 提供讓每一個 frame 都可以對幾何模型改變內容

	void setShaderName(const char vxShader[], const char fsShader[]);
	void setShader(mat4 &mxModelView, mat4 &mxProjection, GLuint uiShaderHandle=MAX_UNSIGNED_INT);
	void setShader(GLuint uiShaderHandle = MAX_UNSIGNED_INT);
	void setColor(vec4 vColor);
	void setViewMatrix(mat4 &mat);
	void setProjectionMatrix(mat4 &mat);
	void setTRSMatrix(mat4 &mat);
};

#endif
