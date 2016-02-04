/*
* shaders.c
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

#include "shaders.h"

#include <stddef.h>

#include <GL/glew.h>

/* ==================================================================== */
#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#elif defined _MSC_VER
#include <malloc.h>
#define alloca _alloca
#endif
#endif

/* ==================================================================== */
extern void DLOG(const char *);

/* ==================================================================== */
#define MULTILINE(...) #__VA_ARGS__

/* ==================================================================== */
static const char *vertex_shader_program = 
"#version 110\n"

MULTILINE(
    varying vec2 position;

    void main(void)
    {
        position = (gl_MultiTexCoord0.st - 0.5) * 5.0;
        gl_Position = ftransform();
    }
);

/* ==================================================================== */
static const char *fragment_shader_program = 
"#version 110\n"
"#extension GL_ARB_gpu_shader_fp64 : enable\n"

"#ifdef GL_ARB_gpu_shader_fp64\n"
"#define my_vec2 dvec2\n"
"#else\n"
"#define my_vec2 vec2\n"
"#endif\n"

MULTILINE(
    uniform sampler1D tex;
    uniform my_vec2 c;
    uniform int iter;
    uniform float zoom;
    uniform my_vec2 center;

    uniform bool z_plane;
    
    varying vec2 position;
    
    my_vec2 f(in my_vec2 z, in my_vec2 c) {
        return my_vec2(z.x * z.x - z.y * z.y,
                       2.0 * z.x * z.y) + c;
    }
    
    int julia(in my_vec2 z, in my_vec2 c) {
        for (int i = 0; i < iter; ++i) {
            if (dot(z, z) > 4.0)
                return i;
            z = f(z, c);
        }
        return 0;
    }
    
    void main(void)
    {
        my_vec2 z = position * zoom + center;
        int n = julia(z, z_plane ? c : z);
        gl_FragColor = texture1D( tex, (float(n) / 100.0) );
    }
);

/* ==================================================================== */
static GLuint program_object = 0;
static GLuint vertex_shader = 0;
static GLuint fragment_shader = 0;

static GLint uniform_parameter = -1;
static GLint uniform_zoom = -1;
static GLint uniform_center = -1;
static GLint uniform_iterations = -1;
static GLint uniform_z_plane = -1;

/* ==================================================================== */
static int
compile_shader(GLuint shader, const char *srccode)
{
    int compile_ok = GL_FALSE;

    glShaderSource( shader, 1, &srccode, NULL );
    glCompileShader( shader );
    glGetShaderiv( shader, GL_COMPILE_STATUS, &compile_ok );
    return (compile_ok == GL_TRUE);
}

/* ==================================================================== */
static int
link_program(GLuint program)
{
    int link_ok = GL_FALSE;

    glLinkProgram( program );
    glGetProgramiv( program, GL_LINK_STATUS, &link_ok );
    return (link_ok == GL_TRUE);
}

/* ==================================================================== */
static void
print_compile_error(GLuint shader)
{
    GLchar *buf = NULL;
    GLint length = 0;

    glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &length );
    buf = (GLchar *)alloca( length );
    glGetShaderInfoLog( shader, length, NULL, buf );

    DLOG( buf );
}

/* ==================================================================== */
static void
print_link_error(GLuint program)
{
    GLchar *buf = NULL;
    GLint length = 0;

    glGetProgramiv( program, GL_INFO_LOG_LENGTH, &length );
    buf = (GLchar *)alloca( length );
    glGetProgramInfoLog( program, length, NULL, buf );

    DLOG( buf );
}

/* ==================================================================== */
int
shaders_init(void)
{
    program_object = 0;
    vertex_shader = 0;
    fragment_shader = 0;

    uniform_parameter = -1;
    uniform_zoom = -1;
    uniform_center = -1;
    uniform_iterations = -1;

    if ( !(program_object = glCreateProgram( )) )
        goto cleanup;
    if ( !(vertex_shader = glCreateShader( GL_VERTEX_SHADER )) )
        goto cleanup;
    if ( !(fragment_shader = glCreateShader( GL_FRAGMENT_SHADER )) )
        goto cleanup;

    if ( !compile_shader( vertex_shader, vertex_shader_program ) ) {
        print_compile_error( vertex_shader );
        goto cleanup;
    }
    if ( !compile_shader( fragment_shader, fragment_shader_program ) ) {
        print_compile_error( fragment_shader );
        goto cleanup;
    }

    glAttachShader( program_object, vertex_shader );
    glAttachShader( program_object, fragment_shader );

    if ( !link_program( program_object ) ) {
        print_link_error( program_object );
        goto cleanup;
    }

    uniform_parameter = glGetUniformLocation( program_object, "c" );
    uniform_zoom = glGetUniformLocation( program_object, "zoom" );
    uniform_center = glGetUniformLocation( program_object, "center" );
    uniform_iterations = glGetUniformLocation( program_object, "iter" );
    uniform_z_plane = glGetUniformLocation( program_object, "z_plane" );

    return 1;

cleanup:
    shaders_close( );
    return 0;
}

/* ==================================================================== */
void
shaders_close(void)
{
    glDetachShader( program_object, fragment_shader );
    glDetachShader( program_object, vertex_shader );
    glDeleteShader( fragment_shader ); fragment_shader = 0;
    glDeleteShader( vertex_shader ); vertex_shader = 0;
    glDeleteProgram( program_object ); program_object = 0;
}

/* ==================================================================== */
GLuint
shaders_get_program(void)
{
    return program_object;
}

/* ==================================================================== */
void
shaders_set_iterations(int val)
{
    glUniform1i( uniform_iterations, val );
}

/* ==================================================================== */
void
shaders_set_zoom(float val)
{
    glUniform1f( uniform_zoom, val );
}

/* ==================================================================== */
void
shaders_set_center(double x, double y)
{
    if (GLEW_ARB_gpu_shader_fp64) {
        glUniform2d( uniform_center, x, y );
    }
    else {
        glUniform2f( uniform_center, (GLfloat)x, (GLfloat)y );
    }
}

/* ==================================================================== */
void
shaders_set_parameter(double cReal, double cImag)
{
    if (GLEW_ARB_gpu_shader_fp64) {
        glUniform2d( uniform_parameter, cReal, cImag );
    }
    else {
        glUniform2f( uniform_parameter, (GLfloat)cReal, (GLfloat)cImag );
    }
}

/* ==================================================================== */
void
shaders_set_z_plane(int val)
{
    glUniform1i( uniform_z_plane, !!val );
}
