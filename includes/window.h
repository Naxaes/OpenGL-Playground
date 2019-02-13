//
// Created by Ted Klein Bergman on 10/31/18.
//

#pragma once

#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


struct OpenGLContextOptions
{
    int version_major = 3;
    int version_minor = 3;
    int profile = GLFW_OPENGL_CORE_PROFILE;
    int forward_compatible = GL_TRUE;
    int debug = GLFW_TRUE;
};

/*
---- Window related hints ----
GLFW_RESIZABLE specifies whether the windowed mode window will be resizable by the user. The window will still be
resizable using the glfwSetWindowSize function. This hint is ignored for full screen windows.

GLFW_VISIBLE specifies whether the windowed mode window will be initially visible. This hint is ignored for full screen
windows.

GLFW_DECORATED specifies whether the windowed mode window will have window decorations such as a border, a close widget,
etc. An undecorated window may still allow the user to generate close events on some platforms. This hint is ignored
for full screen windows.

GLFW_FOCUSED specifies whether the windowed mode window will be given input focus when created. This hint is ignored for
full screen and initially hidden windows.

GLFW_AUTO_ICONIFY specifies whether the full screen window will automatically iconify and restore the previous video
mode on input focus loss. This hint is ignored for windowed mode windows.

GLFW_FLOATING specifies whether the windowed mode window will be floating above other regular windows, also called
topmost or always-on-top. This is intended primarily for debugging purposes and cannot be used to implement proper
full screen windows. This hint is ignored for full screen windows.

GLFW_MAXIMIZED specifies whether the windowed mode window will be maximized when created. This hint is ignored for
full screen windows.

---- Framebuffer related hints ----
GLFW_RED_BITS, GLFW_GREEN_BITS, GLFW_BLUE_BITS, GLFW_ALPHA_BITS, GLFW_DEPTH_BITS and GLFW_STENCIL_BITS specify the
desired bit depths of the various components of the default framebuffer. GLFW_DONT_CARE means the application has no
preference.

GLFW_ACCUM_RED_BITS, GLFW_ACCUM_GREEN_BITS, GLFW_ACCUM_BLUE_BITS and GLFW_ACCUM_ALPHA_BITS specify the desired bit
depths of the various components of the accumulation buffer. GLFW_DONT_CARE means the application has no preference.
!!! Accumulation buffers are a legacy OpenGL feature and should not be used in new code. !!!

GLFW_AUX_BUFFERS specifies the desired number of auxiliary buffers. GLFW_DONT_CARE means the application has no
preference.
!!! Auxiliary buffers are a legacy OpenGL feature and should not be used in new code. !!!

GLFW_STEREO specifies whether to use stereoscopic rendering. This is a hard constraint.

GLFW_SAMPLES specifies the desired number of samples to use for multisampling. Zero disables multisampling.
GLFW_DONT_CARE means the application has no preference.

GLFW_SRGB_CAPABLE specifies whether the framebuffer should be sRGB capable. If supported, a created OpenGL context will
support the GL_FRAMEBUFFER_SRGB enable, also called GL_FRAMEBUFFER_SRGB_EXT) for controlling sRGB rendering and a
created OpenGL ES context will always have sRGB rendering enabled.

GLFW_DOUBLEBUFFER specifies whether the framebuffer should be double buffered. You nearly always want to use double
buffering. This is a hard constraint.

---- Monitor related hints ----
GLFW_REFRESH_RATE specifies the desired refresh rate for full screen windows. If set to GLFW_DONT_CARE, the highest
available refresh rate will be used. This hint is ignored for windowed mode windows.

---- Context related hints ----
GLFW_CLIENT_API specifies which client API to create the context for. Possible values are GLFW_OPENGL_API,
GLFW_OPENGL_ES_API and GLFW_NO_API. This is a hard constraint.

GLFW_CONTEXT_CREATION_API specifies which context creation API to use to create the context. Possible values are
GLFW_NATIVE_CONTEXT_API and GLFW_EGL_CONTEXT_API. This is a hard constraint. If no client API is requested, this hint
is ignored.

OS X: The EGL API is not available on this platform and requests to use it will fail.
Wayland, Mir: The EGL API is the native context creation API, so this hint will have no effect.
Note
An OpenGL extension loader library that assumes it knows which context creation API is used on a given platform may fail if you change this hint. This can be resolved by having it load via glfwGetProcAddress, which always uses the selected API.
Bug:
On some Linux systems, creating contexts via both the native and EGL APIs in a single process will cause the application to segfault. Stick to one API or the other on Linux for now.
GLFW_CONTEXT_VERSION_MAJOR and GLFW_CONTEXT_VERSION_MINOR specify the client API version that the created context must be compatible with. The exact behavior of these hints depend on the requested client API.

OpenGL: GLFW_CONTEXT_VERSION_MAJOR and GLFW_CONTEXT_VERSION_MINOR are not hard constraints, but creation will fail if the OpenGL version of the created context is less than the one requested. It is therefore perfectly safe to use the default of version 1.0 for legacy code and you will still get backwards-compatible contexts of version 3.0 and above when available.
While there is no way to ask the driver for a context of the highest supported version, GLFW will attempt to provide this when you ask for a version 1.0 context, which is the default for these hints.
OpenGL ES: GLFW_CONTEXT_VERSION_MAJOR and GLFW_CONTEXT_VERSION_MINOR are not hard constraints, but creation will fail if the OpenGL ES version of the created context is less than the one requested. Additionally, OpenGL ES 1.x cannot be returned if 2.0 or later was requested, and vice versa. This is because OpenGL ES 3.x is backward compatible with 2.0, but OpenGL ES 2.0 is not backward compatible with 1.x.
GLFW_OPENGL_FORWARD_COMPAT specifies whether the OpenGL context should be forward-compatible, i.e. one where all functionality deprecated in the requested version of OpenGL is removed. This must only be used if the requested OpenGL version is 3.0 or above. If OpenGL ES is requested, this hint is ignored.

Forward-compatibility is described in detail in the OpenGL Reference Manual.
GLFW_OPENGL_DEBUG_CONTEXT specifies whether to create a debug OpenGL context, which may have additional error and performance issue reporting functionality. If OpenGL ES is requested, this hint is ignored.

GLFW_OPENGL_PROFILE specifies which OpenGL profile to create the context for. Possible values are one of GLFW_OPENGL_CORE_PROFILE or GLFW_OPENGL_COMPAT_PROFILE, or GLFW_OPENGL_ANY_PROFILE to not request a specific profile. If requesting an OpenGL version below 3.2, GLFW_OPENGL_ANY_PROFILE must be used. If OpenGL ES is requested, this hint is ignored.

OpenGL profiles are described in detail in the OpenGL Reference Manual.
GLFW_CONTEXT_ROBUSTNESS specifies the robustness strategy to be used by the context. This can be one of GLFW_NO_RESET_NOTIFICATION or GLFW_LOSE_CONTEXT_ON_RESET, or GLFW_NO_ROBUSTNESS to not request a robustness strategy.

GLFW_CONTEXT_RELEASE_BEHAVIOR specifies the release behavior to be used by the context. Possible values are one of GLFW_ANY_RELEASE_BEHAVIOR, GLFW_RELEASE_BEHAVIOR_FLUSH or GLFW_RELEASE_BEHAVIOR_NONE. If the behavior is GLFW_ANY_RELEASE_BEHAVIOR, the default behavior of the context creation API will be used. If the behavior is GLFW_RELEASE_BEHAVIOR_FLUSH, the pipeline will be flushed whenever the context is released from being the current one. If the behavior is GLFW_RELEASE_BEHAVIOR_NONE, the pipeline will not be flushed on release.

Context release behaviors are described in detail by the GL_KHR_context_flush_control extension.
GLFW_CONTEXT_NO_ERROR specifies whether errors should be generated by the context. If enabled, situations that would have generated errors instead cause undefined behavior.

The no error mode for OpenGL and OpenGL ES is described in detail by the GL_KHR_no_error extension.

Hint - Default - Options

GLFW_RESIZABLE	GLFW_TRUE	GLFW_TRUE or GLFW_FALSE
GLFW_VISIBLE	GLFW_TRUE	GLFW_TRUE or GLFW_FALSE
GLFW_DECORATED	GLFW_TRUE	GLFW_TRUE or GLFW_FALSE
GLFW_FOCUSED	GLFW_TRUE	GLFW_TRUE or GLFW_FALSE
GLFW_AUTO_ICONIFY	GLFW_TRUE	GLFW_TRUE or GLFW_FALSE
GLFW_FLOATING	GLFW_FALSE	GLFW_TRUE or GLFW_FALSE
GLFW_MAXIMIZED	GLFW_FALSE	GLFW_TRUE or GLFW_FALSE
GLFW_RED_BITS	8	0 to INT_MAX or GLFW_DONT_CARE
GLFW_GREEN_BITS	8	0 to INT_MAX or GLFW_DONT_CARE
GLFW_BLUE_BITS	8	0 to INT_MAX or GLFW_DONT_CARE
GLFW_ALPHA_BITS	8	0 to INT_MAX or GLFW_DONT_CARE
GLFW_DEPTH_BITS	24	0 to INT_MAX or GLFW_DONT_CARE
GLFW_STENCIL_BITS	8	0 to INT_MAX or GLFW_DONT_CARE
GLFW_ACCUM_RED_BITS	0	0 to INT_MAX or GLFW_DONT_CARE
GLFW_ACCUM_GREEN_BITS	0	0 to INT_MAX or GLFW_DONT_CARE
GLFW_ACCUM_BLUE_BITS	0	0 to INT_MAX or GLFW_DONT_CARE
GLFW_ACCUM_ALPHA_BITS	0	0 to INT_MAX or GLFW_DONT_CARE
GLFW_AUX_BUFFERS	0	0 to INT_MAX or GLFW_DONT_CARE
GLFW_SAMPLES	0	0 to INT_MAX or GLFW_DONT_CARE
GLFW_REFRESH_RATE	GLFW_DONT_CARE	0 to INT_MAX or GLFW_DONT_CARE
GLFW_STEREO	GLFW_FALSE	GLFW_TRUE or GLFW_FALSE
GLFW_SRGB_CAPABLE	GLFW_FALSE	GLFW_TRUE or GLFW_FALSE
GLFW_DOUBLEBUFFER	GLFW_TRUE	GLFW_TRUE or GLFW_FALSE
GLFW_CLIENT_API	GLFW_OPENGL_API	GLFW_OPENGL_API, GLFW_OPENGL_ES_API or GLFW_NO_API
GLFW_CONTEXT_CREATION_API	GLFW_NATIVE_CONTEXT_API	GLFW_NATIVE_CONTEXT_API or GLFW_EGL_CONTEXT_API
GLFW_CONTEXT_VERSION_MAJOR	1	Any valid major version number of the chosen client API
GLFW_CONTEXT_VERSION_MINOR	0	Any valid minor version number of the chosen client API
GLFW_CONTEXT_ROBUSTNESS	GLFW_NO_ROBUSTNESS	GLFW_NO_ROBUSTNESS, GLFW_NO_RESET_NOTIFICATION or GLFW_LOSE_CONTEXT_ON_RESET
GLFW_CONTEXT_RELEASE_BEHAVIOR	GLFW_ANY_RELEASE_BEHAVIOR	GLFW_ANY_RELEASE_BEHAVIOR, GLFW_RELEASE_BEHAVIOR_FLUSH or GLFW_RELEASE_BEHAVIOR_NONE
GLFW_OPENGL_FORWARD_COMPAT	GLFW_FALSE	GLFW_TRUE or GLFW_FALSE
GLFW_OPENGL_DEBUG_CONTEXT	GLFW_FALSE	GLFW_TRUE or GLFW_FALSE
GLFW_OPENGL_PROFILE	GLFW_OPENGL_ANY_PROFILE	GLFW_OPENGL_ANY_PROFILE, GLFW_OPENGL_COMPAT_PROFILE or GLFW_OPENGL_CORE_PROFILE
 */


class Window
{
public:
    static Window create(int width, int height, const std::string& title, OpenGLContextOptions options = {})
    {
        glfwInit();

        // OpenGL context
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, options.version_major);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, options.version_minor);
        glfwWindowHint(GLFW_OPENGL_PROFILE,        options.profile);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, options.forward_compatible);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,  options.debug);

        // Other
        glfwWindowHint(GLFW_SAMPLES, 4);  // ONLY WORK ON DEFAULT RENDER TARGET! NOT OUR OWN FRAMEBUFFERS!


        GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (window == nullptr)
        {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }
        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            throw std::runtime_error("Failed to initialize GLAD");

        // GLCALL(glEnable(GL_MULTISAMPLE));

        return {window, width, height};
    }

    GLFWwindow* id;
    const int width;
    const int height;

    Window(GLFWwindow* id, int width, int height) : id(id), width(width), height(height) {}
};


//     glfwSetFramebufferSizeCallback(window.id, framebuffer_size_callback);
//     glfwSetCursorPosCallback(window.id, mouse_callback);
//     glfwSetScrollCallback(window.id, scroll_callback);
//     glfwSetInputMode(window.id, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


Window Create(int width, int height, const std::string& title, OpenGLContextOptions options = {})
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, options.version_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, options.version_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE,        options.profile);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, options.forward_compatible);

    GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (window == NULL)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw std::runtime_error("Failed to initialize GLAD");


    return {window, width, height};
}

