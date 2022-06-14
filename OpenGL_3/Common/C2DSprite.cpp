#include "C2DSprite.h"

C2DSprite::C2DSprite()
{
	_iNumVtx = QUAD_NUM;
	_pPoints = NULL; _pNormals = NULL; _pTex = NULL;

	_pPoints = new vec4[_iNumVtx];
	_pNormals = new vec3[_iNumVtx];

	// ���B�令���������V�� Z ��V 
	_pPoints[0] = vec4(-0.5f, -0.5f, 0.0f, 1.0f);
	_pPoints[1] = vec4( 0.5f, -0.5f, 0.0f, 1.0f);
	_pPoints[2] = vec4( 0.5f,  0.5f, 0.0f, 1.0f);
	_pPoints[3] = vec4(-0.5f, -0.5f, 0.0f, 1.0f);
	_pPoints[4] = vec4( 0.5f,  0.5f, 0.0f, 1.0f);
	_pPoints[5] = vec4(-0.5f,  0.5f, 0.0f, 1.0f);

	_pNormals[0] = vec3(0, 0, 1.0f);  // Normal Vector �� W �� 0
	_pNormals[1] = vec3(0, 0, 1.0f);
	_pNormals[2] = vec3(0, 0, 1.0f);
	_pNormals[3] = vec3(0, 0, 1.0f);
	_pNormals[4] = vec3(0, 0, 1.0f);
	_pNormals[5] = vec3(0, 0, 1.0f);

	// set shader's name
	setShaderName("vsVtxNormal.glsl", "fsVtxNormal.glsl");

	// Create and initialize a buffer object 
	createBufferObject();

	// �w�]�� -1.0f, �� Normal �M�w�C��
	_fColor[0] = -1.0f; _fColor[1] = -1.0f; _fColor[2] = -1.0f; _fColor[3] = 1;

	// Sprite �����U�P�k�W���� local �y��
	_OriginalLL = vec4(-0.5f, -0.5f, 0.0f, 1.0f);
	_OriginalTR = vec4( 0.5f, 0.5f, 0.0f, 1.0f);

	_bPushed = false;

}

C2DSprite::~C2DSprite()
{
	
}

void C2DSprite::update(float dt)
{
}

void C2DSprite::setTRSMatrix(mat4 &mat)
{
	_mxTRS = mat;
	_bTRSUpdated = true;
	// �p�� Sprite �b�@�ɮy�Ъ����U���P�k�W������m
	_LL = _mxTRS*_OriginalLL;
	_TR = _mxTRS*_OriginalTR;
}

// �ǤJ���I�w�g�O������ (-1,-1) (1,1) ������ 2D �y��
bool C2DSprite::onTouches(const vec2 &tp)
{
	if (_LL.x <= tp.x && tp.x <= _TR.x && _LL.y <= tp.y && tp.y <= _TR.y) // �ӫ��s�Q����
	{
		if ( !_bPushed ) {
			// �ܦ��Ƕ�
			setColor(vec4(0.5, 0.5, 0.5, 1.0f));
			_bPushed = true;
		}
		else {
			_bPushed = false;
			setColor(_DefaultColor);
		}
		return true;		// �Ǧ^���I����s
	}
	else return false;	// �Ǧ^�S���I����s
}

void C2DSprite::setDefaultColor(vec4 vColor)
{
	_DefaultColor = vColor;
	setColor(vColor);
}

void C2DSprite::draw()
{
	drawingSetShader();
	glDrawArrays(GL_TRIANGLES, 0, QUAD_NUM);
}

void C2DSprite::drawW()
{
	drawingWithoutSetShader();
	glDrawArrays(GL_TRIANGLES, 0, QUAD_NUM);
}