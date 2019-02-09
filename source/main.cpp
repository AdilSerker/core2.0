#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <ctime>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
GLFWwindow *window = nullptr;

#include <glm/glm.hpp>
using namespace glm;
using namespace std;

#include "Scene.h"
#include "Camera.h"

Scene *scene = nullptr;

#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ucontext.h>
#include <unistd.h>

/* This structure mirrors the one found in /usr/include/asm/ucontext.h */
typedef struct _sig_ucontext
{
	unsigned long uc_flags;
	struct ucontext *uc_link;
	stack_t uc_stack;
	struct sigcontext uc_mcontext;
	sigset_t uc_sigmask;
} sig_ucontext_t;

void crit_err_hdlr(int sig_num, siginfo_t *info, void *ucontext)
{
	void *array[50];
	void *caller_address;
	char **messages;
	int size, i;
	sig_ucontext_t *uc;

	uc = (sig_ucontext_t *)ucontext;

	/* Get the address at the time the signal was raised */
#if defined(__i386__)							  // gcc specific
	caller_address = (void *)uc->uc_mcontext.eip; // EIP: x86 specific
#elif defined(__x86_64__)						  // gcc specific
	caller_address = (void *)uc->uc_mcontext.rip; // RIP: x86_64 specific
#else
#error Unsupported architecture. // TODO: Add support for other arch.
#endif

	fprintf(stderr, "signal %d (%s), address is %p from %p\n",
			sig_num, strsignal(sig_num), info->si_addr,
			(void *)caller_address);

	size = backtrace(array, 50);

	/* overwrite sigaction with caller's address */
	array[1] = caller_address;

	messages = backtrace_symbols(array, size);

	/* skip first stack frame (points here) */
	for (i = 1; i < size && messages != NULL; ++i)
	{
		fprintf(stderr, "[bt]: (%d) %s\n", i, messages[i]);
	}

	free(messages);

	exit(EXIT_FAILURE);
}
void initWindow()
{
	if (!glfwInit())
	{
		fprintf(stderr, "GLFW Initialization error\n");
		exit(-1);
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	int WINDOW_WIDTH = mode->width;
	int WINDOW_HEIGHT = mode->height;

	window = glfwCreateWindow(
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		"OpenGL template",
		glfwGetPrimaryMonitor(),
		NULL);
	if (window == NULL)
	{
		fprintf(stderr, "Window initialization error\n");
		glfwTerminate();
		exit(-1);
	}

	std::cout << mode->width << "x" << mode->height << "\n";
	glfwMakeContextCurrent(window);

	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "GLEW Initialization error\n");
		exit(-1);
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	glClearColor(0.5f, 0.5f, 0.5f, 0.5f);

	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);

	// #ifndef __APPLE__
	// glDebugMessageCallback(GLUtils::debugCallback, NULL);
	// glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
	// glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0, GL_DEBUG_SEVERITY_NOTIFICATION, -1 , "Start debugging");
	// #endif
}

void mainLoop()
{
	while (!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		scene->render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete scene;
	glfwTerminate();
}

int main()
{
	struct sigaction sigact;

	sigact.sa_sigaction = crit_err_hdlr;
	sigact.sa_flags = SA_RESTART | SA_SIGINFO;

	if (sigaction(SIGSEGV, &sigact, (struct sigaction *)NULL) != 0)
	{
		fprintf(stderr, "error setting signal handler for %d (%s)\n",
				SIGSEGV, strsignal(SIGSEGV));

		exit(EXIT_FAILURE);
	}

	initWindow();

	scene = new Scene();
	scene->initScene();

	mainLoop();

	return 0;
}
