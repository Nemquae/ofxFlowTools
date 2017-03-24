//
//  ftDrawForces.cpp
//  ofxFlowToolsExample
//
//  Created by Ties East on 18/6/15.
//
//

#include "ftDrawInputForces.h"


namespace flowTools {
	
	ftDrawInputForces::ftDrawInputForces() {
		ofAddListener(ofEvents().mouseMoved, this, &ftDrawInputForces::mouseMoved);
		ofAddListener(ofEvents().mouseDragged, this, &ftDrawInputForces::mouseDragged);
		ofAddListener( ofEvents().touchMoved, this, &ftDrawInputForces::touchMoved );
		ofAddListener( ofEvents().touchDown, this, &ftDrawInputForces::touchDown );
	}
    
    ftDrawInputForces::~ftDrawInputForces() {
        ofRemoveListener(ofEvents().mouseMoved, this, &ftDrawInputForces::mouseMoved);
        ofRemoveListener(ofEvents().mouseDragged, this, &ftDrawInputForces::mouseDragged);
		ofRemoveListener( ofEvents().touchMoved, this, &ftDrawInputForces::touchMoved );
		ofRemoveListener( ofEvents().touchDown, this, &ftDrawInputForces::touchDown );
    }
	
	void ftDrawInputForces::setup(int _simulationWidth, int _simulationHeight, int _densityWidth, int _densityHeight) {
		simulationWidth = _simulationWidth;
		simulationHeight = _simulationHeight;
		densityWidth = (!_densityWidth)? simulationWidth : _densityWidth;
		densityHeight = (!_densityHeight)? simulationHeight: _densityHeight;
		
		numDrawForces = 12;
		drawForces = new ftDrawForce[numDrawForces];
		drawForces[0].setup(densityWidth, densityHeight, FT_DENSITY, true);
		drawForces[0].setName("draw full res");
		drawForces[1].setup(simulationWidth, simulationHeight, FT_VELOCITY, true);
		drawForces[1].setName("draw flow res 1");
		drawForces[2].setup(simulationWidth, simulationHeight, FT_TEMPERATURE, true);
		drawForces[2].setName("draw flow res 2");
		drawForces[3].setup(densityWidth, densityHeight, FT_DENSITY, false);
		drawForces[3].setName("draw full res");
		drawForces[4].setup(simulationWidth, simulationHeight, FT_VELOCITY, false);
		drawForces[4].setName("draw flow res 1");
		drawForces[5].setup(simulationWidth, simulationHeight, FT_TEMPERATURE, false);
		drawForces[5].setName("draw flow res 2");
		drawForces[6].setup(densityWidth, densityHeight, FT_DENSITY, true);
		drawForces[6].setName("draw full res");
		drawForces[7].setup(simulationWidth, simulationHeight, FT_VELOCITY, true);
		drawForces[7].setName("draw flow res 1");
		drawForces[8].setup(simulationWidth, simulationHeight, FT_TEMPERATURE, true);
		drawForces[8].setName("draw flow res 2");
		drawForces[ 9 ].setup( densityWidth, densityHeight, FT_DENSITY, true );
		drawForces[ 9 ].setName( "draw full res" );
		drawForces[ 10 ].setup( simulationWidth, simulationHeight, FT_VELOCITY, true );
		drawForces[ 10 ].setName( "draw flow res 1" );
		drawForces[ 11 ].setup( simulationWidth, simulationHeight, FT_TEMPERATURE, true );
		drawForces[ 11 ].setName( "draw flow res 2" );
		
		leftButtonParameters.setName("mouse left button");
		leftButtonParameters.add(doResetDrawForces.set("reset", false));
		rightButtonParameters.setName("mouse right button");
		rightButtonParameters.add(doResetDrawForces.set("reset", false));
		middleButtonParameters.setName("mouse middle button");
		middleButtonParameters.add(doResetDrawForces.set("reset", false));
		singleTouchParameters.setName( "single touch" );
		singleTouchParameters.add( doResetDrawForces.set( "reset", false ) );
		doResetDrawForces.addListener(this, &ftDrawInputForces::resetDrawForcesListner);
		for (int i=0; i<3; i++) {
			leftButtonParameters.add(drawForces[i].parameters);
			rightButtonParameters.add(drawForces[i+3].parameters);
			middleButtonParameters.add(drawForces[i+6].parameters);
			singleTouchParameters.add( drawForces[ i + 9 ].parameters );
		}
	}
	
	void ftDrawInputForces::update(float _deltaTime) {
		deltaTime = _deltaTime;
		
		for (int i=0; i<numDrawForces; i++) {
			drawForces[i].update();
		}
	}

	void ftDrawInputForces::invertForce(int _index) { 
		ofVec4f force = drawForces[_index].getForce();
		force.x = 1.0 - force.x;
		force.y = 1.0 - force.y;
		force.z = 1.0 - force.z;

		drawForces[_index].setForce(force);
	}
	
	//--------------------------------------------------------------
	bool ftDrawInputForces::didChange(int _index) {
		if (_index < 0 || _index >= numDrawForces) {
			ofLogWarning("ftDrawInputForces::getDrawForceType: index out of range");
			return false;
		}
		else
			return drawForces[_index].didChange();
	}
	
	//--------------------------------------------------------------
	ftDrawForceType ftDrawInputForces::getType(int _index) {
		if (_index < 0 || _index >= numDrawForces) {
			ofLogWarning("ftDrawInputForces::getDrawForceType: index out of range");
			return FT_NONE;
		}
		else
			return drawForces[_index].getType();
	}
	
	//--------------------------------------------------------------
	ofTexture& ftDrawInputForces::getTextureReference(int _index) {
		if (_index < 0 || _index >= numDrawForces) {
			ofLogError("ftDrawInputForces::getTexture: index out of range");
		}
		else
			return drawForces[_index].getTexture();
		
	}
	
	//--------------------------------------------------------------
	float ftDrawInputForces::getStrength(int _index) {
		if (_index < 0 || _index >= numDrawForces) {
			ofLogWarning("ftDrawInputForces::getStrength: index out of range");
			return 0;
		}
		else {
			if (drawForces[_index].getIsTemporary()) {
				return drawForces[_index].getStrength();
			}
			else {
				return drawForces[_index].getStrength() * deltaTime;;
			}
		}
	}
	
	//--------------------------------------------------------------
	void ftDrawInputForces::mouseDragged( ofMouseEventArgs& mouse ) {
		ofVec2f normalizedMouse;
		
		normalizedMouse.set(mouse.x / (float)ofGetWindowWidth(), mouse.y / (float)ofGetWindowHeight());
		
		ofVec2f velocity = normalizedMouse - lastNormalizedMouse;
		//velocity.normalize();
		
		if (mouse.button == 0) {
			
			for (int i=0; i<3; i++) {
				if (drawForces[i].getType() == FT_VELOCITY)
					drawForces[i].setForce(velocity);
				drawForces[i].applyForce(normalizedMouse);
			}
		}
		else if(mouse.button == 2)
		{
			
			for (int i=3; i<6; i++) {
				if (drawForces[i].getType() == FT_VELOCITY)
					drawForces[i].setForce(velocity);
				drawForces[i].applyForce(normalizedMouse);
			}
		}
		else if (mouse.button == 1)
		{
			for (int i = 6; i<9; i++) {
				if (drawForces[i].getType() == FT_VELOCITY)
					drawForces[i].setForce(velocity);
				drawForces[i].applyForce(normalizedMouse);
			}
		}
		lastNormalizedMouse.set(normalizedMouse);
		
	}
	
	//---------------------------------------------------------------------------------------------
	void ftDrawInputForces::mouseMoved( ofMouseEventArgs& mouse ){
		ofVec2f normalizedMouse;

		normalizedMouse.set ( mouse.x / (float)ofGetWindowWidth()
							, mouse.y / (float)ofGetWindowHeight()
							);

		lastNormalizedMouse.set(normalizedMouse);
	}

	//---------------------------------------------------------------------------------------------
	void ftDrawInputForces::touchDown( ofTouchEventArgs& touch)
	{
		ofVec2f normalizedTouch;
		normalizedTouch.set  ( touch.x / (float)ofGetWindowWidth()
							 , touch.y / (float)ofGetWindowHeight()
							);
		lastNormalizedTouch.set( normalizedTouch );
	}

	//---------------------------------------------------------------------------------------------
	void ftDrawInputForces::touchMoved( ofTouchEventArgs& touch)
	{
		ofVec2f normalizedTouch;
		normalizedTouch.set( touch.x / (float)ofGetWindowWidth()
							 , touch.y / (float)ofGetWindowHeight()
		);

		ofVec2f normalizedTouchSpeed;
		normalizedTouchSpeed.set( touch.xspeed / (float)ofGetWindowWidth()
							 , touch.yspeed / (float)ofGetWindowHeight()
		);

		ofVec2f normalizedTouchAccel;
		normalizedTouchAccel.set( touch.xaccel / (float)ofGetWindowWidth()
							 , touch.yaccel / (float)ofGetWindowHeight()
		);

		ofVec2f velocity = normalizedTouch - lastNormalizedTouch;
		//ofVec2f velocity = normalizedTouchSpeed;
		ofVec2f acceleration = normalizedTouchAccel;

		for( int i = 0; i<3; i++ )
		{
			if( drawForces[ i ].getType() == FT_VELOCITY )
				drawForces[ i ].setForce( velocity );
			drawForces[ i ].applyForce( normalizedTouch );
		}

		lastNormalizedTouch.set( normalizedTouch );
	}
	
}