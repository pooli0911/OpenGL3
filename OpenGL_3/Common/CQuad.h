#ifndef CQUAD_H
#define CQUAD_H
#include "../header/Angel.h"
#include "CShape.h"

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

#define QUAD_NUM 6		// 2 faces, 2 triangles/face 

class CQuad : public CShape
{
private:

public:
	CQuad();
	~CQuad(){};
	void update(float dt);
	GLuint getShaderHandle() { return _uiProgram;} 

	void draw();
	void drawW();
};




#endif