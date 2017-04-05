
#pragma once
#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftLuminanceShader : public ftShader {
	public:
		ftLuminanceShader(){}

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
				ofLogNotice( "ftLuminanceShader initialized" );
			else
				ofLogWarning( "ftLuminanceShader failed to initialize" );
		}
		
	protected:
		void glOne()
		{
			fragmentShader = GLSL100(
				uniform sampler2DRect tex0;

			void main()
			{
				vec4 color = texture2DRect( tex0, gl_TexCoord[ 0 ].st );
				float p = 0.3 *color.g + 0.59*color.r + 0.11*color.b;
				color.xyz = vec3( p, p, p );
				gl_FragColor = color;
			}
			);

			bInitialized *= shader.setupShaderFromSource( GL_FRAGMENT_SHADER, fragmentShader );
			bInitialized *= shader.linkProgram();
		}

		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect tex0;
									 
									 void main(){
										 vec4 color = texture2DRect(tex0,gl_TexCoord[0].st);
										 float p = 0.3 *color.g + 0.59*color.r + 0.11*color.b;
										 color.xyz = vec3(p,p,p);
										 gl_FragColor =  color;
									 }
									 );
			
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.linkProgram();
		}
		
		void glThree() {
			fragmentShader = GLSL150(
									 uniform sampler2DRect tex0;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main(){
										 vec4 color = texture(tex0,texCoordVarying);
										 float p = 0.3 * color.g + 0.59 * color.r + 0.11 * color.b;
										 color.xyz = vec3(p,p,p);
										 fragColor =  color;
									 }
									 );
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
		}
		
	public:
		
		void update(ofFbo& _drawBuffer, ofTexture& _srcTexture){
			_drawBuffer.begin();
			shader.begin();
			shader.setUniformTexture( "tex0" , _srcTexture, 0 );
			renderFrame(_drawBuffer.getWidth(), _drawBuffer.getHeight());
			shader.end();
			_drawBuffer.end();
		}
	};
}