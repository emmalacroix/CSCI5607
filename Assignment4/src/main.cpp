//3D Game
//CSCI 5607
//File provided in class used as reference
//by Emma Lacroix

#include "include/glad.h"  //Include order can matter here
#ifdef __APPLE__
 #include <SDL2/SDL.h>
 #include <SDL2/SDL_opengl.h>
#else
 #include <SDL.h>
 #include <SDL_opengl.h>
#endif
#include <cstdio>
#include "include/cuboid.h"

#define GLM_FORCE_RADIANS
#define _USE_MATH_DEFINES
#include "../ext/glm/glm.hpp"
#include "../ext/glm/gtc/matrix_transform.hpp"
#include "../ext/glm/gtc/type_ptr.hpp"

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

int screenWidth = 800; 
int screenHeight = 600;  
bool saveOutput = false;

//Store player coordinates
float p_x, p_z; //position of the player in x & z
float p_x_dir = 1;
float p_z_dir = 0; //direction the player is facing in x & z
				   //initially: arbitrary (1,0)
//Store key coordinates
float* key_pos_x = new float[5];
float* key_pos_z = new float[5];
float* key_translate_x = new float[5];
float* key_translate_z = new float[5];
float player_angle_rads = 0;
float* key_angle_rads = new float[5];

bool DEBUG_ON = true;
GLuint InitShader(const char* vShaderFileName, const char* fShaderFileName);
bool fullscreen = false;
void Win2PPM(int width, int height);

//srand(time(NULL));
float rand01(){
	return rand()/(float)RAND_MAX;
}

int main(int argc, char *argv[]){
    SDL_Init(SDL_INIT_VIDEO);  //Initialize Graphics (for OpenGL)
    
    //Ask SDL to get a recent version of OpenGL (3 or greater)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);//
	
	//Create a window (offsetx, offsety, width, height, flags)
	SDL_Window* window = SDL_CreateWindow("My OpenGL Program", 100, 100, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
	
	//Create a context to draw in
	SDL_GLContext context = SDL_GL_CreateContext(window);
	
	//Load OpenGL extentions with GLAD
	if (gladLoadGLLoader(SDL_GL_GetProcAddress)){
		printf("\nOpenGL loaded\n");
		printf("Vendor:   %s\n", glGetString(GL_VENDOR));
		printf("Renderer: %s\n", glGetString(GL_RENDERER));
		printf("Version:  %s\n\n", glGetString(GL_VERSION));
	}
	else {
		printf("ERROR: Failed to initialize OpenGL context.\n");
		return -1;
	}
	
	//Load scene file
	ifstream sceneFile;
	sceneFile.open("../scenefiles/scene2.txt");
	int width, depth;
	float unit = 2; 
	float wall_height = 4*unit;
	sceneFile >> width;
	sceneFile >> depth;
	width = width * unit;
	depth = depth * unit;
	printf("The scene has a width of %i and a depth of %i.\n",width, depth);
	string row;
	// float goal[2];
	int num_walls = 0;
	vector<Cuboid*> walls;
	Cuboid* doors = new Cuboid[5];
	Cuboid* keys = new Cuboid[5];
	Cuboid* goal;
	bool* door_exists = new bool[5];
	bool* key_exists = new bool[5];
	bool* key_in_motion = new bool[5];

	for (int z = 0; z < depth/unit; z++){
		sceneFile >> row;
		vector<char> char_row(row.begin(), row.end());
		for (int x = 0; x < width/unit; x++){
			const char letter = char_row[x];
			if (!strncmp(&letter, "S", 1)){
				p_x = x*unit+(unit/2);
				p_z = z*unit+(unit/2);
				printf("Start position set to (%f, 1, %f).\n", p_x, p_z);
			}
			if (!strncmp(&letter, "G", 1)){
				goal = new Cuboid(x*unit+(unit/2), z*unit+(unit/2), 0, wall_height, unit/2);
				printf("Goal position set to (%f, 1, %f).\n", x*unit+(unit/2), z*unit+(unit/2));
			}
			if (!strncmp(&letter, "W", 1)){
				walls.push_back(new Cuboid(x*unit+(unit/2), z*unit+(unit/2), 0, wall_height, unit/2));
				num_walls++;
			}
			if (!strncmp(&letter, "A", 1)){
				doors[0] = Cuboid(x*unit+(unit/2), z*unit+(unit/2), 0, wall_height, unit/2);
				door_exists[0] = true;
			}
			if (!strncmp(&letter, "B", 1)){
				doors[1] = Cuboid(x*unit+(unit/2), z*unit+(unit/2), 0, wall_height, unit/2);
				door_exists[1] = true;
			}
			if (!strncmp(&letter, "C", 1)){
				doors[2] = Cuboid(x*unit+(unit/2), z*unit+(unit/2), 0, wall_height, unit/2);
				door_exists[2] = true;
			}
			if (!strncmp(&letter, "D", 1)){
				doors[3] = Cuboid(x*unit+(unit/2), z*unit+(unit/2), 0, wall_height, unit/2);
				door_exists[3] = true;
			}
			if (!strncmp(&letter, "E", 1)){
				doors[4] = Cuboid(x*unit+(unit/2), z*unit+(unit/2), 0, wall_height, unit/2);
				door_exists[4] = true;
			}
			if (!strncmp(&letter, "a", 1)){
				key_translate_x[0] = 0.0;
				key_translate_z[0] = 0.0;
				key_pos_x[0] = x*unit+(unit/2);
				key_pos_z[0] = z*unit+(unit/2);
				keys[0] = Cuboid(x*unit+(unit/2), z*unit+(unit/2), unit/3, unit/3, unit/6);
				key_exists[0] = true;
			}
			if (!strncmp(&letter, "b", 1)){
				key_translate_x[1] = 0.0;
				key_translate_z[1] = 0.0;
				key_pos_x[1] = x*unit+(unit/2);
				key_pos_z[1] = z*unit+(unit/2);
				keys[1] = Cuboid(x*unit+(unit/2), z*unit+(unit/2), unit/3, unit/3, unit/6);
				key_exists[1] = true;
			}
			if (!strncmp(&letter, "c", 1)){
				key_translate_x[2] = 0.0;
				key_translate_z[2] = 0.0;
				key_pos_x[2] = x*unit+(unit/2);
				key_pos_z[2] = z*unit+(unit/2);
				keys[2] = Cuboid(x*unit+(unit/2), z*unit+(unit/2), unit/3, unit/3, unit/6);
				key_exists[2] = true;
			}
			if (!strncmp(&letter, "d", 1)){
				key_translate_x[3] = 0.0;
				key_translate_z[3] = 0.0;
				key_pos_x[3] = x*unit+(unit/2);
				key_pos_z[3] = z*unit+(unit/2);
				keys[3] = Cuboid(x*unit+(unit/2), z*unit+(unit/2), unit/3, unit/3, unit/6);
				key_exists[3] = true;
			}
			if (!strncmp(&letter, "e", 1)){
				key_translate_x[4] = 0.0;
				key_translate_z[4] = 0.0;
				key_pos_x[4] = x*unit+(unit/2);
				key_pos_z[4] = z*unit+(unit/2);
				keys[4] = Cuboid(x*unit+(unit/2), z*unit+(unit/2), unit/3, unit/3, unit/6);
				key_exists[4] = true;
			}
		}
	}
	printf("Scene file read complete.\n");

	float exterior_walls_data[] = {
		//X, Y, Z, U, V, N_X, N_Y, N_Z
		//wall 1
		width, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0,
		0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, -1.0,
		0.0, wall_height, 0.0, 1.0, 1.0, 0.0, 0.0, -1.0,
		0.0, wall_height, 0.0, 1.0, 1.0, 0.0, 0.0, -1.0,
		width, wall_height, 0.0, 0.0, 1.0, 0.0, 0.0, -1.0,
		width, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0,
		//wall 2
		0.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0,
		0.0, 0.0, depth, 1.0, 0.0, -1.0, 0.0, 0.0,
		0.0, wall_height, depth, 1.0, 1.0, -1.0, 0.0, 0.0,
		0.0, wall_height, depth, 1.0, 1.0, -1.0, 0.0, 0.0,
		0.0, wall_height, 0.0, 0.0, 1.0, -1.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0,
		//wall 3
		0.0, 0.0, depth, 0.0, 0.0, 0.0, 0.0, 1.0,
		width, 0.0, depth, 1.0, 0.0, 0.0, 0.0, 1.0,
		width, wall_height, depth, 1.0, 1.0, 0.0, 0.0, 1.0,
		width, wall_height, depth, 1.0, 1.0, 0.0, 0.0, 1.0,
		0.0, wall_height, depth, 0.0, 1.0, 0.0, 0.0, 1.0,
		0.0, 0.0, depth, 0.0, 0.0, 0.0, 0.0, 1.0,
		//wall 4
		width, 0.0, depth, 0.0, 0.0, 1.0, 0.0, 0.0,
		width, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0,
		width, wall_height, 0.0, 1.0, 1.0, 1.0, 0, 0.0,
		width, wall_height, 0.0, 1.0, 1.0, 1.0, 0, 0.0,
		width, wall_height, depth, 0.0, 1.0, 1.0, 0.0, 0.0,
		width, 0.0, depth, 0.0, 0.0, 1.0, 0.0, 0.0
	};
	float floor_data[] = {
		//X, Y, Z, U, V, N_X, N_Y, N_Z
		width, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0,
		0.0, 0.0, 0.0, 1.0, 0.0, 0.0, -1.0, 0.0,
		0.0, 0.0, depth, 1.0, 1.0, 0.0, -1.0, 0.0,
		0.0, 0.0, depth, 1.0, 1.0, 0.0, -1.0, 0.0,
		width, 0.0, depth, 0.0, 1.0, 0.0, -1.0, 0.0,
		width, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0
	};

	//Concatenate all data in a big ol' array
	float* scene_data = new float[num_walls*288 + 192 + 48 + 288 + 1440 + 1440]; //# of walls * # of vertices each wall has * # of values each vertex has
												   //+ exterior walls (4 walls * 2 triangles each * 3 vertices * 8 values)
												   //+ floor (2 triangles each * 3 vertices * 8 values) + doors & keys
	for (int i = 0; i < num_walls; i++){
		for (int j = 0; j < 288; j++){
			scene_data[i*288 + j] = walls[i]->data[j];
		}
	}
	for (int i = 0; i < 192; i++){
		scene_data[num_walls*288 + i] = exterior_walls_data[i];
	}
	for (int i = 0; i < 48; i++){
		scene_data[num_walls*288 + 192 + i] = floor_data[i];
	}
	for (int i = 0; i < 288; i++){
		scene_data[num_walls*288 + 192 + 48 + i] = goal->data[i];
	}
	//enter door data into scene_data:
	for (int i = 0; i < 5; i++){
		if (door_exists[i]){
			for (int j = 0; j < 288; j++){
				scene_data[num_walls*288 + 192 + 48 + 288 + i*288 + j] = doors[i].data[j];
			}
		} else {
			for (int j = 0; j < 288; j++){
				scene_data[num_walls*288 + 192 + 48 + 288 + i*288 + j] = 0.0; //placeholder
			}
		}
	}
	//enter key data into scene_data:
	for (int i = 0; i < 5; i++){
		if (key_exists[i]){
			for (int j = 0; j < 288; j++){
				scene_data[num_walls*288 + 192 + 48 + 288 + 1440 + i*288 + j] = keys[i].data[j];
			}
		} else {
			for (int j = 0; j < 288; j++){
				scene_data[num_walls*288 + 192 + 48 + 288 + 1440 + i*288 + j] = 0.0;
			}
		}
	}

	int num_wall_verts = 36*num_walls + 24;
	int total_verts = num_wall_verts + 6 + 36 + 360;
	
	//// Allocate Texture 0 (Moon surface) ///////
	SDL_Surface* surface = SDL_LoadBMP("../textures/moon.bmp");
	if (surface==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    GLuint tex0;
    glGenTextures(1, &tex0);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex0);
    
    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    //Load the texture into memory
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w,surface->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    SDL_FreeSurface(surface);
    //// End Allocate Texture ///////

	//// Allocate Texture 1 (stars!!!) ///////
	SDL_Surface* surface1 = SDL_LoadBMP("../textures/stars.bmp");
	if (surface==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    GLuint tex1;
    glGenTextures(1, &tex1);
    
    //Load the texture into memory
    glActiveTexture(GL_TEXTURE1);
    
    glBindTexture(GL_TEXTURE_2D, tex1);
    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface1->w,surface1->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface1->pixels);
    
    glGenerateMipmap(GL_TEXTURE_2D);
    
    SDL_FreeSurface(surface1);
	//// End Allocate Texture ///////

	//Build a Vertex Array Object. This stores the VBO and attribute mappings in one object
	GLuint vao;
	glGenVertexArrays(1, &vao); //Create a VAO
	glBindVertexArray(vao); //Bind the above created VAO to the current context
	
	//Allocate memory on the graphics card to store geometry (vertex buffer object)
	GLuint vbo[1];
	glGenBuffers(1, vbo);  //Create 1 buffer called vbo
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); //Set the vbo as the active array buffer (Only one buffer can be active at a time)
	glBufferData(GL_ARRAY_BUFFER, total_verts*8*sizeof(float), scene_data, GL_STATIC_DRAW); //upload vertices to vbo
	//GL_STATIC_DRAW means we won't change the geometry, GL_DYNAMIC_DRAW = geometry changes infrequently
	//GL_STREAM_DRAW = geom. changes frequently.  This effects which types of GPU memory is used
	
	int texturedShader = InitShader("../shaders/vertexTex.glsl", "../shaders/fragmentTex.glsl");
	
	int phongShader = InitShader("../shaders/vertex.glsl", "../shaders/fragment.glsl");
	
	//Tell OpenGL how to set fragment shader input 
	GLint posAttrib = glGetAttribLocation(texturedShader, "position");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
	  //Attribute, vals/attrib., type, normalized?, stride, offset
	  //Binds to VBO current GL_ARRAY_BUFFER 
	glEnableVertexAttribArray(posAttrib);
	
	GLint colAttrib = glGetAttribLocation(phongShader, "inColor");
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(colAttrib);
	
	GLint normAttrib = glGetAttribLocation(texturedShader, "inNormal");
	glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(5*sizeof(float)));
	glEnableVertexAttribArray(normAttrib);
	
	GLint texAttrib = glGetAttribLocation(texturedShader, "inTexcoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));

	GLint uniView = glGetUniformLocation(texturedShader, "view");
	GLint uniProj = glGetUniformLocation(texturedShader, "proj");

	glBindVertexArray(0); //Unbind the VAO in case we want to create a new one	                  
	
	glEnable(GL_DEPTH_TEST);  
	
	//Event Loop (Loop forever processing each event as fast as possible)
	SDL_Event windowEvent;
	bool quit = false;
	int start_time = -2000;
	while (!quit){
		while (SDL_PollEvent(&windowEvent)){  //inspect all events in the queue
			if (windowEvent.type == SDL_QUIT) quit = true;
			//List of keycodes: https://wiki.libsdl.org/SDL_Keycode - You can catch many special keys
			//Scancode referes to a keyboard position, keycode referes to the letter (e.g., EU keyboards)
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE){
				quit = true; //Exit event loop
			}
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_f){ //If "f" is pressed
				fullscreen = !fullscreen;
				SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0); //Toggle fullscreen 
			}
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_SPACE){
				start_time = SDL_GetTicks()-600;
			}
			bool inside_wall = false;
			bool inside_door = false;
			//SJG: Use key input to change the state of the object
			//     We can use the ".mod" flag to see if modifiers such as shift are pressed
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_UP){ //If "up key" is pressed
				if ((p_x+.2*unit*p_x_dir>=0) && (p_x+.2*unit*p_x_dir<=width) && (p_z+.2*unit*p_z_dir>=0) && (p_z+.2*unit*p_z_dir)<=depth){
					for (int i = 0; i < num_walls; i++){
						if (walls[i]->Contains(p_x+.2*unit*p_x_dir,p_z+.2*unit*p_z_dir)){
							inside_wall = true;
						}
					}
					if (!inside_wall){
						for (int i = 0; i < 5; i++){
							if (door_exists[i]&&doors[i].Contains(p_x+.2*unit*p_x_dir,p_z+.2*unit*p_z_dir)){
								if (key_in_motion[i]){
									door_exists[i] = false;
									key_exists[i] = false;
									key_in_motion[i] = false;
								} else {
									inside_door = true;
								}
							}
						}
					}
					if (!inside_wall&&!inside_door){
						p_x = p_x + .2*unit*p_x_dir;
						p_z = p_z + .2*unit*p_z_dir;
						for (int i = 0; i < 5; i++){
							if (key_exists[i]){
								if (keys[i].Contains(p_x+.2*unit*p_x_dir,p_z+.2*unit*p_z_dir)&&(!key_in_motion[i])){
									key_in_motion[i] = true;
									key_translate_x[i] += .8*unit*p_x_dir;
									key_translate_z[i] += .8*unit*p_z_dir;
									key_pos_x[i] += .8*unit*p_x_dir;
									key_pos_z[i] += .8*unit*p_z_dir;
								}
								if (key_in_motion[i]){
									key_translate_x[i] += .2*unit*p_x_dir;
									key_translate_z[i] += .2*unit*p_z_dir;
									key_pos_x[i] += .2*unit*p_x_dir;
									key_pos_z[i] += .2*unit*p_z_dir;
								}
							}
						}
					}
				}
			}
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_DOWN){ //If "down key" is pressed
				if ((p_x-.2*unit*p_x_dir>=0) && (p_x-.2*unit*p_x_dir<=width) && (p_z-.2*unit*p_z_dir>=0) && (p_z-.2*unit*p_z_dir)<=depth){
					for (int i = 0; i < num_walls; i++){
						if (walls[i]->Contains(p_x-.2*unit*p_x_dir,p_z-.2*unit*p_z_dir)){
							inside_wall = true;
						}
					}
					if (!inside_wall){
						for (int i = 0; i < 5; i++){
							if (door_exists[i]&&doors[i].Contains(p_x-.2*unit*p_x_dir,p_z-.2*unit*p_z_dir)){
								if (key_in_motion[i]){
									door_exists[i] = false;
									key_exists[i] = false;
									key_in_motion[i] = false;
								} else {
									inside_door = true;
								}
							}
						}
					}
					if (!inside_wall&&!inside_door){
						p_x = p_x - .2*unit*p_x_dir;
						p_z = p_z - .2*unit*p_z_dir;
						for (int i = 0; i < 5; i++){
							if (key_exists[i]){
								if (keys[i].Contains(p_x-.2*unit*p_x_dir,p_z-.2*unit*p_z_dir)&&(!key_in_motion[i])){
									key_in_motion[i] = true; 
									key_angle_rads[i] = player_angle_rads;
									key_translate_x[i] += .4*unit*p_x_dir;
									key_translate_z[i] += .4*unit*p_z_dir;
									key_pos_x[i] += .4*unit*p_x_dir;
									key_pos_z[i] += .4*unit*p_z_dir;
								}
								if (key_in_motion[i]){
									key_translate_x[i] -= .2*unit*p_x_dir;
									key_translate_z[i] -= .2*unit*p_z_dir;
									key_pos_x[i] -= .2*unit*p_x_dir;
									key_pos_z[i] -= .2*unit*p_z_dir;
								}
							}
						}
					}
				}
			}
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_LEFT){ //If "up key" is pressed
				p_x_dir = p_x_dir*0.996195 + p_z_dir*0.087156;
          		p_z_dir = p_x_dir*(-0.087156) + p_z_dir*0.996195;
          		float length = sqrt(p_x_dir*p_x_dir + p_z_dir*p_z_dir);
          		p_x_dir /= length;
          		p_z_dir /= length;
          		// for (int i = 0; i < 5; i++){
          		// 	if (key_in_motion[i]){
          				player_angle_rads = fmod(player_angle_rads - 0.087267, 2*M_PI);
          				//key_angle_degs = fmod(key_angle_degs - 5, 360);
          		// 	}
          		// }
          				for (int i = 0; i < 5; i++){
          					if (key_in_motion[i]){
          						key_angle_rads[i] = fmod(key_angle_rads[i] - 0.087267, 2*M_PI);
          					}
          				}
          	}
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_RIGHT){ //If "down key" is pressed
				p_x_dir = p_x_dir*0.996195 + p_z_dir*(-0.087156);
          		p_z_dir = p_x_dir*0.087156 + p_z_dir*0.996195;
          		float length = sqrt(p_x_dir*p_x_dir + p_z_dir*p_z_dir);
          		p_x_dir /= length;
          		p_z_dir /= length;
          		// for (int i = 0; i < 5; i++){
          		// 	if (key_in_motion[i]){
          				player_angle_rads = fmod(player_angle_rads + 0.087267, 2*M_PI);
          				//key_angle_degs = fmod(key_angle_degs + 5, 360);
          		// 	}
          		// }
          				for (int i = 0; i < 5; i++){
          					if (key_in_motion[i]){
          						key_angle_rads[i] = fmod(key_angle_rads[i] + 0.087267, 2*M_PI);
          					}
          				}
			}
		}
      
		// Clear the screen to default color
		glClearColor(0, 0, 0, 1.0f); //black
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(texturedShader);

		// if (!saveOutput) timePast = SDL_GetTicks()/1000.f; 
		// if (saveOutput) timePast += .07; //Fix framerate at 14 FPS

		glm::mat4 view = glm::lookAt(
		glm::vec3(p_x, unit/2+fmax(0.0,(25-pow(((SDL_GetTicks()-start_time)/80-5),2))*(unit/25)), p_z),  //Cam Position
		glm::vec3(p_x+p_x_dir, unit/2+fmax(0.0,(25-pow(((SDL_GetTicks()-start_time)/80-5),2))*(unit/25)), p_z+p_z_dir),  //Look at point
		glm::vec3(0, 1, 0)); //Up
		glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 proj = glm::perspective(3.14f/4, screenWidth / (float) screenHeight, 1.0f, 10.0f); //FOV, aspect, near, far
		glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex0);
		glUniform1i(glGetUniformLocation(texturedShader, "tex0"), 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex1);
		glUniform1i(glGetUniformLocation(texturedShader, "tex1"), 1);

		glBindVertexArray(vao);

		//draw everything!!!
		GLint uniColor = glGetUniformLocation(texturedShader, "inColor");
		glm::vec3 colVec(1,1,1);
		glUniform3fv(uniColor, 1, glm::value_ptr(colVec));
      	GLint uniTexID = glGetUniformLocation(texturedShader, "texID");

		glm::mat4 model;
	    GLint uniModel = glGetUniformLocation(texturedShader, "model");
    	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    	glUniform1i(uniTexID, 1); //Set texture ID to use (-1 = no texture)
    	//SJG: Here we draw only the first object (start at 0, draw numTris1 triangles)  
    	glDrawArrays(GL_TRIANGLES, 0, num_wall_verts); //(Primitive Type, Start Vertex, End Vertex)
      
    	//SJG: Let's change the model matrix before the next call to show how to change diffent
    	//     models independently
    	model = glm::mat4();
    	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    	glUniform1i(uniTexID, 0); //Set texture ID to use 
    	//SJG: The 2nd model, starts at an offset of num_wall_verts and has 6 floor vertices
    	//      My approach here is not very scalable. You should do better (EJL: oops i did not do better)
    	glDrawArrays(GL_TRIANGLES, num_wall_verts, 6); //(Primitive Type, Start Vertex, End Vertex)

    	model = glm::mat4();
    	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    	glUniform1i(uniTexID, -1);
    	//glm::vec3 colVec(1,1,1);
    	colVec = glm::vec3(1,1,1);
    	glUniform3fv(uniColor, 1, glm::value_ptr(colVec));
    	glDrawArrays(GL_TRIANGLES, num_wall_verts+6, 36);

    	for (int i = 0; i < 5; i++){
    		if (door_exists[i]){
    			model = glm::mat4();
		    	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
		    	glUniform1i(uniTexID, -1); //Set texture ID to use
		    	if (i == 0){
	    			glm::vec3 colVec(1,0,0);
	    			glUniform3fv(uniColor, 1, glm::value_ptr(colVec));
	    		} else if (i == 1){
	    			glm::vec3 colVec(1,1,0);
	    			glUniform3fv(uniColor, 1, glm::value_ptr(colVec));
	    		} else if (i == 2){
	    			glm::vec3 colVec(0,1,0);
	    			glUniform3fv(uniColor, 1, glm::value_ptr(colVec));
	    		} else if (i == 3){
	    			glm::vec3 colVec(0,1,1);
	    			glUniform3fv(uniColor, 1, glm::value_ptr(colVec));
	    		} else if (i == 4){
	    			glm::vec3 colVec(0,0,1);
	    			glUniform3fv(uniColor, 1, glm::value_ptr(colVec));
    			}
		    	glDrawArrays(GL_TRIANGLES, num_wall_verts+6+36+(36*i), 36);
    		}
    		if (key_exists[i]){ 
		    	model = glm::mat4();
		    	if (key_in_motion[i]){
		    	//	model = glm::mat4;
		    		model = glm::translate(model,glm::vec3(key_translate_x[i], 0, key_translate_z[i]));
		    		model = glm::translate(model,glm::vec3((((key_pos_x[i]-p_x)*cos(key_angle_rads[i]))-((key_pos_z[i]-p_z)*sin(key_angle_rads[i]))+p_x)-key_pos_x[i], 0, key_pos_z[i]-(((-key_pos_x[i]+p_x)*sin(key_angle_rads[i]))+((-key_pos_z[i]+p_z)*cos(key_angle_rads[i]))+p_z)));
		    		//model = glm::rotate(model,key_a_degrees,glm::vec3(0.0f, 1.0f, 0.0f));
		    	}
		    	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
		    	glUniform1i(uniTexID, -1); //Set texture ID to use 
		    	if (i == 0){
	    			glm::vec3 colVec(1,0,0);
	    			glUniform3fv(uniColor, 1, glm::value_ptr(colVec));
	    		} else if (i == 1){
	    			glm::vec3 colVec(1,1,0);
	    			glUniform3fv(uniColor, 1, glm::value_ptr(colVec));
	    		} else if (i == 2){
	    			glm::vec3 colVec(0,1,0);
	    			glUniform3fv(uniColor, 1, glm::value_ptr(colVec));
	    		} else if (i == 3){
	    			glm::vec3 colVec(0,1,1);
	    			glUniform3fv(uniColor, 1, glm::value_ptr(colVec));
	    		} else if (i == 4){
	    			glm::vec3 colVec(0,0,1);
	    			glUniform3fv(uniColor, 1, glm::value_ptr(colVec));
	    		}
		    	glDrawArrays(GL_TRIANGLES, num_wall_verts+6+36+180+(36*i), 36); 
    		}
    	}

		if (saveOutput) Win2PPM(screenWidth,screenHeight);

		SDL_GL_SwapWindow(window); //Double buffering

		if (goal->Contains(p_x,p_z)){//(goal[0]-(unit/2)<=p_x)&&(goal[0]+(unit/2)>=p_x)&&(goal[1]-(unit/2)<=p_z)&&(goal[1]+(unit/2)>=p_z)){
			printf("You won!\n");
			quit = true;
		}
	}
	
	//Clean Up
	glDeleteProgram(phongShader);
    glDeleteBuffers(1, vbo);
    glDeleteVertexArrays(1, &vao);

	SDL_GL_DeleteContext(context);
	SDL_Quit();
	return 0;
}

// Create a NULL-terminated string by reading the provided file
static char* readShaderSource(const char* shaderFile){
	FILE *fp;
	long length;
	char *buffer;

	// open the file containing the text of the shader code
	fp = fopen(shaderFile, "r");

	// check for errors in opening the file
	if (fp == NULL) {
		printf("can't open shader source file %s\n", shaderFile);
		return NULL;
	}

	// determine the file size
	fseek(fp, 0, SEEK_END); // move position indicator to the end of the file;
	length = ftell(fp);  // return the value of the current position

	// allocate a buffer with the indicated number of bytes, plus one
	buffer = new char[length + 1];

	// read the appropriate number of bytes from the file
	fseek(fp, 0, SEEK_SET);  // move position indicator to the start of the file
	fread(buffer, 1, length, fp); // read all of the bytes

	// append a NULL character to indicate the end of the string
	buffer[length] = '\0';

	// close the file
	fclose(fp);

	// return the string
	return buffer;
}

// Create a GLSL program object from vertex and fragment shader files
GLuint InitShader(const char* vShaderFileName, const char* fShaderFileName){
	GLuint vertex_shader, fragment_shader;
	GLchar *vs_text, *fs_text;
	GLuint program;

	// check GLSL version
	printf("GLSL version: %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	// Create shader handlers
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

	// Read source code from shader files
	vs_text = readShaderSource(vShaderFileName);
	fs_text = readShaderSource(fShaderFileName);

	// error check
	if (vs_text == NULL) {
		printf("Failed to read from vertex shader file %s\n", vShaderFileName);
		exit(1);
	} else if (DEBUG_ON) {
		printf("Vertex Shader:\n=====================\n");
		printf("%s\n", vs_text);
		printf("=====================\n\n");
	}
	if (fs_text == NULL) {
		printf("Failed to read from fragent shader file %s\n", fShaderFileName);
		exit(1);
	} else if (DEBUG_ON) {
		printf("\nFragment Shader:\n=====================\n");
		printf("%s\n", fs_text);
		printf("=====================\n\n");
	}

	// Load Vertex Shader
	const char *vv = vs_text;
	glShaderSource(vertex_shader, 1, &vv, NULL);  //Read source
	glCompileShader(vertex_shader); // Compile shaders
	
	// Check for errors
	GLint  compiled;
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		printf("Vertex shader failed to compile:\n");
		if (DEBUG_ON) {
			GLint logMaxSize, logLength;
			glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &logMaxSize);
			printf("printing error message of %d bytes\n", logMaxSize);
			char* logMsg = new char[logMaxSize];
			glGetShaderInfoLog(vertex_shader, logMaxSize, &logLength, logMsg);
			printf("%d bytes retrieved\n", logLength);
			printf("error message: %s\n", logMsg);
			delete[] logMsg;
		}
		exit(1);
	}
	
	// Load Fragment Shader
	const char *ff = fs_text;
	glShaderSource(fragment_shader, 1, &ff, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compiled);
	
	//Check for Errors
	if (!compiled) {
		printf("Fragment shader failed to compile\n");
		if (DEBUG_ON) {
			GLint logMaxSize, logLength;
			glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &logMaxSize);
			printf("printing error message of %d bytes\n", logMaxSize);
			char* logMsg = new char[logMaxSize];
			glGetShaderInfoLog(fragment_shader, logMaxSize, &logLength, logMsg);
			printf("%d bytes retrieved\n", logLength);
			printf("error message: %s\n", logMsg);
			delete[] logMsg;
		}
		exit(1);
	}

	// Create the program
	program = glCreateProgram();

	// Attach shaders to program
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);

	// Link and set program to use
	glLinkProgram(program);

	return program;
}

void Win2PPM(int width, int height){
	char outdir[10] = "out/"; //Must be exist!
	int i,j;
	FILE* fptr;
    static int counter = 0;
    char fname[32];
    unsigned char *image;
    
    /* Allocate our buffer for the image */
    image = (unsigned char *)malloc(3*width*height*sizeof(char));
    if (image == NULL) {
      fprintf(stderr,"ERROR: Failed to allocate memory for image\n");
    }
    
    /* Open the file */
    sprintf(fname,"%simage_%04d.ppm",outdir,counter);
    if ((fptr = fopen(fname,"w")) == NULL) {
      fprintf(stderr,"ERROR: Failed to open file for window capture\n");
    }
    
    /* Copy the image into our buffer */
    glReadBuffer(GL_BACK);
    glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,image);
    
    /* Write the PPM file */
    fprintf(fptr,"P6\n%d %d\n255\n",width,height);
    for (j=height-1;j>=0;j--) {
      for (i=0;i<width;i++) {
         fputc(image[3*j*width+3*i+0],fptr);
         fputc(image[3*j*width+3*i+1],fptr);
         fputc(image[3*j*width+3*i+2],fptr);
      }
    }
    
    free(image);
    fclose(fptr);
    counter++;
}