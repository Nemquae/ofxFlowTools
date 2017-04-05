#pragma once

#include "ofMain.h"

#define GLSL100(shader)  "#version 100 \n #extension GL_ARB_texture_rectangle : enable \n" #shader
#define GLSL100PI(shader)  "#version 100 \n #extension GL_ARB_texture_rectangle : enable \n #define PI 3.1415926535897932 \n #define HALF_PI 1.5707963267948966 \n" #shader
#define GLSL100GEO(shader)  "#version 100 \n #extension GL_ARB_texture_rectangle : enable \n #extension GL_EXT_geometry_shader4: enable \n" #shader

#define GLSL120(shader)  "#version 120 \n #extension GL_ARB_texture_rectangle : enable \n" #shader
#define GLSL120PI(shader)  "#version 120 \n #extension GL_ARB_texture_rectangle : enable \n #define PI 3.1415926535897932 \n #define HALF_PI 1.5707963267948966 \n" #shader
#define GLSL120GEO(shader)  "#version 120 \n #extension GL_ARB_texture_rectangle : enable \n #extension GL_EXT_geometry_shader4: enable \n" #shader

#define GLSL150(shader)  "#version 150 \n" #shader
#define GLSL150PI(shader)  "#version 150 \n #define PI 3.1415926535897932 \n #define HALF_PI 1.5707963267948966 \n" #shader

namespace flowTools {
	
	class ftShader{
	public:
		ftShader() {}

		void setup() {
			bInitialized = false;
            
            string glslVer = (char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
            GLint majorVer = 0;
            GLint minorVer = 0;
            
            //glGetIntegerv(GL_MAJOR_VERSION, &majorVer);
            //glGetIntegerv(GL_MINOR_VERSION, &minorVer);

			if( glslVer == "OpenGL ES GLSL ES 1.00" )//|| (majorVer == 2 && minorVer == 0))
                glOne();
            else if( glslVer == "OpenGL ES GLSL ES 2.00" )//|| (majorVer >= 2 && majorVer < 3))
				glTwo();
			else if( ofIsGLProgrammableRenderer() )//|| (ofIsGLProgrammableRenderer() && majorVer >= 3))
				glThree();

			quad.getVertices().resize( 4 );
			quad.getTexCoords().resize( 4 );
			quad.setMode( OF_PRIMITIVE_TRIANGLE_FAN );
		}
		
	protected:
        void glOne()  {
            
            vertexShader = GLSL100(
                                   void main() {
                                       gl_Position = gl_Vertex;
                                       gl_FrontColor = gl_Color;
                                   }
                                   );
            
            
            fragmentShader = GLSL100(
                                     void main()
                                     {
                                         gl_FragColor = vec4(0.0,1.0,0.0,1.0);
                                     }
                                     );
        }
        
		void glTwo()  {
			
			vertexShader = GLSL120(
								   void main() {
									   gl_Position = gl_Vertex;
									   gl_FrontColor = gl_Color;
								   }
								   );
			
			
			fragmentShader = GLSL120(
									 void main()
									 {
										 gl_FragColor = vec4(0.0,1.0,0.0,1.0);
									 }
									 );
		}
		
		void glThree() {
			vertexShader = GLSL150(
								   uniform mat4 modelViewProjectionMatrix;
								   uniform mat4 textureMatrix;
								   
								   in vec4	position;
								   in vec2	texcoord;
								   in vec4	color;
								   
								   out vec2 texCoordVarying;
								   out vec4 colorVarying;
								   
								   void main()
								   {
									   colorVarying = color;
									   texCoordVarying = (textureMatrix*vec4(texcoord.x,texcoord.y,0,1)).xy;
									   gl_Position = modelViewProjectionMatrix * position;
								   }
								   );
			
			fragmentShader = GLSL150(
									 out vec4 fragColor;
									 
									 void main()
									 {
										 fragColor = vec4(0.0,1.0,0.0,1.0);
									 }
									 );
		}
		
		~ftShader() {shader.unload();};
		
	protected:
		
		void renderFrame(float _width, float _height){
			quad.setVertex(0, ofVec3f(0,0,0));
			quad.setVertex(1, ofVec3f(_width,0,0));
			quad.setVertex(2, ofVec3f(_width,_height,0));
			quad.setVertex(3, ofVec3f(0,_height,0));
			
			quad.setTexCoord(0, ofVec2f(0,0));
			quad.setTexCoord(1, ofVec2f(_width,0));
			quad.setTexCoord(2, ofVec2f(_width,_height));
			quad.setTexCoord(3, ofVec2f(0,_height));
			
			quad.draw();
		}
		
		void renderNormalizedFrame(float _width, float _height){
			quad.setVertex(0, ofVec3f(0,0,0));
			quad.setVertex(1, ofVec3f(_width,0,0));
			quad.setVertex(2, ofVec3f(_width,_height,0));
			quad.setVertex(3, ofVec3f(0,_height,0));
			
			quad.setTexCoord(0, ofVec2f(0,0));
			quad.setTexCoord(1, ofVec2f(1,0));
			quad.setTexCoord(2, ofVec2f(1,1));
			quad.setTexCoord(3, ofVec2f(0,1));
			
			quad.draw();
		}
		
		ofMesh		quad;
		ofShader	shader;
		string		fragmentShader;
		string		vertexShader;
		bool		bInitialized;
	};
	
}
