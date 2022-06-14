#include "CWireSphere.h"

CWireSphere::CWireSphere(const GLfloat fRadius, const int iSlices, const  int iStacks)
{
    GLfloat drho = (GLfloat)(3.141592653589) / (GLfloat) iStacks;  
    GLfloat dtheta = 2.0f * (GLfloat)(3.141592653589) / (GLfloat) iSlices;  
    GLfloat ds = 1.0f / (GLfloat) iSlices;  
    GLfloat dt = 1.0f / (GLfloat) iStacks;  
    GLfloat t = 1.0f;      
    GLfloat s = 0.0f;  
    GLint i, j;     // Looping variables  
	int idx = 0; // �x�s vertex ���Ǫ����ޭ�

	_fRadius = fRadius;
	_iSlices = iSlices;
	_iStacks = iStacks;
	_iNumVtx = iStacks*(2*(iSlices+1));

	_pPoints = NULL; _pNormals = NULL; _pTex = NULL;

	_pPoints  = new vec4[_iNumVtx];
	_pNormals = new vec3[_iNumVtx];
	_pTex     = new vec2[_iNumVtx];


	for (i = 0; i < iStacks; i++ ) {  
		GLfloat rho = (GLfloat)i * drho;  
		GLfloat srho = (GLfloat)(sin(rho));  
		GLfloat crho = (GLfloat)(cos(rho));  
		GLfloat srhodrho = (GLfloat)(sin(rho + drho));  
		GLfloat crhodrho = (GLfloat)(cos(rho + drho));  
		
		// Many sources of OpenGL sphere drawing code uses a triangle fan  
		// for the caps of the sphere. This however introduces texturing   
		// artifacts at the poles on some OpenGL implementations  
		s = 0.0f;  
		for ( j = 0; j <= iSlices; j++) {  
            GLfloat theta = (j == iSlices) ? 0.0f : j * dtheta;  
            GLfloat stheta = (GLfloat)(-sin(theta));  
            GLfloat ctheta = (GLfloat)(cos(theta));  
  
            GLfloat x = stheta * srho;  
            GLfloat y = ctheta * srho;  
            GLfloat z = crho;  
              
			_pPoints[idx].x = x * _fRadius;
			_pPoints[idx].y = y * _fRadius;
			_pPoints[idx].z = z * _fRadius;
			_pPoints[idx].w = 1;

			_pNormals[idx].x = x;
			_pNormals[idx].y = y;
			_pNormals[idx].z = z;

			_pTex[idx].x = s;
			_pTex[idx].y = t; // �]�w�K�Ϯy��
			idx++;

            x = stheta * srhodrho;  
            y = ctheta * srhodrho;  
            z = crhodrho;

			_pPoints[idx].x = x * _fRadius;
			_pPoints[idx].y = y * _fRadius;
			_pPoints[idx].z = z * _fRadius;
			_pPoints[idx].w = 1;

			_pNormals[idx].x = x;
			_pNormals[idx].y = y;
			_pNormals[idx].z = z;

			_pTex[idx].x = s;
			_pTex[idx].y = t - dt; // �]�w�K�Ϯy��
			idx++;
			s += ds; 
		}   
		t -= dt;  
	}  

	// Set shader's name
	setShaderName("vsVtxNormal.glsl", "fsVtxNormal.glsl");

	// Create and initialize a buffer object 
	createBufferObject();

	// �w�]�� -1.0f, �� Normal �M�w�C��
	_fColor[0] = -1.0f; _fColor[1] = -1.0f; _fColor[2] = -1.0f; _fColor[3] = 1;
}


void CWireSphere::update(float dt)
{

}

void CWireSphere::draw()
{
	drawingSetShader();
	for (int i = 0; i < _iStacks; i++ ) {  
		glDrawArrays( GL_LINE_LOOP, i*(2*(_iSlices+1)), 2*(_iSlices+1) );
	}
}


void CWireSphere::drawW()
{
	drawingWithoutSetShader();
	for (int i = 0; i < _iStacks; i++ ) {  
		glDrawArrays( GL_LINE_LOOP, i*(2*(_iSlices+1)), 2*(_iSlices+1) );
	}
}

CWireSphere::~CWireSphere()
{

}