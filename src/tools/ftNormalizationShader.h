
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftNormalizationShader : public ftShader {
	public:
		ftNormalizationShader() {}

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
				ofLogNotice( "ftNormalizationShader initialized" );
			else
				ofLogWarning( "ftNormalizationShader failed to initialize" );
		}
		
	protected:
		void glESOne()
		{
			fragmentShader = GLSLES100(

			uniform sampler2D Texture;
			uniform float Min;
			uniform float Range;
			uniform vec2	Scale;

			varying vec4	texCoord;

			void main()
			{
				vec2 st = texCoord.st;
				vec2 st2 = st * Scale;

				vec4 color = texture2D( Texture, st );

				float magnitude = length( color ) - Min;
				if( magnitude > 0.0 )
				{
					color = normalize( color ) * ( magnitude / Range );
				}
				else
				{
					color = vec4( 0.0 );
				}

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
				uniform sampler2D Texture;
				uniform float Min;
				uniform float Range;
				uniform vec2	Scale;

				in vec2 texCoordVarying;
				out vec4 fragColor;

				void main()
				{
					vec2 st = texCoordVarying;
					vec2 st2 = st * Scale;

					vec4 color = texture( Texture, st );

					float magnitude = length( color ) - Min;
					if( magnitude > 0.0 )
					{
						color = normalize( color ) * ( magnitude / Range );
					}
					else
					{
						color = vec4( 0.0 );
					}

					fragColor = color;
				}
			);

			bInitialized *= shader.setupShaderFromSource( GL_VERTEX_SHADER, vertexShader );
			bInitialized *= shader.setupShaderFromSource( GL_FRAGMENT_SHADER, fragmentShader );
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
		}

		void glTwo() {
			fragmentShader = GLSL120(
								  uniform sampler2DRect Texture;
								  uniform float Min;
								  uniform float Range;
								  uniform vec2	Scale;
								  
								  void main(){
									  vec2 st = gl_TexCoord[0].st;
									  vec2 st2 = st * Scale;
									  
									  vec4 color = texture2DRect(Texture, st);
									  
									  float magnitude = length(color) - Min;
									  if(magnitude > 0.0) {
										  color = normalize(color) * (magnitude / Range);
									  }
									  else {
										  color = vec4(0.0);
									  }
									  
									  gl_FragColor = color ;
								  }
								  
								  );
			
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.linkProgram();
		}
		
		void glThree() {
			fragmentShader = GLSL150(
								  uniform sampler2DRect Texture;
								  uniform float Min;
								  uniform float Range;
								  uniform vec2	Scale;
								  
								  in vec2 texCoordVarying;
								  out vec4 fragColor;
								  
								  void main(){
									  vec2 st = texCoordVarying;
									  vec2 st2 = st * Scale;
									  
									  vec4 color = texture(Texture, st);
									  
									  float magnitude = length(color) - Min;
									  if(magnitude > 0.0) {
										  color = normalize(color) * (magnitude / Range);
									  }
									  else {
										  color = vec4(0.0);
									  }
									  
									  fragColor = color ;
								  }
								  );
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
		}
		
	public:
		
		void update(ofFbo& _buffer, ofTexture& _texture, float _min, float _range){
			_buffer.begin();
			shader.begin();
			shader.setUniformTexture("Texture", _texture, 0);
			shader.setUniform1f("Min", _min);
			shader.setUniform1f("Range", _range);
			shader.setUniform2f("Scale", _texture.getWidth() / _buffer.getWidth(), _texture.getHeight()/ _buffer.getHeight());
			renderFrame(_buffer.getWidth(), _buffer.getHeight());
			shader.end();
			_buffer.end();
		}
	};
}
