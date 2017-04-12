
#pragma once

#include "ofMain.h"
#include "ftShader.h"

//#if defined(__APPPLE__)
//#include <OpenGLES/ES3/gl.h>
//#include <OpenGLES/ES3/glext.h>
//#endif

namespace flowTools {
	
	class ftDrawParticleShader : public ftShader {
	public:
		ftDrawParticleShader() {}

		void setup() {
			ftShader::setup();

			bInitialized = 1;

			string glslVer = (char *)glGetString( GL_SHADING_LANGUAGE_VERSION );

			if( glslVer == "OpenGL ES GLSL ES 1.00" || glslVer == "OpenGL ES GLSL ES 2.00" )
				glESOne();
			else if( glslVer == "OpenGL ES GLSL ES 3.00" )
				glESThree();
			else if( !ofIsGLProgrammableRenderer() )
				glTwo();
			else if( ofIsGLProgrammableRenderer() )
				glThree();

			if( bInitialized )
				ofLogNotice( "ftDrawParticleShader initialized" );
			else
				ofLogWarning( "ftDrawParticleShader failed to initialize" );
		}
		
	protected:
		void glESOne()
		{
			vertexShader = GLSLES100(

			uniform sampler2D positionTexture;
			uniform sampler2D ALMSTexture;
			uniform float TwinkleSpeed;
			uniform vec4 Color;
            uniform mat4 mvpMatrix;

            varying vec4    vertCoord;

			void main()
			{

				vec2 st = vertCoord.xy;

				vec2 texPos = texture2D( positionTexture, st ).xy;
				gl_Position = mvpMatrix * vec4( texPos, 0.0, 1.0 );
				vec4 alms = texture2D( ALMSTexture, st );
				float age = alms.x;
				float life = alms.y;
				float mass = alms.z;
				float size = alms.w;
				gl_PointSize = size;

				float alpha = min( 0.5 - ( age / life ) * 0.5, age * 5. );
				alpha *= 0.5 + ( cos( ( age + size ) * TwinkleSpeed * mass ) + 1.0 ) * 0.5;
				alpha = max( alpha, 0.0 );

				gl_FrontColor = vec4( Color, alpha );

			}
			);
            
            ofLogWarning( "Point Sprites not supported by GLSL ES 1.0");


			//bInitialized *= shader.setupShaderFromSource( GL_VERTEX_SHADER, vertexShader );
            //bInitialized *= shader.setupShaderFromSource( GL_FRAGMENT_SHADER, fragmentShader );
            //bInitialized *= shader.bindDefaults();
			//bInitialized *= shader.linkProgram();


		}

		void glESThree()
		{
			vertexShader = GLSLES300(

			uniform	mat4 modelViewProjectionMatrix;
			uniform	mat4 textureMatrix;
			uniform	sampler2DRect PositionTexture;
			uniform	sampler2DRect ALMSTexture;
			uniform	vec3 Color;

			in vec4	position;
			in vec2	texcoord;
			in vec4	color;

			uniform float TwinkleSpeed;

			out vec2 texCoordVarying;
			out vec4 colorVarying;

			void main()
			{
				colorVarying = color;
				texCoordVarying = ( textureMatrix*vec4( texcoord.x, texcoord.y, 0, 1 ) ).xy;


				vec2 st = position.xy;
				vec2 texPos = texture( PositionTexture, st ).xy;
				gl_Position = modelViewProjectionMatrix * vec4( texPos, 0.0, 1.0 );

				vec4 alms = texture( ALMSTexture, st );
				float age = alms.x;
				float life = alms.y;
				float mass = alms.z;
				float size = alms.w;
				gl_PointSize = size;

				float alpha = min( 0.5 - ( age / life ) * 0.5, age * 5. );
				alpha *= 0.5 + ( cos( ( age + size ) * TwinkleSpeed * mass ) + 1.0 ) * 0.5;
				alpha = max( alpha, 0.0 );

				colorVarying = vec4( Color, alpha );
			}
			);



			// thanx to: http://mmmovania.blogspot.nl/2010/12/circular-point-sprites-in-opengl-33.html

			fragmentShader = GLSLES300(

			in vec4 colorVarying;
			out vec4 fragColor;

			void main()
			{
				vec2 p = gl_PointCoord * 2.0 - vec2( 1.0 );
				float d = dot( p, p );
				float r = sqrt( d );

				if( d > r )
					discard;
				else
					fragColor = colorVarying * ( 1.0, 1.0, 1.0, 1.0 - pow( r, 2.5 ) );
			}
			);

			bInitialized *= shader.setupShaderFromSource( GL_VERTEX_SHADER, vertexShader );
			bInitialized *= shader.setupShaderFromSource( GL_FRAGMENT_SHADER, fragmentShader );
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
		}

		void glTwo() {
			vertexShader = GLSL120(
								   uniform sampler2DRect positionTexture;
								   uniform sampler2DRect ALMSTexture;
								   uniform float TwinkleSpeed;
								   uniform vec4 Color;
								   
								   void main(){
									   
									   vec2 st = gl_Vertex.xy;
									   
									   vec2 texPos = texture2DRect(positionTexture, st).xy;
									   gl_Position = gl_ModelViewProjectionMatrix * vec4(texPos, 0.0, 1.0);
									   vec4 alms = texture2DRect(ALMSTexture, st);
									   float age = alms.x;
									   float life = alms.y;
									   float mass = alms.z;
									   float size = alms.w;
									   gl_PointSize = size;
									   
									   float alpha = min (0.5 - (age / life) * 0.5,age * 5.);
									   alpha *= 0.5 + (cos((age + size) * TwinkleSpeed * mass) + 1.0) * 0.5;
									   alpha = max(alpha, 0.0);
									   
									   gl_FrontColor = vec4(Color, alpha);
									   
								   }
								   );
			
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.linkProgram();

			
		}
		
		void glThree() {
			vertexShader = GLSL150(
								   uniform	mat4 modelViewProjectionMatrix;
								   uniform	mat4 textureMatrix;
								   uniform	sampler2DRect PositionTexture;
								   uniform	sampler2DRect ALMSTexture;
								   uniform	vec3 Color;
								   
								   in vec4	position;
								   in vec2	texcoord;
								   in vec4	color;
								   
								   uniform float TwinkleSpeed;
								   
								   out vec2 texCoordVarying;
								   out vec4 colorVarying;
								   
								   void main()
								   {
									   colorVarying = color;
									   texCoordVarying = (textureMatrix*vec4(texcoord.x,texcoord.y,0,1)).xy;
									   
									   
									   vec2 st = position.xy;
									   vec2 texPos = texture(PositionTexture, st).xy;
									   gl_Position = modelViewProjectionMatrix * vec4(texPos, 0.0, 1.0);
									   
									   vec4 alms = texture(ALMSTexture, st);
									   float age = alms.x;
									   float life = alms.y;
									   float mass = alms.z;
									   float size = alms.w;
									   gl_PointSize = size;
									   
									   float alpha = min (0.5 - (age / life) * 0.5,age * 5.);
									   alpha *= 0.5 + (cos((age + size) * TwinkleSpeed * mass) + 1.0) * 0.5;
									   alpha = max(alpha, 0.0);
									   
									   colorVarying = vec4(Color, alpha);
								   }
								);
			
			
			
			// thanx to: http://mmmovania.blogspot.nl/2010/12/circular-point-sprites-in-opengl-33.html
			
			fragmentShader = GLSL150(
								  in vec4 colorVarying;
								  out vec4 fragColor;
								  
								  void main()
								  {
									  vec2 p = gl_PointCoord * 2.0 - vec2(1.0);
									  float d = dot(p,p);
									  float r = sqrt(d);
									  
									  if(d > r)
										  discard;
									  else
										  fragColor = colorVarying * (1.0, 1.0, 1.0, 1.0 - pow(r, 2.5));
								  }
								  );
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
		}
		
	public:
		
		void update(ofVboMesh &particleVbo, int _numParticles, ofTexture& _positionTexture, ofTexture& _ALMSTexture, float _twinkleSpeed, ofVec3f _color){
            
#ifdef DEBUG_GL_ERRORS
            std::stringstream ss;
            ss << "GL Error 13 = " << glGetError() << std::endl;
            ofLogNotice(ss.str());
#endif // DEBUG_GL_ERRORS

            
			shader.begin();
			shader.setUniformTexture("PositionTexture", _positionTexture, 0);
			shader.setUniformTexture("ALMSTexture", _ALMSTexture, 1);
			shader.setUniform1f("TwinkleSpeed", _twinkleSpeed);
			shader.setUniform3f("Color", _color);
            
#ifdef DEBUG_GL_ERRORS
            ss.clear();
            ss << "GL Error 14 = " << glGetError() << std::endl;
            //ss << gluErrorString() << std::endl;
            ofLogNotice(ss.str());
#endif // DEBUG_GL_ERRORS

			
			bool dinges = true;
			//glEnable(GL_POINT_SMOOTH);
            #if !((TARGET_OS_IPHONE_SIMULATOR) || (TARGET_OS_IPHONE) || (TARGET_IPHONE) || (TARGET_IOS))
			glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
            #else
            //glEnable(GL_BLEND);
            #endif
            
#ifdef DEBUG_GL_ERRORS
            ss.clear();
            ss << "GL Error 15a = " << glGetError() << std::endl;
            ofLogNotice(ss.str());
#endif // DEBUG_GL_ERRORS

			
			particleVbo.draw();

#ifdef DEBUG_GL_ERRORS
			ss.clear();
			ss << "GL Error 15b = " << glGetError() << std::endl;
			ofLogNotice( ss.str() );
#endif // DEBUG_GL_ERRORS

			
            #if !((TARGET_OS_IPHONE_SIMULATOR) || (TARGET_OS_IPHONE) || (TARGET_IPHONE) || (TARGET_IOS))
			glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);

            #else
            //glDisable(GL_BLEND);
            #endif

#ifdef DEBUG_GL_ERRORS
			ss.clear();
			ss << "GL Error 15c = " << glGetError() << std::endl;
			ofLogNotice( ss.str() );
#endif // DEBUG_GL_ERRORS


			//glDisable(GL_POINT_SMOOTH);
			shader.end();
            
#ifdef DEBUG_GL_ERRORS
            ss.clear();
            ss << "GL Error 16 = " << glGetError() << std::endl;
            ofLogNotice(ss.str());
#endif // DEBUG_GL_ERRORS

			
		}
	};
}
