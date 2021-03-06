
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftMoveParticleShader : public ftShader {
	public:
		ftMoveParticleShader() {}

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
				ofLogNotice( "ftMoveParticleShader initialized" );
			else
				ofLogWarning( "ftMoveParticleShader failed to initialize" );
		}
		
	protected:
		void glESOne()
		{

			fragmentShader = GLSLES100(

			uniform sampler2D Backbuffer;
			uniform sampler2D ALMSTexture;
			uniform sampler2D Velocity;
			uniform sampler2D HomeTexture;
			uniform float TimeStep;
			uniform float InverseCellSize;
			uniform vec2	Scale;
			uniform vec2	Dimensions;
			uniform vec2	Gravity;

			varying vec4	texCoord;

			void main()
			{
				vec2 st = texCoord.st;
				vec2 particlePos = texture2D( Backbuffer, st ).xy;

				vec4 alms = texture2D( ALMSTexture, st );
				float age = alms.x;
				float life = alms.y;
				float mass = alms.z;

				if( age > 0.0 )
				{
					vec2 st2 = particlePos * Scale;
					vec2 u = texture2D( Velocity, st2 ).rg / Scale;
					vec2 coord = TimeStep * InverseCellSize * u;

					particlePos += coord * ( 1.0 / mass ) + Gravity;
				}
				else
				{
					particlePos = texture2D( HomeTexture, st ).xy;
				}


				gl_FragColor = vec4( particlePos, 0.0, 1.0 ); ;
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
				uniform sampler2D ALMSTexture;
				uniform sampler2D Velocity;
				uniform sampler2D HomeTexture;
				uniform float TimeStep;
				uniform float InverseCellSize;
				uniform vec2	Scale;
				uniform vec2	Dimensions;
				uniform vec2	Gravity;

				in vec2 texCoordVarying;
				out vec4 fragColor;

				void main()
				{
					vec2 st = texCoordVarying;
					vec2 particlePos = texture( Backbuffer, st ).xy;

					vec4 alms = texture( ALMSTexture, st );
					float age = alms.x;
					float life = alms.y;
					float mass = alms.z;

					if( age > 0.0 )
					{
						vec2 st2 = particlePos * Scale;
						vec2 u = texture( Velocity, st2 ).rg / Scale;
						vec2 coord = TimeStep * InverseCellSize * u;

						particlePos += coord * ( 1.0 / mass ) + Gravity;
					}
					else
					{
						particlePos = texture( HomeTexture, st ).xy;
					}


					fragColor = vec4( particlePos, 0.0, 1.0 ); ;
				}
			);




			bInitialized *= shader.setupShaderFromSource( GL_VERTEX_SHADER, vertexShader );
			bInitialized *= shader.setupShaderFromSource( GL_FRAGMENT_SHADER, fragmentShader );
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
		}

		void glTwo() {
			
			fragmentShader = GLSL120(
								  uniform sampler2DRect Backbuffer;
								  uniform sampler2DRect ALMSTexture;
								  uniform sampler2DRect Velocity;
								  uniform sampler2DRect HomeTexture;
								  uniform float TimeStep;
								  uniform float InverseCellSize;
								  uniform vec2	Scale;
								  uniform vec2	Dimensions;
								  uniform vec2	Gravity;
								  
								  void main(){
									  vec2 st = gl_TexCoord[0].st;
									  vec2 particlePos = texture2DRect(Backbuffer, st).xy;
									  
									  vec4 alms = texture2DRect(ALMSTexture, st);
									  float age = alms.x;
									  float life = alms.y;
									  float mass = alms.z;
									  
									  if (age > 0.0) {
										  vec2 st2 = particlePos * Scale;
										  vec2 u = texture2DRect(Velocity, st2).rg / Scale;
										  vec2 coord =  TimeStep * InverseCellSize * u;
										  
										  particlePos += coord * (1.0 / mass) + Gravity;
									  }
									  else {
										  particlePos = texture2DRect(HomeTexture, st).xy;
									  }
									  
									  
									  gl_FragColor = vec4(particlePos, 0.0, 1.0); ;
								  }
								  );
			
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.linkProgram();
			
		}
		
		void glThree() {
			fragmentShader = GLSL150(
								  uniform sampler2DRect Backbuffer;
								  uniform sampler2DRect ALMSTexture;
								  uniform sampler2DRect Velocity;
								  uniform sampler2DRect HomeTexture;
								  uniform float TimeStep;
								  uniform float InverseCellSize;
								  uniform vec2	Scale;
								  uniform vec2	Dimensions;
								  uniform vec2	Gravity;
								  
								  in vec2 texCoordVarying;
								  out vec4 fragColor;
								  
								  void main(){
									  vec2 st = texCoordVarying;
									  vec2 particlePos = texture(Backbuffer, st).xy;
									  
									  vec4 alms = texture(ALMSTexture, st);
									  float age = alms.x;
									  float life = alms.y;
									  float mass = alms.z;
									  
									  if (age > 0.0) {
										  vec2 st2 = particlePos * Scale;
										  vec2 u = texture(Velocity, st2).rg / Scale;
										  vec2 coord =  TimeStep * InverseCellSize * u;
										  
										  particlePos += coord * (1.0 / mass) + Gravity;
									  }
									  else {
										  particlePos = texture(HomeTexture, st).xy;
									  }
									  
									  
									  fragColor = vec4(particlePos, 0.0, 1.0); ;
								  }
								  );
			
			
			
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
		}
		
	public:
		
		void update(ofFbo& _buffer, ofTexture& _backBufferTexture, ofTexture& _ALMSTexture, ofTexture& _velocityTexture, ofTexture& _homeTexture, float _timeStep, float _cellSize, ofVec2f _gravity){
			_buffer.begin();
			shader.begin();
			shader.setUniformTexture("Backbuffer", _backBufferTexture, 0);
			shader.setUniformTexture("ALMSTexture", _ALMSTexture, 1);
			shader.setUniformTexture("Velocity", _velocityTexture, 2);
			shader.setUniformTexture("HomeTexture", _homeTexture, 3);
			shader.setUniform1f("TimeStep", _timeStep);
			shader.setUniform1f("InverseCellSize", 1.0 / _cellSize);
			shader.setUniform2f("Gravity", _gravity.x * _timeStep, _gravity.y * _timeStep);
			shader.setUniform2f("Scale", _velocityTexture.getWidth() / _buffer.getWidth(), _velocityTexture.getHeight()/ _buffer.getHeight());
			shader.setUniform2f("Dimensions", _buffer.getWidth(), _buffer.getHeight());
			renderFrame(_buffer.getWidth(), _buffer.getHeight());
			shader.end();
			_buffer.end();
		}
	};
}
