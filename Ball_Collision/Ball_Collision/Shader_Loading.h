#pragma once

#include <iostream>
#include <fstream>
#include <string>
using namespace std;
std::string Load_Shader(std::string shader_name){

	std::string shader_file_name = shader_name + ".shader";
	ifstream shader_file (shader_file_name);
	
	if (!shader_file) cout << "Error Loading Shader!" << endl;
	else cout << shader_name << " Loaded" << endl;

	std::string shader((std::istreambuf_iterator<char>(shader_file)),
		(std::istreambuf_iterator<char>()));
	return shader;
}