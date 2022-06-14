// Illustrate the parent child relationship with Sun-Earth-Moon system
// Camera position can't be moved

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
#define GRID_SIZE 32 // must be an even number


// For sun、earth and moon
GLfloat g_fSunTx;   // 太陽相對於中心點的 X 軸位移
GLfloat g_fSunRevy; // 太陽相對於中心點對 Y 軸的旋轉(公轉)
mat4    g_mxSunT;		//  Translation Matrix for sun

GLfloat g_fEarthTx;		// 地球相對於太陽中心點的 X 軸位移
GLfloat g_fEarthRevy;	// 地球相對於太陽中心點對 Y 軸的旋轉(公轉)
GLfloat g_fEarthRoty;	// 地球本身的對 Y 軸的旋轉(自轉)
mat4    g_mxEarthT;		//  Translation Matrix for earth

GLfloat g_fMoonTx;    // 月球相對於地球中心點的 X 軸位移
GLfloat g_fMoonRevy; // 月球相對於地球中心點對 Y 軸的旋轉(公轉)
GLfloat g_fMoonRoty; // 月球本身的對 Y 軸的旋轉(自轉)
mat4    g_mxMoonT;		//  Translation Matrix for moon

// For Rotation
GLfloat g_fYAngle = 0;  // Z軸的旋轉角度
GLfloat g_fDir = 1;     // 旋轉方向
bool    g_bAutoRotation = false; // Controlled by Space Key

// For Objects
// CSolidSpheree *g_pSphere;
CQuad *g_pPlane[GRID_SIZE*GRID_SIZE]; // 鋪成一個 8X8 的棋盤格子
float  g_fQuadT[GRID_SIZE*GRID_SIZE][3]={0};

CSolidSphere *g_pSun, *g_pEarth, *g_pMoon;
bool	  g_bAutoPlay = false;				// Controlled by A/a Key

// For View Point
GLfloat g_fRadius = 20.0;
GLfloat g_fTheta = 60 * DegreesToRadians;
GLfloat g_fPhi = 30  * DegreesToRadians;

//----------------------------------------------------------------------------
// 函式的原型宣告
extern void IdleProcess();

void init( void )
{
	mat4 mxT;
	vec4 vT;
	GLuint uiShaderHandle;
	vec4 vColor = vec4(0, 0, 0, 1);

	// 產生所需之 Model View 與 Projection Matrix
	// LookAt 的三個參數，使用球體座標
	point4  eye(g_fRadius*sin(g_fTheta)*sin(g_fPhi), g_fRadius*cos(g_fTheta), g_fRadius*sin(g_fTheta)*cos(g_fPhi), 1.0f);
	point4  at(0.0f, 0.0f, 0.0f, 1.0f);

	auto camera = CCamera::getInstance();
	camera->updateViewLookAt(eye, at);
	camera->updatePerspective(60.0, (GLfloat)SCREEN_SIZE / (GLfloat)SCREEN_SIZE, 1.0, 1000.0);

	// 產生物件的實體

	int idx = 0;
	// 設定棋盤上每一個網格的初始位置
	// 所有的 Quad 使用相同的 Shader 
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
			else { // 第一個被產生的物件，需要取的 shader handle
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

	// 地、日、月 的初始設定
	g_fSunTx = 4; g_fSunRevy = 0;
	g_pSun = new CSolidSphere(2, 12, 8);
	g_pSun->setShader();
	g_mxSunT = Translate(g_fSunTx,0,0);
	g_pSun->setTRSMatrix(g_mxSunT);

	g_fEarthTx = 8;  g_fEarthRevy = 0;  g_fEarthRoty = 0;
	g_pEarth = new CSolidSphere; // 使用預設值
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
void AutomaticRotation(float delta){
	mat4 mxRy, mxT;
	vec4 vT;

	g_fYAngle += g_fDir * delta * 2.5f;     // 設定每秒轉 2.5 度
	if( g_fYAngle > 360.0 ) g_fYAngle -= 360;
	else if (g_fYAngle < 0.0) g_fYAngle += 360.0;
	else ;
	// 計算旋轉矩陣並更新到 Shader 中
	mxRy = RotateY(g_fYAngle); //  degree 

	for( int i = 0 ; i < GRID_SIZE*GRID_SIZE ; i++ ) {
		mxT = Translate(g_fQuadT[i][0],g_fQuadT[i][1],g_fQuadT[i][2]);	
		g_pPlane[i]->setTRSMatrix(mxRy*mxT);
	}
}

//----------------------------------------------------------------------------
void AutoPlay(float dt) 
{
	mat4 mxSunRevy, mxEarthRevy, mxMoonRevy, mxY;
	mat4 mxEarth, mxSun, mxMoon;
	g_fSunRevy += dt * 30.0f; // 太陽公轉，每秒轉 10 度
	mxSunRevy = RotateY(g_fSunRevy); // 太陽公轉
	mxSun = mxSunRevy*g_mxSunT;
	g_pSun->setTRSMatrix(mxSun); // 公轉，先位移再旋轉

	g_fEarthRevy += dt * 20.0f;		// 地球公轉，每秒轉 5 度 
	g_fEarthRoty += dt * 40.0f;		// 地球自轉，每秒轉 4 度 
	mxY = RotateX(g_fEarthRoty);		// 地球自轉
	mxEarthRevy = RotateY(g_fEarthRevy);

	//直接設定位移量，省去矩陣相乘的計算;
	mxEarth = mxEarthRevy*g_mxEarthT;
	mxEarth = mxEarthRevy;
	mxEarth._m[0].w = g_mxEarthT._m[0].w;
	mxEarth._m[1].w = g_mxEarthT._m[1].w;
	mxEarth._m[2].w = g_mxEarthT._m[2].w;
	g_pEarth->setTRSMatrix(mxSun*mxEarth*mxY);
		
	g_fMoonRevy += dt * 40.0f;	// 月球公轉，每秒轉 10 度 
	g_fMoonRoty += dt * 60.0f;  // 月球自轉，每秒轉 6 度 
	mxY = RotateZ(g_fMoonRoty);	// 月球自轉
	mxMoonRevy = RotateY(g_fMoonRevy);
	
	// 直接設定位移量，省去矩陣相乘的計算;
	mxMoon = mxMoonRevy * g_mxMoonT;
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
	g_pEarth->draw();
	g_pMoon->draw();

	g_pPlane[0]->draw();
	for( int i = 1 ; i < GRID_SIZE*GRID_SIZE ; i++ )  {
		g_pPlane[i]->drawW();
	}
	glutSwapBuffers();	// 交換 Frame Buffer
}

void onFrameMove(float delta)
{
	mat4 mvx, mpx;	// view matrix & projection matrix
	bool bVDirty, bPDirty;	// view 與 projection matrix 是否需要更新給物件
	auto camera = CCamera::getInstance();

	if( g_bAutoRotation ) AutomaticRotation(delta);
	if( g_bAutoPlay ) AutoPlay(delta);

	// 由上層更新所有要被繪製物件的 View 與 Projection Matrix
	mvx = camera->getViewMatrix(bVDirty);
	if (bVDirty) { // 更新所有物件的 View Matrix
		g_pSun->setViewMatrix(mvx);
		g_pEarth->setViewMatrix(mvx);
		g_pMoon->setViewMatrix(mvx);
		for (int i = 0; i < GRID_SIZE*GRID_SIZE; i++)  g_pPlane[i]->setViewMatrix(mvx);
	}
	mpx = camera->getProjectionMatrix(bPDirty);
	if (bPDirty) { // 更新所有物件的 View Matrix
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
		g_bAutoRotation = !g_bAutoRotation;
		break;
	case 68: // D key
	case 100: // d key
		g_fDir = -1 * g_fDir;
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
		case GLUT_LEFT_BUTTON:   // 目前按下的是滑鼠左鍵
			//if ( state == GLUT_DOWN ) ; 
			break;
		case GLUT_MIDDLE_BUTTON:  // 目前按下的是滑鼠中鍵 ，換成 Y 軸
			//if ( state == GLUT_DOWN ) ; 
			break;
		case GLUT_RIGHT_BUTTON:   // 目前按下的是滑鼠右鍵
			//if ( state == GLUT_DOWN ) ;
			break;
		default:
			break;
	} 
}
//----------------------------------------------------------------------------
void Win_SpecialKeyboard(int key, int x, int y) {

	switch(key) {
		case GLUT_KEY_LEFT:		// 目前按下的是向左方向鍵

			break;
		case GLUT_KEY_RIGHT:	// 目前按下的是向右方向鍵

			break;
		default:
			break;
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

    glutCreateWindow( "Viewing Example 2a" );

	// The glewExperimental global switch can be turned on by setting it to GL_TRUE before calling glewInit(), 
	// which ensures that all extensions with valid entry points will be exposed.
	glewExperimental = GL_TRUE; 
    glewInit();  

    init();

	glutMouseFunc(Win_Mouse);
    glutKeyboardFunc( Win_Keyboard );	// 處理 ASCI 按鍵如 A、a、ESC 鍵...等等
	glutSpecialFunc( Win_SpecialKeyboard);	// 處理 NON-ASCI 按鍵如 F1、Home、方向鍵...等等
    glutDisplayFunc( GL_Display );
	glutReshapeFunc( GL_Reshape );
	glutIdleFunc( IdleProcess );
	
    glutMainLoop();
    return 0;
}
