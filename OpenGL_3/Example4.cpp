// Illustrate the parent child relationship with Sun-Earth-Moon system
// Camera can be moved by mouse moving

#include "header/Angel.h"
#include "Common/CSolidSphere.h"
#include "Common/CQuad.h"
#include "Common/CWireCube.h"
#include "Common/CLineSegment.h"
#include "Common/CCamera.h"
#include "Common/C2DSprite.h"

#define SPACE_KEY 32
#define NUMBER1_KEY 49
#define NUMBER2_KEY 50
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 800
#define WIDTH_HALF (SCREEN_WIDTH/2) 
#define HEIGHT_HALF (SCREEN_HEIGHT/2) 
#define VP_HALFWIDTH  20.0f
#define VP_HALFHEIGHT 20.0f
#define GRID_SIZE 20 // must be an even number

#define RED_BUTTON   0
#define GREEN_BUTTON 1
#define BLUE_BUTTON  2
#define WHITE_BUTTON 3


// For Three Axes
CLineSegment *g_pXAxis, *g_pYAxis, *g_pZAxis;

// For Two WireCube
CWireCube *g_pUpperArm, *g_pForeArm;
vec3    g_vUpperArmS(4, 0.4f, 2);
GLfloat g_fUpperArmTx = 2;   // Arm ��󤤤��I�� X �b�첾
GLfloat g_fShoulderRZ = 0; // Arm �H������I�����߹� Z �b�����ਤ��
mat4    g_mxUpperArmS;	//  Scaling Matrix for Arm
mat4    g_mxUpperArmT;	//  Translation Matrix for Arm
mat4    g_mxUpperArmRZ;	//  Rotation Matrix for Arm

vec3    g_vForeArmS(4, 0.4f, 2);
GLfloat g_fForeArmTx = 2;   // Shoulder �۹�� Arm X �b���k���첾
GLfloat g_fElbowRZ = 0; // Shoulder �H������I�����߹� Z �b�����ਤ��
mat4    g_mxForeArmS;		//  Scaling Matrix for Shoulder
mat4    g_mxForeArmT;		//  Translation Matrix for Shoulder
mat4    g_mxForeArmRZ;	//  Rotation Matrix for Shoulder

// For chcckered floor
CQuad *g_pPlane[GRID_SIZE*GRID_SIZE]; // �Q���@�� 8X8 ���ѽL��l


// For Shoulder and Elbow Rotation
bool  g_bElbowByMouse, g_bShoulderByMouse;	// �]�w�H�ƹ��ӱ��� elbow �άO Shoulder ������
bool  g_bElbowByKB,    g_bShoulderByKB;		// �]�w�H��L�ӱ��� elbow �άO Shoulder ������
float g_fHalfWidth, g_fHalfHeight;

// For View Point
GLfloat g_fRadius = 20.0;
GLfloat g_fTheta = 45 * DegreesToRadians;
GLfloat g_fPhi = 45  * DegreesToRadians;

// 2D �����һݭn�������ܼ�
// g_p2DBtn ����b�|�Ө������N����s���|���
// g_2DView �P  g_2DProj  �h�O�o�̨ϥ� opengl �w�]�����Y (0,0,0) �ݦV -Z ��V
// �ϥΥ����v
C2DSprite *g_p2DBtn[4];
mat4  g_2DView;
mat4  g_2DProj;

bool  g_bShowXAxis = true;
bool  g_bShowYAxis = true;
bool  g_bShowZAxis = true;


//----------------------------------------------------------------------------
// �禡���쫬�ŧi
void IdleProcess();
void CreateCheckeredFloor();

void init( void )
{
	// ���ͩһݤ� Model View �P Projection Matrix
	// LookAt ���T�ӰѼơA�ϥβy��y��
	point4  eye(g_fRadius*sin(g_fTheta)*sin(g_fPhi), g_fRadius*cos(g_fTheta), g_fRadius*sin(g_fTheta)*cos(g_fPhi), 1.0f);
	point4  at(0.0f, 0.0f, 0.0f, 1.0f);

	auto camera = CCamera::create();
	camera->updateViewLookAt(eye, at);
	camera->updatePerspective(60.0, (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 1.0, 1000.0);

	// ���ͪ��󪺹���
	// ���� X/Y/Z �T�b����ܽu�q
	g_pXAxis = new CLineSegment(vec4(-8, 0, 0, 1),vec4(8,0,0,1),vec4(1,0,0,1));
	g_pXAxis->setShader();

	g_pYAxis = new CLineSegment(vec4( 0,-8, 0, 1),vec4(0,8,0,1),vec4(0,1,0,1));
	g_pYAxis->setShader();

	g_pZAxis = new CLineSegment(vec4( 0, 0,-8, 1),vec4(0,0,8,1),vec4(0,0,1,1));
	g_pZAxis->setShader();

	// For upper Arm Object
	g_pUpperArm = new CWireCube;
	g_pUpperArm->setShader();
	g_mxUpperArmS = Scale(g_vUpperArmS);
	g_mxUpperArmT = Translate(g_fUpperArmTx,0,0);
	g_mxUpperArmRZ  = RotateZ(g_fShoulderRZ);
	g_pUpperArm->setColor(vec4(1,1,0,1));
	g_pUpperArm->setTRSMatrix(g_mxUpperArmRZ*g_mxUpperArmT*g_mxUpperArmS);

	// For fore Arm Object
	g_pForeArm = new CWireCube;
	g_pForeArm->setShader();
	g_mxForeArmS = Scale(g_vForeArmS);
	g_mxForeArmT  = Translate(g_fForeArmTx,0,0);
	g_mxForeArmRZ  = RotateZ(g_fElbowRZ);
	g_pForeArm->setColor(vec4(0,1,1,1));
	g_mxUpperArmT._m[0].w += 2; // �N Fore Arm ���ʨ� Upper Arm ���k��A���B���e�|�]�w�� 4
	g_pForeArm->setTRSMatrix(g_mxUpperArmRZ*g_mxUpperArmT*g_mxForeArmRZ*g_mxForeArmT*g_mxForeArmS);

	CreateCheckeredFloor();

	// For Shoulder and Elbow rotation
	g_bElbowByMouse = g_bShoulderByMouse = false;
	g_bElbowByKB = g_bShoulderByKB = false;
	g_fShoulderRZ = g_fElbowRZ = 0;

	// �H�U���Q�Υ����v���� 2D ������
	// �d��b X/Y ������  -1 �� 1 �����A��������b Z = 0 
	mat4 mxT, mxS;
	vec4 vColor = vec4(0, 0, 0, 1);

	g_p2DBtn[0] = new C2DSprite; g_p2DBtn[0]->setShader();
	vColor.x = 1; vColor.y = 0; vColor.z = 0; g_p2DBtn[0]->setDefaultColor(vColor);
	mxS = Scale(0.1f, 0.1f, 1.0f);
	mxT = Translate(-0.90f, -0.90f, 0);
	g_p2DBtn[0]->setTRSMatrix(mxT*mxS);
	g_p2DBtn[0]->setViewMatrix(g_2DView);
	g_p2DBtn[0]->setViewMatrix(g_2DProj);

	g_p2DBtn[1] = new C2DSprite; g_p2DBtn[1]->setShader();
	vColor.x = 0; vColor.y = 1; vColor.z = 0; g_p2DBtn[1]->setDefaultColor(vColor);
	mxT = Translate(0.90f, -0.90f, 0);
	g_p2DBtn[1]->setTRSMatrix(mxT*mxS);
	g_p2DBtn[1]->setViewMatrix(g_2DView);
	g_p2DBtn[1]->setViewMatrix(g_2DProj);

	g_p2DBtn[2] = new C2DSprite; g_p2DBtn[2]->setShader();
	vColor.x = 0; vColor.y = 0; vColor.z = 1; g_p2DBtn[2]->setDefaultColor(vColor);
	mxT = Translate(0.90f, 0.90f, 0);
	g_p2DBtn[2]->setTRSMatrix(mxT*mxS);
	g_p2DBtn[2]->setViewMatrix(g_2DView);
	g_p2DBtn[2]->setViewMatrix(g_2DProj);

	g_p2DBtn[3] = new C2DSprite; g_p2DBtn[3]->setShader();
	vColor.x = 1; vColor.y = 1; vColor.z = 1; g_p2DBtn[3]->setDefaultColor(vColor);
	mxT = Translate(-0.90f, 0.90f, 0);
	g_p2DBtn[3]->setTRSMatrix(mxT*mxS);
	g_p2DBtn[3]->setViewMatrix(g_2DView);
	g_p2DBtn[3]->setViewMatrix(g_2DProj);

}

//----------------------------------------------------------------------------

void GL_Display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); // clear the window

	g_pPlane[0]->draw();
	for( int i = 1 ; i < GRID_SIZE*GRID_SIZE ; i++ )  {
		g_pPlane[i]->drawW();
	}

	g_pUpperArm->draw();
	g_pForeArm->draw();
	if(g_bShowXAxis) g_pXAxis->draw();
	if(g_bShowYAxis) g_pYAxis->draw();
	if(g_bShowZAxis) g_pZAxis->draw();

	for (int i = 0; i < 4; i++) g_p2DBtn[i]->draw();

	glutSwapBuffers();	// �洫 Frame Buffer
}

void onFrameMove(float delta)
{
	mat4 mvx, mpx;	// view matrix & projection matrix
	bool bVDirty, bPDirty;	// view �P projection matrix �O�_�ݭn��s������
	auto camera = CCamera::getInstance();

	if( g_bShoulderByKB || g_bShoulderByMouse ) 	{ 
		g_mxUpperArmRZ  = RotateZ(g_fShoulderRZ);
		g_mxUpperArmT._m[0].w -= 2; // �N Upper Arm ���k���ʨ�ӳ��A��t���� (0,0,0)
		g_pUpperArm->setTRSMatrix(g_mxUpperArmRZ*g_mxUpperArmT*g_mxUpperArmS);
		g_mxUpperArmT._m[0].w += 2; // �� Fore Arm ���ʨ� Upper Arm ���k��
		g_pForeArm->setTRSMatrix(g_mxUpperArmRZ*g_mxUpperArmT*g_mxForeArmRZ*g_mxForeArmT*g_mxForeArmS);
		if( g_bShoulderByKB ) g_bShoulderByKB = false;
	}
	if( g_bElbowByKB || g_bElbowByMouse ) 	{ 
		g_mxForeArmRZ  = RotateZ(g_fElbowRZ);
		g_pForeArm->setTRSMatrix(g_mxUpperArmRZ*g_mxUpperArmT*g_mxForeArmRZ*g_mxForeArmT*g_mxForeArmS);
		if( g_bElbowByKB ) g_bElbowByKB = false;
	}

	// �ѤW�h��s�Ҧ��n�Qø�s���� View �P Projection Matrix
	mvx = camera->getViewMatrix(bVDirty);
	if (bVDirty) { // ��s�Ҧ����� View Matrix
		g_pUpperArm->setViewMatrix(mvx);
		g_pForeArm->setViewMatrix(mvx);
		g_pXAxis->setViewMatrix(mvx);
		g_pYAxis->setViewMatrix(mvx);
		g_pZAxis->setViewMatrix(mvx);
		for (int i = 0; i < GRID_SIZE*GRID_SIZE; i++)  g_pPlane[i]->setViewMatrix(mvx);
	}
	mpx = camera->getProjectionMatrix(bPDirty);
	if (bPDirty) { // ��s�Ҧ����� View Matrix
		g_pUpperArm->setProjectionMatrix(mpx);
		g_pForeArm->setProjectionMatrix(mpx);
		g_pXAxis->setProjectionMatrix(mpx);
		g_pYAxis->setProjectionMatrix(mpx);
		g_pZAxis->setProjectionMatrix(mpx);
		for (int i = 0; i < GRID_SIZE*GRID_SIZE; i++)  g_pPlane[i]->setProjectionMatrix(mpx);
	}
	GL_Display();
}

//----------------------------------------------------------------------------
void Win_Keyboard( unsigned char key, int x, int y )
{
	switch ( key ) {
		case 's':
			g_fShoulderRZ += 5;  // �ӻH�f������V���� 5 ��
			if( g_fShoulderRZ >= 360 ) g_fShoulderRZ -= 360;
			g_bShoulderByKB = true;
			glutPostRedisplay();
			break;
		case 'S':
			g_fShoulderRZ -= 5; // �ӻH��������V���� 5 ��
			if( g_fShoulderRZ <= -360 ) g_fShoulderRZ += 360;
			g_bShoulderByKB = true;
	        glutPostRedisplay();
	        break;
		case 'e':
	        g_fElbowRZ += 5; // ���u�f������V���� 5 ��
			if( g_fElbowRZ >= 360 ) g_fElbowRZ -= 360;
			g_bElbowByKB = true;
	        glutPostRedisplay();
	        break;
		case 'E':
	        g_fElbowRZ -= 5; // ���u�f������V���� 5 ��
			if( g_fElbowRZ <= -360 ) g_fElbowRZ += 360;
			g_bElbowByKB = true;
	        glutPostRedisplay();
	        break;
		case  NUMBER1_KEY:
			g_bShoulderByMouse = ! g_bShoulderByMouse;
			break;
		case  NUMBER2_KEY:
			g_bElbowByMouse = ! g_bElbowByMouse;
			break;
		case  SPACE_KEY:
			break;
		case 033:
			glutIdleFunc( NULL );
			for( int i = 0 ; i < GRID_SIZE*GRID_SIZE ; i++) delete g_pPlane[i];
			delete g_pXAxis;
			delete g_pYAxis;
			delete g_pZAxis;
			delete g_pUpperArm;
			delete g_pForeArm;
			CCamera::getInstance()->destroyInstance();
			for (int i = 0; i < 4; i++) delete g_p2DBtn[i];
			exit( EXIT_SUCCESS );
			break;
    }
}

inline void ScreenToUICoordinate(int x,int y, vec2 &pt)
{
	pt.x = 2.0f*(float)x / SCREEN_WIDTH - 1.0f;
	pt.y = 2.0f*(float)(SCREEN_HEIGHT-y) / SCREEN_HEIGHT - 1.0f;
}

//----------------------------------------------------------------------------
void Win_Mouse(int button, int state, int x, int y) {
	vec2 pt;
	switch(button) {
		case GLUT_LEFT_BUTTON:   // �ثe���U���O�ƹ�����
			if (state == GLUT_DOWN) {
				ScreenToUICoordinate(x, y, pt);
				if (g_p2DBtn[0]->onTouches(pt)) {
					if (g_p2DBtn[0]->getButtonStatus()) g_bShowXAxis = false;
					else g_bShowXAxis = true;
				}
				if (g_p2DBtn[1]->onTouches(pt)) {
					if (g_p2DBtn[1]->getButtonStatus()) g_bShowYAxis = false;
					else g_bShowYAxis = true;
				}
				if (g_p2DBtn[2]->onTouches(pt)) {
					if (g_p2DBtn[2]->getButtonStatus()) g_bShowZAxis = false;
					else g_bShowZAxis = true;
				}
				g_p2DBtn[3]->onTouches(pt);
			}
			break;
		case GLUT_MIDDLE_BUTTON:  // �ثe���U���O�ƹ����� �A���� Y �b
			//if ( state == GLUT_DOWN ) ; 
			break;
		case GLUT_RIGHT_BUTTON:   // �ثe���U���O�ƹ��k��
			//if( state == GLUT_UP );
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
// The passive motion callback for a window is called when the mouse moves within the window while no mouse buttons are pressed.
void Win_PassiveMotion(int x, int y) {
	if (g_bShoulderByMouse){ // �H�ƹ����� Shoulder ������
		g_fShoulderRZ = 180.0f*(float)(SCREEN_HEIGHT - y) / SCREEN_HEIGHT - 90.0f;
	}
	if (g_bElbowByMouse){ // �H�ƹ����� elbow ������
		g_fElbowRZ = 230.0f*(float)(SCREEN_WIDTH - x) / SCREEN_WIDTH - 115.0f;
	}
}

// The motion callback for a window is called when the mouse moves within the window while one or more mouse buttons are pressed.
void Win_MouseMotion(int x, int y) {

	g_fPhi = (float)-M_PI*(x - WIDTH_HALF) / (WIDTH_HALF);  // �ഫ�� g_fPhi ���� -PI �� PI ���� (-180 ~ 180 ����)
	g_fTheta = (float)M_PI*(float)y/SCREEN_HEIGHT;

	point4  eye(g_fRadius*sin(g_fTheta)*sin(g_fPhi), g_fRadius*cos(g_fTheta), g_fRadius*sin(g_fTheta)*cos(g_fPhi), 1.0f);
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
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( SCREEN_WIDTH, SCREEN_HEIGHT );

	// If you use freeglut the two lines of code can be added to your application 
	glutInitContextVersion( 3, 2 );
	glutInitContextProfile( GLUT_CORE_PROFILE );

    glutCreateWindow( "Draw two triangles" );

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

void CreateCheckeredFloor()
{

	GLuint uiShaderHandle;
	vec4 vColor = vec4( 0, 0, 0, 1);
	vec4 vT;
	mat4 mxT;

	// For Floor
	int idx = 0; vT.y = -6;
	// �]�w�ѽL�W�C�@�Ӻ��檺��l��m
	// �Ҧ��� Quad �ϥάۦP�� Shader 
	for( int i = 0 ; i < GRID_SIZE ; i++ )  {
		vT.z = -GRID_SIZE/2 + i + 0.5f;
		for( int j = 0 ; j < GRID_SIZE ; j++ )  {
			g_pPlane[idx] = new CQuad;
			vT.x = -GRID_SIZE/2 + j + 0.5f;
			if( i != 0 || j != 0 ) {
				g_pPlane[idx]->setShader(uiShaderHandle);
				if( (i+j)%2 ) {
					vColor.x = 0.15f; vColor.y = 0.15f; vColor.z = 0.15f; vColor.w = 1.0f;
					g_pPlane[idx]->setColor(vColor);
				}
				else {
					vColor.x = 0.25f; vColor.y = 0.4f; vColor.z = 0.4f; vColor.w = 1.0f;
					g_pPlane[idx]->setColor(vColor);
				}
			}
			else { // �Ĥ@�ӳQ���ͪ�����A�ݭn���� shader handle
				g_pPlane[idx]->setShader();
				uiShaderHandle = g_pPlane[idx]->getShaderHandle();
				vColor.x = 0.15f; vColor.y = 0.3f; vColor.z = 0.3f; vColor.w = 1.0f;
				g_pPlane[idx]->setColor(vColor);
			}
			mxT = Translate(vT);	
			g_pPlane[idx]->setTRSMatrix(mxT);
			idx++;
		}
	}
}