/*  ************************************************************************************
 *
 *  ftFluidSimulation
 *
 *  Created by Matthias Oostrik on 03/16.14.
 *  Copyright 2014 http://www.MatthiasOostrik.com All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the author nor the names of its contributors
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 *  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 *  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 *  OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *	The Fluid shaders are adapted from various sources. Unfortunately I can't remember all, but the most important ones:
 *		* Mark J Harris: Various online sources
 *		* Patricio Gonzalez Vivo (http://www.patriciogonzalezvivo.com): ofxFluid
 * 
 *  ************************************************************************************ */

#include "ftFluidSimulation.h"
#include <sstream>

#if (TARGET_OS_IPHONE_SIMULATOR) || (TARGET_OS_IPHONE) || (TARGET_IPHONE) || (TARGET_IOS) || (TARGET_SURFACE)
//#include "gl32.h"
//#include "gl2ext.h"
#include "ofColor.h"
#endif

namespace flowTools {
	
	ftFluidSimulation::ftFluidSimulation(){
		
		parameters.setName("fluid solver");
		parameters.add(doReset.set("reset", false));
		parameters.add(doInvert.set("invert", false));
		parameters.add(speed.set("speed", 20, 0, 100));
		parameters.add(cellSize.set("cell size", 1.25, 0.0, 2.0));
		parameters.add(numJacobiIterations.set("iterations", 40, 1, 100));
		parameters.add(viscosity.set("viscosity", 0.1, 0, .4));
		parameters.add(vorticity.set("vorticity", 0.6, 0.0, 1));
		parameters.add(dissipation.set("dissipation", 0.002, 0, 0.01));
		advancedDissipationParameters.setName("advanced dissipation");
		advancedDissipationParameters.add(velocityOffset.set("velocity offset", -0.001, -0.01, 0.01));
		advancedDissipationParameters.add(densityOffset.set("density offset", 0, -0.01, 0.01));
		advancedDissipationParameters.add(temperatureOffset.set("temperature offset", 0.005, -0.01, 0.01));
		parameters.add(advancedDissipationParameters);
		smokeBuoyancyParameters.setName("smoke buoyancy");
		smokeBuoyancyParameters.add(smokeSigma.set("sigma", 0.05, 0.0, 1.0));
		smokeBuoyancyParameters.add(smokeWeight.set("weight", 0.05, 0.0, 1.0));
		smokeBuoyancyParameters.add(ambientTemperature.set("ambient temperature", 0.0, 0.0, 1.0));
		smokeBuoyancyParameters.add(gravity.set("gravity", ofVec2f(0., 9.80665), ofVec2f(-10, -10), ofVec2f(10, 10)));
		parameters.add(smokeBuoyancyParameters);
		maxValues.setName("maximum");
		maxValues.add(clampForce.set("clampForce", 0.05, 0, .1));
		maxValues.add(maxDensity.set("density", 2,0,5));
		maxValues.add(maxVelocity.set("velocity", 4,0,10));
		maxValues.add(maxTemperature.set("temperature", 2,0,5));
		parameters.add(maxValues);
		parameters.add(densityFromPressure.set("density from pressure", 0, -0.1, 0.1));
		parameters.add(densityFromVorticity.set("density from vorticity", -0.1, -0.5, 0.5));
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::setup(int _simulationWidth, int _simulationHeight, int _densityWidth, int _densityHeight) {

#ifdef DEBUG_GL_ERRORS
		stringstream ss; GLint result = glGetError();
		ss << "GL Error ofFbo.allocate FluidSim.setup 1 = " << result << std::endl;
		ss << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
		////glGetIntegerv(GL_FRAMEBUFFER_BINDING, &result);
		ss << result << std::endl;
		if(result != 0) ofLogNotice( ss.str() );
#endif // DEBUG_GL_ERRORS

		mixForceShader.setup();
		invertColorShader.setup();
		diffuseShader.setup();
		advectShader.setup();
		divergenceShader.setup();
		jacobiShader.setup();
		substractGradientShader.setup();
		smokeBuoyancyShader.setup();
		vorticityFirstPassShader.setup();
		vorticitySecondPassShader.setup();
		addShader.setup();
		clampLengthShader.setup();
		densityFloatMultiplierShader.setup();
		densityVec2MultiplierShader.setup();

#ifdef DEBUG_GL_ERRORS
		ss.clear(); result = glGetError();
		ss << "GL Error ofFbo.allocate FluidSim.setup 2 = " << result << std::endl;
		ss << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
		if(result != 0) ofLogNotice( ss.str() );
#endif // DEBUG_GL_ERRORS

		simulationWidth = _simulationWidth;
		simulationHeight = _simulationHeight;
		densityWidth = (!_densityWidth)? simulationWidth : _densityWidth;
		densityHeight = (!_densityHeight)? simulationHeight: _densityHeight;
#if (TARGET_OS_IPHONE_SIMULATOR) || (TARGET_OS_IPHONE) || (TARGET_IPHONE) || (TARGET_IOS) //|| (TARGET_SURFACE)
        int	internalFormatDensity = GL_RGBA;
        int	internalFormatVelocity = GL_RGB;
        int	interformatPressure = GL_RGB;
        int	internalFormatObstacle = GL_RGB;
#else
		int	internalFormatDensity = GL_RGBA32F;
		int	internalFormatVelocity = GL_RG32F;
		int	interformatPressure = GL_R32F;
		int	internalFormatObstacle = GL_R8;
#endif
			
		whiteSwapBuffer.allocate(densityWidth, densityHeight, internalFormatDensity);
		whiteSwapBuffer.black();
		colorSwapBuffer.allocate(densityWidth, densityHeight, internalFormatDensity);
		colorSwapBuffer.black();
		densitySwapBuffer.allocate(densityWidth,densityHeight,internalFormatDensity);
		densitySwapBuffer.black();
		velocitySwapBuffer.allocate(simulationWidth,simulationHeight,internalFormatVelocity);
		velocitySwapBuffer.black();
		temperatureSwapBuffer.allocate(simulationWidth,simulationHeight,interformatPressure);
		//		temperatureSwapBuffer.clear(ambientTemperature.get());
		temperatureSwapBuffer.black();
		pressureSwapBuffer.allocate(simulationWidth,simulationHeight,interformatPressure);
		
		obstacleBuffer.allocate(simulationWidth, simulationHeight, internalFormatObstacle);
		obstacleBuffer.black();

#ifdef DEBUG_GL_ERRORS
		ss.clear(); result = glGetError();
		ss << "GL Error ofFbo.allocate FluidSim.setup 3 = " << result << std::endl;
		ss << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
		if(result != 0) ofLogNotice( ss.str() );
#endif // DEBUG_GL_ERRORS

		createEdgeImage(obstacleBuffer);

#ifdef DEBUG_GL_ERRORS
		ss.clear(); result = glGetError();
		ss << "GL Error ofFbo.allocate FluidSim.setup 4 = " << result << std::endl;
		ss << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
		if(result != 0) ofLogNotice( ss.str() );
#endif // DEBUG_GL_ERRORS
		
		divergenceBuffer.allocate(simulationWidth, simulationHeight, interformatPressure);
		smokeBuoyancyBuffer.allocate(simulationWidth, simulationHeight, internalFormatVelocity);
		smokeBuoyancyBuffer.black();
		vorticityFirstPassBuffer.allocate(simulationWidth, simulationHeight, internalFormatVelocity);
		vorticitySecondPassBuffer.allocate(simulationWidth, simulationHeight, internalFormatVelocity);
		addPressureBuffer.allocate(simulationWidth, simulationHeight, interformatPressure);
		addPressureBufferDidChange = false;
		addTempObstacleBuffer.allocate(simulationWidth, simulationHeight, internalFormatObstacle);
		addTempObstacleBuffer.black();
		addTempObstacleBufferDidChange = false;
		combinedObstacleBuffer.allocate(simulationWidth, simulationHeight, internalFormatObstacle);
		combinedObstacleBuffer.black();
		combinedObstacleBuffer.stretchIntoMe(obstacleBuffer);

#ifdef DEBUG_GL_ERRORS
		ss.clear(); result = glGetError();
		ss << "GL Error ofFbo.allocate FluidSim.setup 5 = " << result << std::endl;
		ss << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
		if(result != 0) ofLogNotice( ss.str() );
#endif // DEBUG_GL_ERRORS
		
		deltaTime = 0;
		lastTime = 0;
		addTempObstacleBufferDidChange = true;
		
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::update(float _deltaTime){
		float time = ofGetElapsedTimef();
		if (_deltaTime != 0)
			deltaTime = _deltaTime;
		else
			deltaTime = time - lastTime;
		lastTime = time;
		timeStep = deltaTime * speed.get();
		
		if (doReset) {
			doReset.set(false);
			reset();
		}

		//whiteSwapBuffer.white();

		//colorSwapBuffer.black();

		//mixForceShader.update(*whiteSwapBuffer.getBuffer(), whiteSwapBuffer.getBackTexture(), ofFloatColor(1.0,0.0));
		//whiteSwapBuffer.swap();

		//invertColorShader.update(*whiteSwapBuffer.getBuffer(), whiteSwapBuffer.getBackTexture());
		//whiteSwapBuffer.swap();

		//addShader.update(*colorSwapBuffer.getBuffer(),
		//	colorSwapBuffer.getBackTexture(),
		//	//_tex,
		//	whiteSwapBuffer.getBackTexture(),
		//	1.0);
		//colorSwapBuffer.swap();

		//invertColorShader.update(*whiteSwapBuffer.getBuffer(), whiteSwapBuffer.getBackTexture());
		//whiteSwapBuffer.swap();

		//mixForceShader.update(*whiteSwapBuffer.getBuffer(), whiteSwapBuffer.getBackTexture(), ofFloatColor(0.0, 0.0));
		//whiteSwapBuffer.swap();
		
		ofPushStyle();

		//ofClear(ofColor(255, 255, 255, 255));
		
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		// OBSTACLE BUFFER;
		
		if (combinedObstacleNeedsToBeCleaned) {
			combinedObstacleBuffer.black();
			combinedObstacleBuffer.stretchIntoMe(obstacleBuffer);
			combinedObstacleNeedsToBeCleaned = false;
		}
		
		if (addTempObstacleBufferDidChange) {
			ofEnableBlendMode(OF_BLENDMODE_ADD);
			combinedObstacleBuffer.stretchIntoMe(addTempObstacleBuffer);
			addTempObstacleBufferDidChange = false;
			addTempObstacleBuffer.black();
			combinedObstacleNeedsToBeCleaned = true;
		}
		//ofEnableBlendMode(OF_BLENDMODE_DISABLED);

		// CLAMP LENGTH
		if (maxDensity.get() > 0.0) {
			clampLengthShader.update(*densitySwapBuffer.getBuffer(),
									 densitySwapBuffer.getBackTexture(),
									 maxDensity.get(),
									 clampForce.get());
			densitySwapBuffer.swap();
		}
		if (maxVelocity.get() > 0.0) {
			clampLengthShader.update(*velocitySwapBuffer.getBuffer(),
									 velocitySwapBuffer.getBackTexture(),
									 maxVelocity.get(),
									 clampForce.get());
			velocitySwapBuffer.swap();
		}
		if (maxTemperature.get() > 0.0) {
			clampLengthShader.update(*temperatureSwapBuffer.getBuffer(),
									 temperatureSwapBuffer.getBackTexture(),
									 maxTemperature.get(),
									 clampForce.get());
			temperatureSwapBuffer.swap();
		}		
		
		
		// VORTEX CONFINEMENT
		if (vorticity.get() > 0.0) {
			vorticityFirstPassShader.update(vorticityFirstPassBuffer,
											velocitySwapBuffer.getBackTexture(),
											combinedObstacleBuffer.getTexture());
			
			vorticitySecondPassShader.update(vorticitySecondPassBuffer,
											 vorticityFirstPassBuffer.getTexture(),
											 timeStep,
											 vorticity.get(),
											 cellSize.get());
			
			addVelocity(vorticitySecondPassBuffer.getTexture());
		}
		
		// ADVECT
		advectShader.update(*velocitySwapBuffer.getBuffer(),
							velocitySwapBuffer.getBackTexture(),
							velocitySwapBuffer.getBackTexture(),
							combinedObstacleBuffer.getTexture(),
							timeStep,
							1.0 - (dissipation.get() + velocityOffset.get()),
							cellSize.get());
		velocitySwapBuffer.swap();
		
		//invertColorShader.update(*densitySwapBuffer.getBuffer(),
		//	densitySwapBuffer.getBackTexture());

		advectShader.update(*densitySwapBuffer.getBuffer(),
							densitySwapBuffer.getBackTexture(),
							velocitySwapBuffer.getBackTexture(),
							combinedObstacleBuffer.getTexture(),
							timeStep,
							1.0 - (dissipation.get() + densityOffset.get()),
							cellSize.get());

		//invertColorShader.update(*densitySwapBuffer.getBuffer(),
		//	densitySwapBuffer.getBackTexture());

		densitySwapBuffer.swap();

		
		// DIFFUSE
		if (viscosity.get() > 0.0) {
			for (int i = 0; i < numJacobiIterations.get(); i++) {
				diffuseShader.update(*velocitySwapBuffer.getBuffer(),
									 velocitySwapBuffer.getBackTexture(),
									 combinedObstacleBuffer.getTexture(),
//									 viscosity.get() * deltaTime); // works better than timeStep
									 viscosity.get() * timeStep);
				velocitySwapBuffer.swap();
			}
		 }
		 
		
		
		// SMOKE BUOYANCY -- UNSTABLE __ DISABLED FOR NOW
//		if (smokeSigma.get() > 0.0 && smokeWeight.get() > 0.0 ) {
//			
//			advectShader.update(*temperatureSwapBuffer.getBuffer(),
//								temperatureSwapBuffer.getBackTexture(),
//								velocitySwapBuffer.getBackTexture(),
//								combinedObstacleBuffer.getTexture(),
//								timeStep,
//								1.0 - (dissipation.get() + temperatureOffset.get()), // WHY?
//								cellSize.get());
//			temperatureSwapBuffer.swap();
//			
//			smokeBuoyancyShader.update(smokeBuoyancyBuffer,
//									   velocitySwapBuffer.getBackTexture(),
//									   temperatureSwapBuffer.getBackTexture(),
//									   densitySwapBuffer.getBackTexture(),
//									   ambientTemperature.get(),
//									   timeStep,
//									   smokeSigma.get(),
//									   smokeWeight.get(),
//									   gravity.get() * timeStep);
//			addVelocity(smokeBuoyancyBuffer.getTexture());
//	
//		}
//		else
			temperatureSwapBuffer.black();
		
		
		// DIVERGENCE AND JACOBI
		divergenceBuffer.black();
		divergenceShader.update(divergenceBuffer,
								velocitySwapBuffer.getBackTexture(),
								combinedObstacleBuffer.getTexture(),
								cellSize.get());
		
		pressureSwapBuffer.black();
		for (int i = 0; i < numJacobiIterations.get(); i++) {
			jacobiShader.update(*pressureSwapBuffer.getBuffer(),
								pressureSwapBuffer.getBackTexture(),
								divergenceBuffer.getTexture(),
								combinedObstacleBuffer.getTexture(),
								cellSize.get());
			pressureSwapBuffer.swap();
		}
		
		// Multiply density by pressure and or vorticity
		if(densityFromPressure != 0) {

			//invertColorShader.update(*densitySwapBuffer.getBuffer(),
			//	densitySwapBuffer.getBackTexture());

			densityFloatMultiplierShader.update(*densitySwapBuffer.getBuffer(),
												densitySwapBuffer.getBackTexture(),
												pressureSwapBuffer.getBackTexture(),
												densityFromPressure.get());

			//invertColorShader.update(*densitySwapBuffer.getBuffer(),
			//	densitySwapBuffer.getBackTexture());

			densitySwapBuffer.swap();
		}
		
		if(densityFromVorticity != 0) {
			//invertColorShader.update(*densitySwapBuffer.getBuffer(),
			//	densitySwapBuffer.getBackTexture());

			densityVec2MultiplierShader.update(*densitySwapBuffer.getBuffer(),
											   densitySwapBuffer.getBackTexture(),
											   vorticitySecondPassBuffer.getTexture(),
											   -densityFromVorticity.get());

			//invertColorShader.update(*densitySwapBuffer.getBuffer(),
			//	densitySwapBuffer.getBackTexture());

			densitySwapBuffer.swap();
		}
		
		
		// Drain some fluid
		if(addPressureBufferDidChange == true) {
			addPressureBufferDidChange = false;
			addShader.update(*pressureSwapBuffer.getBuffer(),
							 pressureSwapBuffer.getBackTexture(),
							 addPressureBuffer.getTexture(),
							 1.0);
			pressureSwapBuffer.swap();
		}
		
		substractGradientShader.update(*velocitySwapBuffer.getBuffer(),
									   velocitySwapBuffer.getBackTexture(),
									   pressureSwapBuffer.getBackTexture(),
									   combinedObstacleBuffer.getTexture(),
									   cellSize.get());
		velocitySwapBuffer.swap();

		//colorSwapBuffer.black();

		//addShader.update(*colorSwapBuffer.getBuffer(), colorSwapBuffer.getBackTexture(), whiteSwapBuffer.getBackTexture(), 1.0);
		//colorSwapBuffer.swap();

		if (doInvert.get())
		{
			invertColorShader.update(*colorSwapBuffer.getBuffer(),
				//	colorSwapBuffer.getBackTexture());
				densitySwapBuffer.getBackTexture());
			colorSwapBuffer.swap();
		}
		else
		{
			colorSwapBuffer.black();
			addShader.update(*colorSwapBuffer.getBuffer(), colorSwapBuffer.getBackTexture(), densitySwapBuffer.getBackTexture(), 1.0f);
			colorSwapBuffer.swap();
		}

		//colorSwapBuffer.black();

		
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::addDensity(ofTexture & _tex, float _strength, bool _invert){
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);

		
		if (_invert)
		{
			whiteSwapBuffer.black();

			//addShader.update(*whiteSwapBuffer.getBuffer(),
			//	whiteSwapBuffer.getBackTexture(),
			//	_tex,
			//	_strength);
			//whiteSwapBuffer.swap();
            
            ofFloatColor color = ofFloatColor(1.0, 0.0);

			mixForceShader.update(*whiteSwapBuffer.getBuffer(),
				_tex, color);
				//whiteSwapBuffer.getBackTexture(), ofFloatColor(1.0, 0.0));

			whiteSwapBuffer.swap();

			invertColorShader.update(*whiteSwapBuffer.getBuffer(),
				//_tex);
				whiteSwapBuffer.getBackTexture());

			whiteSwapBuffer.swap();

			//addShader.update(*densitySwapBuffer.getBuffer(),
			//	densitySwapBuffer.getBackTexture(),
			//	//_tex,
			//	colorSwapBuffer.getBackTexture(),
			//	_strength);
			//densitySwapBuffer.swap();

			addShader.update(*colorSwapBuffer.getBuffer(),
				colorSwapBuffer.getBackTexture(),
				//_tex,
				whiteSwapBuffer.getBackTexture(),
				_strength);
			colorSwapBuffer.swap();

		}
		else
		{
			//ofEnableBlendMode(OF_BLENDMODE_DISABLED);
			addShader.update(*densitySwapBuffer.getBuffer(),
				densitySwapBuffer.getBackTexture(),
				_tex,
				//colorSwapBuffer.getBackTexture(),
				_strength);
			densitySwapBuffer.swap();
		}

		ofPopStyle();
	};
	
	//--------------------------------------------------------------
	void ftFluidSimulation::addVelocity(ofTexture & _tex, float _strength, bool _invert) {
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		//if (_invert)
		//{
		//	colorSwapBuffer.black();

		//	mixForceShader.update(*colorSwapBuffer.getBuffer(), _tex);
		//	colorSwapBuffer.swap();

		//	invertColorShader.update(*colorSwapBuffer.getBuffer(), colorSwapBuffer.getBackTexture());
		//	colorSwapBuffer.swap();

		//	addShader.update(*velocitySwapBuffer.getBuffer(),
		//		velocitySwapBuffer.getBackTexture(),
		//		//_tex,
		//		colorSwapBuffer.getBackTexture(),
		//		_strength);
		//	velocitySwapBuffer.swap();
		//}
		//else
		{
			addShader.update(*velocitySwapBuffer.getBuffer(),
				velocitySwapBuffer.getBackTexture(),
				_tex,
				_strength);
			velocitySwapBuffer.swap();
		}
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::addTemperature(ofTexture & _tex, float _strength, bool _invert){
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		//if (_invert)
		//{
		//	colorSwapBuffer.black();

		//	mixForceShader.update(*colorSwapBuffer.getBuffer(), _tex);
		//	colorSwapBuffer.swap();

		//	invertColorShader.update(*colorSwapBuffer.getBuffer(), colorSwapBuffer.getBackTexture());
		//	colorSwapBuffer.swap();

		//	addShader.update(*temperatureSwapBuffer.getBuffer(),
		//		temperatureSwapBuffer.getBackTexture(),
		//		//_tex,
		//		colorSwapBuffer.getTexture(),
		//		_strength);
		//	temperatureSwapBuffer.swap();
		//}
		//else
		{
			addShader.update(*temperatureSwapBuffer.getBuffer(),
				temperatureSwapBuffer.getBackTexture(),
				_tex,
				_strength);
			temperatureSwapBuffer.swap();
		}
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::addPressure(ofTexture& _tex, float _strength, bool _invert){
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		//if (_invert)
		//{
		//	colorSwapBuffer.black();

		//	mixForceShader.update(*colorSwapBuffer.getBuffer(), _tex);
		//	colorSwapBuffer.swap();

		//	invertColorShader.update(*colorSwapBuffer.getBuffer(), colorSwapBuffer.getBackTexture());
		//	colorSwapBuffer.swap();

		//	addPressureBuffer.black();

		//	addShader.update(addPressureBuffer,
		//		addPressureBuffer.getTexture(), // dubious
		//		//_tex,
		//		colorSwapBuffer.getBackTexture(),
		//		_strength);

		//	addPressureBufferDidChange = true;
		//}
		//else
		{
			addPressureBuffer.black();

			addShader.update(addPressureBuffer,
				addPressureBuffer.getTexture(), // dubious
				_tex,
				_strength);

			addPressureBufferDidChange = true;
		}
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::addObstacle(ofTexture & _obstacleTexture){
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		obstacleBuffer.stretchIntoMe(_obstacleTexture);
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::addTempObstacle(ofTexture & _obstacleTexture){
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		addTempObstacleBuffer.stretchIntoMe(_obstacleTexture);
		addTempObstacleBufferDidChange = true;
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::draw(int x, int y, float _width, float _height){
		//densitySwapBuffer.getBackTexture().draw(x, y, _width, _height);
		colorSwapBuffer.getBackTexture().draw(x,y,_width,_height); // why BACK texture?
		//invertColorShader.update(*colorSwapBuffer.getBuffer(), colorSwapBuffer.getBackTexture());
		//colorSwapBuffer.swap();
		//colorSwapBuffer.black();
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::reset() {
		
		whiteSwapBuffer.black();
		colorSwapBuffer.black();
		densitySwapBuffer.black();
		velocitySwapBuffer.black();
		temperatureSwapBuffer.black();
		createEdgeImage(obstacleBuffer);
		combinedObstacleNeedsToBeCleaned = true;
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::resetBackground() {
		obstacleBuffer.black();
		//obstacleBuffer.white();
		createEdgeImage(obstacleBuffer);
		combinedObstacleNeedsToBeCleaned = true;
	}
	
	void ftFluidSimulation::createEdgeImage(ftFbo &buffer, int _edgeWidth, ofColor _backgroundColor, ofColor _edgeColor) {
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		buffer.begin();
		ofClear(_backgroundColor);
		ofSetColor(_edgeColor);

#ifdef DEBUG_GL_ERRORS
		stringstream ss; GLint result = glGetError();
		ss << "GL Error ofFbo.allocate FluidSim.createEdgeImage = " << result << std::endl;
		ss << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
		////glGetIntegerv(GL_FRAMEBUFFER_BINDING, &result);
		ss << result << std::endl;
		if(result != 0) ofLogNotice( ss.str() );
#endif // DEBUG_GL_ERRORS

		ofDrawRectangle(_edgeWidth, _edgeWidth, buffer.getWidth() - _edgeWidth * 2, buffer.getHeight() - _edgeWidth * 2);
		buffer.end();
		ofPopStyle();
	}
}
