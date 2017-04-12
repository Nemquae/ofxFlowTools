#pragma once

#include "ofMain.h"

#define GLSLES100(shader)  "#version 100 \n precision mediump float; \n precision mediump sampler2D; \n" #shader
#define GLSLES100PI(shader)  "#version 100 \n #define PI 3.1415926535897932 \n #define HALF_PI 1.5707963267948966 \n precision mediump float; \n precision mediump sampler2D; \n" #shader
#define GLSLES100GEO(shader)  "#version 100 \n #extension GL_EXT_geometry_shader4: enable \n precision mediump float; \n precision mediump sampler2D; \n" #shader

#define GLSLES300(shader)  "#version 300 es \n precision mediump float; \n precision mediump sampler2DRect; \n" #shader
#define GLSLES300PI(shader)  "#version 300 es \n #define PI 3.1415926535897932 \n #define HALF_PI 1.5707963267948966 \n precision mediump float; \n precision mediump sampler2D; \n" #shader
#define GLSLES300GEO(shader)  "#version 300 es \n #extension GL_EXT_geometry_shader4: enable \n precision mediump float; \n precision mediump sampler2D; \n" #shader

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
         
			if( glslVer == "OpenGL ES GLSL ES 1.00" || glslVer == "OpenGL ES GLSL ES 2.00" )
				glESOne();
			else if( glslVer == "OpenGL ES GLSL ES 3.00" )
				glESThree();
            else if( !ofIsGLProgrammableRenderer() )
				glTwo();
			else if( ofIsGLProgrammableRenderer() )
				glThree();

			quad.getVertices().resize( 4 );
			quad.getTexCoords().resize( 4 );
			quad.setMode( OF_PRIMITIVE_TRIANGLE_FAN );
		}
		
	protected:
        void glESOne()  {
            
            vertexShader = GLSLES100(
                                   attribute vec4 inVertex;
                                   attribute vec4 inColor;
                                   
                                   varying vec4 texCoord;
                                   
                                   void main() {
                                       gl_Position = inVertex;
                                       texCoord = inColor;
                                   }
                                   );
            
            
            fragmentShader = GLSLES100(
                                     void main()
                                     {
                                         gl_FragColor = vec4(0.0,1.0,0.0,1.0);
                                     }
                                     );
        }

		void glESThree()
		{
			vertexShader = GLSLES300(

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
				texCoordVarying = ( textureMatrix*vec4( texcoord.x, texcoord.y, 0, 1 ) ).xy;
				gl_Position = modelViewProjectionMatrix * position;
			}
			);

			fragmentShader = GLSLES300(

			out vec4 fragColor;

			void main()
			{
				fragColor = vec4( 0.0, 1.0, 0.0, 1.0 );
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
