
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftSvFromVelocityShader : public ftShader {
	public:
		ftSvFromVelocityShader() {}

		void setup() {
			ftShader::setup();

			bInitialized = 1;

			string glslVer = (char *)glGetString( GL_SHADING_LANGUAGE_VERSION );

			if( glslVer == "OpenGL ES GLSL ES 1.00" )
				glOne();
			else if( glslVer == "OpenGL ES GLSL ES 2.00" )
				glTwo();
			else if( ofIsGLProgrammableRenderer() )
				glThree();

			if( bInitialized )
				ofLogNotice( "ftSvShader initialized" );
			else
				ofLogWarning( "ftSvShader failed to initialize" );
		}
		
	protected:
		void glOne()
		{
			fragmentShader = GLSL100(

			uniform sampler2D VelocityTexture;
			uniform float Force;
			uniform vec2	Scale;

			varying vec4	texCoord;

			void main()
			{
				vec2 st = texCoord.st;
				vec2 st2 = st * Scale;

				vec2 velocity = texture2D( VelocityTexture, st2 ).xy * Force;
				vec2 pVel = max( velocity, vec2( 0, 0 ) );
				vec2 nVel = abs( min( velocity, vec2( 0, 0 ) ) );

				gl_FragColor = vec4( pVel, nVel );
			}

			);

            bInitialized *= shader.setupShaderFromSource( GL_FRAGMENT_SHADER, fragmentShader );
            bInitialized *= shader.setupShaderFromSource( GL_VERTEX_SHADER, vertexShader );
            bInitialized *= shader.bindDefaults();
            bInitialized *= shader.linkProgram();
		}

		void glTwo() {
			fragmentShader = GLSL120(
								  uniform sampler2DRect VelocityTexture;
								  uniform float Force;
								  uniform vec2	Scale;
								  
								  void main(){
									  vec2 st = gl_TexCoord[0].st;
									  vec2 st2 = st * Scale;
									  
									  vec2 velocity = texture2DRect(VelocityTexture, st2).xy * Force;
									  vec2 pVel = max(velocity, vec2(0,0));
									  vec2 nVel = abs(min(velocity, vec2(0,0)));
									  
									  gl_FragColor = vec4(pVel, nVel);
								  }
								  
								  );
			
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.linkProgram();
		}
		
		void glThree() {
			fragmentShader = GLSL150(
								  uniform sampler2DRect VelocityTexture;
								  uniform float Force;
								  uniform vec2	Scale;
								  
								  in vec2 texCoordVarying;
								  out vec4 fragColor;
								  
								  void main(){
									  vec2 st = texCoordVarying;
									  vec2 st2 = st * Scale;
									  
									  vec2 velocity = texture(VelocityTexture, st2).xy * Force;
									  vec2 pVel = max(velocity, vec2(0,0));
									  vec2 nVel = abs(min(velocity, vec2(0,0)));
									  fragColor = vec4(pVel, nVel);
								  }
								  );
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
		}
		
	public:
		
		void update(ofFbo& _buffer, ofTexture& _velocityTexture, float _force){
			_buffer.begin();
			shader.begin();
			shader.setUniformTexture("VelocityTexture", _velocityTexture, 0);
			shader.setUniform1f("Force", _force);
			shader.setUniform2f("Scale", _velocityTexture.getWidth() / _buffer.getWidth(), _velocityTexture.getHeight()/ _buffer.getHeight());
			renderFrame(_buffer.getWidth(), _buffer.getHeight());
			shader.end();
			_buffer.end();
		}
	};
}
