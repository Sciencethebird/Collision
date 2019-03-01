#pragma once


#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <vector>
#include "Window_Setting.h"

#define PI 3.14159265

class Base_Object {
public:

	Base_Object() {}

	void show() {

		unsigned int color_offset = 2 * sizeof(float);

		// Link Buffer with data array
		glBufferData(GL_ARRAY_BUFFER, data_buffer_size * sizeof(float), data_buffer, GL_STATIC_DRAW); // init buffer size, drawing method

		// Position Buffer
		glEnableVertexAttribArray(0); // you need to enable the attrib_array to make the following code work
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0); // Tells GPU how to eat data

		// Color Buffer
		glEnableVertexAttribArray(1); // you need to enable the attrib_array to make the following code work
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)color_offset); // Tells GPU how to eat data

		// Draw
		glDrawArrays(draw_type, 0, number_of_verterx);
	}

	void set_data(float *db, int nov, unsigned int dt) {
		data_buffer = db;
		number_of_verterx = nov;
		data_buffer_size = nov * 6;
		draw_type = dt;
	}

private:

	float* data_buffer = nullptr;
	int data_buffer_size = 0;
	int number_of_verterx = 0;
	unsigned int draw_type = 0;

};




class Polygon : public Base_Object {
public:
	Polygon() {}
	Polygon(float center_x, float center_y, float radii, int number_of_side) {

		number_of_vertices = number_of_side + 2;
		buffer_size = number_of_vertices * 6;
		this->radii = radii;

		float doublePI = (float)M_PI * 2;
		float *temp_buffer = new float[buffer_size];

		for (int i = 0; i < buffer_size; i += 6) {

			temp_buffer[i] = (center_x + radii * cos((i / 6) * (doublePI / number_of_side))) / HALF_WIDTH;
			temp_buffer[i + 1] = (center_y + radii * sin((i / 6) * (doublePI / number_of_side))) / HALF_HEIGHT;
			temp_buffer[i + 2] = 0.0;
			temp_buffer[i + 3] = 0.0;
			temp_buffer[i + 4] = 1.0;
			temp_buffer[i + 5] = 1.0;
		}
		data_buffer = temp_buffer;

		set_data(data_buffer, number_of_vertices, GL_TRIANGLE_FAN);
	}

	void change_color(float r, float g, float b, float aph) {
		buffer_size = number_of_vertices * 6;
		for (int i = 0; i < buffer_size; i += 6) {
			data_buffer[i + 2] = r;
			data_buffer[i + 3] = g;
			data_buffer[i + 4] = b;
			data_buffer[i + 5] = aph;
		}
		
	}
	void move_norm(float dx, float dy) {
	
		for (int i = 0; i < buffer_size; i += 6) {
			data_buffer[i] += dx;
			data_buffer[i + 1] += dy;
		}
		center_x = data_buffer[0] - radii / HALF_WIDTH;
		center_y = data_buffer[1];
	}

	void init_velocity(float vx, float vy) {
		this->vx = vx / HALF_WIDTH;
		this->vy = vy / HALF_WIDTH;
		this->speed = sqrt(this->vx * this->vx + this->vy * this->vy);
	}
	void set_velocity_norm(float vx, float vy) {
		this->vx = vx;
		this->vy = vy;
		this->speed = sqrt(this->vx * this->vx + this->vy * this->vy);
	}
	bool edge_collided_x() {
		bool if_edge_collided;
		float radiix = radii / HALF_WIDTH;

		if (center_x + radiix >= 1 || center_x - radiix <= -1) {
			//cout << "x out!!!!" << endl;
			if (center_x + radiix >= 1)  set_position_norm(center_x - 0.01, center_y);
			if (center_x - radiix <= -1) set_position_norm(center_x + 0.01, center_y);
			vx = -vx;
			if_edge_collided = true;
		}
		else {
			if_edge_collided = false;
		}
		return if_edge_collided;
	}

	bool edge_collided_y() {
		bool if_edge_collided;

		float radiiy = radii / HALF_HEIGHT;
		if (center_y + radiiy >= 1 || center_y - radiiy <= -1) {
			//cout << "y out!!!!" << endl;
			if(center_y + radiiy >= 1)  set_position_norm(center_x, center_y -0.01);
			if(center_y - radiiy <= -1) set_position_norm(center_x, center_y +0.01);
			vy = -vy;
			if_edge_collided = true;
		}
		else {
			if_edge_collided = false;
		}
		return if_edge_collided;
	}
	void set_position_norm(float x, float y) {
		x *= HALF_WIDTH;
		y *= HALF_HEIGHT;
		float doublePI = M_PI * 2;
		int number_of_side = number_of_vertices - 2;
		for (int i = 0; i < buffer_size; i += 6) {
			data_buffer[i] = (x + radii * cos((i / 6) * (doublePI / number_of_side))) / HALF_WIDTH;
			data_buffer[i + 1] = (y + radii * sin((i / 6) * (doublePI / number_of_side))) / HALF_HEIGHT;
		}
		center_x = data_buffer[0] - radii / HALF_WIDTH;
		center_y = data_buffer[1];
	}
	void set_acclr_norm(float x) {
		acclr = x;
	}

	void update_position() {

		for (int i = 0; i < buffer_size; i += 6) {
			data_buffer[i] += vx;
			data_buffer[i + 1] += vy;
		}
		center_x = data_buffer[0] - radii / HALF_WIDTH;
		center_y = data_buffer[1];


		edge_collided_x();
		edge_collided_y();

		//if (abs(vx) < 0.00001) vx = 0;
		//if (abs(vy) < 0.00001) vy = 0;
		//if (vx > 0) vx -= 0.000001;
		//if (vx < 0) vx += 0.000001;
		//if (vy > 0) vy -= 0.000001;
		//if (vy < 0) vy += 0.000001;
	}

	float get_velocity_angle() { return atan(vy / vx)* 180.0 / M_PI; };
	float get_center_x() { return center_x; }
	float get_center_y() { return center_y; }
	float get_radii() { return radii; }
	float get_vx() { return vx; }
	float get_vy() { return vy; }
	float get_speed() { return speed; }

	~Polygon() { delete[] data_buffer; }
private:
	int number_of_vertices;

	float center_x = 0;
	float center_y = 0;
	float radii = 0;
	float vx = 0;
	float vy = 0;
	float speed = 0;
	float acclr = 0;

	float *data_buffer;
	int buffer_size;
};


class Traingle : public Base_Object {

public:
	Traingle() {}
	Traingle(float* data_buffer) {
		set_data(data_buffer, 3, GL_TRIANGLES);
	}
private:

};

class Square : public Base_Object {
public:
	Square() {}
	Square(float center_x = 0, float center_y = 0, float width = 100, float height = 100, int degree = 0) {

		center_x /= float(WINDOW_WIDTH / 2);
		center_y /= float(WINDOW_HEIGHT / 2);
		width /= float(WINDOW_WIDTH / 2);
		height /= float(WINDOW_HEIGHT / 2);
		this->center_x = center_x;
		this->center_y = center_y;

		rotate_degree = degree;

		float offsety = height / 2;
		float offsetx = width / 2;

		float temp_buffer[24] = {
			 center_x - offsetx, center_y - offsety, 1, 0, 0, 1,
			 center_x + offsetx, center_y - offsety, 1, 0, 0, 1,
			 center_x + offsetx, center_y + offsety, 1, 0, 0, 1,
			 center_x - offsetx, center_y + offsety, 1, 0, 0, 1
		};
		 
		arrcpy(temp_buffer); // store the data to class member
		rotate_obj(degree);
		set_data(data_buffer, 4, GL_TRIANGLE_FAN);
	}
	float get_center_x() { return center_x; }
	float get_center_y() { return center_y; }
	float get_width() { return width; }
	float get_height() { return height; }

private:

	float center_x = 0;
	float center_y = 0;
	float width = 0;
	float height = 0;
	float rotate_degree = 0;
	float data_buffer[24];

	void arrcpy(float in[24]) { for (int i = 0; i < 24; i++) data_buffer[i] = in[i]; }

	void rotate_obj(float degree) {
		degree = degree * M_PI / 180.0;
		float cs = (float)cos(degree);
		float sn = (float)sin(degree);
		//float rotation_matrix[4] = {cs, -sn, sn, cs};
		for (int i = 0; i < 24; i += 6) {
			float temp_x = data_buffer[i] - center_x ;
			float temp_y = data_buffer[i + 1] - center_y ;
			data_buffer[i]     = cs * temp_x * HALF_WIDTH - sn * temp_y * HALF_HEIGHT;
			data_buffer[i + 1] = sn * temp_x * HALF_WIDTH + cs * temp_y * HALF_HEIGHT; //scaling problem
			data_buffer[i] /= HALF_WIDTH;
			data_buffer[i+1] /= HALF_HEIGHT;
			data_buffer[i] += center_x;
			data_buffer[i + 1] += center_y;

		}
	}
};

