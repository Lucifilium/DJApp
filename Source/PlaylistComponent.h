#pragma once
#include <JuceHeader.h>
#include <vector>
#include <string>

//==============================================================================

class PlaylistComponent  : public Component, 
                           public TableListBoxModel, 
                           public Button::Listener, 
                           public AudioSource, 
                           public FileDragAndDropTarget,
                           public TextEditor::Listener
{
public:
    PlaylistComponent(AudioFormatManager& formatManager);
    ~PlaylistComponent() override;

    void paint (Graphics&) override;
    void resized() override;

    int getNumRows() override;
    void paintRowBackground(Graphics&, int rowNumber, int width, int height, bool rowIsSelected) override;

    void paintCell(Graphics&, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;

    Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate) override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

    void buttonClicked(Button* button) override;

    bool isInterestedInFileDrag(const StringArray& files) override;
    void filesDropped(const StringArray& files, int x, int y) override;

    void textEditorTextChanged(TextEditor&) override;

    std::vector<std::string> playlistDeck1;
    std::vector<std::string> playlistDeck2;

private:

    AudioFormatManager& formatManager;
    std::unique_ptr<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;

    TextEditor searchBar;
    Label searchLabel;

    TableListBox tableComponent;

    std::vector<std::string> inputFiles;
    std::vector<std::string> trackTitles;
    std::vector<int> trackDuration;

    std::vector<std::string> searchFiles;
    std::vector<std::string> searchTitle;
    std::vector<int> searchDuration;

    void addToQueueList(std::string filepath, int playerDeck);
    void getAudioLength(URL audioURL);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlaylistComponent)
};
