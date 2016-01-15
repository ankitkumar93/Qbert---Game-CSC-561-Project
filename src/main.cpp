#include<iostream>
#include<fstream>
#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<vector>
using namespace std;

//include glew
#include "glew/glew.h"

//include glfw
#include "glfw/glfw3.h"

//include glm
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
using namespace glm;

//include tinyobjloader
#include "tiny_obj_loader\tiny_obj_loader.h"
using namespace tinyobj;

//include SOIL Image Loader
#include "SOIL.h"

//include OpenAL
#include "AL/al.h"
#include "AL/alc.h"
#include "AL/alut.h"

static string sceneRootPath = "./scene/";
static string playerRootPath = "./player/";
static string creatureRootPath = "./creatures/";
static string resourceRootPath = "./resources/";
static string disksRootPath = "./disks/";
static string soundsRootPath = "./sounds/";

static const string score_head = "SCORE:";
static const string fps_head = "FPS:";


//Object Structures
struct cube_obj_struct{
	size_t groupCount;
	vector<shape_t> objectData;
	vector<material_t> materialData;
	GLuint vertexArray;
	GLuint *vertexBuffer;
	GLuint *normalBuffer;
	GLuint *indexBuffer;
};

struct player_obj_struct{
	size_t groupCount;
	vector<shape_t> objectData;
	vector<material_t> materialData;
	GLuint vertexArray;
	GLuint *vertexBuffer;
	GLuint *textureBuffer;
	GLuint *normalBuffer;
	GLuint *indexBuffer;
	GLuint texture;
};

struct creature_obj_struct{
	size_t groupCount;
	vector<shape_t> objectData;
	vector<material_t> materialData;
	GLuint vertexArray;
	GLuint *vertexBuffer;
	GLuint *normalBuffer;
	GLuint *indexBuffer;
};

struct creature_obj_struct_texture{
	size_t groupCount;
	vector<shape_t> objectData;
	vector<material_t> materialData;
	GLuint vertexArray;
	GLuint *vertexBuffer;
	GLuint *normalBuffer;
	GLuint *textureBuffer;
	GLuint *indexBuffer;
	GLuint texture;
};

struct life_obj_struct{
	size_t groupCount;
	vector<shape_t> objectData;
	vector<material_t> materialData;
	GLuint vertexArray;
	GLuint *textureBuffer;
	GLuint *vertexBuffer;
	GLuint *normalBuffer;
	GLuint *indexBuffer;
	GLuint texture;
};

struct score_obj_struct{
	GLuint vertexArray;
	GLuint *textureBuffer;
	GLuint *vertexBuffer;
	GLuint *indexBuffer;
};

struct game_over_obj_struct{
	GLuint vertexArray;
	GLuint *textureBuffer;
	GLuint *vertexBuffer;
	GLuint *indexBuffer;
};

struct game_start_obj_struct{
	GLuint vertexArray;
	GLuint *textureBuffer;
	GLuint *vertexBuffer;
	GLuint *indexBuffer;
};

struct disk_obj_struct{
	size_t groupCount;
	GLuint vertexArray;
	GLuint *textureBuffer;
	GLuint *vertexBuffer;
	GLuint *indexBuffer;
	GLuint* normalBuffer;
	GLuint texture;
	vector<shape_t> objectData;
	vector<material_t> materialData;
};


//Element Structures
struct scene_struct{
	string objFilePath;
	size_t cubeCount;
	GLuint shaderID;
	cube_obj_struct* object;
	mat4* transformMat;
	mat4* initialTransformMat;
	mat4* normalMat;
}scene;

struct life_struct{
	GLuint shaderID;
	life_obj_struct* object;
	mat4* transformMat;
	mat4* normalMat;
};

struct score_struct{
	string textureFilePath;
	score_obj_struct* object;
	float font_size;
	GLuint shaderID;
	vector<vec2> vertexPosition;
	vector<vec2> vertexTexture;
	vector<unsigned int> vertexIndex;
	GLuint texture;
	float* color;
};

//Camera Structure
struct camera_struct{
	vec3 position;
	vec3 lookat;
	vec3 lookup;
	mat4 perspectiveMat;
	mat4 lookAtMat;
	bool inUse;
}camera_fp, camera_tp;

//Window Structure
struct window_struct{
	GLFWwindow * window;
	size_t dims[2];
}window_main;

//Game State Structures
struct state_struct{
	int id;
	bool isCovered;
	state_struct* left;
	state_struct* right;
	state_struct* up;
	state_struct* down;
	int row;
	int column;
};
struct game_data_struct{
	life_struct* life;
	score_struct* score_data;
	size_t total_blocks;
	size_t blocks_covered_count;
	state_struct* states;
	state_struct* current_state;
	int score;
	int total_lives;
	int remaining_lives;
}game_data;

//Player Structure
struct player_struct{
	string objFilePath;
	player_obj_struct* object;
	GLuint shaderID;
	mat4 rotationMatY;
	mat4 rotationMatX;
	mat4 translationMat;
	mat4 scalingMat;
	mat4 transformMat;
	mat4 normalMat;
	int state;
	int dir;
	bool animate;
	bool fall;
	bool show;
	int animation_index;
	int animation_start;
	int animation_duration;
	int fall_duration;
	int animation_interval;
	int fall_animation_interval;
	state_struct* nextState;
}player;

//Material Data Structure (only for the cubes)
struct mtl_data{
	float top_ka[3];
	float top_kd[3];
	float top_ks[3];
	float top_n;

	float left_ka[3];
	float left_kd[3];
	float left_ks[3];
	float left_n;

	float front_ka[3];
	float front_kd[3];
	float front_ks[3];
	float front_n;

	float other_ka[3];
	float other_kd[3];
	float other_ks[3];
	float other_n;
};

//Level Structures
struct level_struct{
	int id;
	mtl_data* active;
	mtl_data* inactive;
};
struct level_data_struct{
	bool transition;
	bool transition_move;
	int frame_count;
	int animation_index;
	int transition_start;
	int transition_duration;
	int transition_interval;
	level_struct * current;
	level_struct * levels;
	size_t level_count;
	size_t levels_completed_count;
}level_data;

//FPS Structure
struct fps_obj_struct{
	GLuint* indexBuffer;
	GLuint* vertexBuffer;
	GLuint* textureBuffer;
	GLuint vertexArray;
};
struct fps_struct{
	string texturFilePath;
	GLuint shaderID;
	fps_obj_struct* object;
	int frame_count;
	int time_start;
	int time_end;
	int time_diff;
	float font_size;
	vector<vec2> vertexPosition;
	vector<vec2> vertexTexture;
	vector<unsigned int> vertexIndex;
	GLuint texture;
	bool show;
	float* color;
}fps;

//Creature Structures
struct creature0_data_struct{
	mat4 rotationMat;
	mat4 translationMat;
	mat4 scalingMat;
	mat4 transformMat;
	mat4 normalMat;
	state_struct* current_state;
	bool show;
	int time_start;
	int duration;
	int start_interval;
	int motion_interval;
};

struct creature1_data_struct{
	mat4* rotationMat;
	mat4* translationMat;
	mat4* scalingMat;
	mat4* transformMat;
	mat4* normalMat;
	state_struct* current_state;
	bool show;
	int time_start;
	int duration;
	int start_interval;
	int motion_interval;
};

struct creature0_struct{
	string objFilePath;
	GLuint shaderID;
	creature_obj_struct* object;
	creature0_data_struct* data;
	bool start;
	bool motion;
	bool collided;
	int dir;
	bool animate;
	int animation_index;
	int animation_start;
	int animation_duration;
	int animation_interval;
	state_struct* nextState;
}creature0;

struct creature1_struct{
	string* objFilePath;
	string textureFilePath;
	GLuint* shaderID;
	creature_obj_struct* object;
	creature_obj_struct_texture* object_texture;
	creature1_data_struct* data;
	bool start;
	bool motion;
	bool collided;
	bool transformed;
	int dir;
	bool animate;
	int animation_index;
	int animation_start;
	int animation_duration;
	int animation_interval;
	state_struct* nextState;
}creature1;

struct creature2_struct{
	string objFilePath;
	GLuint shaderID;
	creature_obj_struct* object;
	creature0_data_struct* data;
	bool start;
	bool motion;
	bool collided;
	int dir;
	bool animate;
	int animation_index;
	int animation_start;
	int animation_duration;
	int animation_interval;
	state_struct* nextState;
}creature2;

//Technical Structures
struct tool_struct{
	bool key_press;
}tool;

//Game Over Structure
struct game_over_struct{
	string textureFilePath;
	game_over_obj_struct* object;
	game_over_obj_struct* object_2;
	bool state;
	bool show;
	bool complete;
	int duration;
	int time_start;
	float font_size;
	GLuint shaderID;
	vector<vec2> vertexPosition;
	vector<vec2> vertexTexture;
	vector<unsigned int> vertexIndex;
	vector<vec2> vertexPosition_2;
	vector<vec2> vertexTexture_2;
	vector<unsigned int> vertexIndex_2;
	GLuint texture;
	string text_to_show, text_to_show_2;
	float* color;
}game_over;

//Game Start Structure
struct game_start_struct{
	string textureFilePath;
	game_start_obj_struct* object;
	bool state;
	float font_size_top;
	float font_size_bottom;
	GLuint shaderID;
	vector<vec2>* vertexPosition;
	vector<vec2>* vertexTexture;
	vector<unsigned int>* vertexIndex;
	GLuint texture;
	string text_top;
	string text_bottom;
	float** color;
}game_start;

//Shader Structures
struct shader_data{
	GLuint texture;
	GLuint lighting;
	GLuint lives;
	GLuint text;
}shader;

//Disk Structures
struct disk_data_struct{
	int row_near;
	int column_near;
	int jump_movement;
	bool enabled;
	mat4 translationMat;
	mat4 rotationMatX;
	mat4 rotationMatY;
	mat4 scalingMat;
	mat4 transformMat;
	mat4 normalMat;
	int r_animation_duration;
	int r_animation_start;
	int r_animation_interval;
};
struct disk_struct{
	string objFilePath;
	string textureFilePath;
	disk_data_struct* left;
	disk_data_struct* right;
	disk_obj_struct* object;
	GLuint shaderID;
}disks;

//Animation Structures
struct animation_struct{
	float* x_down;
	float* y_down;
	float* z_down;

	float* x_up;
	float* y_up;
	float* z_up;

	float* x_left;
	float* y_left;
	float* z_left;

	float* x_right;
	float* y_right;
	float* z_right;

	int frame_count;
}animation;

//Sound Structures
struct sound_struct{
	//Game Start
	string sound_game_start;
	ALuint game_start_buffer, game_start_source;
	ALint  game_start_state;

	//Player
	string sound_player_hop, sound_player_fall, sound_player_collision, sound_game_over, sound_coily_hop, sound_bonk;
	ALuint player_hop_buffer, player_fall_buffer, player_hop_source, player_fall_source, player_collision_buffer, player_collision_source, coily_hop_buffer, coily_hop_source, game_over_buffer, game_over_source, bonk_buffer, bonk_source;
	ALint player_hop_state, player_fall_state, player_collision_state, coily_hop_state, game_over_state, bonk_state;
}sounds;

//Function signatures

//Window Maintainence
void checkWindowResize();

//Initializations
void initialize();
void initialize_Shaders();
void initialize_Levels();
void initialize_Scene();
void initialize_Player();
void initialize_Creatures();
void initialize_Disks();
void initialize_Creature_Controller();
void initialize_Game();
void initialize_Game_State_Tree();
void initialize_Life();
void initialize_Score();
void initialize_FPS();
void initialize_Tools();
void initialize_Game_Over();
void initialize_Game_Start();
void initialize_Animation();
void initialize_Animation_Player();
void initialize_Animation_Creature();
void initialize_Sounds();

//Camera Control
void initCameras();
void setView_FirstPerson();
void setView_ThirdPerson();
void toggleCamera();

//Rendering Functions
void startRender();
void renderScene();
void renderPlayer();
void renderCreature_0();
void renderCreature_1();
void renderCreature_1_normal();
void renderCreature_1_transformed();
void renderCreature_2();
void render_Disks();
void render_Disks_left();
void render_Disks_right();
void renderLives();
void renderScore();
void renderFPS();
void renderGameOver();
void renderGameOver_Over();
void renderGameOver_Complete();
void renderGameStart();

//Buffer Handling
void setBuffers();
void setBuffers_Scene();
void setBuffers_Player();
void setBuffers_Creatures();
void setBuffers_Disks();
void setBuffers_Life();
void setBuffers_Score();
void setBuffers_FPS();
void setBuffers_Game_Over();
void setBuffers_Game_Start();

//Handle KeyPress
void handleKeyPress(GLFWwindow *, int, int, int, int);

//Player Movement
void movePlayer_Up();
void movePlayer_Down();
void movePlayer_Left();
void movePlayer_Right();
void resetPlayerPosition();

//Player Transforms
void transformPlayer_Up();
void transformPlayer_Down();
void transformPlayer_Left();
void transformPlayer_Right();

//Player Maintainence
void playerLost(int);
void playerLoseLife(int);

//Game State Maintainence
state_struct* getState(state_struct*, int);
void moveToState(state_struct*);
void resetGameData();
void resetLevelData();
void resetCreatureData(bool);
void resetPlayerData();

//Level Maintainence
void moveToNextLevel();

//Texture Loading
GLuint loadTexture(string, GLuint);

//Score Render Data Management
void updateScoreData();
void manageScoreData();

//FPS Maintainence
void calculateFPS();
void updateFPSData();
void manageFPSData();
void toggleFPS();

//Creature AI Handling
void handleCreature0AI();
void handleCreature1AI();
void handleCreature1AI_normal();
void handleCreature1AI_transformed();
void handleCreature2AI();

//Creature0 Handling
void moveCreature0();
void introduceCreature0();

//Creature1 Handling
void moveCreature1_normal();
void introduceCreature1_normal();
void moveCreature1_transformed();
void introduceCreature1_transformed();
int moveCreature1_row(int);
int moveCreature1_column(int);

//Creature2 Handling
void moveCreature2();
void introduceCreature2();

//Collision Handling
int checkCollisions();
void handleCollision();
void initiateCollision(int);

//Game Over Status Handling
void gameOver();
void handleGameOver();
void createGameOverObject();

//Game Start Status Handling
void gameStart();
void createGameStartObject();

//Disk handling
void moveToDiskLeft();
void moveToDiskRight();

//Animation
//Disk Animation
void animateDiskLeft();
void animateDiskRight();

//Player Animation
void animatePlayer();
void animatePlayerFall();

//Creature Animation
void animateCreature0();
void animateCreature1();
void animateCreature1_normal();
void animateCreature1_transformed();
void animateCreature2();

//Level Transition
void levelTransition();

// Load and runtime compile GLSL shaders.
GLuint LoadShaders( const char* vertex_file_path, const char* fragment_file_path ){

	// Create the shaders
	GLuint VertexShaderID = glCreateShader( GL_VERTEX_SHADER );
	GLuint FragmentShaderID = glCreateShader( GL_FRAGMENT_SHADER );

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}



	GLint Result = GL_FALSE;
	int InfoLogLength;



	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

int main(){

	// initialize GLFW
	if( !glfwInit() ) {
		fprintf( stderr, "Failed to initialize GLFW!\n" );
		return -1;
	}

	glfwWindowHint( GLFW_SAMPLES, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

	window_main.dims[0] = 1024;
	window_main.dims[1] = 768;

	// open a window and create its OpenGL context
	window_main.window = glfwCreateWindow( window_main.dims[0], window_main.dims[1], "Qbert - The Game - CSC 561", NULL, NULL );
	if( window_main.window == NULL ) {
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.\n" );
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent( window_main.window );

	// initialize GLEW
	glewExperimental = GL_TRUE; // Needed for core profile
	if( glewInit() != GLEW_OK ) {
		fprintf( stderr, "Failed to initialize GLEW\n" );
		return -1;
	}

	// ensure we can capture the escape key being pressed below
	glfwSetInputMode( window_main.window, GLFW_STICKY_KEYS, GL_TRUE );

	// set the GL clear color
	glClearColor( 0.f, 0.f, 0.f, 0.f );

	//initliaze the basics
	initialize();

	//set the buffers for the objects
	setBuffers();

	//set camera details
	initCameras();
	setView_ThirdPerson();

	//start rendering the scene
	startRender();
	return 0;
}

//Rendering Functions
void startRender(){
	glClearColor(0.f,0.f,0.f,0.f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glfwSetKeyCallback(window_main.window, handleKeyPress);

	do{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Window and FPS
		checkWindowResize();
		calculateFPS();

		if(game_start.state)
			renderGameStart();
		else{
			if(!level_data.transition){
				//Player Fall
				if(player.fall){
					animatePlayerFall();
				}
				else{
					//AI Handlers
					if(!game_over.state){
						if(creature0.animate)
						animateCreature0();
						else
							handleCreature0AI();
						if(creature1.animate)
							animateCreature1();
						else
							handleCreature1AI();
						if(creature2.animate)
							animateCreature2();
						else
							handleCreature2AI();

						//Collision Check
						if(!creature0.collided && !creature1.collided && !creature2.collided)
							initiateCollision(checkCollisions());
					}
					else
						handleGameOver();
				}

				//Animation handlers
				if(player.animate){
					animatePlayer();
				}
			}else{
				levelTransition();
			}

			//Renders
			renderScene();
			if(player.show)
				renderPlayer();
			if(creature0.data->show)
				renderCreature_0();
			if(creature1.data->show)
				renderCreature_1();
			if(creature2.data->show)
				renderCreature_2();
			render_Disks();
			renderLives();
			renderScore();
			if(game_over.show)
				renderGameOver();
		}
		if(fps.show)
			renderFPS();
		glfwSwapBuffers(window_main.window);
		glfwPollEvents();
	}while(glfwGetKey(window_main.window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window_main.window) == 0);

	glfwTerminate();
}

void renderScene(){
	glUseProgram(scene.shaderID);

	mtl_data* current_level = NULL;

	//Render the Scene
	for(size_t i = 0; i < scene.cubeCount; i++){

		//Set the transform and normal matrices
		glUniformMatrix4fv(glGetUniformLocation(scene.shaderID, "transformMat"), 1, GL_FALSE, &scene.transformMat[i][0][0]);
		glUniformMatrix4fv(glGetUniformLocation(scene.shaderID, "normalMat"), 1, GL_FALSE, &scene.normalMat[i][0][0]);

		if(!game_data.states[i].isCovered)
			current_level = level_data.current->active;
		else
			current_level = level_data.current->inactive;
		
		//Draw the Cube
		for(size_t j = 0; j < scene.object->groupCount; j++){

			//Set the material color data
			if(j == 0){
				glProgramUniform3fv(scene.shaderID, glGetUniformLocation(scene.shaderID, "ka"), 1, current_level->top_ka);
				glProgramUniform3fv(scene.shaderID, glGetUniformLocation(scene.shaderID, "kd"), 1, current_level->top_kd);
				glProgramUniform3fv(scene.shaderID, glGetUniformLocation(scene.shaderID, "ks"), 1, current_level->top_ks);
				glProgramUniform1f(scene.shaderID, glGetUniformLocation(scene.shaderID, "n"), current_level->top_n);
			}else if(j == 2){
				glProgramUniform3fv(scene.shaderID, glGetUniformLocation(scene.shaderID, "ka"), 1, current_level->front_ka);
				glProgramUniform3fv(scene.shaderID, glGetUniformLocation(scene.shaderID, "kd"), 1, current_level->front_kd);
				glProgramUniform3fv(scene.shaderID, glGetUniformLocation(scene.shaderID, "ks"), 1, current_level->front_ks);
				glProgramUniform1f(scene.shaderID, glGetUniformLocation(scene.shaderID, "n"), current_level->front_n);
			}else if(j == 4){
				glProgramUniform3fv(scene.shaderID, glGetUniformLocation(scene.shaderID, "ka"), 1, current_level->left_ka);
				glProgramUniform3fv(scene.shaderID, glGetUniformLocation(scene.shaderID, "kd"), 1, current_level->left_kd);
				glProgramUniform3fv(scene.shaderID, glGetUniformLocation(scene.shaderID, "ks"), 1, current_level->left_ks);
				glProgramUniform1f(scene.shaderID, glGetUniformLocation(scene.shaderID, "n"), current_level->left_n);
			}else{
				glProgramUniform3fv(scene.shaderID, glGetUniformLocation(scene.shaderID, "ka"), 1, current_level->other_kd);
				glProgramUniform3fv(scene.shaderID, glGetUniformLocation(scene.shaderID, "kd"), 1, current_level->other_kd);
				glProgramUniform3fv(scene.shaderID, glGetUniformLocation(scene.shaderID, "ks"), 1, current_level->other_ks);
				glProgramUniform1f(scene.shaderID, glGetUniformLocation(scene.shaderID, "n"), current_level->other_n);
			}

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);

			glBindBuffer(GL_ARRAY_BUFFER, scene.object->vertexBuffer[j]);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

			glBindBuffer(GL_ARRAY_BUFFER, scene.object->normalBuffer[j]);
			glVertexAttribPointer(1 ,3, GL_FLOAT, GL_FALSE, 0, (void*)0);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, scene.object->indexBuffer[j]);

			glDrawElements(GL_TRIANGLES, scene.object->objectData[j].mesh.indices.size(), GL_UNSIGNED_INT, (void*)0);

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);

		}
	}
}

void renderPlayer(){
	//Renderring the Player
	glUseProgram(player.shaderID);
	//Set the transform and normal Matrices
	glUniformMatrix4fv(glGetUniformLocation(player.shaderID, "transformMat"), 1, GL_FALSE, &player.transformMat[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(player.shaderID, "normalMat"), 1, GL_FALSE, &player.normalMat[0][0]);

	//set the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, player.object->texture);
	glProgramUniform1i(player.shaderID, glGetUniformLocation(player.shaderID, "texture"), 0);



	//Draw the Player
	for(size_t i = 0; i < player.object->groupCount; i++){

		//set the material color data
		glProgramUniform3fv(player.shaderID, glGetUniformLocation(player.shaderID, "ka"), 1, player.object->materialData[player.object->objectData[i].mesh.material_ids[0]].ambient);
		glProgramUniform3fv(player.shaderID, glGetUniformLocation(player.shaderID, "kd"), 1, player.object->materialData[player.object->objectData[i].mesh.material_ids[0]].diffuse);
		glProgramUniform3fv(player.shaderID, glGetUniformLocation(player.shaderID, "ks"), 1, player.object->materialData[player.object->objectData[i].mesh.material_ids[0]].specular);
		glProgramUniform1f(player.shaderID, glGetUniformLocation(player.shaderID, "n"), player.object->materialData[player.object->objectData[i].mesh.material_ids[0]].shininess);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, player.object->vertexBuffer[i]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, player.object->normalBuffer[i]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, player.object->textureBuffer[i]);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, player.object->indexBuffer[i]);

		glDrawElements(GL_TRIANGLES, player.object->objectData[i].mesh.indices.size(), GL_UNSIGNED_INT, (void*)0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	}
}

void renderCreature_0(){

	//Rendering the Creatures
	glUseProgram(creature0.shaderID);

	//Set the transform and normal Matrices
	glUniformMatrix4fv(glGetUniformLocation(creature0.shaderID, "transformMat"), 1, GL_FALSE, &creature0.data->transformMat[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(creature0.shaderID, "normalMat"), 1, GL_FALSE, &creature0.data->normalMat[0][0]);

	creature_obj_struct* current_creature = creature0.object;

	//Draw the creatures
	for(size_t j = 0; j < current_creature->groupCount; j++){

		//set the material color data
		glProgramUniform3fv(creature0.shaderID, glGetUniformLocation(creature0.shaderID, "ka"), 1,current_creature->materialData[current_creature->objectData[j].mesh.material_ids[0]].ambient);
		glProgramUniform3fv(creature0.shaderID, glGetUniformLocation(creature0.shaderID, "kd"), 1, current_creature->materialData[current_creature->objectData[j].mesh.material_ids[0]].diffuse);
		glProgramUniform3fv(creature0.shaderID, glGetUniformLocation(creature0.shaderID, "ks"), 1, current_creature->materialData[current_creature->objectData[j].mesh.material_ids[0]].specular);
		glProgramUniform1f(creature0.shaderID, glGetUniformLocation(creature0.shaderID, "n"), current_creature->materialData[current_creature->objectData[j].mesh.material_ids[0]].shininess);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, current_creature->vertexBuffer[j]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, current_creature->normalBuffer[j]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, current_creature->indexBuffer[j]);

		glDrawElements(GL_TRIANGLES, current_creature->objectData[j].mesh.indices.size(), GL_UNSIGNED_INT, (void*)0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}
}

void renderCreature_1(){
	if(creature1.transformed && !creature1.start && creature1.motion)
		renderCreature_1_transformed();
	else
		renderCreature_1_normal();
}

void renderCreature_2(){
	//Rendering the Creatures
	glUseProgram(creature2.shaderID);

	//Set the transform and normal Matrices
	glUniformMatrix4fv(glGetUniformLocation(creature2.shaderID, "transformMat"), 1, GL_FALSE, &creature2.data->transformMat[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(creature2.shaderID, "normalMat"), 1, GL_FALSE, &creature2.data->normalMat[0][0]);

	creature_obj_struct* current_creature = creature2.object;

	//Draw the creatures
	for(size_t j = 0; j < current_creature->groupCount; j++){

		//set the material color data
		glProgramUniform3fv(creature2.shaderID, glGetUniformLocation(creature2.shaderID, "ka"), 1,current_creature->materialData[current_creature->objectData[j].mesh.material_ids[0]].ambient);
		glProgramUniform3fv(creature2.shaderID, glGetUniformLocation(creature2.shaderID, "kd"), 1, current_creature->materialData[current_creature->objectData[j].mesh.material_ids[0]].diffuse);
		glProgramUniform3fv(creature2.shaderID, glGetUniformLocation(creature2.shaderID, "ks"), 1, current_creature->materialData[current_creature->objectData[j].mesh.material_ids[0]].specular);
		glProgramUniform1f(creature2.shaderID, glGetUniformLocation(creature2.shaderID, "n"), current_creature->materialData[current_creature->objectData[j].mesh.material_ids[0]].shininess);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, current_creature->vertexBuffer[j]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, current_creature->normalBuffer[j]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, current_creature->indexBuffer[j]);

		glDrawElements(GL_TRIANGLES, current_creature->objectData[j].mesh.indices.size(), GL_UNSIGNED_INT, (void*)0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}
}

void renderCreature_1_normal(){
	//Rendering the Creatures
	glUseProgram(creature1.shaderID[0]);

	//Set the transform and normal Matrices
	glUniformMatrix4fv(glGetUniformLocation(creature1.shaderID[0], "transformMat"), 1, GL_FALSE, &creature1.data->transformMat[0][0][0]);
	glUniformMatrix4fv(glGetUniformLocation(creature1.shaderID[0], "normalMat"), 1, GL_FALSE, &creature1.data->normalMat[0][0][0]);

	creature_obj_struct* current_creature = creature1.object;

	//Draw the creatures
	for(size_t j = 0; j < current_creature->groupCount; j++){

		//set the material color data
		glProgramUniform3fv(creature1.shaderID[0], glGetUniformLocation(creature1.shaderID[0], "ka"), 1,current_creature->materialData[current_creature->objectData[j].mesh.material_ids[0]].ambient);
		glProgramUniform3fv(creature1.shaderID[0], glGetUniformLocation(creature1.shaderID[0], "kd"), 1, current_creature->materialData[current_creature->objectData[j].mesh.material_ids[0]].diffuse);
		glProgramUniform3fv(creature1.shaderID[0], glGetUniformLocation(creature1.shaderID[0], "ks"), 1, current_creature->materialData[current_creature->objectData[j].mesh.material_ids[0]].specular);
		glProgramUniform1f(creature1.shaderID[0], glGetUniformLocation(creature1.shaderID[0], "n"), current_creature->materialData[current_creature->objectData[j].mesh.material_ids[0]].shininess);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, current_creature->vertexBuffer[j]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, current_creature->normalBuffer[j]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, current_creature->indexBuffer[j]);

		glDrawElements(GL_TRIANGLES, current_creature->objectData[j].mesh.indices.size(), GL_UNSIGNED_INT, (void*)0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}
}

void renderCreature_1_transformed(){
	//Rendering the Creatures
	glUseProgram(creature1.shaderID[1]);

	//Set the transform and normal Matrices
	glUniformMatrix4fv(glGetUniformLocation(creature1.shaderID[0], "transformMat"), 1, GL_FALSE, &creature1.data->transformMat[1][0][0]);
	glUniformMatrix4fv(glGetUniformLocation(creature1.shaderID[0], "normalMat"), 1, GL_FALSE, &creature1.data->normalMat[1][0][0]);

	creature_obj_struct_texture* current_creature = creature1.object_texture;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, current_creature->texture);
	glProgramUniform1i(creature1.shaderID[1], glGetUniformLocation(creature1.shaderID[1], "texture"), 0);

	//Draw the creatures
	for(size_t j = 0; j < current_creature->groupCount; j++){

		//set the material color data
		glProgramUniform3fv(creature1.shaderID[0], glGetUniformLocation(creature1.shaderID[0], "ka"), 1,current_creature->materialData[current_creature->objectData[j].mesh.material_ids[0]].ambient);
		glProgramUniform3fv(creature1.shaderID[0], glGetUniformLocation(creature1.shaderID[0], "kd"), 1, current_creature->materialData[current_creature->objectData[j].mesh.material_ids[0]].diffuse);
		glProgramUniform3fv(creature1.shaderID[0], glGetUniformLocation(creature1.shaderID[0], "ks"), 1, current_creature->materialData[current_creature->objectData[j].mesh.material_ids[0]].specular);
		glProgramUniform1f(creature1.shaderID[0], glGetUniformLocation(creature1.shaderID[0], "n"), current_creature->materialData[current_creature->objectData[j].mesh.material_ids[0]].shininess);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, current_creature->vertexBuffer[j]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, current_creature->normalBuffer[j]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, current_creature->textureBuffer[j]);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, current_creature->indexBuffer[j]);

		glDrawElements(GL_TRIANGLES, current_creature->objectData[j].mesh.indices.size(), GL_UNSIGNED_INT, (void*)0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
	}
}

void render_Disks(){
	if(disks.left->enabled){
		animateDiskLeft();
		render_Disks_left();
	}
	if(disks.right->enabled){
		animateDiskRight();
		render_Disks_right();
	}
}

void render_Disks_left(){
	//Renderring the Player
	glUseProgram(disks.shaderID);

	//Set the transform and normal Matrices
	glUniformMatrix4fv(glGetUniformLocation(disks.shaderID, "transformMat"), 1, GL_FALSE, &disks.left->transformMat[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(disks.shaderID, "normalMat"), 1, GL_FALSE, &disks.left->normalMat[0][0]);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, disks.object->texture);
	glProgramUniform1i(disks.shaderID, glGetUniformLocation(disks.shaderID, "texture"), 0);


	//Draw the Player
	for(size_t i = 0; i < disks.object->groupCount; i++){

		//set the material color data
		glProgramUniform3fv(disks.shaderID, glGetUniformLocation(disks.shaderID, "ka"), 1, disks.object->materialData[disks.object->objectData[i].mesh.material_ids[0]].ambient);
		glProgramUniform3fv(disks.shaderID, glGetUniformLocation(disks.shaderID, "kd"), 1, disks.object->materialData[disks.object->objectData[i].mesh.material_ids[0]].diffuse);
		glProgramUniform3fv(disks.shaderID, glGetUniformLocation(disks.shaderID, "ks"), 1, disks.object->materialData[disks.object->objectData[i].mesh.material_ids[0]].specular);
		glProgramUniform1f(disks.shaderID, glGetUniformLocation(disks.shaderID, "n"), disks.object->materialData[disks.object->objectData[i].mesh.material_ids[0]].shininess);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, disks.object->vertexBuffer[i]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, disks.object->normalBuffer[i]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, disks.object->textureBuffer[i]);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, disks.object->indexBuffer[i]);

		glDrawElements(GL_TRIANGLES, disks.object->objectData[i].mesh.indices.size(), GL_UNSIGNED_INT, (void*)0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	}
}

void render_Disks_right(){
	//Renderring the Player
	glUseProgram(disks.shaderID);

	//Set the transform and normal Matrices
	glUniformMatrix4fv(glGetUniformLocation(disks.shaderID, "transformMat"), 1, GL_FALSE, &disks.right->transformMat[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(disks.shaderID, "normalMat"), 1, GL_FALSE, &disks.right->normalMat[0][0]);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, disks.object->texture);
	glProgramUniform1i(disks.shaderID, glGetUniformLocation(disks.shaderID, "texture"), 0);


	//Draw the Player
	for(size_t i = 0; i < disks.object->groupCount; i++){

		//set the material color data
		glProgramUniform3fv(disks.shaderID, glGetUniformLocation(disks.shaderID, "ka"), 1, disks.object->materialData[disks.object->objectData[i].mesh.material_ids[0]].ambient);
		glProgramUniform3fv(disks.shaderID, glGetUniformLocation(disks.shaderID, "kd"), 1, disks.object->materialData[disks.object->objectData[i].mesh.material_ids[0]].diffuse);
		glProgramUniform3fv(disks.shaderID, glGetUniformLocation(disks.shaderID, "ks"), 1, disks.object->materialData[disks.object->objectData[i].mesh.material_ids[0]].specular);
		glProgramUniform1f(disks.shaderID, glGetUniformLocation(disks.shaderID, "n"), disks.object->materialData[disks.object->objectData[i].mesh.material_ids[0]].shininess);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, disks.object->vertexBuffer[i]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, disks.object->normalBuffer[i]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, disks.object->textureBuffer[i]);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, disks.object->indexBuffer[i]);

		glDrawElements(GL_TRIANGLES, disks.object->objectData[i].mesh.indices.size(), GL_UNSIGNED_INT, (void*)0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	}
}

void renderLives(){
	glUseProgram(game_data.life->shaderID);

	for(int i = 0; i < game_data.remaining_lives - 1; i++){
		glUniformMatrix4fv(glGetUniformLocation(game_data.life->shaderID, "transformMat"), 1, GL_FALSE, &game_data.life->transformMat[i][0][0]);
		glUniformMatrix4fv(glGetUniformLocation(game_data.life->shaderID, "normalMat"), 1, GL_FALSE, &game_data.life->normalMat[i][0][0]);

		//set texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, player.object->texture);
		glProgramUniform1i(game_data.life->shaderID, glGetUniformLocation(game_data.life->shaderID, "texture"), 0);

		for(size_t j = 0; j < game_data.life->object->groupCount; j++){

			//set the material color data
			glProgramUniform3fv(game_data.life->shaderID, glGetUniformLocation(game_data.life->shaderID, "ka"), 1, game_data.life->object->materialData[game_data.life->object->objectData[j].mesh.material_ids[0]].ambient);
			glProgramUniform3fv(game_data.life->shaderID, glGetUniformLocation(game_data.life->shaderID, "kd"), 1, game_data.life->object->materialData[game_data.life->object->objectData[j].mesh.material_ids[0]].diffuse);
			glProgramUniform3fv(game_data.life->shaderID, glGetUniformLocation(game_data.life->shaderID, "ks"), 1, game_data.life->object->materialData[game_data.life->object->objectData[j].mesh.material_ids[0]].specular);
			glProgramUniform1f(game_data.life->shaderID, glGetUniformLocation(game_data.life->shaderID, "n"), game_data.life->object->materialData[game_data.life->object->objectData[j].mesh.material_ids[0]].shininess);

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);

			glBindBuffer(GL_ARRAY_BUFFER, game_data.life->object->vertexBuffer[j]);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

			glBindBuffer(GL_ARRAY_BUFFER, game_data.life->object->normalBuffer[j]);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

			glBindBuffer(GL_ARRAY_BUFFER, game_data.life->object->textureBuffer[j]);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, game_data.life->object->indexBuffer[j]);

			glDrawElements(GL_TRIANGLES, game_data.life->object->objectData[j].mesh.indices.size(), GL_UNSIGNED_INT, (void*)0);

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);

		}
	}
}

void renderScore(){
	glUseProgram(game_data.score_data->shaderID);

	glProgramUniform3fv(game_data.score_data->shaderID, glGetUniformLocation(game_data.score_data->shaderID, "color"), 1, game_data.score_data->color);

	//set texture 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, game_data.score_data->texture);
	glProgramUniform1i(game_data.score_data->shaderID, glGetUniformLocation(game_data.score_data->shaderID, "texture"), 0);

	//draw the score
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, *game_data.score_data->object->vertexBuffer);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, *game_data.score_data->object->textureBuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *game_data.score_data->object->indexBuffer);

	glDrawElements(GL_TRIANGLES, game_data.score_data->vertexIndex.size(), GL_UNSIGNED_INT, (void*)0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void renderFPS(){
	glUseProgram(fps.shaderID);

	glProgramUniform3fv(fps.shaderID, glGetUniformLocation(fps.shaderID, "color"), 1, fps.color);

	//set texture 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, game_data.score_data->texture);
	glProgramUniform1i(fps.shaderID, glGetUniformLocation(fps.shaderID, "texture"), 0);

	//draw the score
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, *fps.object->vertexBuffer);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, *fps.object->textureBuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *fps.object->indexBuffer);

	glDrawElements(GL_TRIANGLES, fps.vertexIndex.size(), GL_UNSIGNED_INT, (void*)0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void renderGameOver(){
	if(game_over.complete)
		renderGameOver_Complete();
	else
		renderGameOver_Over();
}

void renderGameOver_Complete(){
	glUseProgram(game_over.shaderID);

	glProgramUniform3fv(game_over.shaderID, glGetUniformLocation(game_over.shaderID, "color"), 1, game_over.color);

	//set texture 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, game_over.texture);
	glProgramUniform1i(game_over.shaderID, glGetUniformLocation(game_over.shaderID, "texture"), 0);

	//draw the score
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, *game_over.object_2->vertexBuffer);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, *game_over.object_2->textureBuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *game_over.object_2->indexBuffer);

	glDrawElements(GL_TRIANGLES, game_over.vertexIndex_2.size(), GL_UNSIGNED_INT, (void*)0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void renderGameOver_Over(){
	glUseProgram(game_over.shaderID);

	glProgramUniform3fv(game_over.shaderID, glGetUniformLocation(game_over.shaderID, "color"), 1, game_over.color);

	//set texture 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, game_over.texture);
	glProgramUniform1i(game_over.shaderID, glGetUniformLocation(game_over.shaderID, "texture"), 0);

	//draw the score
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, *game_over.object->vertexBuffer);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, *game_over.object->textureBuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *game_over.object->indexBuffer);

	glDrawElements(GL_TRIANGLES, game_over.vertexIndex.size(), GL_UNSIGNED_INT, (void*)0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void renderGameStart(){
	glUseProgram(game_start.shaderID);

	//set texture 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, game_start.texture);
	glProgramUniform1i(game_start.shaderID, glGetUniformLocation(game_start.shaderID, "texture"), 0);


	for(size_t i = 0; i < 2; i++){

		glProgramUniform3fv(game_start.shaderID, glGetUniformLocation(game_start.shaderID, "color"), 1, game_start.color[i]);
	
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, *game_start.object[i].vertexBuffer);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, *game_start.object[i].textureBuffer);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *game_start.object[i].indexBuffer);

		glDrawElements(GL_TRIANGLES, game_start.vertexIndex[i].size(), GL_UNSIGNED_INT, (void*)0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}
}

//Window Maintainence
void checkWindowResize(){
	int width, height;
	glfwGetWindowSize(window_main.window, &width, &height);

	if(width == (int)window_main.dims[0] && height == (int)window_main.dims[1])
		return;

	size_t pos[2] = {0, 0};

	if(width < height){
		int temp_height = height;
		height = (width*window_main.dims[1])/window_main.dims[0];
		pos[1] = (temp_height - height)/2;
	}
	else{
		int temp_width = width;
		width = (height*window_main.dims[0])/window_main.dims[1];
		pos[0] = (temp_width - width)/2;
	}

	window_main.dims[0] = (size_t)width;
	window_main.dims[1] = (size_t)height;

	glViewport(pos[0], pos[1], window_main.dims[0], window_main.dims[1]);
}

//Initialization Functions
//initialize the basics of all the components of the game
void initialize(){
	//initialize object filepaths
	scene.objFilePath = "cube.obj";

	player.objFilePath = "qzard.obj";

	creature0.objFilePath = "sphere_red.obj";

	creature1.objFilePath = new string[2];
	creature1.objFilePath[0] = "sphere_pink.obj";
	creature1.objFilePath[1] = "coily.obj";

	creature2.objFilePath = "android.obj";

	disks.objFilePath = "disk.obj";


	//call the initialization functions
	initialize_Shaders();

	initialize_Scene();

	initialize_Player();

	initialize_Creatures();

	initialize_Disks();

	initialize_Game();

	initialize_Life();

	initialize_Score();

	initialize_FPS();

	initialize_Levels();

	initialize_Tools();

	initialize_Game_Over();

	initialize_Game_Start();

	initialize_Animation();
	
	initialize_Sounds();
}

void initialize_Shaders(){
	shader.texture = LoadShaders("shaders/vs_texture.glsl","shaders/fs_texture.glsl");
	shader.lighting = LoadShaders("shaders/vs_lighting.glsl","shaders/fs_lighting.glsl");
	shader.text = LoadShaders("shaders/vs_text.glsl","shaders/fs_text.glsl");
	shader.lives = LoadShaders("shaders/vs_lives.glsl","shaders/fs_lives.glsl");
}

//initialize the levels for the game
void initialize_Levels(){
	//level transition
	level_data.transition = false;
	level_data.transition_move = false;
	level_data.transition_start = 0;
	level_data.transition_duration = 0;
	level_data.transition_interval = 50;
	level_data.animation_index = 0;
	level_data.frame_count = 40;

	level_data.level_count = 2;
	level_data.levels = new level_struct[level_data.level_count];
	
	//Define Data for Level 1
	level_struct* new_level = &(level_data.levels[0]);
	new_level->id = 0;

	//MTL Data for Active Cube
	new_level->active = new mtl_data;

	float ka[3] = {1.0f, 1.0f, 0.0f};
	float kd[3] = {1.0f, 1.0f, 0.0f};
	float ks[3] = {0.0f, 0.0f, 0.0f};
	float n = 1.0;

	//Top Face
	memcpy(new_level->active->top_ka, ka, sizeof(ka));
	memcpy(new_level->active->top_kd, kd, sizeof(kd));
	memcpy(new_level->active->top_ks, ks, sizeof(ks));
	new_level->active->top_n = n;

	ka[0] = 0.8f; ka[1] = 0.6f; ka[2] = 0.0f;
	kd[0] = 0.8f; kd[1] = 0.6f; kd[2] = 0.0f;

	//Left Face
	memcpy(new_level->active->left_ka, ka, sizeof(ka));
	memcpy(new_level->active->left_kd, kd, sizeof(kd));
	memcpy(new_level->active->left_ks, ks, sizeof(ks));
	new_level->active->left_n = n;

	ka[0] = 1.0f; ka[1] = 0.6f; ka[2] = 0.0f;
	kd[0] = 1.0f; kd[1] = 0.6f; kd[2] = 0.0f;

	//Front Face
	memcpy(new_level->active->front_ka, ka, sizeof(ka));
	memcpy(new_level->active->front_kd, kd, sizeof(kd));
	memcpy(new_level->active->front_ks, ks, sizeof(ks));
	new_level->active->front_n = n;

	ka[0] = 1.0f; ka[1] = 1.0f; ka[2] = 1.0f;
	kd[0] = 1.0f; kd[1] = 1.0f; kd[2] = 1.0f;

	//Other Faces
	memcpy(new_level->active->other_ka, ka, sizeof(ka));
	memcpy(new_level->active->other_kd, kd, sizeof(kd));
	memcpy(new_level->active->other_ks, ks, sizeof(ks));
	new_level->active->other_n = n;

	//MTL Data for InActive Cube
	new_level->inactive = new mtl_data;

	ka[0] = 0.0f; ka[1] = 0.0f; ka[2] = 1.0f;
	kd[0] = 0.0f; kd[1] = 0.0f; kd[2] = 1.0f;

	//Top Face
	memcpy(new_level->inactive->top_ka, ka, sizeof(ka));
	memcpy(new_level->inactive->top_kd, kd, sizeof(kd));
	memcpy(new_level->inactive->top_ks, ks, sizeof(ks));
	new_level->inactive->top_n = n;

	ka[0] = 0.8f; ka[1] = 0.6f; ka[2] = 0.0f;
	kd[0] = 0.8f; kd[1] = 0.6f; kd[2] = 0.0f;

	//Left Face
	memcpy(new_level->inactive->left_ka, ka, sizeof(ka));
	memcpy(new_level->inactive->left_kd, kd, sizeof(kd));
	memcpy(new_level->inactive->left_ks, ks, sizeof(ks));
	new_level->inactive->left_n = n;

	ka[0] = 1.0f; ka[1] = 0.6f; ka[2] = 0.0f;
	kd[0] = 1.0f; kd[1] = 0.6f; kd[2] = 0.0f;

	//Front Face
	memcpy(new_level->inactive->front_ka, ka, sizeof(ka));
	memcpy(new_level->inactive->front_kd, kd, sizeof(kd));
	memcpy(new_level->inactive->front_ks, ks, sizeof(ks));
	new_level->inactive->front_n = n;

	ka[0] = 1.0f; ka[1] = 1.0f; ka[2] = 1.0f;
	kd[0] = 1.0f; kd[1] = 1.0f; kd[2] = 1.0f;

	//Other Faces
	memcpy(new_level->inactive->other_ka, ka, sizeof(ka));
	memcpy(new_level->inactive->other_kd, kd, sizeof(kd));
	memcpy(new_level->inactive->other_ks, ks, sizeof(ks));
	new_level->active->other_n = n;


	//Define Data for Level 2
	new_level = &(level_data.levels[1]);
	new_level->id = 1;

	//MTL Data for Active Cube
	new_level->active = new mtl_data;

	ka[0] = 0.0f; ka[1] = 0.0f; ka[2] = 1.0f;
	kd[0] = 0.0f; kd[1] = 0.0f; kd[2] = 1.0f;

	//Top Face
	memcpy(new_level->active->top_ka, ka, sizeof(ka));
	memcpy(new_level->active->top_kd, kd, sizeof(kd));
	memcpy(new_level->active->top_ks, ks, sizeof(ks));
	new_level->active->top_n = n;

	ka[0] = 0.0f; ka[1] = 0.4f; ka[2] = 1.0f;
	kd[0] = 0.0f; kd[1] = 0.4f; kd[2] = 1.0f;

	//Left Face
	memcpy(new_level->active->left_ka, ka, sizeof(ka));
	memcpy(new_level->active->left_kd, kd, sizeof(kd));
	memcpy(new_level->active->left_ks, ks, sizeof(ks));
	new_level->active->left_n = n;

	ka[0] = 0.2f; ka[1] = 0.4f; ka[2] = 1.0f;
	kd[0] = 0.2f; kd[1] = 0.4f; kd[2] = 1.0f;

	//Front Face
	memcpy(new_level->active->front_ka, ka, sizeof(ka));
	memcpy(new_level->active->front_kd, kd, sizeof(kd));
	memcpy(new_level->active->front_ks, ks, sizeof(ks));
	new_level->active->front_n = n;

	ka[0] = 1.0f; ka[1] = 1.0f; ka[2] = 1.0f;
	kd[0] = 1.0f; kd[1] = 1.0f; kd[2] = 1.0f;

	//Other Faces
	memcpy(new_level->active->other_ka, ka, sizeof(ka));
	memcpy(new_level->active->other_kd, kd, sizeof(kd));
	memcpy(new_level->active->other_ks, ks, sizeof(ks));
	new_level->active->other_n = n;

	//MTL Data for InActive Cube
	new_level->inactive = new mtl_data;

	ka[0] = 0.7f; ka[1] = 0.8f; ka[2] = 0.5f;
	kd[0] = 0.7f; kd[1] = 0.8f; kd[2] = 0.5f;

	//Top Face
	memcpy(new_level->inactive->top_ka, ka, sizeof(ka));
	memcpy(new_level->inactive->top_kd, kd, sizeof(kd));
	memcpy(new_level->inactive->top_ks, ks, sizeof(ks));
	new_level->inactive->top_n = n;

	ka[0] = 0.0f; ka[1] = 0.4f; ka[2] = 1.0f;
	kd[0] = 0.0f; kd[1] = 0.4f; kd[2] = 1.0f;

	//Left Face
	memcpy(new_level->inactive->left_ka, ka, sizeof(ka));
	memcpy(new_level->inactive->left_kd, kd, sizeof(kd));
	memcpy(new_level->inactive->left_ks, ks, sizeof(ks));
	new_level->inactive->left_n = n;

	ka[0] = 0.2f; ka[1] = 0.4f; ka[2] = 1.0f;
	kd[0] = 0.2f; kd[1] = 0.4f; kd[2] = 1.0f;

	//Front Face
	memcpy(new_level->inactive->front_ka, ka, sizeof(ka));
	memcpy(new_level->inactive->front_kd, kd, sizeof(kd));
	memcpy(new_level->inactive->front_ks, ks, sizeof(ks));
	new_level->inactive->front_n = n;

	ka[0] = 1.0f; ka[1] = 1.0f; ka[2] = 1.0f;
	kd[0] = 1.0f; kd[1] = 1.0f; kd[2] = 1.0f;

	//Other Faces
	memcpy(new_level->inactive->other_ka, ka, sizeof(ka));
	memcpy(new_level->inactive->other_kd, kd, sizeof(kd));
	memcpy(new_level->inactive->other_ks, ks, sizeof(ks));
	new_level->active->other_n = n;

	//assign the current level
	level_data.levels_completed_count = 0;
	level_data.current = &(level_data.levels[level_data.levels_completed_count]);	
}

//initialize the cubes for the scene
void initialize_Scene(){
	scene.shaderID = shader.lighting;

	string err;
	
	vector<shape_t> objectData;
	vector<material_t> materialData;

	if(!LoadObj(objectData, materialData, err, (sceneRootPath+scene.objFilePath).c_str(), sceneRootPath.c_str())){
		cout<<"\nError"<<err;
		exit(1);
	}

	//One Structure to hold the cube data
	scene.object = new cube_obj_struct;

	//The number of cubes in the scene
	scene.cubeCount = 28;

	scene.transformMat = new mat4[scene.cubeCount];
	scene.initialTransformMat = new mat4[scene.cubeCount];
	scene.normalMat = new mat4[scene.cubeCount];

	//Attach cube data to the structure's object element
	scene.object->objectData = objectData;
	scene.object->materialData = materialData;
	scene.object->groupCount = objectData.size();

	mat4 translationMat = mat4();
	mat4 scalingMat = mat4();
	mat4 rotationMatX = mat4();
	mat4 rotationMatY = mat4();
	mat4 transformMat = mat4();

	rotationMatX = rotate(rotationMatX, (float)45, vec3(1,0,0));
	rotationMatY = rotate(rotationMatY, (float)45, vec3(0,1,0));
	scalingMat = scale(scalingMat, vec3(0.05,0.05,0.05));
	translationMat = translate(translationMat, vec3(0, 0.5, 0));

	transformMat = translationMat * scalingMat  * rotationMatX * rotationMatY;

	fstream scene_reader;
	scene_reader.open(sceneRootPath+"scene_transforms.txt", ios::in);
	for(size_t i = 0; i < scene.cubeCount; i++){
		int x_trans, y_trans, z_trans;
		scene_reader>>x_trans>>y_trans>>z_trans;
		scene.transformMat[i] = translate(transformMat, vec3(x_trans, y_trans, z_trans));
		scene.initialTransformMat[i] = translate(transformMat, vec3(x_trans, y_trans, z_trans));
	}
	cout<<endl;
	scene_reader.close();
}

//initialize the player's character
void initialize_Player(){
	player.shaderID = shader.texture;

	player.show = true;

	vector<shape_t> objectData;
	vector<material_t> materialData;

	string err;

	if(!LoadObj(objectData, materialData, err, (playerRootPath+player.objFilePath).c_str(), playerRootPath.c_str())){
		cout<<"\nError"<<err;
		exit(1);
	}

	player.object = new player_obj_struct;

	player.object->objectData = objectData;
	player.object->materialData = materialData;
	player.object->groupCount = objectData.size();

	//Player Animation
	player.dir = 0;
	player.animate = false;
	player.fall = false;
	player.animation_index = 0;
	player.animation_duration = 0;
	player.animation_start = 0;
	player.animation_interval = 20;
	player.fall_animation_interval = 20;
	player.fall_duration = 0;

	//State
	player.nextState = NULL;

	//initialize matrices
	player.rotationMatY = mat4();
	player.rotationMatX = mat4();
	player.translationMat = mat4();
	player.scalingMat = mat4();
	player.transformMat = mat4();
	player.normalMat = mat4();

	//define initial transforms
	player.scalingMat = scale(player.scalingMat, vec3(0.0025, 0.0025, 0.0025));
	player.translationMat = translate(player.translationMat, vec3(0, 0.54, -0.1));
	player.rotationMatY = rotate(player.rotationMatY, -135.f, vec3(0,1,0));

	player.transformMat = player.translationMat * player.scalingMat * player.rotationMatY * player.rotationMatX;
	player.normalMat = transpose(inverse(player.transformMat));

	//load the character texture
	player.object->texture = loadTexture(playerRootPath + "QzardB.png", player.shaderID);
}

//initialize the creatures' data
void initialize_Creatures(){

	//initialize creature0
	creature0.shaderID = shader.lighting;

	//initialize creature 0 data
	creature0.data = new creature0_data_struct;

	//initialize the animation data
	creature0.dir = 0;
	creature0.animate = false;
	creature0.animation_index = 0;
	creature0.animation_duration = 0;
	creature0.animation_start = 0;
	creature0.animation_interval = 20;

	//load the object for creature0
	string err;
	vector<shape_t> objectData;
	vector<material_t> materialData;

	creature0.start = true;
	creature0.motion = false;
	creature0.collided = false;

	if(!LoadObj(objectData, materialData, err, (creatureRootPath+creature0.objFilePath).c_str(), creatureRootPath.c_str())){
		cout<<"\nError"<<err;
		exit(1);
	}

	creature0.object = new creature_obj_struct;
	creature0.object->objectData = objectData;
	creature0.object->materialData = materialData;
	creature0.object->groupCount = objectData.size();

	//initialize and set the transform data
	creature0.data->translationMat = mat4();
	creature0.data->rotationMat = mat4();
	creature0.data->scalingMat = mat4();
	creature0.data->transformMat = mat4();
	creature0.data->normalMat = mat4();

	creature0.data->scalingMat = scale(creature0.data->scalingMat, vec3(0.03, 0.03, 0.03));
	creature0.data->translationMat = translate(creature0.data->translationMat, vec3(0, 0.5, 0));
	creature0.data->transformMat = creature0.data->translationMat * creature0.data->scalingMat * creature0.data->rotationMat;
	creature0.data->normalMat = transpose(inverse(creature0.data->transformMat));

	//set state data
	creature0.data->show = false;
	creature0.data->current_state = NULL;
	creature0.data->duration = 0;
	creature0.data->start_interval = 4000;
	creature0.data->motion_interval = 1000;
	creature0.data->time_start = 0;

	//Initialize Creature1
	creature1.shaderID = new GLuint[2];
	creature1.shaderID[0] = shader.lighting;
	creature1.shaderID[1] = shader.texture;

	creature1.textureFilePath = "snake.png";

	//initialize creature 1 data
	creature1.data = new creature1_data_struct;

	//initialize the animation data
	creature1.dir = 0;
	creature1.animate = false;
	creature1.animation_index = 0;
	creature1.animation_duration = 0;
	creature1.animation_start = 0;
	creature1.animation_interval = 20;

	//load the object for creature1
	objectData.clear();
	materialData.clear();

	creature1.start = true;
	creature1.motion = false;
	creature1.collided = false;
	creature1.transformed = false;

	if(!LoadObj(objectData, materialData, err, (creatureRootPath+creature1.objFilePath[0]).c_str(), creatureRootPath.c_str())){
		cout<<"\nError"<<err;
		exit(1);
	}

	creature1.object = new creature_obj_struct;
	creature1.object->objectData = objectData;
	creature1.object->materialData = materialData;
	creature1.object->groupCount = objectData.size();

	objectData.clear();
	materialData.clear();

	if(!LoadObj(objectData, materialData, err, (creatureRootPath+creature1.objFilePath[1]).c_str(), creatureRootPath.c_str())){
		cout<<"\nError"<<err;
		exit(1);
	}

	creature1.object_texture = new creature_obj_struct_texture;
	creature1.object_texture->objectData = objectData;
	creature1.object_texture->materialData = materialData;
	creature1.object_texture->groupCount = objectData.size();

	//initialize and set the transform data
	creature1.data->translationMat = new mat4[2];
	creature1.data->rotationMat = new mat4[2];
	creature1.data->scalingMat = new mat4[2];
	creature1.data->transformMat = new mat4[2];
	creature1.data->normalMat = new mat4[2];
	

	//creature1 normal
	creature1.data->translationMat[0] = mat4();
	creature1.data->rotationMat[0] = mat4();
	creature1.data->scalingMat[0] = mat4();
	creature1.data->transformMat[0] = mat4();
	creature1.data->normalMat[0] = mat4();

	creature1.data->scalingMat[0] = scale(creature1.data->scalingMat[0], vec3(0.03, 0.03, 0.03));
	creature1.data->translationMat[0] = translate(creature1.data->translationMat[0], vec3(0, 0.54, 0));
	creature1.data->transformMat[0] = creature1.data->translationMat[0] * creature1.data->scalingMat[0] * creature1.data->rotationMat[0];
	creature1.data->normalMat[0] = transpose(inverse(creature1.data->transformMat[0]));

	//creature1 transformed
	creature1.data->translationMat[1] = mat4();
	creature1.data->rotationMat[1] = mat4();
	creature1.data->scalingMat[1] = mat4();
	creature1.data->transformMat[1] = mat4();
	creature1.data->normalMat[1] = mat4();

	creature1.data->scalingMat[1] = scale(creature1.data->scalingMat[1], vec3(0.002, 0.002, 0.002));
	creature1.data->translationMat[1] = translate(creature1.data->translationMat[1], vec3(0, 0.5, -0.1));
	creature1.data->transformMat[1] = creature1.data->translationMat[1] * creature1.data->scalingMat[1] * creature1.data->rotationMat[1];
	creature1.data->normalMat[1] = transpose(inverse(creature1.data->transformMat[1]));

	//set state data
	creature1.data->show = false;
	creature1.data->current_state = NULL;
	creature1.data->duration = 0;
	creature1.data->start_interval = 5000;
	creature1.data->motion_interval = 1000;
	creature1.data->time_start = 0;

	creature1.object_texture->texture = loadTexture(creatureRootPath + creature1.textureFilePath, creature1.shaderID[1]);

	//initialize creature2
	creature2.shaderID = shader.lighting;

	//initialize creature 2 data
	creature2.data = new creature0_data_struct;

	//initialize the animation data
	creature2.dir = 0;
	creature2.animate = false;
	creature2.animation_index = 0;
	creature2.animation_duration = 0;
	creature2.animation_start = 0;
	creature2.animation_interval = 20;

	//load the object for creature0
	objectData.clear();
	materialData.clear();

	creature2.start = true;
	creature2.motion = false;
	creature2.collided = false;

	if(!LoadObj(objectData, materialData, err, (creatureRootPath+creature2.objFilePath).c_str(), creatureRootPath.c_str())){
		cout<<"\nError"<<err;
		exit(1);
	}

	creature2.object = new creature_obj_struct;
	creature2.object->objectData = objectData;
	creature2.object->materialData = materialData;
	creature2.object->groupCount = objectData.size();

	//initialize and set the transform data
	creature2.data->translationMat = mat4();
	creature2.data->rotationMat = mat4();
	creature2.data->scalingMat = mat4();
	creature2.data->transformMat = mat4();
	creature2.data->normalMat = mat4();

	creature2.data->scalingMat = scale(creature2.data->scalingMat, vec3(0.002, 0.002, 0.002));
	creature2.data->translationMat = translate(creature2.data->translationMat, vec3(0, 0.54, -0.1));
	creature2.data->rotationMat = rotate(creature2.data->rotationMat, -45.f, vec3(0,1,0));
	creature2.data->transformMat = creature2.data->translationMat * creature2.data->scalingMat * creature2.data->rotationMat;
	creature2.data->normalMat = transpose(inverse(creature0.data->transformMat));

	//set state data
	creature2.data->show = false;
	creature2.data->current_state = NULL;
	creature2.data->duration = 0;
	creature2.data->start_interval = 6000;
	creature2.data->motion_interval = 1000;
	creature2.data->time_start = 0;
}

//initialize the disks' data
void initialize_Disks(){
	disks.shaderID = shader.texture;

	disks.textureFilePath = "disk.png";

	//initialize the disk object
	vector<shape_t> objectData;
	vector<material_t> materialData;

	string err;

	if(!LoadObj(objectData, materialData, err, (disksRootPath + disks.objFilePath).c_str(), disksRootPath.c_str())){
		cout<<"\nError: "<<err;
		return;
	}

	disks.object = new disk_obj_struct;
	disks.object->objectData = objectData;
	disks.object->materialData = materialData;
	disks.object->groupCount = objectData.size();

	disks.object->texture = loadTexture(disksRootPath + disks.textureFilePath, disks.shaderID);

	//initialize the disk data
	disks.left = new disk_data_struct;
	disks.right = new disk_data_struct;

	//left disk
	disks.left->row_near = 0;
	disks.left->column_near = 4;
	disks.left->jump_movement = 1;
	disks.left->enabled = true;

	//animation
	disks.left->r_animation_duration = 0;
	disks.left->r_animation_start = 0;
	disks.left->r_animation_interval = 50;

	//transforms
	//initialize
	disks.left->translationMat = mat4();
	disks.left->rotationMatX = mat4();
	disks.left->rotationMatY = mat4();
	disks.left->scalingMat = mat4();
	disks.left->transformMat = mat4();
	disks.left->normalMat = mat4();

	//set
	disks.left->scalingMat = scale(disks.left->scalingMat, vec3(0.04,0.04,0.04));
	disks.left->rotationMatX = rotate(disks.left->rotationMatX, 45.f, vec3(1,0,0));
	disks.left->translationMat = translate(disks.left->translationMat, vec3(-0.36,0.12,0));

	disks.left->transformMat = disks.left->translationMat * disks.left->scalingMat * disks.left->rotationMatY * disks.left->rotationMatX;
	disks.left->normalMat = transpose(inverse(disks.left->translationMat));

	//right disk
	disks.right->row_near = 4;
	disks.right->column_near = 0;
	disks.right->jump_movement = 3;
	disks.right->enabled = true;

	//animation
	disks.right->r_animation_duration = 0;
	disks.right->r_animation_start = 0;
	disks.right->r_animation_interval = 50;

	//transforms
	//initialize
	disks.right->translationMat = mat4();
	disks.right->rotationMatX = mat4();
	disks.right->rotationMatY = mat4();
	disks.right->scalingMat = mat4();
	disks.right->transformMat = mat4();
	disks.right->normalMat = mat4();
	
	//set
	disks.right->scalingMat = scale(disks.right->scalingMat, vec3(0.04,0.04,0.04));
	disks.right->rotationMatX = rotate(disks.right->rotationMatX, 45.f, vec3(1,0,0));
	disks.right->translationMat = translate(disks.right->translationMat, vec3(0.36,0.12,0));

	disks.right->transformMat = disks.right->translationMat * disks.right->scalingMat * disks.right->rotationMatY * disks.right->rotationMatX;
	disks.right->normalMat = transpose(inverse(disks.right->normalMat));
}

//initialize the basic game data
void initialize_Game(){

	//Initialize Vars
	game_data.total_blocks = scene.cubeCount;
	game_data.blocks_covered_count = 0;
	game_data.states = new state_struct[game_data.total_blocks];


	//Make the covered status of all blocks false
	for(size_t i = 0; i < game_data.total_blocks; i++){
		game_data.states[i].id = i;
		game_data.states[i].isCovered = false;
		game_data.states[i].up = NULL;
		game_data.states[i].down = NULL;
		game_data.states[i].left = NULL;
		game_data.states[i].right = NULL;
		game_data.states[i].row = 0;
		game_data.states[i].column = 0;
	}

	//Initialize the score as 0
	game_data.score = 0;

	//Initialize the lives
	game_data.total_lives = 3;
	game_data.remaining_lives = game_data.total_lives;

	//Initialize Game State Tree
	initialize_Game_State_Tree();

	//Initialize the current state as state 0
	game_data.current_state = &game_data.states[0];
}	

//Initialize the Game State Tree
void initialize_Game_State_Tree(){
	size_t levelTotal = 0;
	int level = 1;
	int temp_count = 1;
	for(size_t i = 0; i < game_data.total_blocks;){
		levelTotal += level;

		int row = 0, column = level - 1;

		//Nothing required for the last level as no children of it exist
		if(level == 7){
			while(i < levelTotal){
				game_data.states[i].row = row;
				game_data.states[i].column = column;
				row++;
				column--;
				i++;
			}
			break;
		}

		//Assign child to parent and parent to that child
		while(i < levelTotal){
			game_data.states[i].down = &(game_data.states[temp_count]);
			game_data.states[i].down->up = &(game_data.states[i]);
			temp_count++;

			game_data.states[i].right = &(game_data.states[temp_count]);
			game_data.states[i].right->left = &(game_data.states[i]);

			game_data.states[i].row = row;
			game_data.states[i].column = column;

			row++;
			column--;
			i++;

		}

		temp_count++;

		level++;
	}
	
	game_data.current_state = &(game_data.states[0]);
}

//initialize the life rendering data
void initialize_Life(){
	game_data.life = new life_struct;

	game_data.life->shaderID = shader.lives;

	game_data.life->object = new life_obj_struct;
	game_data.life->transformMat = new mat4[game_data.total_lives];
	game_data.life->normalMat = new mat4[game_data.total_lives];

	//compute initial transforms
	mat4 temp_transform = mat4();
	temp_transform = translate(temp_transform, vec3(-0.9,-0.3,0));
	temp_transform = scale(temp_transform, vec3(0.0025,0.0025,0.0025));
	

	game_data.life->object->objectData = player.object->objectData;
	game_data.life->object->materialData = player.object->materialData;
	game_data.life->object->groupCount = player.object->groupCount;
	game_data.life->object->texture = player.object->texture;

	//compute and define transforms for each life
	for(int i = 0; i < game_data.total_lives; i++){
		game_data.life->transformMat[i] = translate(temp_transform,vec3(i*40,0,0));
		game_data.life->normalMat[i] = transpose(inverse(game_data.life->transformMat[i]));
	}
}	

//initialize the score rendering data
void initialize_Score(){
	game_data.score_data = new score_struct;

	game_data.score_data->object = new score_obj_struct;

	game_data.score_data->textureFilePath = "font_render.bmp";

	game_data.score_data->shaderID = shader.text;

	game_data.score_data->font_size = 0.05f;

	game_data.score_data->color = new float[3];
	game_data.score_data->color[0] = 0.0f;
	game_data.score_data->color[1] = 0.6f;
	game_data.score_data->color[2] = 0.5f;

	game_data.score_data->texture = loadTexture(resourceRootPath + game_data.score_data->textureFilePath, game_data.score_data->shaderID);

	updateScoreData();
}

//initialize the fps data
void initialize_FPS(){
	fps.shaderID = shader.text;

	fps.texturFilePath = "font_render.bmp";

	fps.object = new fps_obj_struct;

	fps.font_size = 0.05f;
	fps.frame_count = 0;
	fps.time_start = clock();
	fps.time_end = fps.time_start;
	fps.time_diff = 0;

	fps.color = new float[3];
	fps.color[0] = 0.0f;
	fps.color[1] = 1.0f;
	fps.color[2] = 1.0f;

	fps.texture = loadTexture(resourceRootPath+fps.texturFilePath, fps.shaderID);

	fps.show = false;

	updateFPSData();
}

//initialize the tools
void initialize_Tools(){
	tool.key_press = true;
}

//intialize the structure for game over
void initialize_Game_Over(){
	game_over.state = false;
	game_over.complete = false;
	game_over.show = false;
	game_over.duration = 0;
	game_over.time_start = 0;

	game_over.textureFilePath = "font_render.bmp";
	game_over.font_size = 0.08f;

	game_over.shaderID = shader.text;

	game_over.object = new game_over_obj_struct;
	game_over.object_2 = new game_over_obj_struct;

	game_over.text_to_show = "GAME OVER";
	game_over.text_to_show_2 = "YOU WON!";

	game_over.color = new float[3];
	game_over.color[0] = 1.0f;
	game_over.color[1] = 0.0f;
	game_over.color[2] = 0.0f;

	//compute the object data for game over
	createGameOverObject();

	//load texture
	game_over.texture = loadTexture(resourceRootPath+game_over.textureFilePath, game_over.shaderID);

cout<<"Done!";
}

//initialize the structure for game start
void initialize_Game_Start(){
	game_start.state = true;

	game_start.textureFilePath = "font_render.bmp";
	game_start.font_size_top = 0.1f;
	game_start.font_size_bottom = 0.08f;

	//initialize vectors
	game_start.vertexPosition = new vector<vec2>[2];
	game_start.vertexTexture = new vector<vec2>[2];
	game_start.vertexIndex = new vector<unsigned int>[2];


	game_start.shaderID = shader.text;

	game_start.color = new float*[3];

	//top text
	game_start.color[0] = new float[3];
	game_start.color[0][0] = 1.0f;
	game_start.color[0][1] = 1.0f;
	game_start.color[0][2] = 1.0f;

	//bottom text
	game_start.color[1] = new float[3];
	game_start.color[1][0] = 1.0f;
	game_start.color[1][1] = 1.0f;
	game_start.color[1][2] = 1.0f;

	game_start.object = new game_start_obj_struct[2];

	game_start.text_top = "QBERT - THE GAME";
	game_start.text_bottom = "PRESS SPACE TO START!";
	//compute the object data for game over
	createGameStartObject();

	//load texture
	game_start.texture = loadTexture(resourceRootPath+game_over.textureFilePath, game_start.shaderID);
}

//initialize animation
void initialize_Animation(){

	animation.frame_count = 18;

	initialize_Animation_Player();
}

void initialize_Animation_Player(){
	animation.x_down = new float[18];
	animation.y_down = new float[18];
	animation.z_down = new float[18];

	animation.x_up = new float[18];
	animation.y_up = new float[18];
	animation.z_up = new float[18];

	animation.x_left = new float[18];
	animation.y_left = new float[18];
	animation.z_left = new float[18];

	animation.x_right = new float[18];
	animation.y_right = new float[18];
	animation.z_right = new float[18];


	//Define for Player
	float y = 0.f, y_2 = 0.f;
	float x = 0.f, x_2 = 0.f;

	float eq_const = (0.07f*0.07f)/4 ;

	//Up
	animation.x_up[0] = 0.f;
	animation.y_up[0] = 0.03f;
	animation.z_up[0] = 0.05f;

	animation.x_up[1] = 0.f;
	animation.y_up[1] = 0.06f;
	animation.z_up[1] = 0.05f;

	animation.x_up[2] = 0.f;
	animation.y_up[2] = 0.09f;
	animation.z_up[2] = 0.05f;

	animation.x_up[3] = 0.f;
	animation.y_up[3] = 0.12f;
	animation.z_up[3] = 0.05f;

	//Left
	animation.x_left[0] = 0.f;
	animation.y_left[0] = 0.03f;
	animation.z_left[0] = 0.05f;

	animation.x_left[1] = 0.f;
	animation.y_left[1] = 0.06f;
	animation.z_left[1] = 0.05f;

	animation.x_left[2] = 0.f;
	animation.y_left[2] = 0.09f;
	animation.z_left[2] = 0.05f;

	animation.x_left[3] = 0.f;
	animation.y_left[3] = 0.12f;
	animation.z_left[3] = 0.05f;

	for(int i = 0; i < 14; i ++){

		//Up and Down
		x += 0.005f;
		float temp = x - 0.035f;
		y = sqrt(eq_const - pow(temp,2));
		animation.x_down[i] = -x;
		animation.y_down[i] = y;
		animation.z_down[i] = -0.05f;

		animation.x_up[i+4] = x;
		animation.y_up[i+4] = y + 0.12f;
		animation.z_up[i+4] = 0.05f;

		animation.x_right[i] = x;
		animation.y_right[i] = y;
		animation.z_right[i] = -0.05f;

		animation.x_left[i+4] = -x;
		animation.y_left[i+4] = y + 0.12f;
		animation.z_left[i+4] = 0.05f;
	}

	//Down
	animation.x_down[14] = -0.07f;
	animation.y_down[14] = -0.03f;
	animation.z_down[14] = -0.05f;

	animation.x_down[15] = -0.07f;
	animation.y_down[15] = -0.06f;
	animation.z_down[15] = -0.05f;

	animation.x_down[16] = -0.07f;
	animation.y_down[16] = -0.09f;
	animation.z_down[16] = -0.05f;

	animation.x_down[17] = -0.07f;
	animation.y_down[17] = -0.12f;
	animation.z_down[17] = -0.05f;

	//Right
	animation.x_right[14] = 0.07f;
	animation.y_right[14] = -0.03f;
	animation.z_right[14] = -0.05f;
	
	animation.x_right[15] = 0.07f;
	animation.y_right[15] = -0.06f;
	animation.z_right[15] = -0.05f;
		
	animation.x_right[16] = 0.07f;
	animation.y_right[16] = -0.09f;
	animation.z_right[16] = -0.05f;

	animation.x_right[17] = 0.07f;
	animation.y_right[17] = -0.12f;
	animation.z_right[17] = -0.05f;
}

void initialize_Sounds(){
	//init alut
	alutInit(0, NULL);

	//enable errors
	alGetError();

	//initialize file names
	sounds.sound_game_start = "game_start_music.wav";
	sounds.sound_player_hop = "player_hop_music.wav";
	sounds.sound_player_fall = "player_fall_music.wav";
	sounds.sound_player_collision = "player_collision_music.wav";
	sounds.sound_coily_hop = "coily_hop_music.wav";
	sounds.sound_game_over = "game_over_music.wav";
	sounds.sound_bonk = "bonk_music.wav";

	//initialize buffers
	sounds.game_start_buffer = alutCreateBufferFromFile((soundsRootPath + sounds.sound_game_start).c_str());
	sounds.player_hop_buffer = alutCreateBufferFromFile((soundsRootPath + sounds.sound_player_hop).c_str());
	sounds.player_fall_buffer = alutCreateBufferFromFile((soundsRootPath + sounds.sound_player_fall).c_str());
	sounds.player_collision_buffer = alutCreateBufferFromFile((soundsRootPath + sounds.sound_player_collision).c_str());
	sounds.coily_hop_buffer = alutCreateBufferFromFile((soundsRootPath + sounds.sound_coily_hop).c_str());
	sounds.game_over_buffer = alutCreateBufferFromFile((soundsRootPath + sounds.sound_game_over).c_str());
	sounds.bonk_buffer = alutCreateBufferFromFile((soundsRootPath + sounds.sound_bonk).c_str());

	//create sound sources
	alGenSources(1, &sounds.game_start_source);
	alSourcei(sounds.game_start_source, AL_BUFFER, sounds.game_start_buffer);

	alGenSources(1, &sounds.player_hop_source);
	alSourcei(sounds.player_hop_source, AL_BUFFER, sounds.player_hop_buffer);

	alGenSources(1, &sounds.player_fall_source);
	alSourcei(sounds.player_fall_source, AL_BUFFER, sounds.player_fall_buffer);

	alGenSources(1, &sounds.player_collision_source);
	alSourcei(sounds.player_collision_source, AL_BUFFER, sounds.player_collision_buffer);

	alGenSources(1, &sounds.coily_hop_source);
	alSourcei(sounds.coily_hop_source, AL_BUFFER, sounds.coily_hop_buffer);

	alGenSources(1, &sounds.game_over_source);
	alSourcei(sounds.game_over_source, AL_BUFFER, sounds.game_over_buffer);

	alGenSources(1, &sounds.bonk_source);
	alSourcei(sounds.bonk_source, AL_BUFFER, sounds.bonk_buffer);
}

//Camera Control Functions
void initCameras(){
	//Used to initialize the first person View
	camera_fp.inUse = false;
	camera_fp.position = vec3(0,0,2);
	camera_fp.lookat = vec3(0,0,1);
	camera_fp.lookup = vec3(0,1,0);
	camera_fp.lookAtMat = lookAt(camera_fp.position, camera_fp.lookat, camera_fp.lookup);
	camera_fp.perspectiveMat = perspective(45.f, float(window_main.dims[0]/window_main.dims[1]), 1.f, 10.f);
	
	//Used to initialize the third person View
	camera_tp.inUse = false;
	camera_tp.position = vec3(0,0,2);
	camera_tp.lookat = vec3(0,0,1);
	camera_tp.lookup = vec3(0,1,0);
	camera_tp.lookAtMat = mat4();
	camera_tp.perspectiveMat = mat4();
}

void setView_ThirdPerson(){
	cout<<"\nSwitching to the Third Person Camera"<<endl;
	//Set inUse Flag
	camera_tp.inUse = true;
	//Set Camera Uniforms for Lighting Shader
	glUseProgram(scene.shaderID);
	glUniformMatrix4fv(glGetUniformLocation(shader.lighting, "perspectiveMat"), 1, GL_FALSE, &camera_tp.perspectiveMat[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader.lighting, "lookAtMat"), 1, GL_FALSE, &camera_tp.lookAtMat[0][0]);

	//Set Camera Uniforms for Texture Shader
	glUseProgram(player.shaderID);
	glUniformMatrix4fv(glGetUniformLocation(shader.texture, "perspectiveMat"), 1, GL_FALSE, &camera_tp.perspectiveMat[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader.texture, "lookAtMat"), 1, GL_FALSE, &camera_tp.lookAtMat[0][0]);
}

void setView_FirstPerson(){
	cout<<"\nSwitching to the First Person Camera"<<endl;
	//Set inUse Flag
	camera_fp.inUse = true;
	//Set Camera Uniforms for Lighting Shader
	glUseProgram(scene.shaderID);
	glUniformMatrix4fv(glGetUniformLocation(shader.lighting, "perspectiveMat"), 1, GL_FALSE, &camera_fp.perspectiveMat[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader.lighting, "lookAtMat"), 1, GL_FALSE, &camera_fp.lookAtMat[0][0]);

	//Set Camera Uniforms for Texture Shader
	glUseProgram(player.shaderID);
	glUniformMatrix4fv(glGetUniformLocation(shader.texture, "perspectiveMat"), 1, GL_FALSE, &camera_fp.perspectiveMat[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader.texture, "lookAtMat"), 1, GL_FALSE, &camera_fp.lookAtMat[0][0]);
}

void toggleCamera(){
	if(camera_tp.inUse){
		camera_tp.inUse = false;
		setView_FirstPerson();
	}else{
		camera_fp.inUse = false;
		setView_ThirdPerson();
	}
}

//Set the Buffers with Data
void setBuffers(){
	setBuffers_Scene();
	setBuffers_Player();
	setBuffers_Creatures();
	setBuffers_Disks();
	setBuffers_Life();
	setBuffers_Game_Over();
	setBuffers_Game_Start();
}

void setBuffers_Scene(){
	glUseProgram(scene.shaderID);

	glGenVertexArrays(1, &scene.object->vertexArray);
	glBindVertexArray(scene.object->vertexArray);

	scene.object->indexBuffer = new GLuint[scene.object->groupCount];
	scene.object->vertexBuffer = new GLuint[scene.object->groupCount];
	scene.object->normalBuffer = new GLuint[scene.object->groupCount];

	for(size_t i = 0; i < scene.object->groupCount; i++){
		glGenBuffers(1, &scene.object->vertexBuffer[i]);
		glBindBuffer(GL_ARRAY_BUFFER, scene.object->vertexBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, scene.object->objectData[i].mesh.positions.size()*sizeof(float) , &scene.object->objectData[i].mesh.positions[0], GL_STATIC_DRAW);

		glGenBuffers(1, &scene.object->normalBuffer[i]);
		glBindBuffer(GL_ARRAY_BUFFER, scene.object->normalBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, scene.object->objectData[i].mesh.normals.size()*sizeof(float) , &scene.object->objectData[i].mesh.normals[0], GL_STATIC_DRAW);

		glGenBuffers(1, &scene.object->indexBuffer[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, scene.object->indexBuffer[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, scene.object->objectData[i].mesh.indices.size()*sizeof(unsigned int), &scene.object->objectData[i].mesh.indices[0], GL_STATIC_DRAW);
	}
}

void setBuffers_Player(){
	glUseProgram(player.shaderID);

	glGenVertexArrays(1, &player.object->vertexArray);
	glBindVertexArray(player.object->vertexArray);

	player.object->indexBuffer = new GLuint[player.object->groupCount];
	player.object->vertexBuffer = new GLuint[player.object->groupCount];
	player.object->normalBuffer = new GLuint[player.object->groupCount];
	player.object->textureBuffer = new GLuint[player.object->groupCount];

	for(size_t i = 0; i < player.object->groupCount; i++){
		glGenBuffers(1, &player.object->vertexBuffer[i]);
		glBindBuffer(GL_ARRAY_BUFFER, player.object->vertexBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, player.object->objectData[i].mesh.positions.size()*sizeof(float), &player.object->objectData[i].mesh.positions[0], GL_STATIC_DRAW);

		glGenBuffers(1, &player.object->normalBuffer[i]);
		glBindBuffer(GL_ARRAY_BUFFER, player.object->normalBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, player.object->objectData[i].mesh.normals.size()*sizeof(float), &player.object->objectData[i].mesh.normals[0], GL_STATIC_DRAW);

		glGenBuffers(1, &player.object->textureBuffer[i]);
		glBindBuffer(GL_ARRAY_BUFFER, player.object->textureBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, player.object->objectData[i].mesh.texcoords.size()*sizeof(float), &player.object->objectData[i].mesh.texcoords[0], GL_STATIC_DRAW);

		glGenBuffers(1, &player.object->indexBuffer[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, player.object->indexBuffer[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, player.object->objectData[i].mesh.indices.size()*sizeof(unsigned int), &player.object->objectData[i].mesh.indices[0], GL_STATIC_DRAW);
	}
}

void setBuffers_Creatures(){

	//creature0
	glUseProgram(creature0.shaderID);
		
	creature_obj_struct* current_creature = creature0.object;
	glGenVertexArrays(1, &current_creature->vertexArray);
	glBindVertexArray(current_creature->vertexArray);

	current_creature->indexBuffer = new GLuint[current_creature->groupCount];
	current_creature->vertexBuffer = new GLuint[current_creature->groupCount];
	current_creature->normalBuffer = new GLuint[current_creature->groupCount];

	for(size_t i = 0; i < current_creature->groupCount; i++){
		glGenBuffers(1, &current_creature->vertexBuffer[i]);
		glBindBuffer(GL_ARRAY_BUFFER, current_creature->vertexBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, current_creature->objectData[i].mesh.positions.size()*sizeof(float), &current_creature->objectData[i].mesh.positions[0], GL_STATIC_DRAW);

		glGenBuffers(1, &current_creature->normalBuffer[i]);
		glBindBuffer(GL_ARRAY_BUFFER, current_creature->normalBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, current_creature->objectData[i].mesh.normals.size()*sizeof(float), &current_creature->objectData[i].mesh.normals[0], GL_STATIC_DRAW);

		glGenBuffers(1, &current_creature->indexBuffer[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, current_creature->indexBuffer[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, current_creature->objectData[i].mesh.indices.size()*sizeof(unsigned int), &current_creature->objectData[i].mesh.indices[0], GL_STATIC_DRAW);
	}

	//creature1
	//normal
	glUseProgram(creature1.shaderID[0]);

	current_creature = creature1.object;
	
	glGenVertexArrays(1, &current_creature->vertexArray);
	glBindVertexArray(current_creature->vertexArray);

	current_creature->indexBuffer = new GLuint[current_creature->groupCount];
	current_creature->vertexBuffer = new GLuint[current_creature->groupCount];
	current_creature->normalBuffer = new GLuint[current_creature->groupCount];

	for(size_t i = 0; i < current_creature->groupCount; i++){
		glGenBuffers(1, &current_creature->vertexBuffer[i]);
		glBindBuffer(GL_ARRAY_BUFFER, current_creature->vertexBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, current_creature->objectData[i].mesh.positions.size()*sizeof(float), &current_creature->objectData[i].mesh.positions[0], GL_STATIC_DRAW);
		
		glGenBuffers(1, &current_creature->normalBuffer[i]);
		glBindBuffer(GL_ARRAY_BUFFER, current_creature->normalBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, current_creature->objectData[i].mesh.normals.size()*sizeof(float), &current_creature->objectData[i].mesh.normals[0], GL_STATIC_DRAW);

		glGenBuffers(1, &current_creature->indexBuffer[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, current_creature->indexBuffer[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, current_creature->objectData[i].mesh.indices.size()*sizeof(unsigned int), &current_creature->objectData[i].mesh.indices[0], GL_STATIC_DRAW);
	}

	//transformed
	glUseProgram(creature1.shaderID[1]);

	creature_obj_struct_texture* current_creature_1 = creature1.object_texture;
	
	glGenVertexArrays(1, &current_creature_1->vertexArray);
	glBindVertexArray(current_creature_1->vertexArray);

	current_creature_1->indexBuffer = new GLuint[current_creature_1->groupCount];
	current_creature_1->vertexBuffer = new GLuint[current_creature_1->groupCount];
	current_creature_1->normalBuffer = new GLuint[current_creature_1->groupCount];
	current_creature_1->textureBuffer = new GLuint[current_creature_1->groupCount];

	for(size_t i = 0; i < current_creature_1->groupCount; i++){
		glGenBuffers(1, &current_creature_1->vertexBuffer[i]);
		glBindBuffer(GL_ARRAY_BUFFER, current_creature_1->vertexBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, current_creature_1->objectData[i].mesh.positions.size()*sizeof(float), &current_creature_1->objectData[i].mesh.positions[0], GL_STATIC_DRAW);
		
		glGenBuffers(1, &current_creature_1->normalBuffer[i]);
		glBindBuffer(GL_ARRAY_BUFFER, current_creature_1->normalBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, current_creature_1->objectData[i].mesh.normals.size()*sizeof(float), &current_creature_1->objectData[i].mesh.normals[0], GL_STATIC_DRAW);

		glGenBuffers(1, &current_creature_1->textureBuffer[i]);
		glBindBuffer(GL_ARRAY_BUFFER, current_creature_1->textureBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, current_creature_1->objectData[i].mesh.texcoords.size()*sizeof(float), &current_creature_1->objectData[i].mesh.texcoords[0], GL_STATIC_DRAW);

		glGenBuffers(1, &current_creature_1->indexBuffer[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, current_creature_1->indexBuffer[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, current_creature_1->objectData[i].mesh.indices.size()*sizeof(unsigned int), &current_creature_1->objectData[i].mesh.indices[0], GL_STATIC_DRAW);
	}

	//creature2
	glUseProgram(creature2.shaderID);
		
	current_creature = creature2.object;
	glGenVertexArrays(1, &current_creature->vertexArray);
	glBindVertexArray(current_creature->vertexArray);

	current_creature->indexBuffer = new GLuint[current_creature->groupCount];
	current_creature->vertexBuffer = new GLuint[current_creature->groupCount];
	current_creature->normalBuffer = new GLuint[current_creature->groupCount];

	for(size_t i = 0; i < current_creature->groupCount; i++){
		glGenBuffers(1, &current_creature->vertexBuffer[i]);
		glBindBuffer(GL_ARRAY_BUFFER, current_creature->vertexBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, current_creature->objectData[i].mesh.positions.size()*sizeof(float), &current_creature->objectData[i].mesh.positions[0], GL_STATIC_DRAW);

		glGenBuffers(1, &current_creature->normalBuffer[i]);
		glBindBuffer(GL_ARRAY_BUFFER, current_creature->normalBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, current_creature->objectData[i].mesh.normals.size()*sizeof(float), &current_creature->objectData[i].mesh.normals[0], GL_STATIC_DRAW);

		glGenBuffers(1, &current_creature->indexBuffer[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, current_creature->indexBuffer[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, current_creature->objectData[i].mesh.indices.size()*sizeof(unsigned int), &current_creature->objectData[i].mesh.indices[0], GL_STATIC_DRAW);
	}
}

void setBuffers_Disks(){
	glUseProgram(disks.shaderID);

	glGenVertexArrays(1, &disks.object->vertexArray);
	glBindVertexArray(disks.object->vertexArray);

	disks.object->indexBuffer = new GLuint[disks.object->groupCount];
	disks.object->vertexBuffer = new GLuint[disks.object->groupCount];
	disks.object->normalBuffer = new GLuint[disks.object->groupCount];
	disks.object->textureBuffer = new GLuint[disks.object->groupCount];

	for(size_t i = 0; i < disks.object->groupCount; i++){
		glGenBuffers(1, &disks.object->vertexBuffer[i]);
		glBindBuffer(GL_ARRAY_BUFFER, disks.object->vertexBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, disks.object->objectData[i].mesh.positions.size()*sizeof(float), &disks.object->objectData[i].mesh.positions[0], GL_STATIC_DRAW);

		glGenBuffers(1, &disks.object->normalBuffer[i]);
		glBindBuffer(GL_ARRAY_BUFFER, disks.object->normalBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, disks.object->objectData[i].mesh.normals.size()*sizeof(float), &disks.object->objectData[i].mesh.normals[0], GL_STATIC_DRAW);

		glGenBuffers(1, &disks.object->textureBuffer[i]);
		glBindBuffer(GL_ARRAY_BUFFER, disks.object->textureBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, disks.object->objectData[i].mesh.texcoords.size()*sizeof(float), &disks.object->objectData[i].mesh.texcoords[0], GL_STATIC_DRAW);

		glGenBuffers(1, &disks.object->indexBuffer[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, disks.object->indexBuffer[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, disks.object->objectData[i].mesh.indices.size()*sizeof(unsigned int), &disks.object->objectData[i].mesh.indices[0], GL_STATIC_DRAW);
	}
}

void setBuffers_Life(){
	glUseProgram(game_data.life->shaderID);

	glGenVertexArrays(1, &game_data.life->object->vertexArray);
	glBindVertexArray(game_data.life->object->vertexArray);

	game_data.life->object->indexBuffer = new GLuint[game_data.life->object->groupCount];
	game_data.life->object->vertexBuffer = new GLuint[game_data.life->object->groupCount];
	game_data.life->object->normalBuffer = new GLuint[game_data.life->object->groupCount];
	game_data.life->object->textureBuffer = new GLuint[game_data.life->object->groupCount];

	for(size_t i = 0; i < game_data.life->object->groupCount; i++){
		glGenBuffers(1, &game_data.life->object->vertexBuffer[i]);
		glBindBuffer(GL_ARRAY_BUFFER, game_data.life->object->vertexBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, game_data.life->object->objectData[i].mesh.positions.size()*sizeof(float), &game_data.life->object->objectData[i].mesh.positions[0], GL_STATIC_DRAW);

		glGenBuffers(1, &game_data.life->object->normalBuffer[i]);
		glBindBuffer(GL_ARRAY_BUFFER, game_data.life->object->normalBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, game_data.life->object->objectData[i].mesh.normals.size()*sizeof(float), &game_data.life->object->objectData[i].mesh.normals[0], GL_STATIC_DRAW);

		glGenBuffers(1, &game_data.life->object->textureBuffer[i]);
		glBindBuffer(GL_ARRAY_BUFFER, game_data.life->object->textureBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, game_data.life->object->objectData[i].mesh.texcoords.size()*sizeof(float), &game_data.life->object->objectData[i].mesh.texcoords[0], GL_STATIC_DRAW);

		glGenBuffers(1, &game_data.life->object->indexBuffer[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, game_data.life->object->indexBuffer[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, game_data.life->object->objectData[i].mesh.indices.size()*sizeof(unsigned int), &game_data.life->object->objectData[i].mesh.indices[0], GL_STATIC_DRAW);
	}
}

void setBuffers_Score(){
	glUseProgram(game_data.score_data->shaderID);

	glGenVertexArrays(1, &game_data.score_data->object->vertexArray);
	glBindVertexArray(game_data.score_data->object->vertexArray);

	game_data.score_data->object->indexBuffer = new GLuint;
	game_data.score_data->object->vertexBuffer = new GLuint;
	game_data.score_data->object->textureBuffer = new GLuint;

	glGenBuffers(1, game_data.score_data->object->vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, *game_data.score_data->object->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, game_data.score_data->vertexPosition.size()*sizeof(vec2), &game_data.score_data->vertexPosition[0], GL_STATIC_DRAW);

	glGenBuffers(1, game_data.score_data->object->textureBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, *game_data.score_data->object->textureBuffer);
	glBufferData(GL_ARRAY_BUFFER, game_data.score_data->vertexTexture.size()*sizeof(vec2), &game_data.score_data->vertexTexture[0], GL_STATIC_DRAW);

	glGenBuffers(1, game_data.score_data->object->indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *game_data.score_data->object->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, game_data.score_data->vertexIndex.size()*sizeof(unsigned int), &game_data.score_data->vertexIndex[0], GL_STATIC_DRAW);
}

void setBuffers_FPS(){
	glUseProgram(fps.shaderID);

	glGenVertexArrays(1, &fps.object->vertexArray);
	glBindVertexArray(fps.object->vertexArray);

	fps.object->indexBuffer = new GLuint;
	fps.object->vertexBuffer = new GLuint;
	fps.object->textureBuffer = new GLuint;

	glGenBuffers(1, fps.object->vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, *fps.object->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER,fps.vertexPosition.size()*sizeof(vec2), &fps.vertexPosition[0], GL_STATIC_DRAW);

	glGenBuffers(1, fps.object->textureBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, *fps.object->textureBuffer);
	glBufferData(GL_ARRAY_BUFFER, fps.vertexTexture.size()*sizeof(vec2), &fps.vertexTexture[0], GL_STATIC_DRAW);

	glGenBuffers(1, fps.object->indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *fps.object->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, fps.vertexIndex.size()*sizeof(unsigned int), &fps.vertexIndex[0], GL_STATIC_DRAW);
}

void setBuffers_Game_Over(){
	glUseProgram(game_over.shaderID);


	//game over
	glGenVertexArrays(1, &game_over.object->vertexArray);
	glBindVertexArray(game_over.object->vertexArray);

	game_over.object->indexBuffer = new GLuint;
	game_over.object->vertexBuffer = new GLuint;
	game_over.object->textureBuffer = new GLuint;

	glGenBuffers(1, game_over.object->vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, *game_over.object->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER,game_over.vertexPosition.size()*sizeof(vec2), &game_over.vertexPosition[0], GL_STATIC_DRAW);

	glGenBuffers(1, game_over.object->textureBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, *game_over.object->textureBuffer);
	glBufferData(GL_ARRAY_BUFFER, game_over.vertexTexture.size()*sizeof(vec2), &game_over.vertexTexture[0], GL_STATIC_DRAW);

	glGenBuffers(1, game_over.object->indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *game_over.object->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, game_over.vertexIndex.size()*sizeof(unsigned int), &game_over.vertexIndex[0], GL_STATIC_DRAW);


	//game complete
	glGenVertexArrays(1, &game_over.object_2->vertexArray);
	glBindVertexArray(game_over.object_2->vertexArray);

	game_over.object_2->indexBuffer = new GLuint;
	game_over.object_2->vertexBuffer = new GLuint;
	game_over.object_2->textureBuffer = new GLuint;

	glGenBuffers(1, game_over.object_2->vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, *game_over.object_2->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER,game_over.vertexPosition_2.size()*sizeof(vec2), &game_over.vertexPosition_2[0], GL_STATIC_DRAW);

	glGenBuffers(1, game_over.object_2->textureBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, *game_over.object_2->textureBuffer);
	glBufferData(GL_ARRAY_BUFFER, game_over.vertexTexture_2.size()*sizeof(vec2), &game_over.vertexTexture_2[0], GL_STATIC_DRAW);

	glGenBuffers(1, game_over.object_2->indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *game_over.object_2->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, game_over.vertexIndex_2.size()*sizeof(unsigned int), &game_over.vertexIndex_2[0], GL_STATIC_DRAW);
}

void setBuffers_Game_Start(){
	glUseProgram(game_start.shaderID);

	for(size_t i = 0; i < 2; i++){
		glGenVertexArrays(1, &game_start.object[i].vertexArray);
		glBindVertexArray(game_start.object[i].vertexArray);

		game_start.object[i].indexBuffer = new GLuint;
		game_start.object[i].vertexBuffer = new GLuint;
		game_start.object[i].textureBuffer = new GLuint;

		glGenBuffers(1, game_start.object[i].vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, *game_start.object[i].vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, game_start.vertexPosition[i].size()*sizeof(vec2), &game_start.vertexPosition[i][0], GL_STATIC_DRAW);

		glGenBuffers(1, game_start.object[i].textureBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, *game_start.object[i].textureBuffer);
		glBufferData(GL_ARRAY_BUFFER, game_start.vertexTexture[i].size()*sizeof(vec2), &game_start.vertexTexture[i][0], GL_STATIC_DRAW);

		glGenBuffers(1, game_start.object[i].indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *game_start.object[i].indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, game_start.vertexIndex[i].size()*sizeof(unsigned int), &game_start.vertexIndex[i][0], GL_STATIC_DRAW);
	}
}

//Functions to Handle Key Press
//Handle KeyPress
void handleKeyPress(GLFWwindow * window, int keyCode, int scanCode, int action, int mod){
	if(!tool.key_press)
			return;
	if(game_start.state){
		if((keyCode == GLFW_KEY_SPACE) && (action == GLFW_PRESS)){
			gameStart();
		}
	}
	else{
		if((keyCode == GLFW_KEY_T) && (action == GLFW_PRESS)){
			toggleCamera();
		}else if((keyCode == GLFW_KEY_UP) && (action == GLFW_PRESS)){
			movePlayer_Up();
		}else if((keyCode == GLFW_KEY_DOWN) && (action == GLFW_PRESS)){
			movePlayer_Down();
		}else if((keyCode == GLFW_KEY_LEFT) && (action == GLFW_PRESS)){
			movePlayer_Left();
		}else if((keyCode == GLFW_KEY_RIGHT) && (action == GLFW_PRESS)){
			movePlayer_Right();
		}
	}

	if((keyCode == GLFW_KEY_F) && (action == GLFW_PRESS)){
		toggleFPS();
	}
}

//Functions for Player Movement
//player movement up
void movePlayer_Up(){
	tool.key_press = false;
	transformPlayer_Up();
	state_struct* nextState = getState(game_data.current_state, 3);
	player.nextState = nextState;
}

//player movement down
void movePlayer_Down(){
	tool.key_press = false;
	transformPlayer_Down();
	state_struct* nextState = getState(game_data.current_state, 4);
	player.nextState = nextState;
}

//player movement left
void movePlayer_Left(){
	tool.key_press = false;
	transformPlayer_Left();
	state_struct* nextState = getState(game_data.current_state, 1);
	player.nextState = nextState;
}

//player movement right
void movePlayer_Right(){
	tool.key_press = false;
	transformPlayer_Right();
	state_struct* nextState = getState(game_data.current_state, 2);
	player.nextState = nextState;
}

//player position reset
void resetPlayerPosition(){
	player.transformMat = player.translationMat * player.scalingMat * player.rotationMatY * player.rotationMatX;
	player.normalMat = transpose(inverse(player.transformMat));
}

//Functions for Player Transforms for movement
void transformPlayer_Up(){
	player.rotationMatY = rotate(mat4(), 135.f, vec3(0,1,0));
	player.dir = 3;
	player.animate = true;
	player.animation_index = 0;
}

void transformPlayer_Down(){
	player.rotationMatY = rotate(mat4(), -45.f, vec3(0,1,0));
	player.dir = 4;
	player.animate = true;
	player.animation_index = 0;
}

void transformPlayer_Left(){
	player.rotationMatY = rotate(mat4(), -135.f, vec3(0,1,0));
	player.dir = 1;
	player.animate = true;
	player.animation_index = 0;
}

void transformPlayer_Right(){
	player.rotationMatY = rotate(mat4(), 45.f, vec3(0,1,0));
	player.dir = 2;
	player.animate = true;
	player.animation_index = 0;
}

//Functions for Player Maintainence
void playerLost(int movement){
	if(game_data.remaining_lives > 1){
		playerLoseLife(movement);
		tool.key_press = true;
	}
	else{
		gameOver();
	}
}

void playerLoseLife(int movement){
	game_data.remaining_lives--;
	
	resetPlayerData();

	resetCreatureData(false);
}

//Functions for Game State Maintainence
state_struct* getState(state_struct* current, int movement){
	switch(movement){
	case 1: return current->left;
		break;
	case 2: return current->right;
		break;
	case 3: return current->up;
		break;
	case 4: return current->down;
		break;
	}
}

void moveToState(state_struct* nextState){
	cout<<"\n Moved To:: ROW::"<<nextState->row<<" Column::"<<nextState->column;
	if(!nextState->isCovered){
		nextState->isCovered = true;
		game_data.blocks_covered_count++;
		game_data.score += 10;
		updateScoreData();
	}
	game_data.current_state = nextState;

	if(game_data.blocks_covered_count == game_data.total_blocks){
		creature0.data->show = false;
		creature1.data->show = false;
		creature2.data->show = false;
		level_data.transition = true;
		level_data.transition_start = 0;
		level_data.transition_duration = 0;
	}else{
		tool.key_press = true;
	}
}

//Reset Game Data
void resetGameData(){

	//Reset Player Position
	resetLevelData();

	//Reset Game Score
	game_data.score = 0;
	updateScoreData();

	//Reset Game Over
	game_over.duration = 0;
	game_over.show = false;
	game_over.state = false;
	game_over.time_start = 0;
	game_over.complete = false;

	//Reset Game Start
	game_start.state = true;

	//Reset Game Lives
	game_data.remaining_lives = game_data.total_lives;

	//Reset Level Data
	level_data.levels_completed_count = 0;
	level_data.current = &(level_data.levels[0]);
}

//Reset Level Data
void resetLevelData(){

	//Reset Scene Data
	for(size_t i = 0; i < scene.cubeCount; i++){
		scene.transformMat[i] = scene.initialTransformMat[i];
	}

	//Reset Tools
	tool.key_press = true;

	//Reset Creatures
	resetCreatureData(true);


	//Reset Disk Data
	disks.left->enabled = true;
	disks.right->enabled = true;

	//Reset Game State Data
	game_data.blocks_covered_count = 0;
	for(size_t i = 0; i < game_data.total_blocks; i++){
		game_data.states[i].isCovered = false;
	}

	//Reset Player
	resetPlayerData();
}

//Reset Creature Data
void resetCreatureData(bool hard_reset){
	//reset creature0
	creature0.animate = false;
	creature0.start = true;
	creature0.motion = false;
	creature0.collided = false;

	creature0.data->time_start = 0;
	creature0.data->duration = 0;
	if(hard_reset)
		creature0.data->start_interval = 4000;
	else
		creature0.data->start_interval = 2000;
	creature0.data->show = false;

	creature0.data->current_state = NULL;

	//reset creature1
	creature1.animate = false;
	creature1.start = true;
	creature1.motion = false;
	creature1.collided = false;
	creature1.transformed = false;

	creature1.data->time_start = 0;
	creature1.data->duration = 0;
	if(hard_reset)
		creature1.data->start_interval = 5000;
	else
		creature1.data->start_interval = 3000;
	creature1.data->show = false;

	creature1.data->current_state = NULL;

	//reset creature0
	creature2.animate = false;
	creature2.start = true;
	creature2.motion = false;
	creature2.collided = false;

	creature2.data->time_start = 0;
	creature2.data->duration = 0;
	if(hard_reset)
		creature2.data->start_interval = 6000;
	else
		creature2.data->start_interval = 4000;
	creature2.data->show = false;

	creature2.data->current_state = NULL;
}

void resetPlayerData(){
	
	//Reset Player Position
	player.show = true;
	player.translationMat = mat4();
	player.translationMat = translate(player.translationMat, vec3(0, 0.54, -0.1));

	player.rotationMatY = rotate(mat4(), -45.f, vec3(0,1,0));
	player.transformMat = player.translationMat * player.scalingMat * player.rotationMatY * player.rotationMatX;
	player.normalMat = transpose(inverse(player.transformMat));

	game_data.current_state = &(game_data.states[0]);
}

//Functions for Level Maintainence
void moveToNextLevel(){
	level_data.levels_completed_count++;
	if(level_data.levels_completed_count == level_data.level_count){
		game_over.complete = true;
		gameOver();
	}else{
		level_data.current = &(level_data.levels[level_data.levels_completed_count]);
		resetLevelData();
	}
	tool.key_press = true;
}

//Function for Loading Textures
GLuint loadTexture(string path, GLuint shaderID){

	glUseProgram(shaderID);

	int width, height, channels = 0;;
	GLuint texture;

	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	unsigned char *textureData = SOIL_load_image(path.c_str(), &width, &height, &channels, SOIL_LOAD_RGB);

	if(textureData == NULL)
		return 0;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	return texture;
}

//Function for Managing Score Data
//update the score data for rendering
void updateScoreData(){
	manageScoreData();
	setBuffers_Score();
}

//update the object data for rendering the score
void manageScoreData(){

	//initialize vector
	vector<unsigned int> indices;
	vector<vec2> position;
	vector<vec2> tex_coord;

	//define text to render
	string score_text = score_head + to_string(game_data.score);

	float x = -0.95f, y = 0.f;

	//create the data and push to buffers
	for(int i = 0; i < (int)score_text.size(); i++){

		vec2 top_left = vec2(x + i*game_data.score_data->font_size, y + game_data.score_data->font_size);
		vec2 top_right = vec2(x + i*game_data.score_data->font_size + game_data.score_data->font_size, y + game_data.score_data->font_size);
		vec2 bottom_left = vec2(x + i*game_data.score_data->font_size, y);
		vec2 bottom_right = vec2(x + i*game_data.score_data->font_size + game_data.score_data->font_size, y);

		position.push_back(top_left);
		position.push_back(top_right);
		position.push_back(bottom_left);
		position.push_back(bottom_right);

		indices.push_back(4*i);
		indices.push_back(4*i+2);
		indices.push_back(4*i+1);

		indices.push_back(4*i+3);
		indices.push_back(4*i+1);
		indices.push_back(4*i+2);

		char current_char = score_text[i] - 32;
		float tex_x = (current_char%8)/8.0f;
		float tex_y = (current_char/8)/8.0f;
		
		vec2 tex_top_left = vec2(tex_x, 1.f - tex_y);
		vec2 tex_top_right = vec2(tex_x + (1.f/8.f), 1.f - tex_y);
		vec2 tex_bottom_left = vec2(tex_x, 1.f - (tex_y + (1.f/8.f)));
		vec2 tex_bottom_right = vec2(tex_x + (1.f/8.f), 1.f - (tex_y + (1.f/8.f)));

		tex_coord.push_back(tex_top_left);
		tex_coord.push_back(tex_top_right);
		tex_coord.push_back(tex_bottom_left);
		tex_coord.push_back(tex_bottom_right);
	}
	game_data.score_data->vertexPosition = position;
	game_data.score_data->vertexTexture = tex_coord;
	game_data.score_data->vertexIndex = indices;
}

//Function for FPS Maintainence
void calculateFPS(){
	fps.frame_count++;
	fps.time_end = clock();
	fps.time_diff += (fps.time_end - fps.time_start)*1000/CLOCKS_PER_SEC;
	if(fps.time_diff > 1000){
		updateFPSData();
		fps.frame_count = 0;
		fps.time_diff = 0;
	}
	fps.time_start = fps.time_end;
}

//update the fps data for rendering
void updateFPSData(){
	manageFPSData();
	setBuffers_FPS();
}

//update the object data for rendering the fps
void manageFPSData(){

	//initialize vector
	vector<unsigned int> indices;
	vector<vec2> position;
	vector<vec2> tex_coord;

	//define text to render
	string fps_text = fps_head + to_string(fps.frame_count);

	float x = -0.95f, y = -0.95f;

	//create the data and push to buffers
	for(int i = 0; i < (int)fps_text.size(); i++){

		vec2 top_left = vec2(x + i*game_data.score_data->font_size, y + game_data.score_data->font_size);
		vec2 top_right = vec2(x + i*game_data.score_data->font_size + game_data.score_data->font_size, y + game_data.score_data->font_size);
		vec2 bottom_left = vec2(x + i*game_data.score_data->font_size, y);
		vec2 bottom_right = vec2(x + i*game_data.score_data->font_size + game_data.score_data->font_size, y);

		position.push_back(top_left);
		position.push_back(top_right);
		position.push_back(bottom_left);
		position.push_back(bottom_right);

		indices.push_back(4*i);
		indices.push_back(4*i+2);
		indices.push_back(4*i+1);

		indices.push_back(4*i+3);
		indices.push_back(4*i+1);
		indices.push_back(4*i+2);

		char current_char = fps_text[i] - 32;
		float tex_x = (current_char%8)/8.0f;
		float tex_y = (current_char/8)/8.0f;
		
		vec2 tex_top_left = vec2(tex_x, 1.f - tex_y);
		vec2 tex_top_right = vec2(tex_x + (1.f/8.f), 1.f - tex_y);
		vec2 tex_bottom_left = vec2(tex_x, 1.f - (tex_y + (1.f/8.f)));
		vec2 tex_bottom_right = vec2(tex_x + (1.f/8.f), 1.f - (tex_y + (1.f/8.f)));

		tex_coord.push_back(tex_top_left);
		tex_coord.push_back(tex_top_right);
		tex_coord.push_back(tex_bottom_left);
		tex_coord.push_back(tex_bottom_right);
	}

	fps.vertexPosition = position;
	fps.vertexTexture = tex_coord;
	fps.vertexIndex = indices;
}

//toggle the rendering of fps
void toggleFPS(){
	fps.show = !fps.show;
}

//Functions for Handling Creature AI

void handleCreature0AI(){

	//if first time initiate the clock set the start flag
	if(creature0.data->time_start == 0){
		creature0.data->time_start = clock();
		return;
	}

	//Calculate time duration
	int time_end = clock();
	creature0.data->duration += (time_end - creature0.data->time_start)*1000/CLOCKS_PER_SEC;
	creature0.data->time_start = time_end;

	//if collided
	if(creature0.collided){
		if(creature0.data->duration >= 2500){
			handleCollision();
		}else if(creature0.data->duration >= 2000){
			creature0.data->show = true;
		}else if(creature0.data->duration >= 1500){
			creature0.data->show = false;
		}else if(creature0.data->duration >= 1000){
			creature0.data->show = true;
		}else if(creature0.data->duration >= 500){
			creature0.data->show = false;
		}
		return;
	}else if(creature1.collided){
		return;
	}else if(creature2.collided){
		return;
	}

	if(creature0.start){
		if(creature0.data->duration >= creature0.data->start_interval){
			creature0.data->show = true;
			creature0.start = false;
			creature0.motion = true;
			creature0.data->duration = 0;
			creature0.data->start_interval = 2000;
			introduceCreature0();
		}
	}else if(creature0.motion){
		if(creature0.data->duration >= creature0.data->motion_interval){
			creature0.data->duration = 0;
			moveCreature0();
		}
	}
}

void handleCreature1AI(){
	if(creature1.transformed)
		handleCreature1AI_transformed();
	else
		handleCreature1AI_normal();
}

void handleCreature1AI_normal(){
	//if first time initiate the clock set the start flag
	if(creature1.data->time_start == 0){
		creature1.data->time_start = clock();
		return;
	}

	//Calculate time duration
	int time_end = clock();
	creature1.data->duration += (time_end - creature1.data->time_start)*1000/CLOCKS_PER_SEC;
	creature1.data->time_start = time_end;

	//collision
	if(creature1.collided){
		if(creature1.data->duration >= 2500){
			handleCollision();
		}else if(creature1.data->duration >= 2000){
			creature1.data->show = true;
		}else if(creature1.data->duration >= 1500){
			creature1.data->show = false;
		}else if(creature1.data->duration >= 1000){
			creature1.data->show = true;
		}else if(creature1.data->duration >= 500){
			creature1.data->show = false;
		}
		return;
	}else if(creature0.collided){
		return;
	}else if(creature2.collided){
		return;
	}

	if(creature1.start){
		if(creature1.data->duration >= creature1.data->start_interval){
			creature1.data->show = true;
			creature1.start = false;
			creature1.motion = true;
			creature1.data->duration = 0;
			creature1.data->start_interval = 3000;
			introduceCreature1_normal();
		}
	}else if(creature1.motion){
		if(creature1.data->duration >= creature1.data->motion_interval){
			creature1.data->duration = 0;
			moveCreature1_normal();
		}
	}
}

void handleCreature1AI_transformed(){
	if(creature1.data->time_start == 0){
		creature1.data->time_start = clock();
		return;
	}

	int time_end = clock();
	creature1.data->duration += (time_end - creature1.data->time_start)*1000/CLOCKS_PER_SEC;
	creature1.data->time_start = time_end;

	//collision
	if(creature1.collided){
		if(creature1.data->duration >= 2500){
			handleCollision();
		}else if(creature1.data->duration >= 2000){
			creature1.data->show = true;
		}else if(creature1.data->duration >= 1500){
			creature1.data->show = false;
		}else if(creature1.data->duration >= 1000){
			creature1.data->show = true;
		}else if(creature1.data->duration >= 500){
			creature1.data->show = false;
		}
		return;
	}else if(creature0.collided){
		return;
	}else if(creature2.collided){
		return;
	}

	if(creature1.start){
		if(creature1.data->duration >= 1500){
			creature1.transformed = true;
			creature1.start = false;
			creature1.motion = true;
			creature1.data->duration = 0;
			introduceCreature1_transformed();
		}
	}else{
		if(creature1.data->duration >= creature1.data->motion_interval){
			creature1.data->duration = 0;
			moveCreature1_transformed();
		}
	}
}

void handleCreature2AI(){
	//if first time initiate the clock set the start flag
	if(creature2.data->time_start == 0){
		creature2.data->time_start = clock();
		return;
	}

	//Calculate time duration
	int time_end = clock();
	creature2.data->duration += (time_end - creature2.data->time_start)*1000/CLOCKS_PER_SEC;
	creature2.data->time_start = time_end;

	//if collided
	if(creature2.collided){
		if(creature2.data->duration >= 2500){
			handleCollision();
		}else if(creature2.data->duration >= 2000){
			creature2.data->show = true;
		}else if(creature2.data->duration >= 1500){
			creature2.data->show = false;
		}else if(creature2.data->duration >= 1000){
			creature2.data->show = true;
		}else if(creature2.data->duration >= 500){
			creature2.data->show = false;
		}
		return;
	}else if(creature1.collided){
		return;
	}else if(creature0.collided){
		return;
	}

	if(creature2.start){
		if(creature2.data->duration >= creature2.data->start_interval){
			creature2.data->show = true;
			creature2.start = false;
			creature2.motion = true;
			creature2.data->duration = 0;
			creature2.data->start_interval = 4000;
			introduceCreature2();
		}
	}else if(creature2.motion){
		if(creature2.data->duration >= creature2.data->motion_interval){
			creature2.data->duration = 0;
			moveCreature2();
		}
	}
}

//Functions for Creature0 Handling
//move creature0 to a certain position and state
void moveCreature0(){
	//if movements are possible randomly select one
	int movement = ((rand()%2) + 1) * 2;

	creature0.dir = movement;
	creature0.animate = true;
	creature0.nextState = getState(creature0.data->current_state, creature0.dir);
}

//set the initial position and state of creature0
void introduceCreature0(){
	int position = rand()%3;
	creature0.data->current_state = &(game_data.states[position]);

	creature0.data->translationMat = translate(mat4(), vec3(0, 0.54, -0.2));

	if(position == 1)
		creature0.data->translationMat = translate(creature0.data->translationMat,  vec3(-0.07,-0.12,-0.1));
	else if(position == 2)
		creature0.data->translationMat = translate(creature0.data->translationMat,  vec3(0.07,-0.12,-0.1));

	creature0.data->transformMat = creature0.data->translationMat * creature0.data->scalingMat * creature0.data->rotationMat;
	creature0.data->normalMat = transpose(inverse(creature0.data->transformMat));
}

//Functions for Creature1 Handling
//move creature1 to a certain position and state
void moveCreature1_normal(){
	int movement = ((rand()%2) + 1) * 2;

	creature1.dir = movement;
	creature1.nextState = getState(creature1.data->current_state, creature1.dir);

	//if movements are offboard restart the loading counter and remove the creature from screen
	if(creature1.nextState == NULL){
		creature1.start = true;
		creature1.transformed = true;
		creature1.motion = false;
	}else{
		creature1.animate = true;
	}
}

void moveCreature1_transformed(){

	//Calculate the Movement
	int player_row = game_data.current_state->row;
	int player_column = game_data.current_state->column;

	int creature_row = creature1.data->current_state->row;
	int creature_column = creature1.data->current_state->column;

	int row_diff = player_row - creature_row;
	int column_diff = player_column - creature_column;

	int abs_row = abs(row_diff);
	int abs_column = abs(column_diff);

	cout<<abs_row<<" "<<abs_column;

	int movement = 0;

	if(row_diff == 0){
		cout<<"\nRow Diff 0";
		movement = moveCreature1_column(column_diff);
	}else if(column_diff == 0){
		cout<<"\nColumns Diff 0";
		movement = moveCreature1_row(row_diff);
	}else if(abs_row > abs_column){
		cout<<"\nMove in Column";
		if(column_diff > 0){
			movement = 4;
			if(getState(creature1.data->current_state, movement) == NULL){
				movement = moveCreature1_row(row_diff);
			}
		}else{
			movement = 3;
			if(getState(creature1.data->current_state, movement) == NULL){
				movement = moveCreature1_row(row_diff);
			}
		}
	}else{
		cout<<"\nMove in Row";
		if(row_diff > 0){
			movement = 2;
			if(getState(creature1.data->current_state, movement) == NULL){
				movement = moveCreature1_column(column_diff);
			}
		}else{
			movement = 1;
			if(getState(creature1.data->current_state, movement) == NULL){
				movement = moveCreature1_column(column_diff);
			}
		}
	}

	if(movement != 0){

		//Set Rotation
		float angle = 0.f;
		if(movement == 1){
			angle = -135.f;
		}else if(movement == 2){
			angle = 45.f;
		}else if(movement == 3){
			angle = 135.f;
		}else if(movement == 4){
			angle = -45.f;
		}

		creature1.data->rotationMat[1] = rotate(mat4(), angle, vec3(0,1,0));
		//Start animation
		creature1.dir = movement;
		creature1.animate = true;
		creature1.nextState = getState(creature1.data->current_state, creature1.dir);
	}
}

int moveCreature1_row(int diff){
	int movement = 2;
	if(diff > 0){
		movement = 2;
	}else{
		movement = 1;
	}

	return movement;
}

int moveCreature1_column(int diff){
	int movement = 4;
	if(diff > 0){
		movement = 4;
	}else{
		movement = 3;
	}

	return movement;
}

//set the initial position and state of creature1
void introduceCreature1_normal(){
	int position = rand()%3;
	creature1.data->current_state = &(game_data.states[position]);

	creature1.data->translationMat[0] = translate(mat4(), vec3(0, 0.54, -0.2));

	if(position == 1)
		creature1.data->translationMat[0] = translate(creature1.data->translationMat[0],  vec3(-0.07,-0.12,-0.1));
	else if(position == 2)
		creature1.data->translationMat[0] = translate(creature1.data->translationMat[0],  vec3(0.07,-0.12,-0.1));

	creature1.data->transformMat[0] = creature1.data->translationMat[0] * creature1.data->scalingMat[0] * creature1.data->rotationMat[0];
	creature1.data->normalMat[0] = transpose(inverse(creature1.data->transformMat[0]));
}

void introduceCreature1_transformed(){
	creature1.data->translationMat[1] = creature1.data->translationMat[0];

	creature1.data->transformMat[1] = creature1.data->translationMat[1] * creature1.data->scalingMat[1] * creature1.data->rotationMat[1];
	creature1.data->normalMat[1] = transpose(inverse(creature1.data->transformMat[1]));
}

//Functions for Creature2 Handling
//move creature0 to a certain position and state
void moveCreature2(){
	int movement = 2;

	creature2.dir = movement;
	creature2.animate = true;
	creature2.nextState = getState(creature2.data->current_state, creature2.dir);
}

//set the initial position and state of creature0
void introduceCreature2(){
	int temp_random = (rand()%3);
	int position = temp_random*(temp_random+1)/2;

	cout<<"\nTemp::"<<temp_random;
	cout<<"\nPosition::"<<position;

	creature2.data->current_state = &(game_data.states[position]);

	creature2.data->translationMat = translate(mat4(), vec3(0, 0.54, -0.2));
	
	float x = temp_random*0.07f;
	float y = temp_random*0.12f;
	float z = temp_random*0.1f;

	creature2.data->translationMat = translate(creature2.data->translationMat,  vec3(-x,-y,-z));

	creature2.data->transformMat = creature2.data->translationMat * creature2.data->scalingMat * creature2.data->rotationMat;
	creature2.data->normalMat = transpose(inverse(creature2.data->transformMat));
}

//Functions to handle creature player collisions
int checkCollisions(){

	//creature0 collision detection
	if(creature0.data->current_state == game_data.current_state)
		return 0;

	if(creature1.data->current_state == game_data.current_state)
		return 1;

	if(creature2.data->current_state == game_data.current_state)
		return 2;

	//if no collision
	return -1;
}

void initiateCollision(int flag){
	if(flag == -1)
		return;
	tool.key_press = false;
	player.animate = false;
	alSourcePlay(sounds.player_collision_source);
	if(flag == 0){
		//creature0 collided
		creature0.collided = true;
		creature0.data->duration = 0;
		creature0.data->time_start = clock();
	}else if(flag == 1){
		//creature1 collided
		creature1.collided = true;
		creature1.data->duration = 0;
		creature1.data->time_start = clock();
	}else if(flag == 2){
		cout<<"\nCollision";
		//creature2 collided
		creature2.collided = true;
		creature2.data->duration = 0;
		creature2.data->time_start = clock();
	}
}

void handleCollision(){
	if(game_data.remaining_lives > 1){
		game_data.remaining_lives--;
		resetPlayerPosition();
		resetCreatureData(false);
		if(game_data.blocks_covered_count == game_data.total_blocks){
			moveToNextLevel();
		}
		tool.key_press = true;
	}else{
		gameOver();
	}
}

//Functions for Handling Game Over Status
void gameOver(){
	alSourcePlay(sounds.game_over_source);
	tool.key_press = false;
	game_over.state = true;
	game_over.show = true;
	game_over.duration = 0;
	game_over.time_start = clock();
}

void handleGameOver(){
	int time_end = clock();
	game_over.duration += (time_end - game_over.time_start)*1000/CLOCKS_PER_SEC;
	game_over.time_start = time_end;

	if(game_over.duration >= 2500){
		resetGameData();
	}else if(game_over.duration >= 2000){
		game_over.show = true;
	}else if(game_over.duration >= 1500){
		game_over.show = false;
	}else if(game_over.duration >= 1000){
		game_over.show = true;
	}else if(game_over.duration >= 500){
		game_over.show = false;
	}
}

void createGameOverObject(){

	//game over
	//initialize vector
	vector<unsigned int> indices;
	vector<vec2> position;
	vector<vec2> tex_coord;

	//define text to render
	string go_text = game_over.text_to_show;

	float font_size = game_over.font_size;

	float x = -0.36f, y = 0.f;

	//create the data and push to buffers
	for(int i = 0; i < (int)go_text.size(); i++){

		vec2 top_left = vec2(x + i*font_size, y + font_size);
		vec2 top_right = vec2(x + i*font_size + font_size, y + font_size);
		vec2 bottom_left = vec2(x + i*font_size, y);
		vec2 bottom_right = vec2(x + i*font_size + font_size, y);

		position.push_back(top_left);
		position.push_back(top_right);
		position.push_back(bottom_left);
		position.push_back(bottom_right);

		indices.push_back(4*i);
		indices.push_back(4*i+2);
		indices.push_back(4*i+1);

		indices.push_back(4*i+3);
		indices.push_back(4*i+1);
		indices.push_back(4*i+2);

		char current_char = go_text[i] - 32;
		float tex_x = (current_char%8)/8.0f;
		float tex_y = (current_char/8)/8.0f;
		
		vec2 tex_top_left = vec2(tex_x, 1.f - tex_y);
		vec2 tex_top_right = vec2(tex_x + (1.f/8.f), 1.f - tex_y);
		vec2 tex_bottom_left = vec2(tex_x, 1.f - (tex_y + (1.f/8.f)));
		vec2 tex_bottom_right = vec2(tex_x + (1.f/8.f), 1.f - (tex_y + (1.f/8.f)));

		tex_coord.push_back(tex_top_left);
		tex_coord.push_back(tex_top_right);
		tex_coord.push_back(tex_bottom_left);
		tex_coord.push_back(tex_bottom_right);
	}
	game_over.vertexPosition = position;
	game_over.vertexTexture = tex_coord;
	game_over.vertexIndex = indices;

	//game complete

	indices.clear();
	position.clear();
	tex_coord.clear();

	//define text to render
	go_text = game_over.text_to_show_2;

	//create the data and push to buffers
	for(int i = 0; i < (int)go_text.size(); i++){

		vec2 top_left = vec2(x + i*font_size, y + font_size);
		vec2 top_right = vec2(x + i*font_size + font_size, y + font_size);
		vec2 bottom_left = vec2(x + i*font_size, y);
		vec2 bottom_right = vec2(x + i*font_size + font_size, y);

		position.push_back(top_left);
		position.push_back(top_right);
		position.push_back(bottom_left);
		position.push_back(bottom_right);

		indices.push_back(4*i);
		indices.push_back(4*i+2);
		indices.push_back(4*i+1);

		indices.push_back(4*i+3);
		indices.push_back(4*i+1);
		indices.push_back(4*i+2);

		char current_char = go_text[i] - 32;
		float tex_x = (current_char%8)/8.0f;
		float tex_y = (current_char/8)/8.0f;
		
		vec2 tex_top_left = vec2(tex_x, 1.f - tex_y);
		vec2 tex_top_right = vec2(tex_x + (1.f/8.f), 1.f - tex_y);
		vec2 tex_bottom_left = vec2(tex_x, 1.f - (tex_y + (1.f/8.f)));
		vec2 tex_bottom_right = vec2(tex_x + (1.f/8.f), 1.f - (tex_y + (1.f/8.f)));

		tex_coord.push_back(tex_top_left);
		tex_coord.push_back(tex_top_right);
		tex_coord.push_back(tex_bottom_left);
		tex_coord.push_back(tex_bottom_right);
	}
	game_over.vertexPosition_2 = position;
	game_over.vertexTexture_2 = tex_coord;
	game_over.vertexIndex_2 = indices;
}

//Functions for Handling Game Start Status
void gameStart(){
	tool.key_press = false;
	alSourcePlay(sounds.game_start_source);

	do{
		alGetSourcei(sounds.game_start_source, AL_SOURCE_STATE, &sounds.game_start_state);
	}while(sounds.game_start_state == AL_PLAYING);

	game_start.state = false;
	tool.key_press = true;

}

void createGameStartObject(){
	//set data for top text

	//initialize vector
	vector<unsigned int> indices;
	vector<vec2> position;
	vector<vec2> tex_coord;

	//define text to render
	string go_text = game_start.text_top;

	float font_size = game_start.font_size_top;

	float x = -0.8f, y = 0.5f;

	//create the data and push to buffers
	for(int i = 0; i < (int)go_text.size(); i++){

		vec2 top_left = vec2(x + i*font_size, y + font_size);
		vec2 top_right = vec2(x + i*font_size + font_size, y + font_size);
		vec2 bottom_left = vec2(x + i*font_size, y);
		vec2 bottom_right = vec2(x + i*font_size + font_size, y);

		position.push_back(top_left);
		position.push_back(top_right);
		position.push_back(bottom_left);
		position.push_back(bottom_right);

		indices.push_back(4*i);
		indices.push_back(4*i+2);
		indices.push_back(4*i+1);

		indices.push_back(4*i+3);
		indices.push_back(4*i+1);
		indices.push_back(4*i+2);

		char current_char = go_text[i] - 32;
		float tex_x = (current_char%8)/8.0f;
		float tex_y = (current_char/8)/8.0f;
		
		vec2 tex_top_left = vec2(tex_x, 1.f - tex_y);
		vec2 tex_top_right = vec2(tex_x + (1.f/8.f), 1.f - tex_y);
		vec2 tex_bottom_left = vec2(tex_x, 1.f - (tex_y + (1.f/8.f)));
		vec2 tex_bottom_right = vec2(tex_x + (1.f/8.f), 1.f - (tex_y + (1.f/8.f)));

		tex_coord.push_back(tex_top_left);
		tex_coord.push_back(tex_top_right);
		tex_coord.push_back(tex_bottom_left);
		tex_coord.push_back(tex_bottom_right);
	}
	game_start.vertexPosition[0] = position;
	game_start.vertexTexture[0] = tex_coord;
	game_start.vertexIndex[0] = indices;

	//set data for bottom text

	//initialize vector
	indices.clear();
	position.clear();
	tex_coord.clear();

	//define text to render
	go_text = game_start.text_bottom;

	font_size = game_start.font_size_bottom;

	x = -0.8f, y = -0.2f;

	//create the data and push to buffers
	for(int i = 0; i < (int)go_text.size(); i++){

		vec2 top_left = vec2(x + i*font_size, y + font_size);
		vec2 top_right = vec2(x + i*font_size + font_size, y + font_size);
		vec2 bottom_left = vec2(x + i*font_size, y);
		vec2 bottom_right = vec2(x  +i*font_size + font_size, y);

		position.push_back(top_left);
		position.push_back(top_right);
		position.push_back(bottom_left);
		position.push_back(bottom_right);

		indices.push_back(4*i);
		indices.push_back(4*i+2);
		indices.push_back(4*i+1);

		indices.push_back(4*i+3);
		indices.push_back(4*i+1);
		indices.push_back(4*i+2);

		char current_char = go_text[i] - 32;
		float tex_x = (current_char%8)/8.0f;
		float tex_y = (current_char/8)/8.0f;
		
		vec2 tex_top_left = vec2(tex_x, 1.f - tex_y);
		vec2 tex_top_right = vec2(tex_x + (1.f/8.f), 1.f - tex_y);
		vec2 tex_bottom_left = vec2(tex_x, 1.f - (tex_y + (1.f/8.f)));
		vec2 tex_bottom_right = vec2(tex_x + (1.f/8.f), 1.f - (tex_y + (1.f/8.f)));

		tex_coord.push_back(tex_top_left);
		tex_coord.push_back(tex_top_right);
		tex_coord.push_back(tex_bottom_left);
		tex_coord.push_back(tex_bottom_right);
	}
	game_start.vertexPosition[1] = position;
	game_start.vertexTexture[1] = tex_coord;
	game_start.vertexIndex[1] = indices;
}

//Disks Handling
void moveToDiskRight(){
	//Reset Player Position
	player.translationMat = mat4();
	player.translationMat = translate(player.translationMat, vec3(0, 0.58, -0.1));

	player.rotationMatY = rotate(mat4(), -45.f, vec3(0,1,0));
	player.transformMat = player.translationMat * player.scalingMat * player.rotationMatY * player.rotationMatX;
	player.normalMat = transpose(inverse(player.transformMat));

	//Move to first state
	moveToState(&(game_data.states[0]));

	//Make Disk Gone
	disks.right->enabled = false;
}

void moveToDiskLeft(){
	//Reset Player Position
	player.translationMat = mat4();
	player.translationMat = translate(player.translationMat, vec3(0, 0.58, -0.1));

	player.rotationMatY = rotate(mat4(), -45.f, vec3(0,1,0));
	player.transformMat = player.translationMat * player.scalingMat * player.rotationMatY * player.rotationMatX;
	player.normalMat = transpose(inverse(player.transformMat));

	//Move to first state
	moveToState(&(game_data.states[0]));

	//Make Disk Gone
	disks.left->enabled = false;
}

//Animation
void animateDiskLeft(){
	
	if(disks.left->r_animation_start == 0){
		disks.left->r_animation_start = clock();
		return;
	}

	int time_end = clock();
	disks.left->r_animation_duration += (time_end - disks.left->r_animation_start)*1000/CLOCKS_PER_SEC;
	disks.left->r_animation_start = time_end;

	if(disks.left->r_animation_duration >= disks.left->r_animation_interval){
		disks.left->rotationMatY = rotate(disks.left->rotationMatY, 90.f, vec3(0,1,0));
		disks.left->r_animation_duration = 0;

		disks.left->transformMat = disks.left->translationMat * disks.left->scalingMat * disks.left->rotationMatX * disks.left->rotationMatY;
		disks.left->normalMat = transpose(inverse(disks.left->translationMat));
	}
}

void animateDiskRight(){
	if(disks.right->r_animation_start == 0){
		disks.right->r_animation_start = clock();
		return;
	}

	int time_end = clock();
	disks.right->r_animation_duration += (time_end - disks.right->r_animation_start)*1000/CLOCKS_PER_SEC;
	disks.right->r_animation_start = time_end;

	if(disks.right->r_animation_duration >= disks.right->r_animation_interval){
		disks.right->rotationMatY = rotate(disks.right->rotationMatY, 90.f, vec3(0,1,0));
		disks.right->r_animation_duration = 0;

		disks.right->transformMat = disks.right->translationMat * disks.right->scalingMat * disks.right->rotationMatX * disks.right->rotationMatY;
		disks.right->normalMat = transpose(inverse(disks.right->normalMat));
	}
}

//Player Animation
void animatePlayer(){

	if(player.animation_start == 0){
		alSourcePlay(sounds.player_hop_source);
		player.animation_start = clock();
		return;
	}

	int time_end = clock();
	player.animation_duration += (time_end - player.animation_start)*1000/CLOCKS_PER_SEC;
	player.animation_start = time_end;

	if(player.animation_duration >= player.animation_interval){
		//Pick the animation matrix
		float* x, *y, *z;
		if(player.dir == 1){
			x = animation.x_left;
			y = animation.y_left;
			z = animation.z_left;
		}else if(player.dir == 2){
			x = animation.x_right;
			y = animation.y_right;
			z = animation.z_right;
		}else if(player.dir == 3){
			x = animation.x_up;
			y = animation.y_up;
			z = animation.z_up;
		}else if(player.dir == 4){
			x = animation.x_down;
			y = animation.y_down;
			z = animation.z_down;
		}
	
		mat4 temp_translation = translate(player.translationMat, vec3(x[player.animation_index],y[player.animation_index],z[player.animation_index]));
		player.transformMat = temp_translation * player.scalingMat * player.rotationMatY * player.rotationMatX;
		player.normalMat = transpose(inverse(player.transformMat));

		player.animation_duration = 0;

		player.animation_index++;
		if(player.animation_index == animation.frame_count){
			player.translationMat = temp_translation;
			player.animation_index = 0;
			player.animation_start = 0;

			if(player.nextState != NULL)
				moveToState(player.nextState);
			else{
				if(game_data.current_state->column == disks.left->column_near && game_data.current_state->row == disks.left->row_near && disks.left->enabled){
					resetPlayerData();
					moveToState(&game_data.states[0]);

					//Disable Disk
					disks.left->enabled = false;
				}else if(game_data.current_state->column == disks.right->column_near && game_data.current_state->row == disks.right->row_near && disks.right->enabled){
					resetPlayerData();
					moveToState(&game_data.states[0]);

					//Disable Disk
					disks.right->enabled = false;
				}else{
					game_data.current_state = NULL;
					player.fall = true;
				}
			}
			player.animate = false;
		}
	}
}

//Player fall animation
void animatePlayerFall(){
	if(player.animation_start == 0){
		alSourcePlay(sounds.player_fall_source);
		player.animation_start = clock();
		return;
	}

	int time_end = clock();
	player.animation_duration += (time_end - player.animation_start)*1000/CLOCKS_PER_SEC;
	player.fall_duration += (time_end - player.animation_start)*1000/CLOCKS_PER_SEC;
	player.animation_start = time_end;

	if(player.animation_duration >= 2500){
		player.fall_duration = 0;
		player.fall = false;
		playerLost(player.dir);
	}else if(player.animation_duration >= 1500){
		player.show = false;
	}else if(player.fall_duration >= player.fall_animation_interval){
		player.fall_duration = 0;
		player.translationMat = translate(player.translationMat, vec3(0, -0.01f, 0.01f));
		player.transformMat = player.translationMat * player.scalingMat * player.rotationMatY * player.rotationMatX;
		player.normalMat = transpose(inverse(player.transformMat));
	}
}

//Creature0 Animation
void animateCreature0(){

	if(creature0.animation_start == 0){
		creature0.animation_start = clock();
		return;
	}

	int time_end = clock();
	creature0.animation_duration += (time_end - creature0.animation_start)*1000/CLOCKS_PER_SEC;
	creature0.animation_start = time_end;

	if(creature0.animation_duration >= creature0.animation_interval){
		float* x, *y, *z;
		//move down for 4 and move right for 2
		if(creature0.dir == 2){
			x = animation.x_right;
			y = animation.y_right;
			z = animation.z_right;
		}else if(creature0.dir == 4){
			x = animation.x_down;
			y = animation.y_down;
			z = animation.z_down;
		}

		mat4 temp_translation = translate(creature0.data->translationMat, vec3(x[creature0.animation_index],y[creature0.animation_index],z[creature0.animation_index]));
		creature0.data->transformMat = temp_translation * creature0.data->scalingMat * creature0.data->rotationMat;
		creature0.data->normalMat = transpose(inverse(creature0.data->transformMat));

		creature0.animation_duration = 0;

		creature0.animation_index++;

		if(creature0.animation_index == animation.frame_count){
			creature0.data->translationMat = temp_translation;
			creature0.animation_index = 0;
			creature0.animation_start = 0;
			creature0.animate = false;

			//if movements are offboard restart the loading counter and remove the creature from screen
			if(creature0.nextState == NULL){
				creature0.data->show = false;
				creature0.start = true;
				creature0.motion = false;
				creature0.data->current_state = NULL;
			}else{
				//get the next state
				creature0.data->current_state = creature0.nextState;
			}
		}
	}
}

//Creature1 Animation
void animateCreature1(){
	if(creature1.transformed)
		animateCreature1_transformed();
	else
		animateCreature1_normal();
}

//Creature1_normal Animation
void animateCreature1_normal(){
	if(creature1.animation_start == 0){
		creature1.animation_start = clock();
		return;
	}

	int time_end = clock();
	creature1.animation_duration += (time_end - creature1.animation_start)*1000/CLOCKS_PER_SEC;
	creature1.animation_start = time_end;

	if(creature1.animation_duration >= creature1.animation_interval){
		float* x, *y, *z;
		//move down for 4 and move right for 2
		if(creature1.dir == 2){
			x = animation.x_right;
			y = animation.y_right;
			z = animation.z_right;
		}else if(creature1.dir == 4){
			x = animation.x_down;
			y = animation.y_down;
			z = animation.z_down;
		}

		mat4 temp_translation = translate(creature1.data->translationMat[0], vec3(x[creature1.animation_index],y[creature1.animation_index],z[creature1.animation_index]));
		creature1.data->transformMat[0] = temp_translation * creature1.data->scalingMat[0] * creature1.data->rotationMat[0];
		creature1.data->normalMat[0] = transpose(inverse(creature1.data->transformMat[0]));

		creature1.animation_duration = 0;

		creature1.animation_index++;

		if(creature1.animation_index == animation.frame_count){
			creature1.data->translationMat[0] = temp_translation;
			creature1.animation_index = 0;
			creature1.animation_start = 0;
			creature1.animate = false;
			//get the next state
			creature1.data->current_state = creature1.nextState;
		}
	}
}

//Creature1_transformed Animation
void animateCreature1_transformed(){
	if(creature1.animation_start == 0){
		alSourcePlay(sounds.coily_hop_source);
		creature1.animation_start = clock();
		return;
	}

	int time_end = clock();
	creature1.animation_duration += (time_end - creature1.animation_start)*1000/CLOCKS_PER_SEC;
	creature1.animation_start = time_end;

	if(creature1.animation_duration >= creature1.animation_interval){
		float* x, *y, *z;
		//move down for 4 and move right for 2
		if(creature1.dir == 1){
			x = animation.x_left;
			y = animation.y_left;
			z = animation.z_left;
		}else if(creature1.dir == 2){
			x = animation.x_right;
			y = animation.y_right;
			z = animation.z_right;
		}else if(creature1.dir == 3){
			x = animation.x_up;
			y = animation.y_up;
			z = animation.z_up;
		}else if(creature1.dir == 4){
			x = animation.x_down;
			y = animation.y_down;
			z = animation.z_down;
		}

		mat4 temp_translation = translate(creature1.data->translationMat[1], vec3(x[creature1.animation_index],y[creature1.animation_index],z[creature1.animation_index]));
		creature1.data->transformMat[1] = temp_translation * creature1.data->scalingMat[1] * creature1.data->rotationMat[1];
		creature1.data->normalMat[1] = transpose(inverse(creature1.data->transformMat[1]));

		creature1.animation_duration = 0;

		creature1.animation_index++;

		if(creature1.animation_index == animation.frame_count){
			creature1.data->translationMat[1] = temp_translation;
			creature1.animation_index = 0;
			creature1.animation_start = 0;
			creature1.animate = false;

			//if movements are offboard restart the loading counter and remove the creature from screen
			if(creature1.nextState == NULL){
				creature1.data->show = false;
				creature1.start = true;
				creature1.motion = false;
				creature1.data->current_state = NULL;
			}else{
				//get the next state
				creature1.data->current_state = creature1.nextState;
			}
		}
	}
}

//Creature2 Animation
void animateCreature2(){
	if(creature2.animation_start == 0){
		creature2.animation_start = clock();
		return;
	}

	int time_end = clock();
	creature2.animation_duration += (time_end - creature2.animation_start)*1000/CLOCKS_PER_SEC;
	creature2.animation_start = time_end;

	if(creature2.animation_duration >= creature2.animation_interval){
		float* x, *y, *z;
		x = animation.x_right;
		y = animation.y_right;
		z = animation.z_right;

		mat4 temp_translation = translate(creature2.data->translationMat, vec3(x[creature2.animation_index],y[creature2.animation_index],z[creature2.animation_index]));
		creature2.data->transformMat = temp_translation * creature2.data->scalingMat * creature2.data->rotationMat;
		creature2.data->normalMat = transpose(inverse(creature2.data->transformMat));

		creature2.animation_duration = 0;

		creature2.animation_index++;

		if(creature2.animation_index == animation.frame_count){
			creature2.data->translationMat = temp_translation;
			creature2.animation_index = 0;
			creature2.animation_start = 0;
			creature2.animate = false;

			//if movements are offboard restart the loading counter and remove the creature from screen
			if(creature2.nextState == NULL){
				creature2.data->show = false;
				creature2.start = true;
				creature2.motion = false;
				creature2.data->current_state = NULL;
			}else{
				//get the next state
				creature2.data->current_state = creature2.nextState;
			}
		}


	}

}

//Level Transition Animation
void levelTransition(){
	if(level_data.transition_start == 0){
		level_data.transition_start = clock();
		return;
	}

	int time_end = clock();
	level_data.transition_duration += (time_end - level_data.transition_start)*1000/CLOCKS_PER_SEC;
	level_data.transition_start = time_end;

	if(level_data.transition_move){
		if(level_data.transition_duration >= level_data.transition_interval){
			level_data.animation_index++;
			level_data.transition_duration = 0;
			for(size_t i = 0; i < scene.cubeCount; i++){
				scene.transformMat[i] = translate(scene.transformMat[i], vec3(0.f, -1.f, 0.f));
			}

			if(level_data.animation_index == level_data.frame_count){
				moveToNextLevel();
				level_data.transition_move = false;
				level_data.transition = false;
				level_data.transition_start = 0;
			}
		}
	}else{
		if(level_data.transition_duration >= 3000){
			alSourcePlay(sounds.bonk_source);
			player.show = false;
			level_data.transition_move = true;
			level_data.transition_duration = 0;
			level_data.animation_index = 0;
		}else if(level_data.transition_duration >= 2000 && disks.right->enabled){
			alSourcePlay(sounds.bonk_source);
			disks.right->enabled = false;
		}else if(level_data.transition_duration >= 1000 && disks.left->enabled){
			alSourcePlay(sounds.bonk_source);
			disks.left->enabled = false;
		}
	}
}