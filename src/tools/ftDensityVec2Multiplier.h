
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftDensityVec2Multiplier : public ftShader {
	public:
		ftDensityVec2Multiplier() {}

		void setup() {

			ftShader::setup();

			ofLogVerbose( "init " );
			string glslVer = (char *)glGetString( GL_SHADING_LANGUAGE_VERSION );

			if( glslVer == "OpenGL ES GLSL ES 1.00" || glslVer == "OpenGL ES GLSL ES 2.00" )
				glESOne();
			else if( glslVer == "OpenGL ES GLSL ES 3.00" )
				glESThree();
			else if( !ofIsGLProgrammableRenderer() )
				glTwo();
			else if( ofIsGLProgrammableRenderer() )
				glThree();
		}
		
	protected:
		void glESOne()
		{
			fragmentShader = GLSLES100(

			uniform sampler2D Backbuffer;
			uniform sampler2D AddTexture;
			uniform float force;
			uniform vec2	Scale;

			varying vec4	texCoord;

			void main()
			{
				vec2 st = texCoord.st;
				vec2 st2 = st * Scale;

				vec4 color = texture2D( Backbuffer, st );
				float multiplier = length( texture2D( AddTexture, st2 ).xy ) * force;
				color.xyz *= 1.0 - min( multiplier, 0.9 );

				gl_FragColor = color;
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
				uniform sampler2D Backbuffer;
				uniform sampler2D AddTexture;
				uniform float force;
				uniform vec2	Scale;

				in vec2 texCoordVarying;
				out vec4 fragColor;

				void main()
				{
					vec2 st = texCoordVarying;
					vec2 st2 = st * Scale;

					vec4 color = texture( Backbuffer, st );
					float multiplier = length( texture( AddTexture, st2 ).xy ) * force;
					color.xyz *= 1.0 - min( multiplier, 0.9 );

					fragColor = color;
				}
			);


			shader.setupShaderFromSource( GL_VERTEX_SHADER, vertexShader );
			shader.setupShaderFromSource( GL_FRAGMENT_SHADER, fragmentShader );
			shader.bindDefaults();
			shader.linkProgram();
		}

		void glTwo() {
			fragmentShader = GLSL120(
								  uniform sampler2DRect Backbuffer;
								  uniform sampler2DRect AddTexture;
								  uniform float force;
								  uniform vec2	Scale;
								  
								  void main(){
									  vec2 st = gl_TexCoord[0].st;
									  vec2 st2 = st * Scale;
									  
									  vec4 color = texture2DRect(Backbuffer, st);
									  float multiplier = length(texture2DRect(AddTexture, st2).xy) * force;
									  color.xyz *= 1.0 - min(multiplier, 0.9);
									  
									  gl_FragColor = color ;
								  }
								  );
			
			shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			shader.linkProgram();

		}
		
		void glThree() {
			fragmentShader = GLSL150(
								  uniform sampler2DRect Backbuffer;
								  uniform sampler2DRect AddTexture;
								  uniform float force;
								  uniform vec2	Scale;
								  
								  in vec2 texCoordVarying;
								  out vec4 fragColor;
								  
								  void main(){
									  vec2 st = texCoordVarying;
									  vec2 st2 = st * Scale;
									  
									  vec4 color = texture(Backbuffer, st);
									  float multiplier = length(texture(AddTexture, st2).xy) * force;
									  color.xyz *= 1.0 - min(multiplier, 0.9);
									  
									  fragColor = color ;
								  }
								  );
			
			
			shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			shader.bindDefaults();
			shader.linkProgram();
		}
		
	public:
		
		void update(ofFbo& _buffer, ofTexture& _backBufferTexture, ofTexture& _addTexture, float _force){
			_buffer.begin();
			shader.begin();
			shader.setUniformTexture("Backbuffer", _backBufferTexture, 0);
			shader.setUniformTexture("AddTexture", _addTexture, 1);
			shader.setUniform1f("force", _force);
			shader.setUniform2f("Scale", _addTexture.getWidth() / _buffer.getWidth(), _addTexture.getHeight()/ _buffer.getHeight());
			renderFrame(_buffer.getWidth(), _buffer.getHeight());
			shader.end();
			_buffer.end();
		}
	};
}
