#include "read_obj.h"


#include <fstream>
#include <sstream>
#include <iostream>

#include <algorithm>


ObjParser::ObjParser(std::string filename) :
	filename(filename) {}


ObjParser::~ObjParser() {

}

int ObjParser::Parse() {
	std::ifstream file(filename, std::ifstream::in);
	if (file.fail()) {
		return 1;
	}

	std::string line;
	while (std::getline(file, line)) {
		if (line.rfind("v ", 0) == 0) {
			auto tokens = Split(line, ' ');

			float4 ver {0,0,0,1};
			for (unsigned int i = 1; i < tokens.size(); i++) {
				ver[i - 1] = std::stof(tokens[i]);
			}
			vertexes.push_back(ver);

		} else if (line.rfind("f", ' ') == 0) {
			auto tokens = Split(line, ' ');

			float4 first;
			auto ind = std::stoi(Split(tokens[1], '/')[0]);
			first = vertexes[vertexes.size() + ind];

			float4 last;
			float4 curr;
			bool checked = false;

			for (auto i = 2; i < tokens.size(); i++) {
				if (tokens[i].size() != 0) {
					auto ind = std::stoi(Split(tokens[i], '/')[0]);

					curr = vertexes[vertexes.size() + ind];

					if (checked) {
						faces.push_back(face {first, last, curr});
					}

					last = curr;
					checked = true;
				}
			}

		}
	}

	return 0;
}

const std::vector<face> &ObjParser::GetFaces() {
	return faces;
}

std::vector<std::string> ObjParser::Split(const std::string &s, char delimiter) {
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(s);
	while (std::getline(tokenStream, token, delimiter)) {
		tokens.push_back(token);
	}
	return tokens;
}


ReadObj::ReadObj(unsigned short width, unsigned short height, std::string obj_file) : LineDrawing(width, height) {
	parser = new ObjParser(obj_file);
}

ReadObj::~ReadObj() {
	delete parser;
}

void ReadObj::DrawScene() {
	parser->Parse();

	float x_center = width / 2.0;
	float y_center = height / 2.0;
	float radius = std::min(x_center, y_center) - 40.0;

	for (auto face : parser->GetFaces()) {
		DrawLine(static_cast<unsigned short>(x_center + radius * face.vertexes[0].x),
				 static_cast<unsigned short>(y_center + radius * face.vertexes[0].y),
				 static_cast<unsigned short>(x_center + radius * face.vertexes[1].x),
				 static_cast<unsigned short>(y_center + radius * face.vertexes[1].y),
				 color(255, 0, 0));
		DrawLine(static_cast<unsigned short>(x_center + radius * face.vertexes[1].x),
				 static_cast<unsigned short>(y_center + radius * face.vertexes[1].y),
				 static_cast<unsigned short>(x_center + radius * face.vertexes[2].x),
				 static_cast<unsigned short>(y_center + radius * face.vertexes[2].y),
				 color(0, 255, 0));
		DrawLine(static_cast<unsigned short>(x_center + radius * face.vertexes[2].x),
				 static_cast<unsigned short>(y_center + radius * face.vertexes[2].y),
				 static_cast<unsigned short>(x_center + radius * face.vertexes[0].x),
				 static_cast<unsigned short>(y_center + radius * face.vertexes[0].y),
				 color(0, 0, 255));
	}
}
