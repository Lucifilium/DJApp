#include <JuceHeader.h>
#include "PlaylistComponent.h"

//==============================================================================

PlaylistComponent::PlaylistComponent(AudioFormatManager& _formatManager) : formatManager(_formatManager)
{
    //playlist table
    tableComponent.getHeader().addColumn("Track title", 1, 400);
    tableComponent.getHeader().addColumn("Duration", 2, 100);
    tableComponent.getHeader().addColumn("Deck 1", 3, 100);
    tableComponent.getHeader().addColumn("Deck 2", 4, 100);
    tableComponent.getHeader().addColumn("Delete", 5, 50);
    tableComponent.setModel(this);
    addAndMakeVisible(tableComponent);

    //search bar
    addAndMakeVisible(searchBar);
    addAndMakeVisible(searchLabel);
    searchBar.addListener(this);
    searchLabel.setJustificationType(Justification::centred);
    searchLabel.setText("Search Track: ", dontSendNotification);
}

PlaylistComponent::~PlaylistComponent()
{
}

void PlaylistComponent::paint (Graphics& g)
{

    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (Colours::white);
    g.setFont (14.0f);
    g.drawText ("PlaylistComponent", getLocalBounds(), Justification::centred, true);   // draw some placeholder text
}

void PlaylistComponent::resized()
{
    int rowH = getHeight() / 8;
    int colW = getWidth() / 6;

    //playlist table
    tableComponent.setBounds(0, rowH / 3, getWidth(), getHeight());

    //search bar
    searchBar.setBounds(colW, 0, colW * 5 + 1, rowH / 3);
    searchLabel.setBounds(0, -25, colW, rowH);
}

int PlaylistComponent::getNumRows()
{
    return searchTitle.size();
}

void PlaylistComponent::paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
    {
        //colour of selected row
        g.fillAll(Colours::lightgreen);
    }
    else
    {
        //colour of unselected row
        g.fillAll(Colours::darkgrey);
    }
}

void PlaylistComponent::paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    if (columnId == 1)
    {
        g.drawText(searchTitle[rowNumber],1, rowNumber, width - 4, height, Justification::centredLeft, true);
    }
    if (columnId == 2)
    {
        g.drawText(std::to_string(searchDuration[rowNumber]) + "s", 1, rowNumber, width - 4, height, Justification::centredLeft, true);
    }
}

Component* PlaylistComponent::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate)
{
    if (columnId == 3)
    {
        if (existingComponentToUpdate == nullptr)
        {
            TextButton * btn = new TextButton("Add to Deck 1");
            String id{ std::to_string(rowNumber) };
            btn->setComponentID(id);
            btn->addListener(this);
            existingComponentToUpdate = btn;
            btn->setColour(TextButton::buttonColourId, Colours::blue);
        }
    }
    if (columnId == 4)
    {
        if (existingComponentToUpdate == nullptr)
        {
            TextButton* btn = new TextButton{ "Add to Deck 2" };
            //add 500 to id to identify in table columns
            String id{ std::to_string(rowNumber + 500) };
            btn->setComponentID(id);
            btn->addListener(this);
            existingComponentToUpdate = btn;
            btn->setColour(TextButton::buttonColourId, Colours::forestgreen);
        }
    }
    if(columnId == 5)
    {
        TextButton* btn = new TextButton{ "X" };
        //add 1000 to id to identify in table columns
        String id{ std::to_string(rowNumber + 1000) };
        btn->setComponentID(id);
        btn->addListener(this);
        existingComponentToUpdate = btn;
        btn->setColour(TextButton::buttonColourId, Colours::red);
    }

    return existingComponentToUpdate;
}

//audioSource virtual functions
void PlaylistComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {}
void PlaylistComponent::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) {}
void PlaylistComponent::releaseResources() {}

void PlaylistComponent::buttonClicked(Button* button)
{
    //get id of button 
    int id = std::stoi(button->getComponentID().toStdString());
    //if id is less than 500, track goes to left queue
    if (id < 500)
    {
        DBG("Add to deck 1 clicked");
        addToQueueList(searchFiles[id], 0);
    }
    //if id is 500 or more and less than 1000, track goes to right queue
    else if (id >= 500 && id < 1000)
    {
        DBG("Add to deck 2 clicked");
        addToQueueList(searchFiles[id - 500], 1);
    }
    else if (id >= 1000)
    {
        //if id is 1000 or more, track will be deleted
        DBG("X clicked");
        trackTitles.erase(trackTitles.begin());
        searchTitle.erase(searchTitle.begin());
        tableComponent.updateContent();
    }
}

//drag and drop files into playlist table
bool PlaylistComponent::isInterestedInFileDrag(const StringArray& files)
{
    return true;
}

void PlaylistComponent::filesDropped(const StringArray& files, int x, int y)
{
    for (String filename : files)
    {
        //for each file URL, get filepath and file name 
        std::string filepath = String(filename).toStdString();
        std::size_t startFilePos = filepath.find_last_of("\\");
        std::size_t startExtPos = filepath.find_last_of(".");
        std::string extn = filepath.substr(startExtPos + 1, filepath.length() - startExtPos);
        std::string file = filepath.substr(startFilePos + 1, filepath.length() - startFilePos - extn.size() - 2);

        //update vectors for file details
        inputFiles.push_back(filepath);
        trackTitles.push_back(file);

        //compute audio length
        getAudioLength(URL{ File{filepath} });
    }

    searchTitle = trackTitles;
    searchFiles = inputFiles;

    //update playlist table
    tableComponent.updateContent();
}

void PlaylistComponent::textEditorTextChanged(TextEditor& textEditor)
{
    //clears table when searching
    searchTitle.clear();
    searchDuration.clear();
    searchFiles.clear();

    int pos = 0;
    for (std::string track : trackTitles)
    {
        //check substring of track
        if (track.find(searchBar.getText().toStdString()) != std::string::npos)
        {
            //push the track to table
            searchTitle.push_back(trackTitles[pos]);
            searchDuration.push_back(trackDuration[pos]);
            searchFiles.push_back(inputFiles[pos]);
        }
        ++pos;
    }
    tableComponent.updateContent();
}

//add track to queue
void PlaylistComponent::addToQueueList(std::string filepath, int playerDeck)
{
    if (playerDeck == 0) //left
    {
        playlistDeck1.push_back(filepath);
    }
    if (playerDeck == 1) //right
    {
        playlistDeck2.push_back(filepath);
    }
}

void PlaylistComponent::getAudioLength(URL audioURL)
{
    auto* reader = formatManager.createReaderFor(audioURL.createInputStream(false));

    if (reader != nullptr) // good file!
    {
        std::unique_ptr<AudioFormatReaderSource> newSource(new AudioFormatReaderSource(reader, true));
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset(newSource.release());
        int trackLength = transportSource.getLengthInSeconds(); // get length of audio
        trackDuration.push_back(trackLength); // add audio length to vector
    }
    searchDuration = trackDuration;
}