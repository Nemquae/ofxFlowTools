// ================================================================================================
// Inkling is a game of wonder, where a fluid world emerges from your own designs.
//
// Copyright (C) 2017 Ledge Foundation Limited
//
// This program is free software; you can redistribute it and/or modify it under the terms of the 
// GNU General Public License as published by the Free Software Foundation; either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See 
// the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program; if 
// not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 
// 02110-1301  USA
// ================================================================================================

#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools
{

class ftInvertColorShader : public ftShader
{
public:
	ftInvertColorShader() {}

	void setup() 
	{
		ftShader::setup();

		ofLogVerbose( "init ftInvertColorShader" );
		string glslVer = (char *)glGetString( GL_SHADING_LANGUAGE_VERSION );

		if( glslVer == "OpenGL ES GLSL ES 1.00" || glslVer == "OpenGL ES GLSL ES 2.00" )
			glESOne();
		else if( glslVer == "OpenGL ES GLSL ES 3.00" )
			glESThree();
		else if( !ofIsGLProgrammableRenderer() )
			glTwo();
		else if( ofIsGLProgrammableRenderer() )
			glThree();
	}

protected:
	void glESOne()
	{
		fragmentShader = GLSLES100(

		uniform sampler2D Backbuffer;

		varying vec4	texCoord;

		void main()
		{
			vec2 st = texCoord.st;

			vec4 color = texture2D( Backbuffer, st );

			//if (color.w > 0.1)
			//{
			float max = max( color.x, max( color.y, color.z ) );
			float min = min( color.x, min( color.y, color.z ) );

			//color.x = 1.0 - ( min * min / max + max - color.x );//color.x;//max(color.x, 0.0);
			//color.y = 1.0 - ( min * min / max + max - color.y );//color.y;//max(color.y, 0.0);
			//color.z = 1.0 - ( min * min / max + max - color.z );//color.z;// max(color.z, 0.0);
			//}
			//else
			//{
			//	color.x = 0.0;
			//	color.y = 0.0;
			//	color.z = 0.0;
			//}

			//color = normalize(color);

			color.x = color.x + 1 - maxv;
			color.y = color.y + 1 - maxv;
			color.z = color.z + 1 - maxv;


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
			uniform sampler2D Backbuffer;

			in vec2 texCoordVarying;
			out vec4 fragColor;

			void main()
			{
				vec2 st = texCoordVarying;

				vec4 color = texture( Backbuffer, st );

				float max = max( color.x, max( color.y, color.z ) );
				float min = min( color.x, min( color.y, color.z ) );

				//color.x = 1.0 - ( min * min / max + max - color.x );//color.x;//max(color.x, 0.0);
				//color.y = 1.0 - ( min * min / max + max - color.y );//color.y;//max(color.y, 0.0);
				//color.z = 1.0 - ( min * min / max + max - color.z );//color.z;// max(color.z, 0.0);

				color.x = color.x + 1 - maxv;
				color.y = color.y + 1 - maxv;
				color.z = color.z + 1 - maxv;

				fragColor = color;
			}
		);

		shader.setupShaderFromSource( GL_VERTEX_SHADER, vertexShader );
		shader.setupShaderFromSource( GL_FRAGMENT_SHADER, fragmentShader );
		shader.bindDefaults();
		shader.linkProgram();
	}

	void glTwo()
	{
		fragmentShader = GLSL120(

			uniform sampler2DRect Backbuffer;

		void main()
		{
			vec2 st = gl_TexCoord[ 0 ].st;

			vec4 color = texture2DRect( Backbuffer, st );

			//if (color.w > 0.1)
			//{
			float max = max( color.x, max( color.y, color.z ) );
			float min = min( color.x, min( color.y, color.z ) );

			//color.x = 1.0 - ( min * min / max + max - color.x );//color.x;//max(color.x, 0.0);
			//color.y = 1.0 - ( min * min / max + max - color.y );//color.y;//max(color.y, 0.0);
			//color.z = 1.0 - ( min * min / max + max - color.z );//color.z;// max(color.z, 0.0);
			//}
			//else
			//{
			//	color.x = 0.0;
			//	color.y = 0.0;
			//	color.z = 0.0;
			//}

			color.x = color.x + 1 - maxv;
			color.y = color.y + 1 - maxv;
			color.z = color.z + 1 - maxv;

			//color = normalize(color);


			gl_FragColor = color;
		}
		);

		shader.setupShaderFromSource( GL_FRAGMENT_SHADER, fragmentShader );
		shader.linkProgram();

	}

	void glThree()
	{
		fragmentShader = GLSL150(

			uniform sampler2DRect Backbuffer;

		in vec2 texCoordVarying;
		out vec4 fragColor;

		void main()
		{
			vec2 st = texCoordVarying;

			vec4 color = texture( Backbuffer, st );
			float maxv = max( color.x, max( color.y, color.z ) );
			float minv = min( color.x, min( color.y, color.z ) );
			//float lenv = length( color.xyz );
			float dsqv = dot( color.xyz, color.xyz );

			//float iRed		= 1.0 - ( minv * minv / maxv + maxv - color.x );
			//float iGreen	= 1.0 - ( minv * minv / maxv + maxv - color.y );
			//float iBlue		= 1.0 - ( minv * minv / maxv + maxv - color.z );

			//color.x = max( ( 1 - floor( maxv + 0.5 ) ) * iRed,	1 - color.x );	
			//color.y = max( ( 1 - floor( maxv + 0.5 ) ) * iGreen, 1 - color.y );	
			//color.z = max( ( 1 - floor( maxv + 0.5 ) ) * iBlue,	1 - color.z );	

			//color.x = 1.0 - ( minv * minv / dsqv + maxv - color.x );//color.x;//max(color.x, 0.0);
			//color.y = 1.0 - ( minv * minv / dsqv + maxv - color.y );//color.y;//max(color.y, 0.0);
			//color.z = 1.0 - ( minv * minv / dsqv + maxv - color.z );//color.z;// max(color.z, 0.0);

			color.x = color.x + 1 - maxv;
			color.y = color.y + 1 - maxv;
			color.z = color.z + 1 - maxv;

			//color.x = 1 - color.x;
			//color.y = 1 - color.y;
			//color.z = 1 - color.z;

			fragColor = color;
		}
		);

		shader.setupShaderFromSource( GL_VERTEX_SHADER, vertexShader );
		shader.setupShaderFromSource( GL_FRAGMENT_SHADER, fragmentShader );
		shader.bindDefaults();
		shader.linkProgram();
	}

public:

	void update( ofFbo& _buffer, ofTexture& _backBufferTexture )
	{
		_buffer.begin();
		shader.begin();
		shader.setUniformTexture( "Backbuffer", _backBufferTexture, 0 );
		renderFrame( _buffer.getWidth(), _buffer.getHeight() );
		shader.end();
		_buffer.end();
	}
};
}
