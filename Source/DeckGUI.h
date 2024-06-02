#pragma once
#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"
#include "PlaylistComponent.h"

//==============================================================================
class DeckGUI  : public Component, 
                 public Button::Listener,
                 public Slider::Listener,
                 public FileDragAndDropTarget,
                 public Timer,
                 public TableListBoxModel
{
    public:
        DeckGUI(DJAudioPlayer* player, 
                PlaylistComponent* playlistComponent, 
                AudioFormatManager& formatManagerToUse, 
                AudioThumbnailCache& cacheToUse, 
                int playerDeckToUse);
        
        ~DeckGUI() override;

        void paint (Graphics&) override;
        void resized() override;

        /** implement Button::Listener */
        void buttonClicked(Button* button) override;

        /** implement Slider::Listener */
        void sliderValueChanged(Slider* slider) override;

        /** implement queue table */
        int getNumRows() override;
        void paintRowBackground(Graphics&, int rowNumber, int width, int height, bool rowIsSelected) override;
        void paintCell(Graphics&, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;

        /** implement file drag and drop*/
        bool isInterestedInFileDrag(const StringArray &files) override;
        void filesDropped(const StringArray& files, int x, int y) override;
        
        void timerCallback() override;  

    private:
    
        //buttons
        TextButton playButton{ "PLAY" };
        TextButton stopButton{ "STOP" };
        TextButton loadButton{ "LOAD" };
        TextButton playNextButton{ "PLAY NEXT" };
        TextButton refreshButton{ "REFRESH" };

        //sliders
        Slider volSlider;
        Slider speedSlider;
        Slider posSlider;

        //labels
        Label volLabel;   
        Label speedLabel;
        Label posLabel;

        FileChooser fChooser{ "Select a file..." };

        DJAudioPlayer* player;
        WaveformDisplay waveformDisplay;
        PlaylistComponent* playlistComponent;

        TableListBox queue;

        int playerDeck;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeckGUI)
};
