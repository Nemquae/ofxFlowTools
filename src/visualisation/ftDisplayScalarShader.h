
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftDisplayScalarShader : public ftShader {
	public:
		ftDisplayScalarShader() {}

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
				ofLogNotice( "ftDisplayScalarShader initialized" );
			else
				ofLogWarning( "ftDisplayScalarShader failed to initialize" );
		}
		
	protected:
		void glOne()
		{
			fragmentShader = GLSL100(

			uniform sampler2D FloatTexture;
			uniform float Scale;

			varying vec4	texCoord;

			void main()
			{
				vec4	velocity = texture2D( FloatTexture, texCoord.st );
				velocity.xyz *= vec3( Scale );
				velocity.w = pow( length( velocity.xyz ), 0.33 );
				velocity.xyz += vec3( 0.5 );
				gl_FragColor = velocity;
			}
			);

            bInitialized *= shader.setupShaderFromSource( GL_FRAGMENT_SHADER, fragmentShader );
            bInitialized *= shader.setupShaderFromSource( GL_VERTEX_SHADER, vertexShader );
            bInitialized *= shader.bindDefaults();
            bInitialized *= shader.linkProgram();
		}

		void glTwo() {
			fragmentShader = GLSL120(
								  uniform sampler2DRect FloatTexture;
									   uniform float Scale;
									   void main(){
										   vec4	velocity = texture2DRect(FloatTexture, gl_TexCoord[0].st);
										   velocity.xyz *= vec3(Scale);
										   velocity.w = pow(length(velocity.xyz), 0.33);
										   velocity.xyz += vec3(0.5);
										   gl_FragColor = velocity;
									   }
									   );
			
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.linkProgram();
		}
		
		void glThree() {
			
			fragmentShader = GLSL150(
								  uniform sampler2DRect FloatTexture;
								  uniform float Scale;
								  
								  in vec2 texCoordVarying;
								  out vec4 fragColor;
								  
								  void main(){
									  vec2 st = texCoordVarying;
									  vec4	velocity = texture(FloatTexture, st);
									  velocity.xyz *= vec3(Scale);
									  velocity.w = pow(length(velocity.xyz), 0.33);
									  velocity.xyz += vec3(0.5);
									  fragColor = velocity;
								  }
								  );
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
			
		}
	
	public:
		void update(ofFbo& _buffer, ofTexture& _floatTexture, float _scale){
			_buffer.begin();
			shader.begin();
			shader.setUniformTexture("FloatTexture", _floatTexture, 0);
			shader.setUniform1f("Scale", _scale);
			renderFrame(_buffer.getWidth(), _buffer.getHeight());
			shader.end();
			_buffer.end();
		}
	};
}
