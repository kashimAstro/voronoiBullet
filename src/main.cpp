#include "ofMain.h"
#include "ofAppGlutWindow.h"
#include "ofxBullet.h"
#include "ofxVoro.h"

class xApp : public ofBaseApp{
public :
	bool DEBUGWORLD=false;
	ofEasyCam cam;
	ofLight pLight,sLight,dLight;
	ofMaterial material;
	vector<ofPoint> cellCentroids;
	vector<float>   cellRadius;
	vector<ofVboMesh>  cellMeshes;
	ofxBulletWorldRigid world;
	ofxBulletBox ground;
	vector<ofxBulletBox * > box;

	void makeTissue(int _nCells, int _width, int _height, int _deep){
	    cellMeshes.clear();
	    cellCentroids.clear();
	    cellRadius.clear();
	    voro::container con(-_width*0.5,_width*0.5, -_height*0.5,_height*0.5, -_deep*0.5,_deep*0.5, 1,1,1, true,true,true, 8);
	    voro::wall_cylinder cyl(0,0,0,0,0,20, min(_width*0.5, _height*0.5));
	    con.add_wall(cyl);
	    for(int i = 0; i < _nCells;i++){
	          ofPoint newCell = ofPoint(ofRandom(-_width*0.5,_width*0.5), ofRandom(-_height*0.5,_height*0.5), ofRandom(-_deep*0.25,_deep*0.25));
	          addCellSeed(con, newCell, i, true);
	    }
	    cellMeshes = getCellsFromContainer(con,0.0);
	    cellRadius = getCellsRadius(con);
	    cellCentroids = getCellsCentroids(con);
	}

	void setup(){
	    ofEnableAlphaBlending();
	    ofSetVerticalSync(true);
	    ofSetSmoothLighting(true);
	    pLight.setDiffuseColor( ofColor(0.f, 255.f, 0.f));
	    pLight.setSpecularColor( ofColor(255.f, 255.f, 0.f));
	    pLight.setPointLight();
	    sLight.setDiffuseColor( ofColor(255.f, 0.f, 0.f));
	    sLight.setSpecularColor( ofColor(255.f, 255.f, 255.f));
	    sLight.setSpotlight();
	    sLight.setSpotlightCutOff( 50 );
	    sLight.setSpotConcentration( 45 );
	    dLight.setDiffuseColor(ofColor(0.f, 0.f, 255.f));
	    dLight.setSpecularColor(ofColor(255.f, 255.f, 255.f));
	    dLight.setDirectional();
	    dLight.setOrientation( ofVec3f(0, 90, 0) );
	    material.setShininess( 120 );
	    material.setSpecularColor(ofColor(255, 255, 255, 255));
	    cam.setAutoDistance(true);

	    makeTissue(150, ofGetHeight()*0.8, ofGetHeight()*0.8, 20);
	    world.setup();
	    world.enableGrabbing();
	    world.enableDebugDraw();
	    world.setCamera(&cam);
	    world.setGravity( ofVec3f(0, 0, 0) );

	    for (int i = 0; i < cellCentroids.size(); i++){
		box.push_back(new ofxBulletBox());
	        box[i]->create( world.world, cellCentroids[i], .5, 100, 100, 10 );
	        box[i]->add();
	    }
	    ground.create( world.world, ofVec3f(0., -500, 0.), 0., 2200.f, 50.f, 2200.f );
	    ground.setProperties(.25, .95);
	    ground.add();
	}

	void update(){
	    ofSetWindowTitle(ofToString(ofGetFrameRate()));
	    world.update();
	    float radius          = 180.f;
            ofVec3f center = ofVec3f(ofGetWidth()*.5, ofGetHeight()*.5, 0);

	    pLight.setPosition(cos(ofGetElapsedTimef()*.6f) * radius * 2 + center.x, sin(ofGetElapsedTimef()*.8f) * radius * 2 + center.y,
                               -cos(ofGetElapsedTimef()*.8f) * radius * 2 + center.z);
            sLight.setOrientation( ofVec3f( 0, cos(ofGetElapsedTimef()) * RAD_TO_DEG, 0) );
            sLight.setPosition( mouseX, mouseY, 200);
	}

	void draw(){
	    ofBackgroundGradient(155,55);
	    cam.begin();
	    glEnable(GL_DEPTH_TEST);
	    ofEnableLighting();
	    material.begin();
	    pLight.enable();
	    sLight.enable();
	    dLight.enable();

//	    ground.draw();

	    ofPushMatrix();
	    for(int i = 0; i < cellMeshes.size(); i++){
	        ofSetColor(255,250,155);
	        ofTranslate(box[i]->getPosition()/25);
	        ofRotate(box[i]->getRotationQuat().w()/25);
	        cellMeshes[i].drawFaces();
		ofSetColor(0,0,0);
		cellMeshes[i].drawWireframe();
	    }
	    ofPopMatrix();

 	    ofDisableLighting();
	    material.end();
	    pLight.disable();
	    sLight.disable();
	    dLight.disable();
	    glDisable(GL_DEPTH_TEST);

	    if(world.checkWorld() && DEBUGWORLD==true){
	       ofSetLineWidth(1.f);
	       ofSetColor(255, 0, 200);
	       world.drawDebug();
	    }

	    cam.end();
	    ofVec3f gravity = world.getGravity();
	    stringstream ss;
	    ss << "Gravity(up/down/left/right): x=" << gravity.x << " y= " << gravity.y << " z= " << gravity.z << endl;
	    ofSetColor(255, 255, 255);
	    ofDrawBitmapString(ss.str().c_str(), 20, 20);
	}

	ofVec3f gravity;

	void keyPressed(int key){
		switch(key) {
	        	case OF_KEY_UP:
        	                gravity.y -= 5.;
                        	world.setGravity( gravity );
	                        break;
	                case OF_KEY_DOWN:
	                        gravity.y += 5.;
	                        world.setGravity( gravity );
	                        break;
	                case OF_KEY_RIGHT:
	                        gravity.x += 5.;
	                        world.setGravity( gravity );
	                        break;
	                case OF_KEY_LEFT:
	                        gravity.x -= 5.;
	                        world.setGravity( gravity );
	                        break;
	    }
	    if ( key == 'f' ){
	        ofToggleFullscreen();
	    }
	    //
	    if( key == 'd' ){
	            DEBUGWORLD=!DEBUGWORLD;
	            cout<<"DEBUG:"<<DEBUGWORLD<<endl;
	    }
	    //
	    if( key == 'c' ){
	        makeTissue(150, ofGetHeight()*0.8, ofGetHeight()*0.8, 20);
                if(box.size() > 0) { for (int i = 0; i < box.size(); i++) { delete box[i]; } box.clear(); }

	        for (int i = 0; i < cellCentroids.size(); i++){
		    box.push_back(new ofxBulletBox());
	            box[i]->create( world.world, cellCentroids[i], .5, 100, 100, 10 );
                    box[i]->add();
	        }
	    }
	}

};
	
int main( ){
    ofAppGlutWindow window;
    ofSetupOpenGL(&window, 1024,768, OF_WINDOW);
    ofRunApp( new xApp());
}
