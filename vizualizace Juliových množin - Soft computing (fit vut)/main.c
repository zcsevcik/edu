/*
 * main.c
 *
 * Author(s):   Ševčík Radek, Bc. <xsevci44@stud.fit.vutbr.cz>
 * Date:        2014/10/26
 *
 * Soft Computing (SFC)
 * Vizualizace Juliových množin
 *
 * Faculty of Information Technology
 * Brno University of Technology
 *
 * This file is part of sfc2014.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "params.h"
#include "palletes.h"
#include "shaders.h"

#if !defined(_MSC_VER)
#define sprintf_s(buf, size, ...) \
    sprintf(buf, __VA_ARGS__)
#endif

#define KEY_ESCAPE 27

// (a + bi) * (c + di) = (ac - bd) + (ad+bc)i
// (a + bi)**2 = (aa - bb) + (2ab)i
// (a + bi)**3 = (aaa - 3abb) + (3aab - bbb)i

typedef struct {
    int width;
    int height;
    char* title;
} glutWindow;

glutWindow win;

/* ==================================================================== */
void DLOG(const char *s)
{
#ifdef _WIN32
    MessageBoxA(NULL, s, "sfc2014", MB_OK | MB_ICONERROR);
#else
    fprintf(stderr, "%s\n", s);
#endif
}

/* ==================================================================== */
static GLuint display_list = 0;
static GLuint texture = 0;

static int i_param = 0;
static int i_pallete = 0;
static int iter = 250;
static float zoom = 1.0f / 1.2f;
static double centerX = 0.0f;
static double centerY = 0.0f;

static double cReal = 0.0;
static double cImag = 0.0;

static const float zoom_factor = 0.95f;
static const double c_factor = 0.05;

static int hold_rmb = 0;
static int c_x0 = 0;
static int c_y0 = 0;
static int c_move_factor = 1;

static int display_help = 1;
static int z_plane = 1;

/* ==================================================================== */
static void
display(void)
{
    glCallList(display_list);

    if (display_help) {
        static const unsigned char author[] =
            "    Julia set explorer\n"
            "2014 Sevcik Radek, Bc.\n";

        static const unsigned char fp64[] =
            "ARB_gpu_shader_fp64";

        static const unsigned char manual[] =
            "ESC: quit\n"
            "+/-: iterations\n"
            "1: predef. parameter\n"
            "2: predef. pallete\n"
            "3: switch origin\n"
            "LMB: centering\n"
            "MMB scroll: zoom\n"
            "RMB + move: parameter\n";

        static const char fmt[] =
            "parameter: [%g; %gi]\n"
            "center: [%g; %g]\n"
            "iterations: %d\n"
            "zoom: %g\n";

        unsigned char buf[255];

        sprintf_s((char *)buf, sizeof buf, fmt,
                  cReal, cImag,
                  centerX, centerY,
                  iter, 1.0f / zoom);

        GLuint program;
        glGetIntegerv(GL_CURRENT_PROGRAM, (GLint *)&program);
        
        glUseProgram(0);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_1D);

        glColor3f(0.0f, 1.0f, 0.0f);
        glWindowPos2i(2, 53);
        glutBitmapString(GLUT_BITMAP_9_BY_15, buf);

        glColor3f(0.0f, 1.0f, 0.0f);
        glWindowPos2i(win.width - 202, win.height - 15);
        glutBitmapString(GLUT_BITMAP_9_BY_15, author);

        glColor3f(0.0f, 1.0f, 0.0f);
        glWindowPos2i(win.width - 175, 5);
        glutBitmapString(GLUT_BITMAP_9_BY_15, fp64);

        glColor3f(0.0f, 1.0f, 0.0f);
        glWindowPos2i(2, win.height - 15);
        glutBitmapString(GLUT_BITMAP_9_BY_15, manual);

        glEnable(GL_TEXTURE_1D);
        glEnable(GL_LIGHTING);
        glUseProgram(program);
    }

    glutSwapBuffers();
}

/* ==================================================================== */
static void
initialize(void)
{
    glClearColor(0.0, 0.0, 0.0, 1.0);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_1D, texture);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexImage1D(GL_TEXTURE_1D, 0, 4, predefined_palletes[i_pallete].width, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, predefined_palletes[i_pallete].pixels);
    glEnable(GL_TEXTURE_1D);

    if (!shaders_init())
        exit(1);

    glUseProgram( shaders_get_program() );
    shaders_set_z_plane( z_plane );
    shaders_set_center(centerX, centerY);
    shaders_set_iterations(iter);
    shaders_set_zoom(zoom);
    shaders_set_parameter( cReal = predefined_parameters[i_param].cReal,
                           cImag = predefined_parameters[i_param].cImag );

    display_list = glGenLists(1);
    glNewList(display_list, GL_COMPILE);
      glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(-1.0f, -1.0f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(-1.0f, 1.0f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(1.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(1.0f, 1.0f);
      glEnd();
    glEndList();
}

/* ==================================================================== */
static void
resize(int w, int h)
{
    GLfloat aspect = (GLfloat)w / (GLfloat)h;
    glViewport(0, 0, win.width = w, win.height = h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //if (w > h)
    //    glOrtho(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0, -1.0, 1.0);
    //else
    //    glOrtho(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect, -1.0, 1.0);

    glOrtho(-aspect, aspect, -1.0, 1.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glScalef(aspect, aspect, 1.0);
}

/* ==================================================================== */
static void
mouse(int btn, int state, int mouseX, int mouseY)
{
    if (btn == 0x0003 && state == GLUT_DOWN) {
        zoom *= zoom_factor;
        shaders_set_zoom(zoom);
        glutPostRedisplay();
    }
    else if (btn == 0x0004 && state == GLUT_DOWN) {
        zoom /= zoom_factor;
        shaders_set_zoom(zoom);
        glutPostRedisplay();
    }
    else if (btn == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        double dx = (((double)mouseX / (double)win.width) - 0.5f) * 5.0f;
        double dy = (0.5f - ((double)mouseY / (double)win.height)) * 5.0f;

        dx = dx * zoom + centerX;
        dy = dy * zoom + centerY;

        shaders_set_center( centerX = dx, centerY = dy );
        glutPostRedisplay();
    }
    else if (btn == GLUT_RIGHT_BUTTON) {
        c_move_factor = 1;
        c_x0 = mouseX;
        c_y0 = mouseY;
        hold_rmb = (state == GLUT_DOWN);
    }
}

/* ==================================================================== */
static void
mousemove(int x, int y)
{
    double dx, dy;
    if (!hold_rmb) return;

    dx = (double)((x - c_x0) * c_move_factor) / (double)(2 * win.width);
    dy = (double)((y - c_y0) * c_move_factor) / (double)(2 * win.height);

    c_x0 = x;
    c_y0 = y;
    c_move_factor++;
    if (c_move_factor > 10) c_move_factor = 10;

    dx *= c_factor;
    dy *= c_factor;

    cReal += dx;
    cImag -= dy;

    shaders_set_parameter(cReal, cImag);
    glutPostRedisplay();
}

/* ==================================================================== */
static void
keyboard(unsigned char key, int mouseX, int mouseY)
{ 
    (void)mouseX;
    (void)mouseY;

    switch (key) {
    case KEY_ESCAPE:
        glutLeaveMainLoop();
        break;

    case '0':
        display_help = !display_help;
        glutPostRedisplay();
        break;

    case '1':
        ++i_param, (i_param %= predefined_parameters_size);
        if (!z_plane) {
            shaders_set_center( centerX = predefined_parameters[i_param].cReal,
                                centerY = predefined_parameters[i_param].cImag );
        }
        else {
            shaders_set_parameter( cReal = predefined_parameters[i_param].cReal,
                                   cImag = predefined_parameters[i_param].cImag);
        }
        glutPostRedisplay();
        break;

    case '2':
        ++i_pallete, (i_pallete %= predefined_palletes_size);
        glTexImage1D(
            GL_TEXTURE_1D, 0, 4, predefined_palletes[i_pallete].width, 0,
            GL_RGB, GL_UNSIGNED_BYTE, predefined_palletes[i_pallete].pixels);
        glutPostRedisplay();
        break;

    case '3':
        if (!z_plane) {
            shaders_set_parameter( cReal = centerX,
                                   cImag = centerY );
            shaders_set_center( centerX = 0.0,
                                centerY = 0.0 );
        }
        else {
            shaders_set_center( centerX = cReal, 
                                centerY = cImag );
            shaders_set_parameter( cReal = 0.0,
                                   cImag = 0.0 );
        }

        shaders_set_z_plane( z_plane = !z_plane );
        glutPostRedisplay();
        break;

    case '-':
        --iter;
        if (iter < 0)
            iter = 0;
        shaders_set_iterations(iter);
        glutPostRedisplay();
        break;

    case '+':
        ++iter;
        if (iter < 0)
            iter = 0;
        shaders_set_iterations(iter);
        glutPostRedisplay();
        break;

    default:
        break;
    }
}

/* ==================================================================== */
int main(int argc, char *argv[])
{
    GLenum err;

    win.width = 640;
    win.height = 480;
    win.title = "sfc2014 - Julia set explorer";

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE );
    glutInitWindowSize( win.width,win.height );
    glutCreateWindow( win.title );
    glutDisplayFunc( &display );
    glutReshapeFunc( &resize );
    glutKeyboardFunc( &keyboard );
    glutMouseFunc( &mouse );
    glutMotionFunc( &mousemove );

    err = glewInit( );
    if (GLEW_OK != err) {
        DLOG( (const char *)glewGetErrorString(err) );
        return 1;
    }

    if (!GLEW_VERSION_2_0) {
        DLOG( "OpenGL version 2.0 required!" );
        return 1;
    }

    initialize( );
    glutMainLoop( );

    shaders_close( );
    glDeleteTextures( 1, &texture );
    glDeleteLists( display_list, 1 );

    return 0;
}

/* ==================================================================== */
#ifdef _WIN32
#include <windows.h>

int CALLBACK
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
        char *lpCmdLine, int nCmdShow)
{
    return main(__argc, __argv);
}
#endif
