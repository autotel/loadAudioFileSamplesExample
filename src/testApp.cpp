#include "testApp.h"



//--------------------------------------------------------------
void testApp::setup(){

	ofBackground(255,255,255);
    ofSetFrameRate(30);

	sampleRate 			= 44100;
	phase 				= 0;
	phaseAdder 			= 0.0f;
	phaseAdderTarget 	= 0.0f;
	volume				= 0.4f;
	bRingModulation 	= false;
	lAudio = new float[256];
	rAudio = new float[256];

	sample.load("Kupferberg-Tuli_No-Deposit.wav"); // supports mono or stereo .wav files
	sample.setLooping(true);
    sample.play();

	sample.generateWaveForm(&waveForm);

    bScrubMode = false;
    bReverse = false;

    targetFrequency = 1000.0f;
	phaseAdderTarget = (targetFrequency / (float) sampleRate) * TWO_PI;


	ofSoundStreamSetup(2,0,this, sampleRate,256, 4);

}

//--------------------------------------------------------------
void testApp::exit(){
    ofSoundStreamStop();
    ofSoundStreamClose();
    delete lAudio;
    delete rAudio;

}

//--------------------------------------------------------------
void testApp::update(){

    static int last_x = 0;
    static int curr_x = 0;

    last_x = curr_x;
    curr_x = mouseX;
    deltax = (1.0f + fabs(curr_x - last_x))/1.0f;

    if(bScrubMode) {
        float next_pos = widthPct;
        float curr_pos = sample.getPosition();

        deltapos = (next_pos - curr_pos);

        speed = deltapos*((float)sample.getLength()/(float)sample.getSampleRate())*10.0f;
        sample.setSpeed(speed);
    }

}

//--------------------------------------------------------------
void testApp::draw(){

	// draw waveform
	sample.drawWaveForm(5, 500, ofGetWidth()-10, 100, &waveForm);

	// draw the left:
	ofSetColor(0x333333);
	ofRect(100,100,256,200);
	ofSetColor(0xFFFFFF);
	for (int i = 0; i < 256; i++){
		ofLine(100+i,200,100+i,200+lAudio[i]*200.0f);
	}

	// draw the right:
	ofSetColor(0x333333);
	ofRect(600,100,256,200);
	ofSetColor(0xFFFFFF);
	for (int i = 0; i < 256; i++){
		ofLine(600+i,200,600+i,200+rAudio[i]*200.0f);
	}


	ofSetColor(0x333333);
	char reportString[255];
	sprintf(reportString, "volume: (%f) modify with -/+ keys\npan: (%f)\nspeed: (%f)\nplayback: %s\nposition: %f\npaused: %s", volume, pan, speed, bRingModulation ? "ring modulation" : "normal",sample.getPosition(),sample.getIsPaused()?"yes":"no");
	if (bRingModulation) sprintf(reportString, "%s (%fhz)", reportString, targetFrequency);

	ofDrawBitmapString(reportString,80,380);

}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){
    static bool toggle = false;

	if (key == '-'){
		volume -= 0.05;
		volume = MAX(volume, 0);
	} else if (key == '+'){
		volume += 0.05;
		volume = MIN(volume, 1);
	}

	if(key == ' ')
	{
	    toggle = !toggle;
	    sample.setPaused(toggle);
	}

	if(key == 'r')
	{
	    bReverse = !bReverse;
	    if(bReverse) {
	        speed = -1.0f;
            sample.setSpeed(speed);
	    }
        else
        {
            speed = 1.0f;
            sample.setSpeed(speed);
        }
	}

	if(key == 's')
	{
	    if(bScrubMode) {speed = 1.0f;sample.setSpeed(speed);}
	    bScrubMode = !bScrubMode;
	}

	if(key == 'h')
	{
	    sample.stop();
	}
	if(key == 'p')
	{
	    sample.play();
	}
}

//--------------------------------------------------------------
void testApp::keyReleased  (int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

	int width = ofGetWidth();
	widthPct = (float)x / (float)width;
	pan = widthPct;

	float height = (float)ofGetHeight();
	float heightPct = 1 - ((height-y) / height);

	if(!bScrubMode) {
        speed = 2*(-1.0f + 2*heightPct);
        sample.setSpeed(speed);
	}


}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	int width = ofGetWidth();
	pan = (float)x / (float)width;

	float height = (float)ofGetHeight();
	float heightPct = ((height-y) / height);
	targetFrequency = 2000.0f * heightPct;
	phaseAdderTarget = (targetFrequency / (float) sampleRate) * TWO_PI;
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	bRingModulation = true;
}


//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	bRingModulation = false;
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}
//--------------------------------------------------------------
void testApp::audioRequested 	(float * output, int bufferSize, int nChannels){

	float leftScale = 1 - pan;
	float rightScale = pan;

	// sin (n) seems to have trouble when n is very large, so we
	// keep phase in the range of 0-TWO_PI like this:
	while (phase > TWO_PI){
		phase -= TWO_PI;
	}

    phaseAdder = 0.95f * phaseAdder + 0.05f * phaseAdderTarget;

    for (int i = 0; i < bufferSize; i++){
        if ( bRingModulation == true)
        {
            phase += phaseAdder;
            float mod = sin(phase);

            // mono
            if(sample.getChannels() == 1) {
                float smp = sample.update();
                lAudio[i] = output[i*nChannels    ] = mod * smp * volume * leftScale;
                rAudio[i] = output[i*nChannels + 1] = mod * smp * volume * rightScale;
            }
            // stereo
            else if (sample.getChannels() == 2) {
                lAudio[i] = output[i*nChannels    ] = mod * sample.update() * volume * leftScale;
                rAudio[i] = output[i*nChannels + 1] = mod * sample.update() * volume * rightScale;
            }
        }
        else //normal playback
        {
            // mono
            if(sample.getChannels() == 1) {
                float smp = sample.update();
                lAudio[i] = output[i*nChannels    ] = smp * volume * leftScale;
                rAudio[i] = output[i*nChannels + 1] = smp * volume * rightScale;
            }
            // stereo
            else if (sample.getChannels() == 2) {
                lAudio[i] = output[i*nChannels    ] = sample.update() * volume * leftScale;
                rAudio[i] = output[i*nChannels + 1] = sample.update() * volume * rightScale;
            }
            else
            {
                lAudio[i]  = output[i*nChannels    ] = 0.0;
                rAudio[i]  = output[i*nChannels + 1] = 0.0;
            }
        }


    }
}

