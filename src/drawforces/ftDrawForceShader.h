
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {

	class ftDrawForceShader : public ftShader {
	public:
		ftDrawForceShader() {}

		void setup()
		{
			ftShader::setup();

			bInitialized = 1;

			if( ofIsGLProgrammableRenderer() )
				glThree();
			else
				glTwo();

			if( bInitialized )
				ofLogNotice( "ftDrawForceShader initialized" );
			else
				ofLogWarning( "ftDrawForceShader failed to initialize" );
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
								  uniform vec2		Point;
								  uniform float	Radius;
								  uniform float	EdgeSmooth;
								  uniform vec4		Value;
								  
								  void main(){
									  vec4 color = Value;
									  float d = distance(Point, gl_TexCoord[0].st);
									  float a = max((Radius - d) / Radius, 0.0);
									  float c = ceil(a);
									  color.xyz *= c;//= mix(color.xyz, vec3(1, 1, 1), 1.0 - c);
									  color.w *= pow(a, EdgeSmooth + 0.1);
									  gl_FragColor = color;
									  
								  }
								  );
			
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.linkProgram();

			
		}
		
		void glThree() {
			
			fragmentShader = GLSL150(
								  uniform vec2	Point;
								  uniform float	Radius;
								  uniform float	EdgeSmooth;
								  uniform vec4	Value;
								  uniform float Invert;
								  
								  in vec2 texCoordVarying;
								  out vec4 fragColor;
								  
								  void main(){
									  vec4 color = Value;// vec4(abs(Value.x), abs(Value.y), abs(Value.z), Value.w);
									  float d = distance(Point, texCoordVarying);
									  float a = max((Radius - d) / Radius, 0.0);
									  float c = ceil(a);

									  color.w *= pow(a, EdgeSmooth + 0.1);
									  //color = mix(color, vec4(1.0, 1.0, 1.0, 1.0), 1.0 - color.w)*Invert + color*(1.0-Invert);
									  color.xyz *= c;//= color.xyz*c + vec3(1.0,1.0,1.0)*(1-c)*Invert;// ((1.0 - a) / 10 + 0.9)*c;// mix(color.xyz, vec3(1, 1, 1), 1.0 - a);//= vec3(d, a, c);//= mix(color.xyz, vec3(1, 1, 1), 1.0 - c);
									  

									  fragColor = color;
								  }
								  );
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
		}
		
	public:
		
		void update(ofFbo& _buffer, ofVec4f _value, ofPoint _point, float _radius, float _edge, float _invert){
			
			_buffer.begin();
			shader.begin();
			
			shader.setUniform2f("Point", _point.x, _point.y );
			shader.setUniform1f("Radius", _radius );
			shader.setUniform1f("EdgeSmooth", _edge );
			shader.setUniform4f("Value", _value.x, _value.y, _value.z, _value.w);
			shader.setUniform1f("Invert", _invert);
			renderFrame(_buffer.getWidth(), _buffer.getHeight());
			
			shader.end();
			_buffer.end();
			
		}
		
	private:
	};
}