
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftInitPositionShader : public ftShader {
	public:
		ftInitPositionShader() {}

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
				ofLogNotice( "ftInitPositionShader initialized" );
			else
				ofLogWarning( "ftInitPositionShader failed to initialize" );
		}
		
	protected:
		void glESOne()
		{

			fragmentShader = GLSLES100(

			uniform vec2 Dimensions;
			varying vec4	texCoord;

			void main()
			{
				vec2 st = texCoord.st;

				gl_FragColor = vec4( st, 0.0, 1.0 );

			}
			);

            bInitialized *= shader.setupShaderFromSource( GL_FRAGMENT_SHADER, fragmentShader );
            bInitialized *= shader.setupShaderFromSource( GL_VERTEX_SHADER, vertexShader );
            bInitialized *= shader.bindDefaults();
            bInitialized *= shader.linkProgram();
		}

		void glESThree()
		{

            fragmentShader = GLSLES300
			(
				uniform vec2 Dimensions;

				in vec2 texCoordVarying;
				out vec4 fragColor;

				void main()
				{
					vec2 st = texCoordVarying;

					fragColor = vec4( st, 0.0, 1.0 );

				}
			);

			bInitialized *= shader.setupShaderFromSource( GL_VERTEX_SHADER, vertexShader );
			bInitialized *= shader.setupShaderFromSource( GL_FRAGMENT_SHADER, fragmentShader );
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
		}

		void glTwo() {
			
			fragmentShader = GLSL120(
								  uniform vec2 Dimensions;
								  void main(){
									  vec2 st = gl_TexCoord[0].st;
									  
									  gl_FragColor = vec4(st, 0.0, 1.0);
									  
								  }
								  );
			
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.linkProgram();
		}
		
		void glThree() {
			
			fragmentShader = GLSL150(
								  uniform vec2 Dimensions;
								  
								  in vec2 texCoordVarying;
								  out vec4 fragColor;
								  
								  void main(){
									  vec2 st = texCoordVarying;
									  
									  fragColor = vec4(st, 0.0, 1.0);
									  
								  }
								  );
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
		}
		
	public:
		
		
		void update(ofFbo& _buffer){
			_buffer.begin();
			shader.begin();
			shader.setUniform2f("Dimensions", _buffer.getWidth(), _buffer.getHeight());
			renderFrame(_buffer.getWidth(), _buffer.getHeight());
			shader.end();
			_buffer.end();

		}
	};
}
