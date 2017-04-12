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

class ftMixForceShader : public ftShader
{
public:
	ftMixForceShader() {}

	void setup() 
	{
		ftShader::setup();

		ofLogVerbose( "init ftMixForceShader" );
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

			color = mix( color, vec4( 1.0, 1.0, 1.0, 1.0 ), 1.0 - color.w );

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
		fragmentShader = GLSLES300(

		uniform sampler2DRect Backbuffer;
		uniform vec4 MixColor;

		in vec2 texCoordVarying;
		out vec4 fragColor;

		void main()
		{
			vec2 st = texCoordVarying;

			vec4 color = texture( Backbuffer, st );
			float a = color.w;// ceil(color.w);

							  //color = color*a + vec4(1.0, 1.0, 1.0, 0.0)*(1.0-a);
			color = mix( color, MixColor, 1.0 - a );
			//color.y = color.w;

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

			color = mix( color, vec4( 1.0, 1.0, 1.0, 1.0 ), 1.0 - color.w );

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
		uniform vec4 MixColor;

		in vec2 texCoordVarying;
		out vec4 fragColor;

		void main()
		{
			vec2 st = texCoordVarying;

			vec4 color = texture( Backbuffer, st );
			float a = color.w;// ceil(color.w);

			//color = color*a + vec4(1.0, 1.0, 1.0, 0.0)*(1.0-a);
			color = mix( color, MixColor, 1.0 - a );
			//color.y = color.w;

			fragColor = color;
		}
		);

		shader.setupShaderFromSource( GL_VERTEX_SHADER, vertexShader );
		shader.setupShaderFromSource( GL_FRAGMENT_SHADER, fragmentShader );
		shader.bindDefaults();
		shader.linkProgram();
	}

public:

	void update( ofFbo& _buffer, ofTexture& _backBufferTexture, ofFloatColor& _mixColor )
	{
		_buffer.begin();
		shader.begin();
		shader.setUniformTexture( "Backbuffer", _backBufferTexture, 0 );
		shader.setUniform4f( "MixColor", _mixColor );
		renderFrame( _buffer.getWidth(), _buffer.getHeight() );
		shader.end();
		_buffer.end();
	}
};
}
