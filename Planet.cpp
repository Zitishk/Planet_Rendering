#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"
#include "include/camera.h"
#include "include/shader.h"
#include "include/head.hpp"
#include "include/Sphere.cpp"

int main(int argc, char** argv) {

	GLFWwindow* window;
	glfwSetErrorCallback(opengl::error_callback);

	if (!glfwInit()) return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 

	window = glfwCreateWindow(SCR_WIDTH , SCR_HEIGHT , "Lights", NULL, NULL);
	if (!window){
	    glfwTerminate();
	    return -1;
	}
  
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window,key_callback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) std::cerr<<"GLEW Init Failed : %s"<<std::endl;

	opengl::initGL();

	float skyVertices[] = {
	      // positions          
		-1.0f,  1.0f, -1.0f,
	        -1.0f, -1.0f, -1.0f,
	       	 1.0f, -1.0f, -1.0f,
	       	 1.0f, -1.0f, -1.0f,
	       	 1.0f,  1.0f, -1.0f,
	      	-1.0f,  1.0f, -1.0f,

	      	-1.0f, -1.0f,  1.0f,
	      	-1.0f, -1.0f, -1.0f,
	      	-1.0f,  1.0f, -1.0f,
	      	-1.0f,  1.0f, -1.0f,
	      	-1.0f,  1.0f,  1.0f,
	      	-1.0f, -1.0f,  1.0f,

         	 1.0f, -1.0f, -1.0f,
         	 1.0f, -1.0f,  1.0f,
         	 1.0f,  1.0f,  1.0f,
         	 1.0f,  1.0f,  1.0f,
         	 1.0f,  1.0f, -1.0f,
         	 1.0f, -1.0f, -1.0f,

        	-1.0f, -1.0f,  1.0f,
        	-1.0f,  1.0f,  1.0f,
                 1.0f,  1.0f,  1.0f,
         	 1.0f,  1.0f,  1.0f,
         	 1.0f, -1.0f,  1.0f,
         	-1.0f, -1.0f,  1.0f,

        	-1.0f,  1.0f, -1.0f,
        	 1.0f,  1.0f, -1.0f,
        	 1.0f,  1.0f,  1.0f,
        	 1.0f,  1.0f,  1.0f,
        	-1.0f,  1.0f,  1.0f,
        	-1.0f,  1.0f, -1.0f,

        	-1.0f, -1.0f, -1.0f,
        	-1.0f, -1.0f,  1.0f,
        	 1.0f, -1.0f, -1.0f,
        	 1.0f, -1.0f, -1.0f,
        	-1.0f, -1.0f,  1.0f,
        	 1.0f, -1.0f,  1.0f
  	};

  	const glm::vec4 white(1);
  	const glm::vec4 black(0);
  	const glm::vec4 ambient( 0.1f, 0.1f, 0.1f, 1.0f );
	
  	glm::vec4 sunPos;
  	glm::vec3 orbit(60.0f, 0.0f, 0.0f);
  	glm::vec4 earthPos(0.0f, 0.0f, 0.0f,1.0f);
	
	Shader earthShader("Shader/earth_vs.glsl","Shader/earth_fs.glsl");
  	Shader sunShader("Shader/sun_vs.glsl","Shader/sun_fs.glsl");
  	Shader atmosphereShader("Shader/atmos_vs.glsl","Shader/atmos_fs.glsl");
  	Shader skyShader("Shader/sky_vs.glsl","Shader/sky_fs.glsl");

  	unsigned int loadTexture(const char *path);

  	unsigned int earth_tex= loadTexture("Texture/Albedo.jpg");
  	unsigned int earth_night_map=loadTexture("Texture/earth_nightmap.jpg");
  	unsigned int earth_height_map=loadTexture("Texture/heightmap.jpg");
  	unsigned int earth_normal_map=loadTexture("Texture/earth_normalmap.png");
  	unsigned int earth_spec_map=loadTexture("Texture/EarthSpec.png");

  	std::vector<std::string> faces= {
    		"Texture/skybox/right.png",
    		"Texture/skybox/left.png",
    		"Texture/skybox/top.png",
    		"Texture/skybox/bottom.png",
    		"Texture/skybox/front.png",
    		"Texture/skybox/back.png"
  	};

  	unsigned int cubemapTexture = loadCubemap(faces);   

  	earthShader.use();
  	earthShader.setInt("diffuseSampler", 0);
  	earthShader.setInt("nightSampler",1);
  	earthShader.setInt("bumpSampler",2);
  	earthShader.setInt("normalSampler",3);
  	earthShader.setInt("specularSampler",4);

  	unsigned int skyVAO, skyVBO;
  	glGenVertexArrays(1, &skyVAO);
  	glGenBuffers(1, &skyVBO);
  	glBindVertexArray(skyVAO);
  	glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
  	glBufferData(GL_ARRAY_BUFFER, sizeof(skyVertices), &skyVertices, GL_STATIC_DRAW);
  	glEnableVertexAttribArray(0);
  	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

  	Sphere sun(1.0f, 36, 18);

  	float g_fSunRotation = 0.0f;
  	const float fRotationRate3 = 20.0f;

  	GLuint sunVAO, sunVBO, sunIBO;
    
  	glGenVertexArrays(1, &sunVAO);
  	glBindVertexArray(sunVAO);
  	glGenBuffers(1, &sunVBO);
  	glBindBuffer(GL_ARRAY_BUFFER, sunVBO);
  	glBufferData(GL_ARRAY_BUFFER, sun.getInterleavedVertexSize(), sun.getInterleavedVertices(), GL_STATIC_DRAW);
  	
  	glEnableVertexAttribArray(0);
  	int stride = sun.getInterleavedStride();
  	glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, 0);

  	glGenBuffers(1, &sunIBO);
  	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sunIBO);
  	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sun.getIndexSize(), sun.getIndices(), GL_STATIC_DRAW);
 
  	float earthRadius = 30;
  	Sphere earth(earthRadius, 36, 18);
  	float maxHeight = earthRadius* 0.3;
  	GLuint earthVAO, earthVBO, earthIBO;

  	glGenVertexArrays(1, &earthVAO);
  	glBindVertexArray(earthVAO);
  	glGenBuffers(1, &earthVBO);
  	glBindBuffer(GL_ARRAY_BUFFER, earthVBO);
  	glBufferData(GL_ARRAY_BUFFER, earth.getInterleavedVertexSize(), earth.getInterleavedVertices(), GL_STATIC_DRAW);
  
  	glEnableVertexAttribArray(0);
  	glEnableVertexAttribArray(1);
  	glEnableVertexAttribArray(2);
  	stride = earth.getInterleavedStride();
  	glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, 0);
  	glVertexAttribPointer(1, 3, GL_FLOAT, false, stride, (void*)(3 * sizeof(float)));
  	glVertexAttribPointer(2, 2, GL_FLOAT, false, stride, (void*)(6 * sizeof(float)));
  	
 	glGenBuffers(1, &earthIBO);
  	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, earthIBO);
  	glBufferData(GL_ELEMENT_ARRAY_BUFFER, earth.getIndexSize(), earth.getIndices(), GL_STATIC_DRAW);

  	float atmosphereRadius = earthRadius * 1.10;
  	Sphere atmosphere(atmosphereRadius , 36, 18);
  	GLuint atmosphereVAO, atmosphereVBO, atmosphereIBO;
   
  	glGenVertexArrays(1, &atmosphereVAO);
 	glBindVertexArray(atmosphereVAO);
  	glGenBuffers(1, &atmosphereVBO);
  	glBindBuffer(GL_ARRAY_BUFFER, atmosphereVBO);
  	glBufferData(GL_ARRAY_BUFFER, atmosphere.getInterleavedVertexSize(), atmosphere.getInterleavedVertices(), GL_STATIC_DRAW);
  	
  	glEnableVertexAttribArray(0);
  	stride = atmosphere.getInterleavedStride();
  	glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, 0);
  	
  	glGenBuffers(1, &atmosphereIBO);
  	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, atmosphereIBO);
  	glBufferData(GL_ELEMENT_ARRAY_BUFFER, atmosphere.getIndexSize(), atmosphere.getIndices(), GL_STATIC_DRAW);

  	while (glfwWindowShouldClose(window) == 0) {
    	
    		float currentFrame = glfwGetTime();
    		deltaTime = currentFrame - lastFrame;
    		lastFrame = currentFrame;

    		fps();
		
    		processInput(window);
    		glfwSetScrollCallback(window, scroll_callback); 
    		glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
    		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    		//Matrices to be used
    		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    		glm::mat4 view = camera.GetViewMatrix();
    		glm::mat4 model = glm::mat4(1.0f);
	
    		sunShader.use();

    		if(sunRotate) {
	    		model=glm::translate(glm::rotate(model,glm::radians(g_fSunRotation), glm::vec3(0,-1,0) ) ,orbit);
			sunPos = model * earthPos;
    		}
    		else { 
	    		model = glm::mat4(1.0f);
	    		model=  glm::translate(model,glm::vec3(sunPos));
    		}

    		g_fSunRotation += fRotationRate3 * deltaTime;
    		g_fSunRotation = fmod(g_fSunRotation, 360.0f);

    		sunShader.setMat4("model", model);
    		sunShader.setMat4("projection", projection);
    		sunShader.setMat4("view", view);

    		GLCall(glBindVertexArray(sunVAO));
    		GLCall(glDrawElements(GL_TRIANGLES,sun.getIndexCount(),GL_UNSIGNED_INT, 0));               

    		earthShader.use();

    		model = glm::mat4(1.0f);
    		model = glm::translate(model, glm::vec3(earthPos));
      
    		float angle = -90.0f;
    		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
    		earthShader.setMat4("model", model);
    		earthShader.setMat4("projection", projection);
    		earthShader.setMat4("view", view);

    		glm::vec4 eyePosW = glm::vec4(camera.Position, 1 );

    		earthShader.setVec4("EyePosW",eyePosW);
    		earthShader.setVec4("LightPosW",sunPos);
    		earthShader.setVec4("LightColor",white);
    		earthShader.setVec4("MaterialEmissive",black);
    		earthShader.setVec4("MaterialDiffuse",white);
    		earthShader.setFloat("MaterialShininess",50.0f);
    		earthShader.setFloat("maxHeight",maxHeight);

    		glActiveTexture(GL_TEXTURE0);
    		glBindTexture(GL_TEXTURE_2D, earth_tex);
    		glActiveTexture(GL_TEXTURE1);
    		glBindTexture(GL_TEXTURE_2D, earth_night_map);
    		glActiveTexture(GL_TEXTURE2);
    		glBindTexture(GL_TEXTURE_2D, earth_height_map);
    		glActiveTexture(GL_TEXTURE3);
    		glBindTexture(GL_TEXTURE_2D, earth_normal_map);
    		glActiveTexture(GL_TEXTURE4);
    		glBindTexture(GL_TEXTURE_2D, earth_spec_map);
	
    		glBindVertexArray(earthVAO);
    		glDrawElements(GL_TRIANGLES,earth.getIndexCount(), GL_UNSIGNED_INT, 0);
    
    		glEnable(GL_BLEND);
    		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
    		atmosphereShader.use();
    		model = glm::mat4(1.0f);
    		model = glm::translate(model, glm::vec3(earthPos));
         	
    		atmosphereShader.setMat4("model", model);
    		atmosphereShader.setMat4("projection", projection);
    		atmosphereShader.setMat4("view", view);
    		atmosphereShader.setVec4("EyePosW",eyePosW);
    		atmosphereShader.setVec4("LightPosW",sunPos);
    		atmosphereShader.setFloat("inner_radius",earthRadius);
    		atmosphereShader.setFloat("outer_radius",atmosphereRadius);
		
    		glBindVertexArray(atmosphereVAO);
    		glDrawElements(GL_TRIANGLES,atmosphere.getIndexCount(), GL_UNSIGNED_INT, 0);
    		glDisable(GL_BLEND);

    		glDepthFunc(GL_LEQUAL);
    		skyShader.use();

    		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
    		skyShader.setMat4("view", view);
    		skyShader.setMat4("projection", projection);
    		glBindVertexArray(skyVAO);
    		glActiveTexture(GL_TEXTURE0);
    		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    		glDrawArrays(GL_TRIANGLES, 0, 36);
    		glBindVertexArray(0);
    		glDepthFunc(GL_LESS); // set depth function back to default
	
  		glfwSwapBuffers(window);
  		glfwPollEvents();
  	}

  	glfwTerminate();
  	return 0;
}
