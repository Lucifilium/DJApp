#include <JuceHeader.h>
#include "DeckGUI.h"

//==============================================================================
DeckGUI::DeckGUI(DJAudioPlayer* _player, 
                PlaylistComponent* _playlistComponent,
                AudioFormatManager& formatManagerToUse, 
                AudioThumbnailCache& cacheToUse, 
                int playerDeckToUse) 
                : player(_player), 
                playlistComponent(_playlistComponent),
                waveformDisplay(formatManagerToUse, cacheToUse),
                playerDeck(playerDeckToUse)
                
{
    //add and make visible
    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(volSlider);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(posSlider);
    addAndMakeVisible(waveformDisplay);
    addAndMakeVisible(volLabel);
    addAndMakeVisible(speedLabel);
    addAndMakeVisible(posLabel);
    addAndMakeVisible(queue);
    addAndMakeVisible(playNextButton);
    addAndMakeVisible(refreshButton);

    //add listeners
    playButton.addListener(this);
    stopButton.addListener(this);
    loadButton.addListener(this);
    playNextButton.addListener(this);
    refreshButton.addListener(this);
    volSlider.addListener(this);
    speedSlider.addListener(this);
    posSlider.addListener(this);

    //vol sliders and labels
    volSlider.setRange(0.0, 1.0);
    volSlider.setNumDecimalPlacesToDisplay(2);
    volSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    volSlider.setTextBoxStyle(Slider::TextBoxBelow,false, 50, volSlider.getTextBoxHeight());
    volSlider.setValue(0.5);
    volLabel.setText("Volume", dontSendNotification);
    volLabel.attachToComponent(&volSlider, false);
    volLabel.setJustificationType(Justification::centred);

    //speed sliders and labels
    speedSlider.setRange(0.25, 2.0);
    speedSlider.setNumDecimalPlacesToDisplay(2);
    speedSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    speedSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 50, volSlider.getTextBoxHeight());
    speedSlider.setValue(1);
    speedLabel.setText("Speed", dontSendNotification);
    speedLabel.attachToComponent(&speedSlider, false);
    speedLabel.setJustificationType(Justification::centred);

    //pos sliders and labels
    posSlider.setRange(0.0, 1.0);
    posSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    posLabel.setText("Playback", dontSendNotification);
    posLabel.attachToComponent(&posSlider, true);

    //queue table
    queue.getHeader().addColumn("In Queue", 1, 133);
    queue.setModel(this);

    startTimer(500);
}

DeckGUI::~DeckGUI()
{
    stopTimer();
}

void DeckGUI::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    g.setColour(Colours::grey);
    g.drawRect(getLocalBounds(), 1);
}

void DeckGUI::resized()
{
    int rowH = getHeight() / 10;
    int colW = getWidth() / 3;

    //waveform and playback slider
    waveformDisplay.setBounds(0, 0, getWidth(), rowH * 2);
    posSlider.setBounds(60, rowH * 2, getWidth() - 60, rowH);
   
    //play, stop and load buttons
    playButton.setBounds(0, rowH * 3, colW, rowH);
    stopButton.setBounds(colW, rowH * 3, colW, rowH);
    loadButton.setBounds(colW * 2, rowH * 3, colW, rowH);
   
    //volume and speed slider
    volSlider.setBounds(0, rowH * 5 - 10, colW, rowH * 3.5);
    speedSlider.setBounds(colW * 0.5, rowH * 5 - 10, colW, rowH * 3.5);
    
    //queue table and buttons
    queue.setBounds(colW * 1.5, rowH * 5 - 35, colW, rowH * 3.5);
    playNextButton.setBounds(colW * 1.5, rowH * 7.7, colW / 2, rowH / 1.2);
    refreshButton.setBounds(colW * 2, rowH * 7.7, colW / 2, rowH / 1.2);
}

void DeckGUI::buttonClicked(Button* button)
{
    if (button == &playButton)
    {
        player->start();
    }
    if (button == &stopButton)
    {
        player->stop();
    }
    if (button == &loadButton)
    {
        auto fileChooserFlags = FileBrowserComponent::canSelectFiles;
        fChooser.launchAsync(fileChooserFlags, [this](const FileChooser& chooser)
        {
            auto chosenFile = chooser.getResult();
            player->loadURL(URL{ chosenFile });
            waveformDisplay.loadURL(URL{ chosenFile });
        });
    }
    if (button == &refreshButton) //refresh queue table
    {
        DBG(" DeckGUI::buttonClicked: refreshButton");
        queue.updateContent();
    }
    if (button == &playNextButton) //plays next track
    {
        if (playerDeck == 0 && playlistComponent->playlistDeck1.size() > 0)
        {
            URL fileURL = URL{ File{playlistComponent->playlistDeck1[0]} };
            player->loadURL(fileURL);
            waveformDisplay.loadURL(fileURL);
            //deletes the first track in queue
            playlistComponent->playlistDeck1.erase(playlistComponent->playlistDeck1.begin());
            player->start();
        }
        if (playerDeck == 1 && playlistComponent->playlistDeck2.size() > 0)
        {
            URL fileURL = URL{ File{playlistComponent->playlistDeck2[0]} };
            player->loadURL(fileURL);
            waveformDisplay.loadURL(fileURL);
            //deletes the first track in queue
            playlistComponent->playlistDeck2.erase(playlistComponent->playlistDeck2.begin());
            player->start();
        }
    }
    queue.updateContent();
}

void DeckGUI::sliderValueChanged(Slider* slider)
{
    if (slider == &volSlider) //volume slider
    {
        player->setGain(slider->getValue());
    }
    if (slider == &speedSlider) //speed slider
    {
        player->setSpeed(slider->getValue());
    }
    if (slider == &posSlider) //playback slider
    {
        player->setPositionRelative(slider->getValue());
    }
}

//drag and drop files into deck
bool DeckGUI::isInterestedInFileDrag(const StringArray& files)
{
    return true;
}

void DeckGUI::filesDropped(const StringArray& files, int x, int y)
{
    for (String filename : files)
    {
        URL fileURL = URL{ File{filename} };
        player->loadURL(fileURL);
        waveformDisplay.loadURL(URL{ fileURL });
        return;
    }
}

int DeckGUI::getNumRows()
{
    if (playerDeck == 0) //left
    {
        return playlistComponent->playlistDeck1.size();
    }
    if (playerDeck == 1) //right
    {
        return playlistComponent->playlistDeck2.size();
    }
}

//set row colour in queue table
void DeckGUI::paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    g.fillAll(Colours::lightgreen);
}

void DeckGUI::paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    std::string filepath = "";
    if (playerDeck == 0) //left
    {
        filepath = playlistComponent->playlistDeck1[rowNumber];
    }
    if (playerDeck == 1) //right
    {
        filepath = playlistComponent->playlistDeck2[rowNumber];
    }

    //extract file name from path 
    std::size_t startFilePos = filepath.find_last_of("\\");
    std::size_t startExtPos = filepath.find_last_of(".");
    std::string extn = filepath.substr(startExtPos + 1, filepath.length() - startExtPos);
    std::string file = filepath.substr(startFilePos + 1, filepath.length() - startFilePos - extn.size() - 2);
    
    //draw name to each cell
    g.drawText(file, 1, rowNumber, width - 4, height, Justification::centredLeft, true);
}

void DeckGUI::timerCallback()
{
    waveformDisplay.setPositionRelative(player->getPositionRelative());
}