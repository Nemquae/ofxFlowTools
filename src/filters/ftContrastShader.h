
#pragma once
#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {

	class ftContrastShader : public ftShader {
	public:
		ftContrastShader(){}

		void setup()
		{
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
				ofLogNotice( "ftContrastShader initialized" );
			else
				ofLogWarning( "ftContrastShader failed to initialize" );
		}
		
	protected:

		void glESOne()
		{
			fragmentShader = GLSLES100(

			uniform sampler2D tex0;
			uniform float contrast;
			uniform float brightness;

			varying vec4	texCoord;

			void main()
			{
				vec4 color = texture2D( tex0, texCoord.st );
				float alpha = color.a;
				float p = 0.3 *color.g + 0.59*color.r + 0.11*color.b;
				p = p * brightness;
				color = vec4( p, p, p, 1.0 );
				color = mix( vec4( 1.0, 1.0, 1.0, 1.0 ), color, contrast );

				gl_FragColor = vec4( color.r, color.g, color.b, alpha );
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
				uniform sampler2D tex0;
				uniform float contrast;
				uniform float brightness;

				in vec2 texCoordVarying;
				out vec4 fragColor;

				void main()
				{
					vec4 color = texture( tex0, texCoordVarying );
					float alpha = color.a;
					float p = 0.3 *color.g + 0.59*color.r + 0.11*color.b;
					p = p * brightness;
					color = vec4( p, p, p, 1.0 );
					color = mix( vec4( 1.0, 1.0, 1.0, 1.0 ), color, contrast );

					fragColor = vec4( color.r, color.g, color.b, alpha );
				}
			);

			bInitialized *= shader.setupShaderFromSource( GL_VERTEX_SHADER, vertexShader );
			bInitialized *= shader.setupShaderFromSource( GL_FRAGMENT_SHADER, fragmentShader );
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();

		}

		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect tex0;
									 uniform float contrast;
									 uniform float brightness;
									 
									 void main(){
										 vec4 color = texture2DRect(tex0,gl_TexCoord[0].st);
										 float alpha = color.a;
										 float p = 0.3 *color.g + 0.59*color.r + 0.11*color.b;
										 p = p * brightness;
										 color = vec4(p,p,p,1.0);
										 color = mix( vec4(1.0,1.0,1.0,1.0),color,contrast);
										 
										 gl_FragColor =  vec4(color.r , color.g, color.b, alpha);
									 }
									 );
			
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.linkProgram();
			
		}
		
		void glThree() {
			
			fragmentShader = GLSL150(
									 uniform sampler2DRect tex0;
									 uniform float contrast;
									 uniform float brightness;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main(){
										 vec4 color = texture(tex0, texCoordVarying);
										 float alpha = color.a;
										 float p = 0.3 *color.g + 0.59*color.r + 0.11*color.b;
										 p = p * brightness;
										 color = vec4(p,p,p,1.0);
										 color = mix( vec4(1.0,1.0,1.0,1.0),color,contrast);
										 
										 fragColor =  vec4(color.r , color.g, color.b, alpha);
									 }
									 );
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
			
		}
		
	public:
		void update(ofFbo& _drawBuffer, ofTexture& _srcTexture, float _contrast, float _brightness){
			_drawBuffer.begin();
			shader.begin();
			shader.setUniformTexture( "tex0" , _srcTexture, 0 );
			shader.setUniform1f("contrast", _contrast);
			shader.setUniform1f("brightness", _brightness);
			renderFrame(_drawBuffer.getWidth(), _drawBuffer.getHeight());
			shader.end();
			_drawBuffer.end();
		}
	};
}
