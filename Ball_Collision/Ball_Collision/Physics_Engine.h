#pragma once
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <vector>
#include "Window_Setting.h"
#include "Shape.h"


/****** Function Declaration ******/
static float two_point_distance(float, float, float, float);
static float two_point_atan(float, float, float, float);


class collision_engine {

public:

	collision_engine() {};

	void new_object(Polygon& newobj) {

		round_object_ptr.push_back(&newobj);
		round_object_data.push_back(newobj.get_center_x());
		round_object_data.push_back(newobj.get_center_y());
		round_object_data.push_back(newobj.get_radii());

	}
	void new_object(Square& newobj) {

		square_object_ptr.push_back(&newobj);
		square_object_data.push_back(newobj.get_center_x());
		square_object_data.push_back(newobj.get_center_y());
		square_object_data.push_back(newobj.get_width());
		square_object_data.push_back(newobj.get_height());

	}

	void collision_check() {

		// Collision Between Round Objects
		
		int data_number = round_object_data.size();
		for (int i = 0; i < data_number; i += 3) {
			for (int j = i + 3; j < data_number; j += 3) {

				
				round_data_update();
				// extract data (in real scale)
				float obj1_x = round_object_data[i] * HALF_WIDTH;
				float obj1_y = round_object_data[i + 1] * HALF_HEIGHT;
				float obj2_x = round_object_data[j] * HALF_WIDTH;
				float obj2_y = round_object_data[j + 1] * HALF_HEIGHT;

				float obj1_rad = round_object_data[i + 2];  //radius (in real scale)
				float obj2_rad = round_object_data[j + 2];

				float obj1_vx = round_object_ptr[i / 3]-> get_vx();
				float obj1_vy = round_object_ptr[i / 3]-> get_vy();
				float obj2_vx = round_object_ptr[j / 3]-> get_vx();
				float obj2_vy = round_object_ptr[j / 3]-> get_vy();


				float two_radii_sum = obj1_rad + obj2_rad;
				float center_to_center = two_point_distance(obj1_x, obj1_y, obj2_x, obj2_y);
				float contact_angle = two_point_atan(obj1_x, obj1_y, obj2_x, obj2_y);

				float velocity_angle_obj1 = round_object_ptr[i / 3]-> get_velocity_angle();
				float velocity_angle_obj2 = round_object_ptr[j / 3]-> get_velocity_angle();
				float rotation_angle_obj1 = contact_angle - velocity_angle_obj1; // eta
				float rotation_angle_obj2 = contact_angle - velocity_angle_obj2;

				float obj1_speed = round_object_ptr[i / 3]->get_speed();
				float obj2_speed = round_object_ptr[j / 3]->get_speed();

				float obj1_vx_exchange, obj1_vy_exchange;
				float obj1_vx_remain, obj1_vy_remain;
				float obj2_vx_exchange, obj2_vy_exchange;
				float obj2_vx_remain, obj2_vy_remain;

				float obj1_cs = cos(rotation_angle_obj1 / 180.0 * M_PI);
				float obj1_sn = sin(rotation_angle_obj1 / 180.0 * M_PI);
				float obj2_cs = cos(rotation_angle_obj2 / 180.0 * M_PI);
				float obj2_sn = sin(rotation_angle_obj2 / 180.0 * M_PI);

				//float 

				Square v_obj1(obj1_x, obj1_y, obj1_speed * 10000, 2, velocity_angle_obj1);
				v_obj1.show();
				Square v_obj2(obj2_x, obj2_y, obj2_speed * 10000, 2, velocity_angle_obj2);
				v_obj2.show();
				
				if (center_to_center <= two_radii_sum) {
					
					//std::cout << "collided" << endl;

					// hit surface 
					Square hit_surface( obj1_x + (obj2_x - obj1_x)*(obj1_rad / center_to_center),
										obj1_y + (obj2_y - obj1_y)*(obj1_rad / center_to_center), 2, 200, contact_angle);
					hit_surface.show();

					// real 2d collision

					obj1_vx_exchange = (obj1_cs * obj1_vx * HALF_WIDTH - obj1_sn * obj1_vy * HALF_HEIGHT) * obj1_cs; 
					obj1_vy_exchange = (obj1_sn * obj1_vx * HALF_WIDTH + obj1_cs * obj1_vy * HALF_HEIGHT) * obj1_cs;
					obj2_vx_exchange = (obj2_cs * obj2_vx * HALF_WIDTH - obj2_sn * obj2_vy * HALF_HEIGHT) * obj2_cs;
					obj2_vy_exchange = (obj2_sn * obj2_vx * HALF_WIDTH + obj2_cs * obj2_vy * HALF_HEIGHT) * obj2_cs;

					obj1_vx_exchange /= HALF_WIDTH;
					obj1_vy_exchange /= HALF_HEIGHT;
					obj2_vx_exchange /= HALF_WIDTH;
					obj2_vy_exchange /= HALF_HEIGHT;

					obj1_vx_remain = obj1_vx - obj1_vx_exchange;
					obj1_vy_remain = obj1_vy - obj1_vy_exchange;
					obj2_vx_remain = obj2_vx - obj2_vx_exchange;
					obj2_vy_remain = obj2_vy - obj2_vy_exchange;

					round_object_ptr[i / 3]->set_velocity_norm(obj1_vx_remain + obj2_vx_exchange, obj1_vy_remain + obj2_vy_exchange);
					round_object_ptr[j / 3]->set_velocity_norm(obj2_vx_remain + obj1_vx_exchange, obj2_vy_remain + obj1_vy_exchange);

					// old two object speed change
					/*
					float tempvx1 = round_object_ptr[i / 3]->get_vx();
					float tempvy1 = round_object_ptr[i / 3]->get_vy();
					float tempvx2 = round_object_ptr[j / 3]->get_vx();
					float tempvy2 = round_object_ptr[j / 3]->get_vy();

					cout << tempvx1 << " " << tempvy2 << endl;
					round_object_ptr[i / 3]->set_velocity_norm(tempvx2, tempvy2);
					round_object_ptr[j / 3]->set_velocity_norm(tempvx1, tempvy1);
					*/

					//anti-stick 
					if (round_object_data[i] - round_object_data[j] < 0) {
						round_object_ptr[i / 3]->move_norm(-0.01, 0);
					}
					if (round_object_data[i] - round_object_data[j] > 0) {
						round_object_ptr[i / 3]->move_norm(0.01, 0);
					}			
				}
			}
		}
	}

private:
	std::vector<float> round_object_data;
	std::vector<Polygon*> round_object_ptr;

	std::vector<float> square_object_data;
	std::vector<Square*> square_object_ptr;

	void round_data_update() {
		int data_number = round_object_data.size();
		for (int i = 0; i < data_number; i += 3) {
			int object_idx = i / 3;
			round_object_data[i] = (round_object_ptr[object_idx])-> get_center_x();
			round_object_data[i + 1] = (round_object_ptr[object_idx])-> get_center_y();
		}
	}
};


static float two_point_distance(float x1, float y1, float x2, float y2) {
	float dx = x2 - x1;
	float dy = y2 - y1;
	return sqrt(dx * dx + dy * dy);
}

static float two_point_atan(float x1, float y1, float x2, float y2) {
	float dx = x2 - x1;
	float dy = y2 - y1;
	return 180 * atan(dy/dx)/M_PI;
}
/*

/////// round_object_data //////////

				data form
	center_x	 center_y	  radius

	- Each round object takes 3 space
	- data in normalize form
	- radius is not normalized!!!!!!!!!!!!

*/