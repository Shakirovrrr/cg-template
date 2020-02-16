#include "projection.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>


Projection::Projection(unsigned short width, unsigned short height, std::string obj_file) : ReadObj(width, height, obj_file) {}

Projection::~Projection() {}

std::ostream &operator<<(std::ostream &os, const float4x4 &mat) {
	for (char i = 0; i < 4; i++) {
		for (char j = 0; j < 4; j++) {
			std::cout << mat[i][j] << ' ';
		}
		std::cout << std::endl;
	}

	return os;
}

#if 1
void Projection::DrawScene() {
	parser->Parse();

	x_center = width / 2.0f;
	y_center = height / 2.0f;
	float radius = std::min(x_center, y_center) - 40.0f;

	float3 rotAngleXYZ = {0, 0, 0};
	rotAngleXYZ.x *= M_PI / 180.0f;
	rotAngleXYZ.y *= M_PI / 180.0f;
	rotAngleXYZ.z *= M_PI / 180.0f;

	float fovAngleY = 60.0 * M_PI / 180.0f;

	float zFarPlane = 20, zNearPlane = 1;

#pragma region World
	// Translate
	float4x4 translate = linalg::translation_matrix(float3 {0, 0, 0});

	// Scale
	float4x4 scale = linalg::scaling_matrix(float3 {1, 1, 1});

	// Rotate
	float4x4 rotateX {
		{1, 0, 0, 0},
		{0, cos(rotAngleXYZ.x), -sin(rotAngleXYZ.x), 0},
		{0, sin(rotAngleXYZ.x), cos(rotAngleXYZ.x), 0},
		{0, 0, 0, 1}
	}; rotateX = linalg::transpose(rotateX);

	float4x4 rotateY {
		{cos(rotAngleXYZ.y), 0, sin(rotAngleXYZ.y), 0},
		{0, 1, 0, 0},
		{-sin(rotAngleXYZ.y), 0, cos(rotAngleXYZ.y), 0},
		{0, 0, 0, 1}
	}; rotateY = linalg::transpose(rotateY);

	float4x4 rotateZ {
		{cos(rotAngleXYZ.z), -sin(rotAngleXYZ.z), 0, 0},
		{sin(rotAngleXYZ.z), cos(rotAngleXYZ.z), 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1}
	}; rotateZ = linalg::transpose(rotateZ);

	// Combine
	float4x4 rotateXYZ = linalg::mul(rotateX, rotateY, rotateZ);
	float4x4 world = linalg::mul(translate, rotateXYZ, scale);
#pragma endregion

#pragma region Camera
	float3 vEye {0, 0, 0};
	float3 vAt {0, 0, 1};
	float3 vUp {0, 1, 0};

	float3 zAxis = linalg::normalize(vEye - vAt);
	float3 xAxis = linalg::normalize(linalg::cross(vUp, zAxis));
	float3 yAxis = linalg::cross(zAxis, xAxis);

	float4x4 view = {
		{xAxis.x, yAxis.x, zAxis.x, 0},
		{xAxis.y, yAxis.y, zAxis.y, 0},
		{xAxis.z, yAxis.z, zAxis.z, 0},
		{-linalg::dot(xAxis, vEye), -linalg::dot(yAxis, vEye), -linalg::dot(zAxis, vEye), 1}
	}; view = linalg::transpose(view);

	float yScale = 1 / tan(fovAngleY / 2);
	float xScale = yScale * (height / width);

	float4x4 projection {
		{xScale, 0, 0, 0},
		{0, yScale, 0, 0},
		{0, 0, (zFarPlane + zNearPlane) / (zFarPlane - zNearPlane), -1},
		{0, 0, -(zNearPlane * zFarPlane) / (zFarPlane - zNearPlane), 0}
	};
#pragma endregion

	float4x4 interpolator = linalg::mul(projection, view, world);

	for (auto face : parser->GetFaces()) {
		float4 translated[3];
		for (unsigned char i = 0; i < 3; i++) {
			translated[i] = linalg::mul(interpolator, face.vertexes[i]);
		}
		DrawTriangle(translated, color(255, 255, 255));
	}
}
#endif

#if 0
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

	float4x4 world = translate * rotate; // linalg::mul?

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
	float4x4 scale {
		{x_scale, 0, 0, 0},
		{0, y_scale, 0, 0},
		{0, 0, z_far / (z_far - z_near), 1},
		{0, 0, -z_near * z_far / (z_far - z_near), 0}
	};
	scale = linalg::transpose(scale);

	world = linalg::mul(scale, view, world);

	for (auto face : parser->GetFaces()) {
		float4 translated[3];
		for (unsigned int i = 0; i < 3; i++) {
			translated[i] = linalg::mul(world, face.vertexes[i]);
		}
		DrawTriangle(translated, color(255, 255, 255));
	}
	}
#endif

void Projection::DrawTriangle(float4 triangle[3], color mColor) {
	DrawLine(static_cast<unsigned short>(x_center + x_center * triangle[0].x),
			 static_cast<unsigned short>(y_center - y_center * triangle[0].y),
			 static_cast<unsigned short>(x_center + x_center * triangle[1].x),
			 static_cast<unsigned short>(y_center - y_center * triangle[1].y),
			 color(255, 0, 0));
	DrawLine(static_cast<unsigned short>(x_center + x_center * triangle[1].x),
			 static_cast<unsigned short>(y_center - y_center * triangle[1].y),
			 static_cast<unsigned short>(x_center + x_center * triangle[2].x),
			 static_cast<unsigned short>(y_center - y_center * triangle[2].y),
			 color(0, 255, 0));
	DrawLine(static_cast<unsigned short>(x_center + x_center * triangle[2].x),
			 static_cast<unsigned short>(y_center - y_center * triangle[2].y),
			 static_cast<unsigned short>(x_center + x_center * triangle[0].x),
			 static_cast<unsigned short>(y_center - y_center * triangle[0].y),
			 color(0, 0, 255));
}
