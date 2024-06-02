#pragma once
#include <JuceHeader.h>

class DJAudioPlayer : public AudioSource
{
    public:
	
        DJAudioPlayer(AudioFormatManager& _formatManager);
        ~DJAudioPlayer();
    
        //==============================================================================
        void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
        void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
        void releaseResources() override;

        /** load the file */
        void loadURL(URL audioURL);
        /** set gain volume */
        void setGain(double gain);
        /** set speed of music */
        void setSpeed(double ratio);
        /** set position of music track */
        void setPosition(double posInSecs);
        void setPositionRelative(double pos);
        /** start playing the file */
        void start();
        /** stop playing the file */
        void stop();
        /** get the relative position of the playhead */
        double getPositionRelative();

    private:

        AudioFormatManager& formatManager;
        std::unique_ptr<AudioFormatReaderSource> readerSource;
        AudioTransportSource transportSource;
        ResamplingAudioSource resampleSource{&transportSource, false, 2};
};