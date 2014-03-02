#include <windows.h>
#include "glut.h"
#include "glaux.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "glm/glm.hpp"
#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"glaux.lib")

using namespace std;
float points[45][45][3];

int count = 0,sky = 0 ,flag = 0, flag2 = 0, view_side_LR = 0, view_side_upDown = 0, all_light_flag = 0, tex_off = 0;// the keyborad

bool light_flag = 1;
GLfloat hold[3], Clight = 0.0f, view_move_LR = 0.0, view_move_upDown = 0.0;
GLuint texture[6];//6 textures for 6 faces of the cube
GLfloat xRot,yRot,zRot;//control cube's rotation
vector<glm::vec3> vertices;
vector<glm::vec2> uvs;
vector<glm::vec3> normals;

GLfloat light_ambient[2][4]  = { 1.0f , 1.0f , 1.0f , 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[4][4] = {{ -2.0f, 0.0f, 2.0f, 1.0f },{ -2.0f, 0.0f, 2.0f, 1.0f },{ 0.0f, 2.0f, -1.0f, 1.0f },{ 2.0f, -2.0f, 4.0f, 1.0f }};


const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[2][1] = {{ 700.0f },{ 700.0f }};

void light(){

    glDisable(GL_LIGHT1);
    glDisable(GL_LIGHT2);
    glDisable(GL_LIGHT3);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient[0]);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position[0]);

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess[0]);

}
void light_all(){

    glDisable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    glEnable(GL_LIGHT3);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);

    glLightfv(GL_LIGHT1, GL_AMBIENT,  light_ambient[1]);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position[1]);

    glLightfv(GL_LIGHT2, GL_AMBIENT,  light_ambient[1]);
    glLightfv(GL_LIGHT2, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT2, GL_POSITION, light_position[2]);

    glLightfv(GL_LIGHT3, GL_AMBIENT,  light_ambient[1]);
    glLightfv(GL_LIGHT3, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT3, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT3, GL_POSITION, light_position[3]);

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess[1]);
}



//read bmp image file
AUX_RGBImageRec *LoadBMP(char *FileName)
{
    FILE *File = NULL;
    if(!FileName)
        return NULL;
    File = fopen(FileName,"r");
    if (File)
    {
        fclose(File);
        return auxDIBImageLoad(FileName);
    }
    return NULL;
}

//load the bitmap and convert it into a texture
int LoadGLTextures()
{
    int Status = FALSE;
    char *bmpFile[6] = {"FLOOR.BMP","PUF.BMP","SKY.BMP",
        "FLOOR.BMP","SKY.BMP","SKY.BMP"};
    //AUX_RGBImageRec *TextureImage[6] = new AUX_RGBImageRec[6];//create storage for the texture
    AUX_RGBImageRec *TextureImage[6] = {NULL,NULL,NULL,NULL,NULL,NULL};
    for(int i = 0;i < 6;++i)
    {
        //memset(TextureImage[i],0,sizeof(void*) * 1);//set the point to NULL
        if (TextureImage[i] = LoadBMP(bmpFile[i]))
        {
            Status = TRUE;
            glGenTextures(1,&texture[i]);//命名纹理对象
            glBindTexture(GL_TEXTURE_2D,texture[i]);//绑定纹理
            glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,TextureImage[i]->sizeX,
                TextureImage[i]->sizeY,0,GL_RGB,GL_UNSIGNED_BYTE,
                TextureImage[i]->data);//指定纹理
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);//指定过滤模式
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        }

        if (TextureImage[i])
        {
            if (TextureImage[i]->data){}
                //free(TextureImage[i]->data);
            //free(TextureImage[i]);
        }
    }


    return Status;
}

int init()
{
    if(!LoadGLTextures())
        return FALSE;
    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f,0.0f,0.0f,0.5f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
    glPolygonMode( GL_BACK, GL_FILL );			// 后表面完全填充
    glPolygonMode( GL_FRONT, GL_FILL );

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

	for(int x=0; x<45; x++)
	{
		for(int y=0; y<45; y++)
		{
			points[x][y][0]=float((x/5.0f)-4.5f);
			points[x][y][1]=float((y/5.0f)-4.5f);
			points[x][y][2]=float(0.0f);
            //printf("%d  %d  %d\n" ,points[x][y][0], points[x][y][1], points[x][y][2] );
		}
	}

    return TRUE;
}

void display()
{
    int x, y;
	float float_x, float_y, float_xb, float_yb;
    //light();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    //gluLookAt(0,0,0.15,  0,1,0.4,  0,1,0);

    glEnable(GL_TEXTURE_2D);
	glTranslatef(0.0f,0.0f,-25.0f);
    //glRotatef(xRot,1.0f,0.0f,0.0f);
    //glRotatef(yRot,0.0f,1.0f,0.0f);
    //glRotatef(zRot,0.0f,0.0f,1.0f);
    //gluLookAt(-2,-2,-7,   0,0,0,   0,1,0);
    gluLookAt(0,0,0.15 ,  0,1,0.4,  0,1,0);

    glRotatef(view_move_LR,0.0f,0.0f,0.15f);
	//glTranslatef(0.0f,1.0f,0.4f);
    //glRotatef(view_move_upDown,0.010f,0.0f,0.0f);

    if(view_side_LR != 0)
        view_move_LR = 10*view_side_LR;
    if(view_side_LR == 0)
        view_move_LR = 0;
    if(view_side_upDown != 0){
        glPushMatrix();
        view_move_upDown = 10*view_side_upDown;
    	glTranslatef(0.0f,1.0f,0.4f);
        gluLookAt(3,3,0.15 ,  0,1,0.4,  0,1,0);
         glPopMatrix();
        glRotatef(view_move_upDown,0.010f,0.0f,0.0f);

    }
    if(view_side_upDown == 0)
        view_move_upDown = 0;
    //gluLookAt(-10,-30,70.2,  0,0,0.4,  0,1,0);

    if(tex_off % 2 == 0){
    glBindTexture(GL_TEXTURE_2D,texture[0]);
    glBegin(GL_QUADS);
        for( x = 43; x > 1; x-- )
        {
            for( y = 43; y > 1; y-- )
            {
                float_x = float(x+sky*2*flag)/44.0f ;
                float_y = float(y+ count*2)/44.0f ;
                float_xb = float(x+1+sky*2*flag)/44.0f ;
                float_yb = float(y+1+ count*2)/44.0f ;



                glTexCoord2f( float_x, float_y);
                glVertex3f( points[x][y][0]*10, points[x][y][1]*10, points[x][y][2]*10 +0.7);

                glTexCoord2f( float_x, float_yb );
                glVertex3f( points[x][y+1][0]*10, points[x][y+1][1]*10, points[x][y+1][2]*10 +0.7);

                glTexCoord2f( float_xb, float_yb );
                glVertex3f( points[x+1][y+1][0]*10, points[x+1][y+1][1]*10, points[x+1][y+1][2]*10 +0.7);

                glTexCoord2f( float_xb, float_y );
                glVertex3f( points[x+1][y][0]*10, points[x+1][y][1]*10, points[x+1][y][2]*10 +0.7);

            }
        }
    glEnd();
    }
	glTranslatef(-0.0f,-0.0f,-10.0f);
    glBindTexture(GL_TEXTURE_2D,texture[2]);
    if(tex_off % 2 == 0){
    glBegin(GL_QUADS);
        for( x = 43; x > 1; x-- )
        {
            for( y = 43; y > 1; y-- )
            {
                float_x = float(x +sky*flag)/44.0f ;
                float_y = float(y+ count)/44.0f ;
                float_xb = float(x+1 +sky*flag)/44.0f ;
                float_yb = float(y+1+ count)/44.0f ;



                glTexCoord2f( float_x, float_y);
                glVertex3f( points[x][y][0]*10, points[x][y][1]*10, points[x][y][2]*5 +0.7);

                glTexCoord2f( float_x, float_yb );
                glVertex3f( points[x][y+1][0]*10, points[x][y+1][1]*10, points[x][y+1][2]*5 +0.7);

                glTexCoord2f( float_xb, float_yb );
                glVertex3f( points[x+1][y+1][0]*10, points[x+1][y+1][1]*10, points[x+1][y+1][2]*5 +0.7);

                glTexCoord2f( float_xb, float_y );
                glVertex3f( points[x+1][y][0]*10, points[x+1][y][1]*10, points[x+1][y][2]*5 +0.7);

            }
        }
    glEnd();
    }
    glDisable(GL_TEXTURE_2D);
    //glColor3d(1,0,0);
	glTranslatef(-0.0f,-0.0f,15.0f);

    glRotated(90,0,0,1);
    glRotated(180,1,0,0);

    if(flag == 1)
        glRotated(30,0,0,1);
    if(flag == -1)
        glRotated(-30,0,0,1);
    if(flag2 == 1)
        glRotated(-30,1,0,0);
    if(flag2 == -1)
        glRotated(30,1,0,0);
    glBegin(GL_TRIANGLES);
    for(int i = 0; i<vertices.size(); i++)
    {
        //printf("%f\n",vertices[i].y/100.0);
        if(vertices[i].z  >= 0)
            glColor3d(0.0,0.50,1.0);
        if(vertices[i].z  <  0)
            glColor3d(1.0,1.0,1.0);
        if(vertices[i].z  < 0)
            glColor3d(0.0,0.50,1.0);
        if((vertices[i].y/150.0  < -3.3)||(vertices[i].y/150.0  > 3.3))//swing of the flight
            glColor3d(1.0,1.0,1.0);
        if(vertices[i].x/150.0  <  -10)//喷射器
            glColor3d(1.0,1.0,1.0);

    glNormal3f( normals[i].x, normals[i].y, normals[i].z);
    glVertex3f( vertices[i].x/150.0, vertices[i].y/150.0, vertices[i].z/150.0 + 7);
    }

    glEnd();
    glColor3d(1,1,1);
/*
	glTranslatef(-0.0f,-0.0f,-10.0f);
    gluLookAt(0,0,0.2,  0,1,0.4,  0,1,0);
    glBindTexture(GL_TEXTURE_2D,texture[3]);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( -20.0f,  20.0f, 20.0f);	// 纹理和四边形的左下
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 20.0f,  20.0f,  20.0f);	// 纹理和四边形的右下
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 20.0f,  -20.0f,  20.0f);	// 纹理和四边形的右上
    glTexCoord2f(0.0f, 1.0f); glVertex3f( -20.0f,  -20.0f,  20.0f);
    glEnd();

*/  if(all_light_flag % 2 == 1)
        light_all();
    else
        light();
    //printf("%d--%d--%d--%d\n", glIsEnabled(GL_LIGHT0),glIsEnabled(GL_LIGHT1),glIsEnabled(GL_LIGHT2),glIsEnabled(GL_LIGHT3));
    light_ambient[1][0] = 0.30f;
    light_ambient[1][1] = 0.30f;
    light_ambient[1][2] = 0.30f;
    //printf("%d--%d--%d--\n", light_ambient[1][0],light_ambient[1][1],light_ambient[1][2]);
    for(int i = 0; i < 3; i ++)
        light_ambient[0][i]  = 1.0f - Clight;
    count ++;
    sky ++;
    if(Clight >= 1.3)
        light_flag = 0;
    if(Clight <= 0.0)
        light_flag = 1;

    if(light_flag == 1)
        Clight += 0.006;
    else
        Clight -= 0.006;
    if(sky > 44)
        sky = 0;
    if(count > 44)
        count = 0;
    //printf("%f\n", Clight);
    xRot += 0.3f;
    yRot += 0.2f;
    zRot += 0.4f;
    glutSwapBuffers();
}

void reshape(int w,int h)
{
    if (0 == h)
        h = 1;

    glViewport(0,0,(GLsizei)w,(GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f,(GLfloat)w / (GLfloat)h, 1,100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keyboard(unsigned char key,int x,int y)
{
    switch(key){
        case 'x':
            xRot += 1.0f;
            glutPostRedisplay();
            break;
        case 'y':
            yRot += 1.0f;
            glutPostRedisplay();
            break;
        case 'z':
            zRot += 1.0f;
            glutPostRedisplay();
            break;
        case 'a':
            flag = 1;
            glutPostRedisplay();
            break;
        case 's':
            flag = 0;
            glutPostRedisplay();
            break;
        case 'd':
            flag = -1;
            glutPostRedisplay();
            break;
        case '4':
            view_side_LR ++;
            glutPostRedisplay();
            break;
        case '5':
            view_side_LR = 0;
            glutPostRedisplay();
            break;
        case '6':
            view_side_LR --;
            glutPostRedisplay();
            break;
        case '1':
            view_side_upDown ++;
            glutPostRedisplay();
            break;
        case '2':
            view_side_upDown = 0;
            glutPostRedisplay();
            break;
        case '3':
            view_side_upDown --;
            glutPostRedisplay();
            break;
        case '0':
            all_light_flag ++;
            glutPostRedisplay();
            break;
        case '.':
            tex_off ++;
            glutPostRedisplay();
            break;
        case 'q':
            flag2 =  1;
            glutPostRedisplay();
            break;
        case 'w':
            flag2 =  0;
            glutPostRedisplay();
            break;
        case 'e':
            flag2 = -1;
            glutPostRedisplay();
            break;
        default:
            break;
    }
}

void fog(){
    bool gp; //check g
    GLuint filter;
    GLuint fogMode[] = {GL_EXP, GL_EXP2, GL_LINEAR};
    GLuint fogfilter = 0;//which fog will be used
    GLfloat fogColor[4] = {0.5f, 0.5f, 0.5f, 1.0f};//grep fog
    GLfloat f_start = -3.0f;
    GLfloat f_end = 3.0f;
    //glClearColor(0.5f, 0.5f, 0.5f, 1.0f);// useful?

    glFogi(GL_FOG_MODE, fogMode[fogfilter]);
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_DENSITY, 0.01f);
    glHint(GL_FOG_HINT, GL_DONT_CARE);
    glFogf(GL_FOG_START, f_start);
    glFogf(GL_FOG_END, f_end);
    glEnable(GL_FOG); // maybe can use in initializer_list
}


static void idle(void)
{
    glutPostRedisplay();
}
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
bool loadOBJ(char * path,std::vector<glm::vec3> & out_vertices,std::vector<glm::vec2> & out_uvs,std::vector<glm::vec3> & out_normals){
        printf("Loading OBJ file %s...\n", path);

        std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
        std::vector<glm::vec3> temp_vertices;
        std::vector<glm::vec2> temp_uvs;
        std::vector<glm::vec3> temp_normals;


        FILE * file = fopen(path, "r");
        if( file == NULL ){
                printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
                return false;
        }

        while( 1 ){

                char lineHeader[128];
                // read the first word of the line
                int res = fscanf(file, "%s", lineHeader);
                if (res == EOF)
                        break; // EOF = End Of File. Quit the loop.

                // else : parse lineHeader

                if ( strcmp( lineHeader, "v" ) == 0 ){
                    //cout<<"Get v"<<endl;
                        glm::vec3 vertex;
                        fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
                        temp_vertices.push_back(vertex);
                }else if ( strcmp( lineHeader, "vt" ) == 0 ){
                    //cout<<"Get vt"<<endl;
                        glm::vec2 uv;
                        fscanf(file, "%f %f\n", &uv.x, &uv.y );
                        uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
                        temp_uvs.push_back(uv);
                }else if ( strcmp( lineHeader, "vn" ) == 0 ){
                    //cout<<"Get vn"<<endl;
                        glm::vec3 normal;
                        fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
                        temp_normals.push_back(normal);
                }else if ( strcmp( lineHeader, "f" ) == 0 ){
                    //cout<<"Get f"<<endl;
                        std::string vertex1, vertex2, vertex3;
                        unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
                        int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1],&uvIndex[1], &normalIndex[1], &vertexIndex[2],&uvIndex[2], &normalIndex[2]);
                        if (matches != 9){
                                printf("File can't be read by our simple parser :-( Try exporting with other options\n");
                                return false;
                        }
                        vertexIndices.push_back(vertexIndex[0]);
                        vertexIndices.push_back(vertexIndex[1]);
                        vertexIndices.push_back(vertexIndex[2]);
                        uvIndices.push_back(normalIndex[0]);
                        uvIndices.push_back(normalIndex[1]);
                        uvIndices.push_back(normalIndex[2]);
                        normalIndices.push_back(normalIndex[0]);
                        normalIndices.push_back(normalIndex[1]);
                        normalIndices.push_back(normalIndex[2]);
                }else{
                        // Probably a comment, eat up the rest of the line
                        char stupidBuffer[1000];
                        fgets(stupidBuffer, 1000, file);
                }


        }

        // For each vertex of each triangle
        for( unsigned int i=0; i<vertexIndices.size(); i++ ){

                // Get the indices of its attributes
                unsigned int vertexIndex = vertexIndices[i];
                unsigned int normalIndex = normalIndices[i];
                unsigned int uvIndex = uvIndices[i];

                // Get the attributes thanks to the index
                glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
                glm::vec3 normal = temp_normals[ normalIndex-1 ];
                glm::vec2 uv = temp_uvs[ uvIndex-1 ];

                // Put the attributes in buffers
                out_vertices.push_back(vertex);
                out_normals .push_back(normal);
                out_uvs .push_back(uv);

        }

        return true;
}






int main(int argc,char** argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1000,500);
    glutInitWindowPosition(0,0);
    glutCreateWindow("Texture Map");
    loadOBJ("prt0003.obj", vertices, uvs, normals);
    init();
    //fog();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);


    glutMainLoop();


    return 0;
}


   /*
    glBegin(GL_QUADS);
    // Front Face
    // Bottom Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
    // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
    // Top Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
    // Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
    glEnd();

    glBindTexture(GL_TEXTURE_2D,texture[1]);
    glBegin(GL_QUADS);
    // Back Face
    // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
    // Top Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
    // Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
    // Bottom Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
    glEnd();

    glBindTexture(GL_TEXTURE_2D,texture[2]);
    glBegin(GL_QUADS);
    // Top Face
    // Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
    // Bottom Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
    // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
    // Top Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
    glEnd();

    glBindTexture(GL_TEXTURE_2D,texture[3]);
    glBegin(GL_QUADS);
    // Bottom Face
    // Top Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
    // Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
    // Bottom Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
    // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
    glEnd();

    glBindTexture(GL_TEXTURE_2D,texture[4]);
    glBegin(GL_QUADS);
    // Right face
    // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
    // Top Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
    // Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
    // Bottom Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
    glEnd();

    glBindTexture(GL_TEXTURE_2D,texture[5]);
    glBegin(GL_QUADS);
    // Left Face
    // Bottom Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
    // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
    // Top Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
    // Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
    glEnd();
    */
