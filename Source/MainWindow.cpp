#include "MainWindow.h"
#include "MainComponent.h"

namespace falconSound {

MainWindow::MainWindow(FalconSonificationApplication& application, juce::String name)
    : DocumentWindow(
        name,
        juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId),
        DocumentWindow::allButtons)
{
    setUsingNativeTitleBar(true);
    setContentOwned(new MainComponent(application), true);

    setResizable(true, false);
    centreWithSize(getWidth(), getHeight());
    setVisible(true);
}

void MainWindow::closeButtonPressed()
{
    juce::JUCEApplication::getInstance()->systemRequestedQuit();
}

}