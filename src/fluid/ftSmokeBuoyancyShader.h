
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {

	class ftSmokeBuoyancyShader : public ftShader {
	public:
		ftSmokeBuoyancyShader() {}

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
				ofLogNotice( "ftSmokeBuoyancyShader initialized" );
			else
				ofLogWarning( "ftSmokeBuoyancyShader failed to initialize" );
		}
		
	protected:
		void glESOne()
		{
			fragmentShader = GLSLES100(

			uniform sampler2D Velocity;
			uniform sampler2D Temperature;
			uniform sampler2D Density;

			uniform float AmbientTemperature;
			uniform float TimeStep;
			uniform float Sigma;
			uniform float Kappa;

			uniform vec2  Gravity;

			varying vec4	texCoord;

			void main()
			{
				vec2 st = texCoord.st;

				float T = texture2D( Temperature, st ).r;
				vec2 V = texture2D( Velocity, st ).rg;

				//   gl_FragColor = vec4(0);
				//   if (T > AmbientTemperature) {
				float D = length( texture2D( Density, st ).rgb );
				gl_FragColor = vec4( ( TimeStep * ( T - AmbientTemperature ) * Sigma - D * Kappa ) * Gravity, 0.0, 0.0 );
				//   }
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
				uniform sampler2D Velocity;
				uniform sampler2D Temperature;
				uniform sampler2D Density;

				uniform float AmbientTemperature;
				uniform float TimeStep;
				uniform float Sigma;
				uniform float Kappa;

				uniform vec2  Gravity;

				in vec2 texCoordVarying;
				out vec4 fragColor;

				void main()
				{
					vec2 st = texCoordVarying;

					float T = texture( Temperature, st ).r;
					vec2 V = texture( Velocity, st ).rg;

					//   gl_FragColor = vec4(0);
					//   if (T > AmbientTemperature) {
					float D = length( texture( Density, st ).rgb );
					fragColor = vec4( ( TimeStep * ( T - AmbientTemperature ) * Sigma - D * Kappa ) * Gravity, 0.0, 0.0 );
					//   }
				}

			);


			bInitialized *= shader.setupShaderFromSource( GL_VERTEX_SHADER, vertexShader );
			bInitialized *= shader.setupShaderFromSource( GL_FRAGMENT_SHADER, fragmentShader );
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
		}

		void glTwo() {
			fragmentShader = GLSL120(
								  uniform sampler2DRect Velocity;
								  uniform sampler2DRect Temperature;
								  uniform sampler2DRect Density;
								  
								  uniform float AmbientTemperature;
								  uniform float TimeStep;
								  uniform float Sigma;
								  uniform float Kappa;
								  
								  uniform vec2  Gravity;
								  
								  void main(){
									  vec2 st = gl_TexCoord[0].st;
									  
									  float T = texture2DRect(Temperature, st).r;
									  vec2 V = texture2DRect(Velocity, st).rg;
									  
									  //   gl_FragColor = vec4(0);
									  //   if (T > AmbientTemperature) {
									  float D = length(texture2DRect(Density, st).rgb);
									  gl_FragColor = vec4((TimeStep * (T - AmbientTemperature) * Sigma - D * Kappa ) * Gravity, 0.0, 0.0);
									  //   }
								  }
								  
								  );
			
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.linkProgram();
		}
		
		void glThree() {
			fragmentShader = GLSL150(
								  uniform sampler2DRect Velocity;
								  uniform sampler2DRect Temperature;
								  uniform sampler2DRect Density;
								  
								  uniform float AmbientTemperature;
								  uniform float TimeStep;
								  uniform float Sigma;
								  uniform float Kappa;
								  
								  uniform vec2  Gravity;
								  
								  in vec2 texCoordVarying;
								  out vec4 fragColor;
								  
								  void main(){
									  vec2 st = texCoordVarying;
									  
									  float T = texture(Temperature, st).r;
									  vec2 V = texture(Velocity, st).rg;
									  
									  //   gl_FragColor = vec4(0);
									  //   if (T > AmbientTemperature) {
									  float D = length(texture(Density, st).rgb);
									  fragColor = vec4((TimeStep * (T - AmbientTemperature) * Sigma - D * Kappa ) * Gravity, 0.0, 0.0);
									  //   }
								  }
								  
								  );
			
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
		}
		
	public:
		
		void update(ofFbo& _buffer, ofTexture& _velocityTexture, ofTexture& _temperatureTexture, ofTexture _colorTexture, float _ambientTemperature, float _timeStep, float _smokeBuoyancy, float _smokeWeight, ofVec2f _gForce){
			
			_buffer.begin();
			shader.begin();
			shader.setUniform1f("AmbientTemperature", _ambientTemperature);
			shader.setUniform1f("TimeStep", _timeStep);
			shader.setUniform1f("Sigma", _smokeBuoyancy);
			shader.setUniform1f("Kappa", _smokeWeight);
			
			shader.setUniform2f("Gravity", _gForce.x, _gForce.y );
			
			shader.setUniformTexture("Velocity", _velocityTexture, 0);
			shader.setUniformTexture("Temperature", _temperatureTexture, 1);
			shader.setUniformTexture("Density", _colorTexture, 2);
			
			renderFrame(_buffer.getWidth(), _buffer.getHeight());
			
			shader.end();
			_buffer.end();
		}

		
		
	private:
	};
}
