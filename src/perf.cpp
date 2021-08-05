#define DEBUG

#define ITERATION_COUNT 9999999

#define PERF_START\
\
	if (current_item_index >= ends.size())\
	{\
		ends.push_back(0);\
	}\
	else\
	{\
		ends[current_item_index] = 0;\
	}\
\
	start = clock64();\
\
	for (size_t i {}; i < iteration_count; ++i)\
	{

#define PERF_END\
	}\
\
	ends[current_item_index] = clock64() - start;\
\
	sum_i += ends[current_item_index];\
\
	++current_item_index;



#include <cstdint>
#include <iostream>
#include <cstring>
#include <cmath>
#include <vector>
#include <immintrin.h>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "denis-belov/xgk-math/src/data/quat/quat.h"
// #include <cstddef>



using std::cout;
using std::endl;
using std::vector;



extern "C" uint64_t clock64 ();



const char* vertex_shader_code =
R"(
	#version 300 es
	precision highp float;

	layout (location = 0) in vec3 vertex_data;

	uniform float perf[32];

	out float v_perf;

	void main (void)
	{
		int item_index = int(vertex_data.z);

		v_perf = perf[item_index];

		gl_Position = vec4(vertex_data.x, (vertex_data.y * perf[item_index] * 2.0f) - 1.0f, 0, 1);

		// gl_Position = vec4(vertex_data.x, vertex_data.y * perf[item_index], 0, 1);
	}
)";

const char* fragment_shader_code =
R"(
	#version 300 es
	precision highp float;

	in float v_perf;

	layout (location = 0) out vec4 output_color;

	void main (void)
	{
		output_color.rg = vec2(v_perf, 1.0f - v_perf);
	}
)";



// clock64 writes into 64-bit register so variables ends, start, sum_i must be of uint64_t, not size_t.
uint64_t start {};
uint64_t sum_i {};
vector<uint64_t> ends;
size_t current_item_index {};
size_t iteration_count { ITERATION_COUNT };
float sum_f {};



int main (void)
{
	if (!glfwInit())
	{
		return 1;
	}

	GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(primary_monitor);
	cout << "height:" << mode->height << endl;

	GLFWwindow* window = glfwCreateWindow(640, mode->height, "", nullptr, nullptr);

	if (!window)
	{
		glfwTerminate();

		return 1;
	}

	glfwMakeContextCurrent(window);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	glfwSwapInterval(0);

	gladLoadGL();

	glViewport(0, 0, 640, mode->height);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);



	vector<float> vertex_data;

	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_code, nullptr);
	glCompileShader(vertex_shader);

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_code, nullptr);
	glCompileShader(fragment_shader);

	GLuint program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);
	glUseProgram(program);

	glLineWidth(20.0f);



	// std::cout << glGetString(GL_VENDOR);



	glFinish();



	XGK::DATA::Quat q1 { 0, 0, 0, 1.0f };
	XGK::DATA::Quat q2 { 0, 0, 0, 1.0f };
	XGK::DATA::Quat q3 { 1.123f, 3.8f, 0.98f, 8.123f };
	XGK::DATA::Quat q4 { 2.4f, 1.536f, 10.78f, 545.878f };
	float axis [4] { 1.0f, 0.0f, 0.0f, 0.0f };

	q1.preRot32(axis, 0.5f);
	q2.preRot128(axis, 0.5f);

	q1.print();
	q2.print();

	return 0;



	// for (; 1; )
	for (size_t ii {}; ii < 999; ++ii)
	{
		PERF_START
			q1.preRot32(axis, 0.5f);
		PERF_END

		PERF_START
			q2.preRot128(axis, 0.5f);
		PERF_END

		// PERF_START
		// PERF_END

		// PERF_START
		// PERF_END



		if (vertex_data.size() == 0)
		{
			for (size_t i {}; i < current_item_index; ++i)
			{
				vertex_data.push_back((2.0f / float(current_item_index + 1) * float(i + 1)) - 1.0f);
				vertex_data.push_back(0.0f);
				vertex_data.push_back(float(i));
				vertex_data.push_back((2.0f / float(current_item_index + 1) * float(i + 1)) - 1.0f);
				vertex_data.push_back(1.0f);
				vertex_data.push_back(float(i));
			}

			glEnableVertexAttribArray(0);

			GLuint vertex_buffer;
			glGenBuffers(1, &vertex_buffer);
			glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
			glBufferData(GL_ARRAY_BUFFER, 4 * vertex_data.size(), vertex_data.data(), GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);
		}

		sum_f = ((float) sum_i);
		sum_i = 0;


		for (size_t item_index {}; item_index < current_item_index; ++item_index)
		{
			glUniform1f(item_index, (((float) ends[item_index]) / sum_f));
		}



		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_LINES, 0, current_item_index * 2);
		glfwSwapBuffers(window);

		current_item_index = 0;
	}

	glfwDestroyWindow(window);
	glfwTerminate();



	q1.print();
	q2.print();



	return 0;
}
