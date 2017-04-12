
#pragma once

#include "ofMain.h"
#include "ftShader.h"
#include "ftSwapBuffer.h"
#include "ftDecayShader.h"

namespace flowTools {
	
	class ftTimeBlurShader : public ftShader {
	public:
		
		ofParameterGroup	parameters;
		ftTimeBlurShader(){}

		void setup() {
			ftShader::setup();

			decayShader.setup();

			internalFormat = GL_RGBA;

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
				ofLogNotice( "ftTimeBlurShader initialized" );
			else
				ofLogWarning( "ftTimeBlurShader failed to initialize" );

		}
		
	protected:
		void glESOne()
		{
			string fragmentHorizontalBlurShader = GLSLES100(

			uniform sampler2D backbuffer;
			uniform float radius;

			varying vec4	texCoord;

			const float total = ( 1. + 8. + 28. + 56. ) * 2. + 70.;

			void main( void )
			{
				vec2 st = texCoord.st;

				vec4 color = vec4( 0.0, 0.0, 0.0, 0.0 );
				color += ( 1. / total ) * texture2D( backbuffer, st - radius * vec2( 4. / 4., 0. ) );
				color += ( 8. / total )  * texture2D( backbuffer, st - radius * vec2( 3. / 4., 0. ) );
				color += ( 28. / total )  * texture2D( backbuffer, st - radius * vec2( 2. / 4., 0. ) );
				color += ( 56. / total )  * texture2D( backbuffer, st - radius * vec2( 1. / 4., 0. ) );

				color += ( 70. / total ) * texture2D( backbuffer, st );

				color += ( 1. / total ) * texture2D( backbuffer, st + radius * vec2( 4. / 4., 0. ) );
				color += ( 8. / total )  * texture2D( backbuffer, st + radius * vec2( 3. / 4., 0. ) );
				color += ( 28. / total )  * texture2D( backbuffer, st + radius * vec2( 2. / 4., 0. ) );
				color += ( 56. / total )  * texture2D( backbuffer, st + radius * vec2( 1. / 4., 0. ) );

				gl_FragColor = color;
			}
			);
			blurShader[ 0 ].unload();
			bInitialized *= blurShader[ 0 ].setupShaderFromSource( GL_FRAGMENT_SHADER, fragmentHorizontalBlurShader );
            bInitialized *= blurShader[ 0 ].setupShaderFromSource( GL_VERTEX_SHADER, vertexShader );
            bInitialized *= blurShader[ 0 ].bindDefaults();
			bInitialized *= blurShader[ 0 ].linkProgram();

			string fragmentVerticalBlurShader = GLSLES100(

			uniform sampler2D backbuffer;
			uniform float radius;
                                                        
            varying vec4	texCoord;

			const float total = ( 1. + 8. + 28. + 56. ) * 2. + 70.;

			void main( void )
			{
				vec2 st = texCoord.st;

				vec4 color = vec4( 0.0, 0.0, 0.0, 0.0 );
				color += ( 1. / total ) * texture2D( backbuffer, st - radius * vec2( 0., 4. / 4. ) );
				color += ( 8. / total )  * texture2D( backbuffer, st - radius * vec2( 0., 3. / 4. ) );
				color += ( 28. / total )  * texture2D( backbuffer, st - radius * vec2( 0., 2. / 4. ) );
				color += ( 56. / total )  * texture2D( backbuffer, st - radius * vec2( 0., 1. / 4. ) );

				color += ( 70. / total ) * texture2D( backbuffer, st );

				color += ( 1. / total ) * texture2D( backbuffer, st + radius * vec2( 0., 4. / 4. ) );
				color += ( 8. / total )  * texture2D( backbuffer, st + radius * vec2( 0., 3. / 4. ) );
				color += ( 28. / total )  * texture2D( backbuffer, st + radius * vec2( 0., 2. / 4. ) );
				color += ( 56. / total )  * texture2D( backbuffer, st + radius * vec2( 0., 1. / 4. ) );

				gl_FragColor = color;
			}
			);
			blurShader[ 1 ].unload();
			bInitialized *= blurShader[ 1 ].setupShaderFromSource( GL_FRAGMENT_SHADER, fragmentVerticalBlurShader );
            bInitialized *= blurShader[ 1 ].setupShaderFromSource( GL_VERTEX_SHADER, vertexShader );
            bInitialized *= blurShader[ 1 ].bindDefaults();
            bInitialized *= blurShader[ 1 ].linkProgram();
		}

		void glESThree()
		{

			string fragmentHorizontalBlurShader = GLSLES300
			(
				uniform sampler2D backbuffer;
				uniform float radius;

				const float total = ( 1. + 8. + 28. + 56. ) * 2. + 70.;

				in vec2 texCoordVarying;
				out vec4 fragColor;

				void main( void )
				{
					vec2 st = texCoordVarying;

					vec4 color = vec4( 0.0, 0.0, 0.0, 0.0 );
					color += ( 1. / total ) * texture( backbuffer, st - radius * vec2( 4. / 4., 0. ) );
					color += ( 8. / total )  * texture( backbuffer, st - radius * vec2( 3. / 4., 0. ) );
					color += ( 28. / total )  * texture( backbuffer, st - radius * vec2( 2. / 4., 0. ) );
					color += ( 56. / total )  * texture( backbuffer, st - radius * vec2( 1. / 4., 0. ) );

					color += ( 70. / total ) * texture( backbuffer, st );

					color += ( 1. / total ) * texture( backbuffer, st + radius * vec2( 4. / 4., 0. ) );
					color += ( 8. / total )  * texture( backbuffer, st + radius * vec2( 3. / 4., 0. ) );
					color += ( 28. / total )  * texture( backbuffer, st + radius * vec2( 2. / 4., 0. ) );
					color += ( 56. / total )  * texture( backbuffer, st + radius * vec2( 1. / 4., 0. ) );

					fragColor = color;
				}
			);

			blurShader[ 0 ].unload();
			bInitialized *= blurShader[ 0 ].setupShaderFromSource( GL_VERTEX_SHADER, vertexShader );
			bInitialized *= blurShader[ 0 ].setupShaderFromSource( GL_FRAGMENT_SHADER, fragmentHorizontalBlurShader );
			bInitialized *= blurShader[ 0 ].linkProgram();

			string fragmentVerticalBlurShader = GLSLES300
			(
				uniform sampler2D backbuffer;
				uniform float radius;

				const float total = ( 1. + 8. + 28. + 56. ) * 2. + 70.;

				in vec2 texCoordVarying;
				out vec4 fragColor;

				void main( void )
				{
					vec2 st = texCoordVarying;

					vec4 color = vec4( 0.0, 0.0, 0.0, 0.0 );
					color += ( 1. / total ) * texture( backbuffer, st - radius * vec2( 0., 4. / 4. ) );
					color += ( 8. / total )  * texture( backbuffer, st - radius * vec2( 0., 3. / 4. ) );
					color += ( 28. / total )  * texture( backbuffer, st - radius * vec2( 0., 2. / 4. ) );
					color += ( 56. / total )  * texture( backbuffer, st - radius * vec2( 0., 1. / 4. ) );

					color += ( 70. / total ) * texture( backbuffer, st );

					color += ( 1. / total ) * texture( backbuffer, st + radius * vec2( 0., 4. / 4. ) );
					color += ( 8. / total )  * texture( backbuffer, st + radius * vec2( 0., 3. / 4. ) );
					color += ( 28. / total )  * texture( backbuffer, st + radius * vec2( 0., 2. / 4. ) );
					color += ( 56. / total )  * texture( backbuffer, st + radius * vec2( 0., 1. / 4. ) );

					fragColor = color;
				}
			);

			blurShader[ 1 ].unload();
			bInitialized *= blurShader[ 1 ].setupShaderFromSource( GL_VERTEX_SHADER, vertexShader );
			bInitialized *= blurShader[ 1 ].setupShaderFromSource( GL_FRAGMENT_SHADER, fragmentVerticalBlurShader );
			bInitialized *= blurShader[ 1 ].linkProgram();

		}

		void glTwo() {
			string fragmentHorizontalBlurShader = GLSL120(
													   uniform sampler2DRect backbuffer;
													   uniform float radius;
													   
													   const float total = (1. + 8. + 28. + 56.) * 2. + 70.;
													   
													   void main(void) {
														   vec2 st = gl_TexCoord[0].st;
														   
														   vec4 color = vec4(0.0,0.0,0.0,0.0);
														   color += (1. / total) * texture2DRect(backbuffer, st - radius * vec2(4. / 4., 0.));
														   color += (8. / total)  * texture2DRect(backbuffer, st - radius * vec2(3. / 4., 0.));
														   color += (28. / total)  * texture2DRect(backbuffer, st - radius * vec2(2. / 4., 0.));
														   color += (56. / total)  * texture2DRect(backbuffer, st - radius * vec2(1. / 4., 0.));
														   
														   color +=  (70. / total) * texture2DRect(backbuffer, st);
														   
														   color += (1. / total) * texture2DRect(backbuffer, st + radius * vec2(4. / 4., 0.));
														   color += (8. / total)  * texture2DRect(backbuffer, st + radius * vec2(3. / 4., 0.));
														   color += (28. / total)  * texture2DRect(backbuffer, st + radius * vec2(2. / 4., 0.));
														   color += (56. / total)  * texture2DRect(backbuffer, st + radius * vec2(1. / 4., 0.));
														   
														   gl_FragColor = color;
													   }
													   );
			blurShader[0].unload();
			bInitialized *= blurShader[0].setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentHorizontalBlurShader);
			bInitialized *= blurShader[0].linkProgram();
			
			string fragmentVerticalBlurShader = GLSL120(
													 uniform sampler2DRect backbuffer;
													 uniform float radius;
													 
													 const float total = (1. + 8. + 28. + 56.) * 2. + 70.;
													 
													 void main(void) {
														 vec2 st = gl_TexCoord[0].st;
														 
														 vec4 color = vec4(0.0,0.0,0.0,0.0);
														 color += (1. / total) * texture2DRect(backbuffer, st - radius * vec2(0., 4. / 4.));
														 color += (8. / total)  * texture2DRect(backbuffer, st - radius * vec2(0., 3. / 4.));
														 color += (28. / total)  * texture2DRect(backbuffer, st - radius * vec2(0., 2. / 4.));
														 color += (56. / total)  * texture2DRect(backbuffer, st - radius * vec2(0., 1. / 4.));
														 
														 color +=  (70. / total) * texture2DRect(backbuffer, st);
														 
														 color += (1. / total) * texture2DRect(backbuffer, st + radius * vec2(0., 4. / 4.));
														 color += (8. / total)  * texture2DRect(backbuffer, st + radius * vec2(0., 3. / 4.));
														 color += (28. / total)  * texture2DRect(backbuffer, st + radius * vec2(0., 2. / 4.));
														 color += (56. / total)  * texture2DRect(backbuffer, st + radius * vec2(0., 1. / 4.));
														 
														 gl_FragColor = color;
													 }
													 );
			blurShader[1].unload();
			bInitialized *= blurShader[1].setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentVerticalBlurShader);
			bInitialized *= blurShader[1].linkProgram();
		}
		
		void glThree() {
			
			string fragmentHorizontalBlurShader = GLSL150(
													   uniform sampler2DRect backbuffer;
													   uniform float radius;
																
													   const float total = (1. + 8. + 28. + 56.) * 2. + 70.;
													   
													   in vec2 texCoordVarying;
													   out vec4 fragColor;
													   
													   void main(void) {
														   vec2 st = texCoordVarying;
														
														   vec4 color = vec4(0.0,0.0,0.0,0.0);
														   color += (1. / total) * texture(backbuffer, st - radius * vec2(4. / 4., 0.));
														   color += (8. / total)  * texture(backbuffer, st - radius * vec2(3. / 4., 0.));
														   color += (28. / total)  * texture(backbuffer, st - radius * vec2(2. / 4., 0.));
														   color += (56. / total)  * texture(backbuffer, st - radius * vec2(1. / 4., 0.));
														
														   color +=  (70. / total) * texture(backbuffer, st);
														
														   color += (1. / total) * texture(backbuffer, st + radius * vec2(4. / 4., 0.));
														   color += (8. / total)  * texture(backbuffer, st + radius * vec2(3. / 4., 0.));
														   color += (28. / total)  * texture(backbuffer, st + radius * vec2(2. / 4., 0.));
														   color += (56. / total)  * texture(backbuffer, st + radius * vec2(1. / 4., 0.));
														
														   fragColor = color;
													   }
													   );
			blurShader[0].unload();
			bInitialized *= blurShader[0].setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= blurShader[0].setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentHorizontalBlurShader);
			bInitialized *= blurShader[0].linkProgram();
			
			string fragmentVerticalBlurShader = GLSL150(
													 uniform sampler2DRect backbuffer;
													 uniform float radius;
														  
													 const float total = (1. + 8. + 28. + 56.) * 2. + 70.;
													 
													 in vec2 texCoordVarying;
													 out vec4 fragColor;
													 
													 void main(void) {
														  vec2 st = texCoordVarying;
														  
														  vec4 color = vec4(0.0,0.0,0.0,0.0);
														  color += (1. / total) * texture(backbuffer, st - radius * vec2(0., 4. / 4.));
														  color += (8. / total)  * texture(backbuffer, st - radius * vec2(0., 3. / 4.));
														  color += (28. / total)  * texture(backbuffer, st - radius * vec2(0., 2. / 4.));
														  color += (56. / total)  * texture(backbuffer, st - radius * vec2(0., 1. / 4.));
														  
														  color +=  (70. / total) * texture(backbuffer, st);
														  
														  color += (1. / total) * texture(backbuffer, st + radius * vec2(0., 4. / 4.));
														  color += (8. / total)  * texture(backbuffer, st + radius * vec2(0., 3. / 4.));
														  color += (28. / total)  * texture(backbuffer, st + radius * vec2(0., 2. / 4.));
														  color += (56. / total)  * texture(backbuffer, st + radius * vec2(0., 1. / 4.));
														  
														 fragColor = color;
													 }
													 );
			blurShader[1].unload();
			bInitialized *= blurShader[1].setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= blurShader[1].setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentVerticalBlurShader);
			bInitialized *= blurShader[1].linkProgram();
			
		}

	public:
		
		
		void update(ftFbo& _buffer, ofTexture _texture, float _decay, int _radius = 5, int _passes = 1){
			if (pingPong.getWidth() != _buffer.getWidth() ||
				pingPong.getHeight() != _buffer.getHeight() ||
				pingPong.getInternalFormat() != _buffer.getTexture().getTextureData().glInternalFormat) {
				pingPong.allocate(_buffer.getWidth(),  _buffer.getHeight(), _buffer.getTexture().getTextureData().glInternalFormat );
				
			}
			
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_DISABLED);
			
			if (_decay > 0) {
				pingPong.swap();
				decayShader.update(*pingPong.getBuffer(), pingPong.getBackTexture(), _texture, _decay);
			}
			else {
				pingPong.getBuffer()->black();
				pingPong.getBuffer()->begin();
				_texture.draw(0, 0, pingPong.getWidth(), pingPong.getHeight());
				pingPong.getBuffer()->end();
			}
			
			if (_radius > 0 && _passes > 0) {
				for(int i = 0; i < _passes; i++) {
					for(int j = 0; j < 2; j++) {
						pingPong.swap();
						pingPong.getBuffer()->begin();
						blurShader[j].begin();
						blurShader[j].setUniformTexture("backbuffer", pingPong.getBackTexture(), 0 );
						blurShader[j].setUniform1f("radius", _radius);
						renderFrame(pingPong.getWidth(), pingPong.getHeight());
						blurShader[j].end();
						pingPong.getBuffer()->end();
					}
				}
			}
			
			_buffer.begin();
			pingPong.getTexture().draw(0,0, pingPong.getWidth(), pingPong.getHeight());
			_buffer.end();
			
			ofPopStyle();
		}
		
	protected:
		
		void allocate(int _width, int _height, int _internalFormat = GL_RGBA){
			pingPong.allocate( _width, _height, _internalFormat);
			printf("now");
		}
		
		
		ofShader    blurShader[2];
		ftSwapBuffer pingPong;
		ftDecayShader	decayShader;
		
		int		internalFormat;
		int		width;
		int		height;
		
	};
}
