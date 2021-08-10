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
#include "denis-belov/xgk-math/src/quat/quat.h"
#include "denis-belov/xgk-math/src/mat4/mat4.h"
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



	XGK::MATH::Quat q1 { 0.0f, 0.0f, 0.0f, 1.0f };
	XGK::MATH::Quat q2 { 0.0f, 0.0f, 0.0f, 1.0f };
	XGK::MATH::Quat q3 { 1.123f, 3.8f, 0.98f, 8.123f };
	XGK::MATH::Quat q4 { 2.4f, 1.536f, 10.78f, 545.878f };
	float axis [4] { 1.0f, 0.0f, 0.0f, 0.0f };

	XGK::MATH::Mat4 m1
	{
		0.0f, 4.0f, 8.0f , 12.0f,
		1.0f, 5.0f, 9.0f , 13.0f,
		2.0f, 6.0f, 10.0f, 14.0f,
		3.0f, 7.0f, 11.0f, 15.0f,
	};

	m1.print();

	XGK::MATH::Mat4 m2;

	m1.invns32();
	m2.invns128();

	m1.print();
	m2.print();

	// q1.preRot32(axis, 0.5f);
	// q2.preRot128(axis, 0.5f);

	// q1.print();
	// q2.print();

	// return 0;



	// float BezierCubicCurve (const float time, const float x1, const float y1, const float x2, const float y2)
	// {
	// 	// B(t) = (1-t)^3*p0 + 3*t*(1-t)^2*p1 + 3*t^2*(1-t)*p2 + t^3*p3
	// 	// x(t) = (1-t)^3*x0 + 3*t*(1-t)^2*x1 + 3*t^2*(1-t)*x2 + t^3*x3
	// 	// y(t) = (1-t)^3*y0 + 3*t*(1-t)^2*y1 + 3*t^2*(1-t)*y2 + t^3*y3
	// 	// p0 = vec2(0,0), p3 = vec2(1,1)
	// 	// x(t) = 3*t*(1-t)^2*x1 + 3*t^2*(1-t)*x2 + t^3
	// 	const float a = 3 * x1 - 3 * x2 + 1;
	// 	const float b = 3 * x2 - 6 * x1;
	// 	const float c = 3 * x1;
	// 	const float d = - time;
	// 	// x(t) = a*t^3 + b*t^2 + c*t
	// 	// x(t) = time => a*t^3 + b*t^2 + c*t - d = 0
	// 	const float A = b / a;
	// 	const float B = c / a;
	// 	const float C = d / a;
	// 	const float Q = ( 3 * B - A*A ) / 9;
	// 	const float R = ( 9 * A * B - 27 * C - 2 * A*A*A ) / 54;
	// 	const float D = Q*Q*Q + R*R; // discriminant
	// 	float t;

	// 	if ( D >= 0 )
	// 	{
	// 		// float asd = 0;
	// 		// if (R + sqrt( D ) > 0) {

	// 		// 	asd = 1;
	// 		// }
	// 		// else if (R + sqrt( D ) < 0) {

	// 		// 	asd = -1;
	// 		// }

	// 		// float qwe = 0;
	// 		// if (R - sqrt( D ) > 0) {

	// 		// 	qwe = 1;
	// 		// }
	// 		// else if (R - sqrt( D ) < 0) {

	// 		// 	qwe = -1;
	// 		// }

	// 		// const float S = asd * pow( abs( R + sqrt( D ) ), 1 / 3 );
	// 		// const float T = qwe * pow( abs( R - sqrt( D ) ), 1 / 3 );
	// 		const float sqrtD = sqrt( D );
	// 		const float S = cbrt(R + sqrtD);
	// 		const float T = cbrt(R - sqrtD);
	// 		t = - A / 3 + ( S + T );
	// 	}
	// 	else
	// 	{
	// 		const float th = acos( R / sqrt( - pow( Q, 3 ) ) );
	// 		const float t1 = 2 * sqrt( - Q ) * cos( th / 3 ) - A / 3;
	// 		const float t2 = 2 * sqrt( - Q ) * cos( th / 3 + 2 * M_PI / 3 ) - A / 3;
	// 		const float t3 = 2 * sqrt( - Q ) * cos( th / 3 - 2 * M_PI / 3 ) - A / 3;
	// 		if ( t1 >= 0 && t1 <= 1 ) t = t1;
	// 		else if ( t2 >= 0 && t2 <= 1 ) t = t2;
	// 		else if ( t3 >= 0 && t3 <= 1 ) t = t3;
	// 	}
	// 	float y;
	// 	if ( t == 0 ) y = 0;
	// 	else if ( t == 1 ) y = 1;
	// 	else y = 3 * t * pow( 1 - t, 2 ) * y1 + 3 * t*t * ( 1 - t ) * y2 + t*t*t;
	// 	return y;
	// };

	// float a = 0.0f;
	// float b = 0.0f;

	// for (size_t i = 0; i < 9999999; ++i)
	// {
	// 	b += BezierCubicCurve(((float) i) / 9999999.0f, .2,.97,.82,-0.97);
	// }

	// for (size_t i = 0; i < 9999999; ++i)
	// {
	// 	a += curve_values[uint64_t(((float) i) / 9999999.0f * 1000000.0f)];
	// }



	// for (; 1; )
	for (size_t ii {}; ii < 999; ++ii)
	{
		PERF_START
			m1.invns32();
			// q1.preRot32(axis, 0.5f);
		PERF_END

		PERF_START
			m2.invns128();
			// q2.preRot128(axis, 0.5f);
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

	m1.print();
	m2.print();



	return 0;
}
