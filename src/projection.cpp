#include "projection.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>


Projection::Projection(unsigned short width, unsigned short height, std::string obj_file) : ReadObj(width, height, obj_file) {}

Projection::~Projection() {}

void Projection::DrawScene() {
	parser->Parse();

	x_center = width / 2.0;
	y_center = height / 2.0;

	float rot_angle = 30.0 * M_PI / 180.0;
	float fov_y_angle = 90.0 * M_PI / 180.0;
	float z_far = 10, z_near = 1;

	float4x4 rotate {
		{1, 0, 0, 0},
		{0, cos(rot_angle), -sin(rot_angle), 0},
		{0, sin(rot_angle), cos(rot_angle), 0},
		{0, 0, 0, 1}
	};

	float4x4 translate {
		{1, 0, 0, 0},
		{0, 1, 0, 0},
		{0, 0, 1, 0},
		{0, 0, 10, 1}
	};

	float4x4 world = linalg::mul(translate, rotate);

	float3 eye {0, 0, 0};
	float3 look_at {0, 0, 1};
	float3 up {0, 1, 0};

	float3 z_axis = linalg::normalize(eye - look_at);
	float3 x_axis = linalg::normalize(linalg::cross(up, z_axis));
	float3 y_axis = linalg::cross(x_axis, z_axis);

	float4x4 view {
		{x_axis.x, x_axis.y, x_axis.z, -linalg::dot(x_axis, eye)},
		{y_axis.x, y_axis.y, y_axis.z, -linalg::dot(y_axis, eye)},
		{z_axis.x, z_axis.y, z_axis.z, -linalg::dot(z_axis, eye)},
		{0, 0, 0, 1}
	};
	// view = linalg::transpose(view);

	// float scale = 0.5;
	float y_scale = 1 / tan(fov_y_angle / 2);
	float x_scale = y_scale * height / width;

	float4x4 projection1 {
		{x_scale, 0, 0, 0},
		{0, y_scale, 0, 0},
		{0, 0, (z_far - z_near), -z_near * z_far / (z_far - z_near)},
		{0, 0, 1, 0}
	};
	float4x4 projection {
		{x_scale, 0, 0, 0},
		{0, y_scale, 0, 0},
		{0, 0, z_far / (z_far - z_near), 1},
		{0, 0, -z_near * z_far / (z_far - z_near), 0}
	};
	projection = linalg::transpose(projection);

	world = linalg::mul(projection, view, world);

	for (auto face : parser->GetFaces()) {
		float4 translated[3];
		for (unsigned int i = 0; i < 3; i++) {
			translated[i] = linalg::mul(world, face.vertexes[i]);
		}
		DrawTriangle(translated, color(255, 255, 255));
	}
}

void Projection::DrawTriangle(float4 triangle[3], color color) {
	DrawLine(static_cast<unsigned short>(x_center + x_center * triangle[0].x),
			 static_cast<unsigned short>(y_center - y_center * triangle[0].y),
			 static_cast<unsigned short>(x_center + x_center * triangle[1].x),
			 static_cast<unsigned short>(y_center - y_center * triangle[1].y),
			 color);
	DrawLine(static_cast<unsigned short>(x_center + x_center * triangle[1].x),
			 static_cast<unsigned short>(y_center - y_center * triangle[1].y),
			 static_cast<unsigned short>(x_center + x_center * triangle[2].x),
			 static_cast<unsigned short>(y_center - y_center * triangle[2].y),
			 color);
	DrawLine(static_cast<unsigned short>(x_center + x_center * triangle[2].x),
			 static_cast<unsigned short>(y_center - y_center * triangle[2].y),
			 static_cast<unsigned short>(x_center + x_center * triangle[0].x),
			 static_cast<unsigned short>(y_center - y_center * triangle[0].y),
			 color);
}
