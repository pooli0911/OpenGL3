#ifndef C2DSPRITE_H
#define C2DSPRITE_H

#include "../header/Angel.h"
#include "CShape.h"

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

#define QUAD_NUM 6		// 2 faces, 2 triangles/face 

class C2DSprite : public CShape
{
public:
	C2DSprite();
	~C2DSprite();

	vec4  _OriginalLL, _OriginalTR;  // sprite �b local �y�Ъ����U�P�k�W��ӳ��I�y��
	vec4  _LL, _TR;  // sprite �b�@�ɮy�Ъ����W�P�k�U��ӳ��I�y��
	vec4  _DefaultColor;
	bool  _bPushed;

	void setTRSMatrix(mat4 &mat);
	void update(float dt);
	void setDefaultColor(vec4 vColor);
	bool getButtonStatus(){ return _bPushed; }
	bool onTouches(const vec2 &tp);

	GLuint getShaderHandle() { return _uiProgram; }

	void draw();
	void drawW();
};




#endif