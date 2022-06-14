// Illustrate the parent child relationship with Sun-Earth-Moon system
// Camera can be moved by mouse moving

#include "header/Angel.h"
#include "Common/CSolidSphere.h"
#include "Common/CQuad.h"
#include "Common/CSolidCube.h"
#include "Common/CCamera.h"

#define SPACE_KEY 32
#define SCREEN_SIZE 800
#define HALF_SIZE SCREEN_SIZE /2 
#define VP_HALFWIDTH  20.0f
#define VP_HALFHEIGHT 20.0f
#define GRID_SIZE 26 // must be an even number

// For sun�Bearth and moon
GLfloat g_fSunTx;   // �Ӷ��۹�󤤤��I�� X �b�첾
GLfloat g_fSunRevy; // �Ӷ��۹�󤤤��I�� Y �b������(����)
mat4    g_mxSunT;		//  Translation Matrix for sun

GLfloat g_fEarthTx;		// �a�y�۹��Ӷ������I�� X �b�첾
GLfloat g_fEarthRevy;	// �a�y�۹��Ӷ������I�� Y �b������(����)
GLfloat g_fEarthRoty;	// �a�y�������� Y �b������(����)
mat4    g_mxEarthT;		//  Translation Matrix for earth

GLfloat g_fMoonTx;    // ��y�۹��a�y�����I�� X �b�첾
GLfloat g_fMoonRevy; // ��y�۹��a�y�����I�� Y �b������(����)
GLfloat g_fMoonRoty; // ��y�������� Y �b������(����)
mat4    g_mxMoonT;		//  Translation Matrix for moon

// For Rotation
GLfloat g_fYAngle = 0;  // Z�b�����ਤ��
bool    g_bAutoRotation = false; // Controlled by Space Key

// For Objects
// CSolidSpheree *g_pSphere;
CQuad *g_pPlane[GRID_SIZE*GRID_SIZE]; // �Q���@�� 8X8 ���ѽL��l
float  g_fQuadT[GRID_SIZE*GRID_SIZE][3]={0};
CSolidSphere *g_pSun, *g_pEarth, *g_pMoon;
bool	  g_bAutoPlay = false;				// Controlled by A/a Key

// For View Point
GLfloat g_fRadius = 20.0;
GLfloat g_fTheta = 30 * DegreesToRadians;
GLfloat g_fPhi = 30  * DegreesToRadians;

//----------------------------------------------------------------------------
// �禡���쫬�ŧi
void IdleProcess();

void init( void )
{
	mat4 mxT;
	vec4 vT;
	GLuint uiShaderHandle;
	vec4 vColor = vec4( 0, 0, 0, 1);

	// ���ͩһݤ� Model View �P Projection Matrix
	// LookAt ���T�ӰѼơA�ϥβy��y��

	point4  eye(g_fRadius*sin(g_fTheta)*sin(g_fPhi), g_fRadius*cos(g_fTheta), g_fRadius*sin(g_fTheta)*cos(g_fPhi), 1.0f);
	point4  at(0.0f, 0.0f, 0.0f, 1.0f);

	auto camera = CCamera::getInstance();
	camera->updateViewLookAt(eye, at);
	camera->updatePerspective(60.0, (GLfloat)SCREEN_SIZE / (GLfloat)SCREEN_SIZE, 1.0, 1000.0);

	// ���ͪ��󪺹���

	int idx = 0;
	// �]�w�ѽL�W�C�@�Ӻ��檺��l��m
	// �Ҧ��� Quad �ϥάۦP�� Shader 
	for( int i = 0 ; i < GRID_SIZE ; i++ )  {
		for( int j = 0 ; j < GRID_SIZE ; j++ )  {
			g_fQuadT[idx][2] = -GRID_SIZE/2 + i + 0.5f;
			g_fQuadT[idx][0] = -GRID_SIZE/2 + j + 0.5f;
			g_fQuadT[idx][1] = -2.0f;
			g_pPlane[idx] = new CQuad;
			if( i != 0 || j != 0 ) {
				g_pPlane[idx]->setShader(uiShaderHandle);
				if( (i+j)%2 ) {
					vColor.x = 0.2f; vColor.y = 0.2f; vColor.z = 0.2f; vColor.w = 1.0f;
					g_pPlane[idx]->setColor(vColor);
				}
				else {
					vColor.x = 0.3f; vColor.y = 0.6f; vColor.z = 0.6f; vColor.w = 1.0f;
					g_pPlane[idx]->setColor(vColor);
				}
			}
			else { // �Ĥ@�ӳQ���ͪ�����A�ݭn���� shader handle
				g_pPlane[idx]->setShader();
				uiShaderHandle = g_pPlane[idx]->getShaderHandle();
				vColor.x = 0.3f; vColor.y = 0.6f; vColor.z = 0.6f; vColor.w = 1.0f;
				g_pPlane[idx]->setColor(vColor);
			}
			mxT = Translate(g_fQuadT[idx][0],g_fQuadT[idx][1],g_fQuadT[idx][2]);	
			g_pPlane[idx]->setTRSMatrix(mxT);
			idx++;
		}
	}

	// �a�B��B�� ����l�]�w
	g_fSunTx = 4; g_fSunRevy = 0;
	g_pSun = new CSolidSphere(2, 12, 8);
	g_pSun->setShader();
	g_mxSunT = Translate(g_fSunTx,0,0);
	g_pSun->setTRSMatrix(g_mxSunT);

	g_fEarthTx = 8;  g_fEarthRevy = 0;  g_fEarthRoty = 0;
	g_pEarth = new CSolidSphere; // �ϥιw�]��
	g_pEarth->setShader();
	g_mxEarthT = Translate(g_fEarthTx,0,0);
	g_pEarth->setTRSMatrix(g_mxSunT*g_mxEarthT);

	g_fMoonTx = 2.5; g_fMoonRevy = 0; g_fMoonRoty = 0;
	g_pMoon = new CSolidSphere(0.5, 12, 8);
	g_pMoon->setShader();
	g_mxMoonT = Translate(g_fMoonTx,0,0);
	g_pMoon->setTRSMatrix(g_mxSunT*g_mxEarthT*g_mxMoonT);
}


//----------------------------------------------------------------------------
void AutoPlay(float dt) 
{
	mat4 mxSunRevy, mxEarthRevy, mxMoonRevy, mxY;
	mat4 mxEarth, mxSun, mxMoon;
	g_fSunRevy += dt * 10.0f; // �Ӷ�����A�C���� 10 ��
	mxSunRevy = RotateY(g_fSunRevy); // �Ӷ�����
	mxSun = mxSunRevy*g_mxSunT;
	g_pSun->setTRSMatrix(mxSun); // ����A���첾�A����

	g_fEarthRevy += dt * 20.0f;		// �a�y����A�C���� 5 �� 
	g_fEarthRoty += dt * 40.0f;		// �a�y����A�C���� 4 �� 
	mxY = RotateX(g_fEarthRoty);		// �a�y����
	mxEarthRevy = RotateY(g_fEarthRevy);

	//�����]�w�첾�q�A�٥h�x�}�ۭ����p��; mxEarth = mxEarthRevy*g_mxEarthT;
	mxEarth = mxEarthRevy;
	mxEarth._m[0].w = g_mxEarthT._m[0].w;
	mxEarth._m[1].w = g_mxEarthT._m[1].w;
	mxEarth._m[2].w = g_mxEarthT._m[2].w;
	g_pEarth->setTRSMatrix(mxSun*mxEarth*mxY);
		
	g_fMoonRevy += dt * 40.0f;	// ��y����A�C���� 10 �� 
	g_fMoonRoty += dt * 60.0f;  // ��y����A�C���� 6 �� 
	mxY = RotateZ(g_fMoonRoty);	// ��y����
	mxMoonRevy = RotateY(g_fMoonRevy);
	
	// �����]�w�첾�q�A�٥h�x�}�ۭ����p��; mxMoon = mxMoonRevy * g_mxMoonT
	mxMoon = mxMoonRevy;
	mxMoon._m[0].w = g_mxMoonT._m[0].w;
	mxMoon._m[1].w = g_mxMoonT._m[1].w;
	mxMoon._m[2].w = g_mxMoonT._m[2].w;
	g_pMoon->setTRSMatrix(mxSun*mxEarth*mxMoon*mxY);
}

//----------------------------------------------------------------------------
void GL_Display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); // clear the window

	g_pSun->draw();
	g_pEarth->drawW();
	g_pMoon->drawW();

	g_pPlane[0]->draw();
	for( int i = 0 ; i < GRID_SIZE*GRID_SIZE ; i++ )  {
		g_pPlane[i]->drawW();
	}
	glutSwapBuffers();	// �洫 Frame Buffer
}

void onFrameMove(float delta)
{
	mat4 mvx, mpx;	// view matrix & projection matrix
	bool bVDirty, bPDirty;	// view �P projection matrix �O�_�ݭn��s������
	auto camera = CCamera::getInstance();

	if( g_bAutoPlay ) AutoPlay(delta);

	// �ѤW�h��s�Ҧ��n�Qø�s���� View �P Projection Matrix
	mvx = camera->getViewMatrix(bVDirty);
	if (bVDirty) { // ��s�Ҧ����� View Matrix
		g_pSun->setViewMatrix(mvx);
		g_pEarth->setViewMatrix(mvx);
		g_pMoon->setViewMatrix(mvx);
		for (int i = 0; i < GRID_SIZE*GRID_SIZE; i++)  g_pPlane[i]->setViewMatrix(mvx);
	}
	mpx = camera->getProjectionMatrix(bPDirty);
	if (bPDirty) { // ��s�Ҧ����� View Matrix
		g_pSun->setProjectionMatrix(mpx);
		g_pEarth->setProjectionMatrix(mpx);
		g_pMoon->setProjectionMatrix(mpx);
		for (int i = 0; i < GRID_SIZE*GRID_SIZE; i++)  g_pPlane[i]->setProjectionMatrix(mpx);
	}
	GL_Display();
}

//----------------------------------------------------------------------------
void Win_Keyboard( unsigned char key, int x, int y )
{
    switch ( key ) {
	case  SPACE_KEY:

		break;
	case 68: // D key
	case 100: // d key

		break;
	case 65: // A Key
	case 97: // a Key
		g_bAutoPlay = !g_bAutoPlay;
		break;
    case 033:
		glutIdleFunc( NULL );
		for( int i = 0 ; i < GRID_SIZE*GRID_SIZE ; i++) delete g_pPlane[i];
		delete g_pSun;
		delete g_pEarth;
		delete g_pMoon;
		CCamera::getInstance()->destroyInstance();
        exit( EXIT_SUCCESS );
        break;
    }
}

//----------------------------------------------------------------------------
void Win_Mouse(int button, int state, int x, int y) {
	switch(button) {
		case GLUT_LEFT_BUTTON:   // �ثe���U���O�ƹ�����
			//if ( state == GLUT_DOWN ) ; 
			break;
		case GLUT_MIDDLE_BUTTON:  // �ثe���U���O�ƹ����� �A���� Y �b
			//if ( state == GLUT_DOWN ) ; 
			break;
		case GLUT_RIGHT_BUTTON:   // �ثe���U���O�ƹ��k��
			//if ( state == GLUT_DOWN ) ;
			break;
		default:
			break;
	} 
}
//----------------------------------------------------------------------------
void Win_SpecialKeyboard(int key, int x, int y) {

	switch(key) {
		case GLUT_KEY_LEFT:		// �ثe���U���O�V����V��

			break;
		case GLUT_KEY_RIGHT:	// �ثe���U���O�V�k��V��

			break;
		default:
			break;
	}
}

//----------------------------------------------------------------------------
// The passive motion callback for a window is called when the mouse moves within the window while "no" mouse buttons are pressed.
void Win_PassiveMotion(int x, int y) {

	g_fPhi = -(float)M_PI*(x - HALF_SIZE)/(HALF_SIZE); // �ഫ�� g_fPhi ���� -PI �� PI ���� (-180 ~ 180 ����)
	g_fTheta = (float)M_PI*(float)y/SCREEN_SIZE;

	// ���s�p�� view ����m
	point4  eye(g_fRadius*sin(g_fTheta)*sin(g_fPhi), g_fRadius*cos(g_fTheta), g_fRadius*sin(g_fTheta)*cos(g_fPhi), 1.0f);

	// ������s�� CCamera ��
	CCamera::getInstance()->updateViewPosition(eye);

}

// The motion callback for a window is called when the mouse moves within the window while one or more mouse buttons are pressed.
void Win_MouseMotion(int x, int y) {
	g_fPhi = -(float)M_PI*(x - HALF_SIZE)/(HALF_SIZE);  // �ഫ�� g_fPhi ���� -PI �� PI ���� (-180 ~ 180 ����)
	g_fTheta = (float)M_PI*(float)y/SCREEN_SIZE;

	// ���s�p�� view ����m
	point4  eye(g_fRadius*sin(g_fTheta)*sin(g_fPhi), g_fRadius*cos(g_fTheta), g_fRadius*sin(g_fTheta)*cos(g_fPhi), 1.0f);

	// ������s�� CCamera ��
	CCamera::getInstance()->updateViewPosition(eye);
}

//----------------------------------------------------------------------------
void GL_Reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
	glClearColor( 0.0, 0.0, 0.0, 1.0 ); // black background
	glEnable(GL_DEPTH_TEST);
}

//----------------------------------------------------------------------------

int main( int argc, char **argv )
{
    
	glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DOUBLE );
    glutInitWindowSize( SCREEN_SIZE, SCREEN_SIZE );

	// If you use freeglut the two lines of code can be added to your application 
	glutInitContextVersion( 3, 2 );
	glutInitContextProfile( GLUT_CORE_PROFILE );

    glutCreateWindow( "Viewing Example 2b");

	// The glewExperimental global switch can be turned on by setting it to GL_TRUE before calling glewInit(), 
	// which ensures that all extensions with valid entry points will be exposed.
	glewExperimental = GL_TRUE; 
    glewInit();  

    init();

	glutMouseFunc(Win_Mouse);
	glutMotionFunc(Win_MouseMotion);
	glutPassiveMotionFunc(Win_PassiveMotion);  
    glutKeyboardFunc( Win_Keyboard );	// �B�z ASCI ����p A�Ba�BESC ��...����
	glutSpecialFunc( Win_SpecialKeyboard);	// �B�z NON-ASCI ����p F1�BHome�B��V��...����
    glutDisplayFunc( GL_Display );
	glutReshapeFunc( GL_Reshape );
	glutIdleFunc( IdleProcess );
	
    glutMainLoop();
    return 0;
}
