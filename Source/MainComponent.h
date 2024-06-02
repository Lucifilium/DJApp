#pragma once
#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "DeckGUI.h"
#include "PlaylistComponent.h"

//==============================================================================
class MainComponent : public AudioAppComponent
{
    public:
        //==============================================================================
        MainComponent();
        ~MainComponent() override;

        //==============================================================================
        void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
        void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
        void releaseResources() override;

        //==============================================================================
        void paint (Graphics& g) override;
        void resized() override;

    private:
        //==============================================================================
        AudioFormatManager formatManager;
        AudioThumbnailCache thumbCache{100};

        int playerDeck1 = 0;
        int playerDeck2 = 1;

        DJAudioPlayer player1{ formatManager };
        DeckGUI deckGUI1{&player1, &playlistComponent, formatManager, thumbCache, playerDeck1 };
    
        DJAudioPlayer player2{ formatManager };
        DeckGUI deckGUI2{&player2, &playlistComponent, formatManager, thumbCache, playerDeck2 };

        MixerAudioSource mixerSource;

        PlaylistComponent playlistComponent{ formatManager };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};