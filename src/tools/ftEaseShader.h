
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftEaseShader : public ftShader {
	public:
		ftEaseShader() {}

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
				ofLogNotice( "ftEaseShader initialized" );
			else
				ofLogWarning( "ftEaseShader failed to initialize" );
		}
		
	protected:
		void glOne()
		{
			fragmentShader = GLSL100(

			uniform sampler2D RedTexture;
			uniform vec2	Scale;

			varying vec4	texCoord;

			void main()
			{
				vec2 st = gl_TexCoord[ 0 ].st;
				vec2 st2 = st * Scale;

				vec4 color = texture2D( RedTexture, st );
				gl_FragColor = vec4( color.x, color.x, color.x, 1.0 );
			}

			);

            bInitialized *= shader.setupShaderFromSource( GL_FRAGMENT_SHADER, fragmentShader );
            bInitialized *= shader.setupShaderFromSource( GL_VERTEX_SHADER, vertexShader );
            bInitialized *= shader.bindDefaults();
            bInitialized *= shader.linkProgram();
		}

		void glTwo() {
			fragmentShader = GLSL120(
								  uniform sampler2DRect RedTexture;
								  uniform vec2	Scale;
								  
								  void main(){
									  vec2 st = gl_TexCoord[0].st;
									  vec2 st2 = st * Scale;
									  
									  vec4 color = texture2DRect(RedTexture, st);
									  gl_FragColor = vec4(color.x, color.x, color.x, 1.0);
								  }
								  
								  );
			
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.linkProgram();
		}
		
		void glThree() {
			fragmentShader = GLSL150(
									 uniform sampler2DRect RedTexture;
									 uniform vec2	Scale;
									 
									 float linear(float t)		{ return t; }
									 float cubicIn(float t)		{ return t * t * t; }
									 float cubicOut(float t)	{ float f = t - 1.0; return f * f * f + 1.0; }
									 float cubicInOut(float t)	{ return t < 0.5 ? 4.0 * t * t * t : 0.5 * pow(2.0 * t - 2.0, 3.0) + 1.0; }
									 float circularIn(float t)	{ return 1.0 - sqrt(1.0 - t * t); }
									 float circularOut(float t)	{ return sqrt((2.0 - t) * t); }
									 float circularInOut(float t) { return t < 0.5 ? 0.5 * (1.0 - sqrt(1.0 - 4.0 * t * t)) : 0.5 * (sqrt((3.0 - 2.0 * t) * (2.0 * t - 1.0)) + 1.0); }
									 float exponentialIn(float t) { return t == 0.0 ? t : pow(2.0, 10.0 * (t - 1.0)); }
									 float exponentialOut(float t) { return t == 1.0 ? t : 1.0 - pow(2.0, -10.0 * t); }
									 float exponentialInOut(float t) { return t == 0.0 || t == 1.0 ? t : t < 0.5 ? +0.5 * pow(2.0, (20.0 * t) - 10.0) : -0.5 * pow(2.0, 10.0 - (t * 20.0)) + 1.0; }
									 float quadraticIn(float t)	{ return t * t; }
									 float quadraticOut(float t){ return -t * (t - 2.0); }
									 float quadraticInOut(float t) { float p = 2.0 * t * t; return t < 0.5 ? p : -p + (4.0 * t) - 1.0; }
									 float quarticIn(float t)	{ return pow(t, 4.0); }
									 float quarticOut(float t)	{ return pow(t - 1.0, 3.0) * (1.0 - t) + 1.0; }
									 float quarticInOut(float t){ return t < 0.5 ? +8.0 * pow(t, 4.0) : -8.0 * pow(t - 1.0, 4.0) + 1.0; }
									 float qinticIn(float t)	{ return pow(t, 5.0); }
									 float qinticOut(float t)	{ return 1.0 - (pow(t - 1.0, 5.0)); }
									 float qinticInOut(float t)	{ return t < 0.5 ? +16.0 * pow(t, 5.0) : -0.5 * pow(2.0 * t - 2.0, 5.0) + 1.0; }
									 float sineIn(float t)		{ return sin((t - 1.0) * HALF_PI) + 1.0; }
									 float sineOut(float t)		{ return sin(t * HALF_PI); }
									 float sineInOut(float t)	{ return -0.5 * (cos(PI * t) - 1.0); }
									 float elasticIn(float t)	{ return sin(13.0 * t * HALF_PI) * pow(2.0, 10.0 * (t - 1.0)); }
									 float elasticOut(float t)  { return sin(-13.0 * (t + 1.0) * HALF_PI) * pow(2.0, -10.0 * t) + 1.0; }
									 float elasticInOut(float t) { return t < 0.5 ? 0.5 * sin(+13.0 * HALF_PI * 2.0 * t) * pow(2.0, 10.0 * (2.0 * t - 1.0)) : 0.5 * sin(-13.0 * HALF_PI * ((2.0 * t - 1.0) + 1.0)) * pow(2.0, -10.0 * (2.0 * t - 1.0)) + 1.0; }
									 float backIn(float t)		{ return pow(t, 3.0) - t * sin(t * PI); }
									 float backOut(float t)		{ float f = 1.0 - t; return 1.0 - (pow(f, 3.0) - f * sin(f * PI)); }
									 float backInOut(float t)	{ float f = t < 0.5 ? 2.0 * t : 1.0 - (2.0 * t - 1.0); float g = pow(f, 3.0) - f * sin(f * PI); return t < 0.5 ? 0.5 * g : 0.5 * (1.0 - g) + 0.5; }
									 
									 float bounceOut(float t) {
										 const float a = 4.0 / 11.0;
										 const float b = 8.0 / 11.0;
										 const float c = 9.0 / 10.0;
										 
										 const float ca = 4356.0 / 361.0;
										 const float cb = 35442.0 / 1805.0;
										 const float cc = 16061.0 / 1805.0;
										 
										 float t2 = t * t;
										 
										 return t < a
										 ? 7.5625 * t2
										 : t < b
										 ? 9.075 * t2 - 9.9 * t + 3.4
										 : t < c
										 ? ca * t2 - cb * t + cc
										 : 10.8 * t * t - 20.52 * t + 10.72;
									 }
									 float bounceIn(float t)	{ return 1.0 - bounceOut(1.0 - t); }
									 float bounceInOut(float t) {
										 return t < 0.5
										 ? 0.5 * (1.0 - bounceOut(1.0 - t * 2.0))
										 : 0.5 * bounceOut(t * 2.0 - 1.0) + 0.5;
									 }

								  in vec2 texCoordVarying;
								  out vec4 fragColor;
								  
								  void main(){
									  vec2 st = texCoordVarying;
									  vec2 st2 = st * Scale;
									  
									  vec4 color = texture(RedTexture, st);
									  fragColor = vec4(color.x, color.x, color.x, 1.0);
								  }
								  );
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
		}
		
	public:
		
		void update(ofFbo& _buffer, ofTexture& _redTexture){
			_buffer.begin();
			shader.begin();
			shader.setUniformTexture("RedTexture", _redTexture, 0);
			shader.setUniform2f("Scale", _redTexture.getWidth() / _buffer.getWidth(), _redTexture.getHeight()/ _buffer.getHeight());
			renderFrame(_buffer.getWidth(), _buffer.getHeight());
			shader.end();
			_buffer.end();
		}
	};
}
