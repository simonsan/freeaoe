/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "Engine.h"

#include <sstream>

#include <SFML/Graphics.hpp>

#include "ui/UiScreen.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include "resource/AssetManager.h"
#include "resource/Resource.h"
#include "render/GraphicRender.h"
#include "global/Config.h"
#include <genie/resource/SlpFile.h>
#include <genie/resource/UIFile.h>

const sf::Clock Engine::GameClock;

//------------------------------------------------------------------------------
void Engine::start()
{
    DBG << "Starting engine.";

    ScreenPos mousePos;
    // Start the game loop
    while (renderWindow_->isOpen()) {
        std::shared_ptr<GameState> state = state_manager_.getActiveState();

        const int renderStart = GameClock.getElapsedTime().asMilliseconds();

        bool updated = false;

        // Process events
        sf::Event event;
        while (renderWindow_->pollEvent(event)) {
            // Close window : exit
            if (event.type == sf::Event::Closed) {
                renderWindow_->close();
            }

            if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased) {
                sf::Vector2f mappedPos = renderWindow_->mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                event.mouseButton.x = mappedPos.x;
                event.mouseButton.y = mappedPos.y;
                mousePos = ScreenPos(mappedPos);
            }

            if (event.type == sf::Event::MouseMoved) {
                sf::Vector2f mappedPos = renderWindow_->mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
                event.mouseMove.x = mappedPos.x;
                event.mouseMove.y = mappedPos.y;
                mousePos = ScreenPos(mappedPos);
            }

            if (!handleEvent(event)) {
                state->handleEvent(event);
            }

            updated = true;
        }

        if (!m_currentDialog) {
            updated = state->update(GameClock.getElapsedTime().asMilliseconds()) || updated;
        } else {
            state->cursor().sprite.setPosition(mousePos);
        }

        if (updated) {
            // Clear screen
            renderWindow_->clear(sf::Color::Green);
            state->draw();
            if (m_currentDialog) {
                m_currentDialog->render(renderWindow_);
            }

            drawButtons();
            const int renderTime = GameClock.getElapsedTime().asMilliseconds() - renderStart;

            if (renderTime > 0) {
                fps_label_.setString("fps: " + std::to_string(1000/renderTime));
            }

            renderWindow_->draw(fps_label_);
            renderWindow_->draw(state->cursor().sprite);

            // Update the window
            renderWindow_->display();
        } else {
            sf::sleep(sf::milliseconds(1000 / 60));
        }

    }
}

void Engine::showStartScreen()
{
    genie::UIFilePtr uiFile = AssetManager::Inst()->getUIFile("scrstart.sin");
    if (!uiFile) {
        WARN << "failed to load ui file for start screen";
        return;
    }

    genie::SlpFilePtr loadingImageFile = AssetManager::Inst()->getSlp(uiFile->backgroundSmall.fileId);
    if (!loadingImageFile) {
        WARN << "Failed to load background for start screen" << uiFile->backgroundSmall.filename << uiFile->backgroundSmall.alternateFilename;
        return;
    }

    sf::Texture loadingScreen;
    loadingScreen.loadFromImage(Resource::convertFrameToImage(
                                    loadingImageFile->getFrame(0),
                                    AssetManager::Inst()->getPalette(uiFile->paletteFile.id)
                                    ));

    sf::Sprite sprite;
    sprite.setTexture(loadingScreen);
    sprite.setPosition(0, 0);
    sprite.setScale(renderWindow_->getSize().x / float(loadingScreen.getSize().x),
                    renderWindow_->getSize().y / float(loadingScreen.getSize().y));
    renderTarget_->draw(sprite);
    renderWindow_->display();
}

void Engine::loadTopButtons()
{
    genie::SlpFilePtr buttonsFile = AssetManager::Inst()->getSlp("btngame2x.shp");
    if (!buttonsFile) {
        WARN << "Failed to load SLP for buttons";
        return;
    }

    if (buttonsFile->getFrameCount() < TopMenuButton::ButtonsCount * 2) {
        WARN << "Not enough buttons";
    }

    int x = renderWindow_->getSize().x - 5;
    for (int i=0; i<TopMenuButton::ButtonsCount; i++) {
        TopMenuButton button;
        button.type = TopMenuButton::Type(i);

        button.texture.loadFromImage(Resource::convertFrameToImage(buttonsFile->getFrame(i * 2)));
        button.pressedTexture.loadFromImage(Resource::convertFrameToImage(buttonsFile->getFrame(i * 2 + 1)));

        button.rect.setSize(button.texture.getSize());
        x -= button.rect.width;

        button.rect.x = x;
        button.rect.y = 5;

        m_buttons.push_back(std::move(button));
    }
}

void Engine::drawButtons()
{
    for (const TopMenuButton &button : m_buttons) {
        sf::Sprite sprite;
        if (button.type == m_pressedButton) {
            sprite.setTexture(button.pressedTexture);
        } else {
            sprite.setTexture(button.texture);
        }

        sprite.setPosition(button.rect.topLeft());
        renderWindow_->draw(sprite);
    }
}

bool Engine::handleEvent(sf::Event event)
{
    if (m_currentDialog) {
        Dialog::Choice choice = m_currentDialog->handleEvent(event);
        if (choice == Dialog::Cancel) {
            m_currentDialog.reset();
        } else if (choice == Dialog::Quit) {
            renderWindow_->close();
        }

        return true;
    }

    if (event.type == sf::Event::MouseButtonReleased) {
        if (m_pressedButton == TopMenuButton::GameMenu) {
            showMenu();
        }

        if (m_pressedButton != TopMenuButton::Invalid) {
            m_pressedButton = TopMenuButton::Invalid;
            return true;
        }

        return false;
    }

    if (event.type != sf::Event::MouseButtonPressed) {
        return false;
    }
    const ScreenPos mousePos(event.mouseButton.x, event.mouseButton.y);
    for (const TopMenuButton &button : m_buttons) {
        if (button.rect.contains(mousePos)) {
            m_pressedButton = button.type;
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
Engine::Engine()
{
    m_mainScreen = std::make_unique<UiScreen>("dlg_men.sin");
}

bool Engine::setup(const std::shared_ptr<genie::ScnFile> &scenario)
{
    renderWindow_ = std::make_unique<sf::RenderWindow>(sf::VideoMode(1280, 1024), "freeaoe", sf::Style::None);
    renderWindow_->setMouseCursorVisible(false);
    renderWindow_->setFramerateLimit(60);

    m_mainScreen->setRenderWindow(renderWindow_);
    m_mainScreen->init();

    renderTarget_ = std::make_shared<SfmlRenderTarget>(*renderWindow_);

    showStartScreen();

    std::shared_ptr<GameState> gameState = std::make_shared<GameState>(renderTarget_);
    if (scenario) {
        gameState->setScenario(scenario);
    }

    if (!state_manager_.addActiveState(gameState)) {
        return false;
    }

    renderWindow_->setSize(gameState->uiSize());
    renderTarget_->setSize(gameState->uiSize());

    fps_label_.setPosition(sf::Vector2f(gameState->uiSize().width - 75, gameState->uiSize().height - 20));
    fps_label_.setFillColor(sf::Color::White);
    fps_label_.setFont(SfmlRenderTarget::defaultFont());
    fps_label_.setCharacterSize(15);

    loadTopButtons();

    return true;
}

void Engine::showMenu()
{
    genie::UIFilePtr uiFile = AssetManager::Inst()->getUIFile("dlg_men.sin");
    if (!uiFile) {
        WARN << "failed to load ui file for menu";
        return;
    }

    genie::SlpFilePtr backgroundSlp = AssetManager::Inst()->getSlp(uiFile->backgroundSmall.fileId);
    if (!backgroundSlp) {
        WARN << "Failed to load menu background";
        return;
    }
    sf::Image menuBg = Resource::convertFrameToImage(backgroundSlp->getFrame(0));

    m_currentDialog = std::make_unique<Dialog>(m_mainScreen.get());
    if (!m_currentDialog->background.loadFromImage(menuBg)) {
        WARN << "Failed to load menu background";
    }
    DBG << "showing menu";

}

Dialog::Dialog(UiScreen *screen) :
    m_screen(screen)
{
    m_buttons[Quit].text = "Quit";
    m_buttons[Achievements].text = "Achivements (TODO)";
    m_buttons[Save].text = "Save (TODO)";
    m_buttons[Options].text = "Options (TODO)";
    m_buttons[About].text = "About (TODO)";
    m_buttons[Cancel].text = "Cancel";

}

void Dialog::render(std::shared_ptr<sf::RenderWindow> &renderTarget)
{
    Size windowSize = renderTarget->getSize();
    Size textureSize(295, 300); //background.getSize(); can't use the actual size, because of the shadow...
    const ScreenPos windowCenter(windowSize.width / 2, windowSize.height / 2);
    ScreenPos position(windowCenter.x - textureSize.width/2, windowCenter.y - textureSize.height/2);
    sf::Sprite sprite;
    sprite.setPosition(position);
    sprite.setTexture(background);
    renderTarget->draw(sprite);

    const int buttonWidth = textureSize.width - 80;
    const int buttonHeight = 30;
    const int buttonMargin = 10;

    const int allButtonsHeight = ChoicesCount * (buttonHeight + buttonMargin);


    const int x = windowCenter.x - buttonWidth/2;
    int y = windowCenter.y - allButtonsHeight / 2;

    for (int i=0; i<ChoicesCount; i++) {
        m_buttons[i].rect.x = x;
        m_buttons[i].rect.y = y;

        m_buttons[i].rect.height = buttonHeight;
        m_buttons[i].rect.width = buttonWidth;
        y += buttonHeight + buttonMargin;

        m_buttons[i].render(m_screen);
    }
}

Dialog::Choice Dialog::handleEvent(const sf::Event &event)
{
    if (event.type == sf::Event::MouseButtonReleased) {
        const ScreenPos mousePos(event.mouseButton.x, event.mouseButton.y);
        Choice choice = Invalid;
        for (int i=0; i<ChoicesCount; i++) {
            m_buttons[i].pressed = false;

            if (m_buttons[i].rect.contains(mousePos)) {
                choice = Choice(i);
            }
        }

        if (choice != m_pressedButton) {
            m_pressedButton = Invalid;

        }

        return m_pressedButton;
    }
    if (event.type != sf::Event::MouseButtonPressed) {
        return Invalid;
    }
    const ScreenPos mousePos(event.mouseButton.x, event.mouseButton.y);
    for (int i=0; i<ChoicesCount; i++) {
        if (m_buttons[i].rect.contains(mousePos)) {
            m_pressedButton = Choice(i);
            m_buttons[i].pressed = true;
        } else {
            m_buttons[i].pressed = false;
        }
    }

    return Invalid;
}
