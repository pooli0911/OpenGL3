// Model-View matrix and Projection matrix
// Perspective projection with fixed camera position
// Rotate the grid and the cube around the Y axis

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
#define GRID_SIZE 8 // must be an even number


// For Rotation
GLfloat g_fYAngle = 0;  // Z�b�����ਤ��
GLfloat g_fDir = 1;     // �����V
bool    m_bAutoRotation = false; // Controlled by Space Key

// For Objects
CSolidSphere *g_pSphere;
CSolidCube   *g_pHouse;
CQuad        *g_plane;

// For View Point
GLfloat g_fRadius = 8.0;
GLfloat g_fTheta = 45.0f*DegreesToRadians;
GLfloat g_fPhi = 45.0f*DegreesToRadians;

//----------------------------------------------------------------------------
// �禡���쫬�ŧi
extern void IdleProcess();

void init( void )
{
	mat4 mxT;
	vec4 vT;

	// ���ͩһݤ� Model View �P Projection Matrix
	// LookAt ���T�ӰѼơA�ϥβy��y��
	point4  eye(g_fRadius*sin(g_fTheta)*sin(g_fPhi), g_fRadius*cos(g_fTheta), g_fRadius*sin(g_fTheta)*cos(g_fPhi), 1.0f);
	point4  at(0.0f, 0.0f, 0.0f, 1.0f);

	auto camera = CCamera::getInstance();
	camera->updateViewLookAt(eye, at);
	camera->updatePerspective(60.0, (GLfloat)SCREEN_SIZE/(GLfloat)SCREEN_SIZE, 1.0, 1000.0);

	// ���� SolidSphere ���骫��
	g_pSphere = new CSolidSphere[2]; // �w�] �b�| 1, Slices 12 Stacks 6
	g_pSphere->setShader();
	vT.x = -1.5; vT.y = 0.5; vT.z = 1.5;
	mxT = Translate(vT);
	g_pSphere->setTRSMatrix(mxT);


	// ���� SolidCube ���骫�� 
	g_pHouse = new CSolidCube;
	g_pHouse->setShader();
	// �]�w Cube
	vT.x = 1.5; vT.y = 0.5; vT.z = -1.5;
	mxT = Translate(vT);
	g_pHouse->setTRSMatrix(mxT);

	// ���� quad ���骫�� 
	g_plane = new CQuad();
	g_plane->setShader();
	g_plane->setColor(vec4(61.0f / 255.0f, 129.0f / 255.0f, 56.0f / 255.0f, 1.0f));
	g_plane->setTRSMatrix(Scale(7.0f, 1.0f, 7.0f));

	g_pSphere[1].setShader();
	vT.x = 1.0; vT.y = 0.5; vT.z = -1.5;
	mxT = Translate(vT);
	g_pSphere[1].setTRSMatrix(mxT);

}

void AutomaticRotation(float delta){
	mat4 ry, mxT;
	vec4 vT;

	g_fYAngle += g_fDir * delta * 15;     // �]�w�C���� 15 ��
	if( g_fYAngle > 360.0 ) g_fYAngle -= 360;
	else if (g_fYAngle < 0.0) g_fYAngle += 360.0;
	else ;
	// �p�����x�}�ç�s�� Shader ��
	ry = RotateY(g_fYAngle); //  degree 

	// For Sphere
	vT.x = -1.5; vT.y = 0.5; vT.z = 1.5;
	mxT = Translate(vT);
	g_pSphere->setTRSMatrix(ry*mxT);

	// For Cube
	vT.x = 1.5; vT.y = 0.5; vT.z = -1.5; // (1.5, 0.5, -1.5);
	mxT = Translate(vT);
	g_pHouse->setTRSMatrix(ry*mxT);

	// For Quad
	g_plane->setTRSMatrix(ry*Scale(7.0f, 1.0f, 7.0f));
}
//----------------------------------------------------------------------------

void GL_Display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); // clear the window

	g_pHouse->draw();
	g_pSphere->draw();
	(g_pSphere + 1)->draw();
	g_plane->draw();

	glutSwapBuffers();	// �洫 Frame Buffer
}

void onFrameMove(float delta)
{
	mat4 mvx, mpx;	// view matrix & projection matrix
	bool bVDirty, bPDirty;	// view �P projection matrix �O�_�ݭn��s������
	auto camera = CCamera::getInstance();
	if( m_bAutoRotation ) AutomaticRotation(delta);

	// �ѤW�h��s�Ҧ��n�Qø�s���� View �P Projection Matrix
	mvx = camera->getViewMatrix(bVDirty);
	if (bVDirty) { // ��s�Ҧ����� View Matrix
		g_pHouse->setViewMatrix(mvx);
		g_pSphere->setViewMatrix(mvx);
		(g_pSphere + 1)->setViewMatrix(mvx);
		g_plane->setViewMatrix(mvx);
	}
	mpx = camera->getProjectionMatrix(bPDirty);
	if (bPDirty) { // ��s�Ҧ����� View Matrix
		g_pHouse->setProjectionMatrix(mpx);
		g_pSphere->setProjectionMatrix(mpx);
		(g_pSphere + 1)->setProjectionMatrix(mpx);
		g_plane->setProjectionMatrix(mpx);
	}
	GL_Display();
}

//----------------------------------------------------------------------------

void Win_Keyboard( unsigned char key, int x, int y )
{
	auto camera = CCamera::getInstance();
    switch ( key ) {
	case  SPACE_KEY:
		m_bAutoRotation = !m_bAutoRotation;
		break;
	case 68: // D key
	case 100: // d key
		g_fDir = -1 * g_fDir;
		break;
	case 'p':
	case 'P':
		if (camera->getProjectionType() != CCamera::Type::PERSPECTIVE ) {
			camera->updatePerspective(60.0, (GLfloat)SCREEN_SIZE / (GLfloat)SCREEN_SIZE, 1.0, 1000.0);
		}
		break;
	case 'o':
	case 'O':
		if (camera->getProjectionType() != CCamera::Type::ORTHOGRAPHIC ) {
			camera->updateOrthographic(-5.0f, 5.0f, -5.0f, 5.0f, 1.0f, 100.0f);
		}
		break;
    case 033:
		glutIdleFunc( NULL );
		delete g_pHouse;
		delete [] g_pSphere;
		delete g_plane;
		camera->destroyInstance();
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
	mat4 ry, mxT;
	vec4 vT;
	if( !m_bAutoRotation ) { // �S���۰ʱ���U�~���@��
		switch(key) {
			case GLUT_KEY_LEFT:		// �ثe���U���O�V����V��
				g_fYAngle -= 2.0;		// �f�ɰw������V
				break;
			case GLUT_KEY_RIGHT:	// �ثe���U���O�V�k��V��
				g_fYAngle += 2.0;		// ���ɰw���t��V
				break;
			default:
				break;
		}

		// �p�����x�}�ç�s�� Shader ��
		ry = RotateY(g_fYAngle); //  degree 

		// Cube ������
		vT.x = 1.5; vT.y = 0.5; vT.z = -1.5;
		mxT = Translate(vT);
		g_pHouse->setTRSMatrix(ry*mxT);

//		glutPostRedisplay();  
	}
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
    glutInitWindowSize( 800, 800 );

	// If you use freeglut the two lines of code can be added to your application 
	glutInitContextVersion( 3, 2 );
	glutInitContextProfile( GLUT_CORE_PROFILE );

    glutCreateWindow( "Viewing Example 1" );

	// The glewExperimental global switch can be turned on by setting it to GL_TRUE before calling glewInit(), 
	// which ensures that all extensions with valid entry points will be exposed.
	glewExperimental = GL_TRUE; 
    glewInit();  
	
    init();

	glutMouseFunc(Win_Mouse);
    glutKeyboardFunc( Win_Keyboard );	// �B�z ASCI ����p A�Ba�BESC ��...����
	glutSpecialFunc( Win_SpecialKeyboard);	// �B�z NON-ASCI ����p F1�BHome�B��V��...����
	glutDisplayFunc( GL_Display );
	glutReshapeFunc( GL_Reshape );
	glutIdleFunc( IdleProcess );
	
    glutMainLoop();
    return 0;
}
