#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "Sample.h"

class testApp : public ofBaseApp{

	public:


		void setup();
		void update();
		void draw();
		void exit();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);

		void audioRequested 	(float * input, int bufferSize, int nChannels);

		float 	pan;
		int		sampleRate;
		bool 	bRingModulation;
		float 	volume;
		float   speed;
		int		bufferSamples;

		float 	* lAudio;
		float   * rAudio;

		//------------------- for the simple sine wave synthesis
		float 	targetFrequency;
		float 	phase;
		float 	phaseAdder;
		float 	phaseAdderTarget;

		vector<MiniMaxima> waveForm;

		Sample sample;

		float deltax;
		float deltapos;
		float widthPct;
		bool bScrubMode;
		bool bReverse;
};

#endif
