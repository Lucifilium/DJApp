#include <JuceHeader.h>
#include "WaveformDisplay.h"

//==============================================================================
WaveformDisplay::WaveformDisplay(AudioFormatManager& formatManagerToUse, AudioThumbnailCache& cacheToUse) 
                                : audioThumb(1000, formatManagerToUse, cacheToUse), fileLoaded(false), position(0)
{
    audioThumb.addChangeListener(this);
}

WaveformDisplay::~WaveformDisplay()
{
}

void WaveformDisplay::paint(Graphics& g)
{
    //background
    g.fillAll(Colours::black);

    //component outline
    g.setColour(Colours::grey);
    g.drawRect(getLocalBounds(), 1);

    //waveform colour
    g.setColour(Colours::lightgreen);

    //load file
    if (fileLoaded)
    {
        //playhead
        audioThumb.drawChannel(g, getLocalBounds(), 0, audioThumb.getTotalLength(), 0, 1.0f);
        g.setColour(Colours::white);
        g.drawRect(position * getWidth(), 0, getWidth() / 200, getHeight());

        //display name of track
        g.setColour(Colours::white);
        g.setFont(16.0f);
        g.drawText(nowPlaying, getLocalBounds(), Justification::centredBottom, true);
    }
    else
    {
        //file not loaded text
        g.setColour(Colours::red);
        g.setFont(20.0f);
        g.drawText("File not loaded...", getLocalBounds(), Justification::centred, true);
        
    }
}

void WaveformDisplay::resized()
{

}

void WaveformDisplay::loadURL(URL audioURL)
{
    audioThumb.clear();

    fileLoaded = audioThumb.setSource(new URLInputSource(audioURL));
    if (fileLoaded)
    {
        DBG("WaveformDisplay loaded");
        
        std::string justFile = audioURL.toString(false).toStdString();
        std::size_t startFilePos = justFile.find_last_of("/");
        std::size_t startExtPos = justFile.find_last_of(".");
        std::string extn = justFile.substr(startExtPos + 1, justFile.length() - startExtPos);
        std::string file = justFile.substr(startFilePos + 1, justFile.length() - startFilePos - extn.size() - 2);

        nowPlaying = file;
        repaint();
    }
    else
    {
        DBG("WaveformDisplay not loaded");
    }
}

void WaveformDisplay::changeListenerCallback(ChangeBroadcaster* source)
{
    repaint();
}

void WaveformDisplay::setPositionRelative(double pos)
{
    if (pos != position)
    {
        position = pos;
        repaint();
    }
}