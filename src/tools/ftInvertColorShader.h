
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {

	class ftInvertColorShader : public ftShader {
	public:
		ftInvertColorShader() {

			ofLogVerbose("init ftInvertColorShader");
			if (ofIsGLProgrammableRenderer())
				glThree();
			else
				glTwo();
		}

	protected:
		void glTwo() {
			fragmentShader = GLSL120(
			
			uniform sampler2DRect Backbuffer;

			void main() {
				vec2 st = gl_TexCoord[0].st;

				vec4 color = texture2DRect(Backbuffer, st);
				
				//if (color.w > 0.1)
				//{
					color.x = 1.0 - color.x;
					color.y = 1.0 - color.y;
					color.z = 1.0 - color.z;
				//}
				//else
				//{
				//	color.x = 0.0;
				//	color.y = 0.0;
				//	color.z = 0.0;
				//}
				
				//color = normalize(color);


				gl_FragColor = color;
			}
			);

			shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			shader.linkProgram();

		}

		void glThree() {
			fragmentShader = GLSL150(
			
			uniform sampler2DRect Backbuffer;

			in vec2 texCoordVarying;
			out vec4 fragColor;

			void main() {
				vec2 st = texCoordVarying;

				vec4 color = texture(Backbuffer, st);
				color.x = 1.0 - color.x;//max(color.x, 0.0);
				color.y = 1.0 - color.y;//max(color.y, 0.0);
				color.z = 1.0 - color.z;// max(color.z, 0.0);

				fragColor = color;
			}
			);

			shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			shader.bindDefaults();
			shader.linkProgram();
		}

	public:

		void update(ofFbo& _buffer, ofTexture& _backBufferTexture) {
			_buffer.begin();
			shader.begin();
			shader.setUniformTexture("Backbuffer", _backBufferTexture, 0);
			renderFrame(_buffer.getWidth(), _buffer.getHeight());
			shader.end();
			_buffer.end();
		}
	};
}