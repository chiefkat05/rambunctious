#include "../headers/dungeon.h"
#include "../headers/system.h"
#include "../headers/gamestate.h"
#include "../headers/collision.h"

float mouseX, mouseY;
bool mousePressed, mouseClicked, mouseReleased;
float massScale = 1.0f, massYOffset = 0.0f;
float delta_time = 0.0f;
bool buttonHovered = false;

void dungeonInit(game_system &game, dungeon &dg);

void unitControl(game_system &game, player &p, sf::RenderWindow &window, dungeon *floor)
{
    static sprite sTopLeft("../img/ui/unit-control/playerselect-c.png", 0.0f, 0.0f, 8.0f, 8.0f, 1, 1);
    static sprite sTopRight("../img/ui/unit-control/playerselect-c-h.png", 0.0f, 0.0f, 8.0f, 8.0f, 1, 1);
    static sprite sBottomLeft("../img/ui/unit-control/playerselect-c-v.png", 0.0f, 0.0f, 8.0f, 8.0f, 1, 1);
    static sprite sBottomRight("../img/ui/unit-control/playerselect-c-vh.png", 0.0f, 0.0f, 8.0f, 8.0f, 1, 1);
    static sprite sLeft("../img/ui/unit-control/playerselect-h.png", 0.0f, 0.0f, 8.0f, 8.0f, 1, 1);
    static sprite sRight("../img/ui/unit-control/playerselect-h.png", 0.0f, 0.0f, 8.0f, 8.0f, 1, 1);
    static sprite sBottom("../img/ui/unit-control/playerselect-w.png", 0.0f, 0.0f, 8.0f, 8.0f, 1, 1);
    static sprite sTop("../img/ui/unit-control/playerselect-w.png", 0.0f, 0.0f, 8.0f, 8.0f, 1, 1);
    static sprite sMove("../img/ui/unit-control/playertarget-move.png", 0.0f, 0.0f, 16.0f, 16.0f, 1, 1);
    static sprite sAttack("../img/ui/unit-control/playertarget-attack.png", 0.0f, 0.0f, 16.0f, 16.0f, 1, 1);
    static float boxClickX, boxClickY, boxMinX, boxMinY, boxMaxX, boxMaxY;

    if (mouseClicked && !buttonHovered)
    {
        for (int i = 0; i < character_limit; ++i)
        {
            p.selected[i] = false;
        }

        boxClickX = mouseX;
        boxClickY = mouseY - massYOffset / massScale;
        boxMinX = mouseX;
        boxMinY = mouseY - massYOffset / massScale;
    }
    if (mousePressed && !buttonHovered)
    {
        boxClickX -= floor->screenChangeDistanceX / massScale;
        boxClickY -= floor->screenChangeDistanceY / massScale;

        if (mouseX > boxClickX)
        {
            boxMaxX = mouseX;
            boxMinX = boxClickX;
        }
        if (mouseX <= boxClickX)
        {
            boxMinX = mouseX;
            boxMaxX = boxClickX;
        }
        if (mouseY > boxClickY)
        {
            boxMaxY = mouseY - massYOffset / massScale;
            boxMinY = boxClickY;
        }
        if (mouseY <= boxClickY)
        {
            boxMinY = mouseY - massYOffset / massScale;
            boxMaxY = boxClickY;
        }

        p.select(boxMinX - floor->screenPositionX / massScale, boxMinY - floor->screenPositionY / massScale - massYOffset / massScale,
                 boxMaxX - floor->screenPositionX / massScale, boxMaxY - floor->screenPositionY / massScale - massYOffset / massScale);

        sTopLeft.Put(boxMinX * massScale, boxMinY * massScale);
        window.draw(sTopLeft.rect);
        sTopRight.Put(boxMaxX * massScale - 8.0f, boxMinY * massScale);
        window.draw(sTopRight.rect);
        sBottomLeft.Put(boxMinX * massScale, boxMaxY * massScale - 8.0f);
        window.draw(sBottomLeft.rect);
        sBottomRight.Put(boxMaxX * massScale - 8.0f, boxMaxY * massScale - 8.0f);
        window.draw(sBottomRight.rect);
        sTop.Put(boxMinX * massScale + 8, boxMinY * massScale);
        sTop.rect.setTextureRect(sf::IntRect(0, 0, (boxMaxX - boxMinX) * massScale - 16, 8));
        window.draw(sTop.rect);
        sBottom.Put(boxMinX * massScale + 8, boxMaxY * massScale - 3);
        sBottom.rect.setTextureRect(sf::IntRect(0, 0, (boxMaxX - boxMinX) * massScale - 16, 8));
        window.draw(sBottom.rect);

        sLeft.Put(boxMinX * massScale, boxMinY * massScale + 8);
        sLeft.rect.setTextureRect(sf::IntRect(0, 0, 8, (boxMaxY - boxMinY) * massScale - 16));
        window.draw(sLeft.rect);
        sRight.Put(boxMaxX * massScale - 3, boxMinY * massScale + 8);
        sRight.rect.setTextureRect(sf::IntRect(0, 0, 8, (boxMaxY - boxMinY) * massScale - 16));
        window.draw(sRight.rect);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        for (int i = 0; i < character_limit; ++i)
        {
            if (!p.selected[i] || p.allies[i].visual.empty)
                continue;

            p.allies[i].target = nullptr;
            p.allies[i].MoveTo(p.allies[i].visual.rect.getPosition().x - 1.0f * massScale, p.allies[i].visual.rect.getPosition().y, floor);

            // Options:

            // Width and Height indicators on selection box???
            // Draw concepts, update a character to 32 bit and do simple animations for all actions
            // Design intro with one starter character and make music track for it
            // Implement sfml audio support
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        for (int i = 0; i < character_limit; ++i)
        {
            if (!p.selected[i] || p.allies[i].visual.empty)
                continue;

            p.allies[i].target = nullptr;
            p.allies[i].MoveTo(p.allies[i].visual.rect.getPosition().x + 1.0f * massScale, p.allies[i].visual.rect.getPosition().y, floor);
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        for (int i = 0; i < character_limit; ++i)
        {
            if (!p.selected[i] || p.allies[i].visual.empty)
                continue;

            p.allies[i].target = nullptr;
            p.allies[i].MoveTo(p.allies[i].visual.rect.getPosition().x, p.allies[i].visual.rect.getPosition().y - 1.0f * massScale, floor);
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        for (int i = 0; i < character_limit; ++i)
        {
            if (!p.selected[i] || p.allies[i].visual.empty)
                continue;

            p.allies[i].target = nullptr;
            p.allies[i].MoveTo(p.allies[i].visual.rect.getPosition().x, p.allies[i].visual.rect.getPosition().y + 1.0f * massScale, floor);
        }
    }
    if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
    {
        int selectedCount = 0;
        float averageX = 0, averageY = 0;
        character *targeted = nullptr;

        for (int i = 0; i < game.characterCount; ++i)
        {
            if (game.characters[i] == nullptr || game.characters[i]->id != CH_MONSTER && game.characters[i]->id != CH_EVIL)
                continue;

            if (mouseX - floor->screenPositionX / massScale > game.characters[i]->posX / massScale - game.characters[i]->selectionSize / massScale &&
                mouseX - floor->screenPositionX / massScale < game.characters[i]->posX / massScale + game.characters[i]->selectionSize / massScale &&
                mouseY - floor->screenPositionY / massScale > game.characters[i]->posY / massScale + massYOffset - game.characters[i]->selectionSize / massScale &&
                mouseY - floor->screenPositionY / massScale < game.characters[i]->posY / massScale + massYOffset + game.characters[i]->selectionSize / massScale && game.characters[i]->hp > 0)
            {
                targeted = game.characters[i];
                break;
            }
        }

        // two for-loops seems redundant, please optimize
        for (int i = 0; i < character_limit; ++i)
        {
            if (!p.selected[i] || p.allies[i].visual.empty)
                continue;

            averageX += p.allies[i].posX;
            averageY += p.allies[i].posY;
            ++selectedCount;
        }
        averageX /= selectedCount;
        averageY /= selectedCount;
        for (int i = 0; i < character_limit; ++i)
        {
            if (!p.selected[i] || p.allies[i].visual.empty)
                continue;

            float offsetX = p.allies[i].posX - averageX;
            float offsetY = p.allies[i].posY - averageY;

            p.allies[i].MoveTo(mouseX * massScale + offsetX,
                               mouseY * massScale + offsetY - massYOffset * massScale, floor);

            p.allies[i].target = targeted;
            if (targeted != nullptr)
            {
                sAttack.Put(mouseX * massScale - 8.0f, mouseY * massScale - massYOffset * massScale - 16.0f);
                window.draw(sAttack.rect);
            }
            else
            {
                sMove.Put(mouseX * massScale - 8.0f, mouseY * massScale - massYOffset * massScale - 16.0f);
                window.draw(sMove.rect);
            }
        }
    }
    buttonHovered = false;
}

float current_time = 0.0f;
float previous_time = 0.0f;

void mouseUpdate()
{
    mouseClicked = false;
    mouseReleased = false;
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !mousePressed)
    {
        mouseClicked = true;
        mousePressed = true;
    }

    if (!sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        if (mousePressed)
            mouseReleased = true;
        mouseClicked = false;
        mousePressed = false;
    }
}

int prevState = -1;
int hpAmounts[character_limit];
int entityHP[entity_limit];
int entityHP_UI_Index = 0;
int expAmounts[character_limit];
int abilitySelection[att_limit];
int abilityImageID[att_limit];
// edit all guis here
void menuData(game_system &mainG, player &mainP, dungeon &floor)
{
    if (state == prevState)
        return;

    gui_data.elements.clear();

    std::string temp_path;
    switch (state)
    {
    case START_SCREEN:
        if (!mainG.game_music.openFromFile("../snd/mus/01.mp3"))
        {
            std::cout << "failed to load ../snd/mus/01.mp3\n";
        }
        gui_data.background = sprite("../img/ui/backgrounds/start.png", 0.0f, 0.0f, 256.0f, 128.0f, 1, 1);
        gui_data.background.rect.setColor(sf::Color(255, 255, 255, 255));
        gui_data.elements.push_back(ui_element(UI_CLICKABLE, "../img/ui/buttons/start.png", 40.0f, 40.0f, 16.0f, 16.0f, 1, 1, startGame));
        break;
    case MENU_SCREEN:
        gui_data.background = sprite("../img/ui/backgrounds/menu.png", 0.0f, 0.0f, 256.0f, 128.0f, 3, 1);
        gui_data.background.rect.setColor(sf::Color(255, 255, 255, 255));
        gui_data.elements.push_back(ui_element(UI_CLICKABLE, "../img/ui/buttons/play.png", 40.0f, 40.0f, 16.0f, 16.0f, 1, 1, startGame));
        gui_data.elements.push_back(ui_element(UI_CLICKABLE, "../img/ui/buttons/quit.png", 80.0f, 40.0f, 16.0f, 16.0f, 1, 1, quitGame));
        break;
    case CHARACTER_CREATION_SCREEN:
        gui_data.background = sprite("../img/ui/backgrounds/character.png", 0.0f, 0.0f, 256.0f, 128.0f, 1, 1);
        gui_data.background.rect.setColor(sf::Color(255, 255, 255, 255));
        gui_data.elements.push_back(ui_element(UI_CLICKABLE, "../img/ui/buttons/play.png", 40.0f, 40.0f, 16.0f, 16.0f, 1, 1, startGame));
        break;
    case DUNGEON_SCREEN:
        dungeonInit(mainG, floor);
        if (!mainG.game_music.openFromFile("../snd/mus/Blue-1.mp3"))
        {
            std::cout << "failed to load ../snd/mus/Blue-1.mp3\n";
        }
        gui_data.background = sprite("../img/ui/backgrounds/dungeon.png", 0.0f, 0.0f, 256.0f, 128.0f, 1, 1);
        gui_data.background.rect.setColor(sf::Color(255, 255, 255, 255));
        // gui_data.background.rect.setPosition(-500.0f, -500.0f);
        gui_data.background.rect.setTextureRect(sf::IntRect(0, 0, 3200, 128));

        entityHP_UI_Index = gui_data.elements.size();
        for (int i = 0; i < mainG.characterCount; ++i)
        {
            if (mainG.characters[i]->visual.empty)
                continue;

            // gui_data.elements.push_back(ui_element(UI_VALUEISFRAME, "../img/ui/health.png", mainG.characters[i]->posX, mainG.characters[i]->posY,
            //    16.0f, 4.0f, 15, 1, nullFunc, nullptr, nullptr, 0, &entityHP[i]));
        }

        gui_data.elements.push_back(ui_element(UI_IMAGE, "../img/ui/dungeon_ui.png", 0.0f, 0.0f, 256.0f, 128.0f, 1, 1, nullFunc));
        for (int i = 0; i < character_limit; ++i)
        {
            if (mainP.allies[i].visual.empty)
                continue;

            mainP.allies[i].posX = floor.spawnLocationX;
            mainP.allies[i].posY = floor.spawnLocationY + i * 16.0f;
            mainP.allies[i].walkToX = floor.spawnLocationX;
            mainP.allies[i].walkToY = floor.spawnLocationY + i * 16.0f;

            // temp_path = std::string("../img/ui/icons/" + mainP.allies[i]._class.name + ".png");

            // gui_data.elements.push_back(ui_element(UI_IMAGE, temp_path.c_str(), 225.0f, 6.0f + i * 20.0f, 12.0f, 14.0f, 1, 1, nullFunc));

            // gui_data.elements.push_back(ui_element(UI_CLICKABLE, "../img/ui/profile_card.png", 224.0f, 5.0f + i * 20.0f, 32.0f, 16.0f, 1, 1, characterMenu, &mainP, nullptr, i));
            gui_data.elements.push_back(ui_element(UI_CLICKABLE, "../img/ui/goto_character.png", 120.0f, 100.0f + i * 20.0f, 7.0f, 7.0f, 1, 1, gotoCharacter, &mainP, &floor, i));
            // gui_data.elements.push_back(ui_element(UI_VALUEISFRAME, "../img/ui/health.png", 238.0f, 9.0f + i * 20.0f, 16.0f, 4.0f, 15, 1, nullFunc, nullptr, nullptr, 0, &hpAmounts[i]));
            // gui_data.elements.push_back(ui_element(UI_VALUEISFRAME, "../img/ui/experience.png", 150.0f, 100.0f + i * 20.0f, 24.0f, 2.0f, 25, 1, nullFunc,
            //    nullptr, nullptr, 0, &expAmounts[i]));
        }

        // for (int j = 0; j < att_limit; ++j)
        // {
        //     gui_data.elements.push_back(ui_element(
        //         UI_CLICKABLE, "../img/ui/ability_card.png", 2.0f + j * 26.0f, 100.0f, 24.0f, 24.0f, 2, 1, characterAbility, &mainP, nullptr, j));

        //     gui_data.elements.push_back(ui_element(
        //         UI_VALUEISFRAME, "../img/ui/ability_select.png", 2.0f + j * 26.0f, 100.0f, 24.0f, 24.0f, 2, 1,
        //         nullptr, nullptr, nullptr, 0, &abilitySelection[j]));
        // }
        break;
    case LOSE_SCREEN:
        // if (!mainG.game_music.openFromFile("../snd/mus/fail.mp3"))
        // {
        //     std::cout << "failed to load ../snd/mus/fail.mp3\n";
        // }
        // gui_data.elements.push_back(ui_element(UI_IMAGE, "../img/ui/game_over.png", 0.0f, 0.0f, 256.0f, 128.0f, 1, 1, nullFunc));
        break;
    case WIN_SCREEN:
        if (!mainG.game_music.openFromFile("../snd/mus/win.mp3"))
        {
            std::cout << "failed to load ../snd/mus/win.mp3\n";
        }
        gui_data.elements.push_back(ui_element(UI_IMAGE, "../img/ui/victory.png", 0.0f, 0.0f, 256.0f, 128.0f, 1, 1, nullFunc));
        break;
    default:
        break;
    }

    // gui_data.menuBG = animation(&gui_data.background, 0, gui_data.background.framesX - 1, 18.0f);

    prevState = state;
}

bool pauseKeyHeld = false, uiKeyHeld = false, showUI = true;
void playerInit(player &pl, game_system &game)
{
    ch_class walker_class("walker", 10, 1, 10.0f, 100.0f, 100.0f, 100, ability_null);

    pl.allies[0] = character("../img/classes/walker/traveller.png", 120.0f, 40.0f, 16.0f, 16.0f, 1, 1, CH_PLAYER, walker_class);

    pl.allies[0].SetAnimation(ANIM_IDLE, 0, 8, 150.0f);
    pl.allies[0].SetAnimation(ANIM_HURT, 9, 14, 150.0f);
    pl.allies[0].SetAnimation(ANIM_ABILITY_0, 15, 21, 300.0f);

    game.Add(&pl);
}
void dungeonInit(game_system &game, dungeon &dg)
{
    dg = dungeon("../img/ui/empty.png", 64.0f, 64.0f, massScale, massYOffset);
    dg.readRoomFile("../dungeons/road.sdf", massScale, massYOffset);
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(256, 128), "rambunctious_alpha_0.0");
    sf::View screen(sf::FloatRect(0.0f, 0.0f, 256.0f, 128.0f));

    window.setView(screen);

    game_system game;
    sf::Clock sfClock;
    sf::Time sfTime;

    dungeon mainDungeon;
    player mainPlayer;
    playerInit(mainPlayer, game);

    sprite resolutionBlinderTop("../img/ui/resolution_blinder.png", 0.0f, massYOffset * massScale, 1.0f, 1.0f, 1, 1);
    sprite resolutionBlinderBottom("../img/ui/resolution_blinder.png", 0.0f, massYOffset * massScale, 1.0f, 1.0f, 1, 1);

    particlesystem snowparticles("../img/particles/snow.png", 1.0f, 1.0f, 0, 0, 4, 4, 64);
    game.Add(&snowparticles);

    while (window.isOpen())
    {
        mouseX = sf::Mouse::getPosition(window).x;
        mouseY = sf::Mouse::getPosition(window).y;

        previous_time = current_time;
        sfTime = sfClock.getElapsedTime();
        current_time = sfTime.asSeconds();
        delta_time = current_time - previous_time;

        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::Resized)
            {
                float ratio = static_cast<float>(window.getSize().x) / static_cast<float>(window.getSize().y);
                massScale = 256.0f / static_cast<float>(window.getSize().x);
                massYOffset = (1 - ratio * 0.5f) * static_cast<float>(window.getSize().y * massScale * 0.5f);
                screen.setSize(256.0f, 256.0f / ratio);
                window.setView(screen);
            }
        }

        mouseUpdate();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) // player can walk through gaps in walls for some reason
            window.close();

        if (!pauseKeyHeld && sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            game.paused = !game.paused;
        }
        pauseKeyHeld = false;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            pauseKeyHeld = true;

        window.clear();

        menuData(game, mainPlayer, mainDungeon);
        game.handleMusic();

        gui_data.background.rect.setPosition(mainDungeon.screenPositionX, mainDungeon.screenPositionY);
        window.draw(gui_data.background.rect);
        if (state == DUNGEON_SCREEN && mainDungeon.dungeonInitialized)
        {
            snowparticles.spawn(-5.0f, 0.0f, -2.0f, 128.0f, 20.0f, 50.0f, 50.0f, -10.0f, 150.0f, 10.0f, mainDungeon.screenPositionX, mainDungeon.screenPositionY);
            mainDungeon.updateScreenPosition(mouseX, mouseY, delta_time, massScale, massYOffset);

            mainDungeon.draw(&window);
            game.update(mainDungeon, delta_time);
            unitControl(game, mainPlayer, window, &mainDungeon);

            mainPlayer.firstUnitSelected = -1;
            for (int i = 0; i < character_limit; ++i)
            {
                if (mainPlayer.firstUnitSelected == -1 && mainPlayer.selected[i])
                {
                    mainPlayer.firstUnitSelected = i;
                }

                if (mainPlayer.allies[i].visual.empty)
                    continue;

                expAmounts[i] = 24 - static_cast<int>((static_cast<float>(mainPlayer.allies[i].experiencePoints) /
                                                       static_cast<float>(mainPlayer.allies[i]._class.experienceToLvlUp)) *
                                                      24.0f);
            }
            for (int i = 0; i < game.characterCount; ++i)
            {
                if (game.characters[i]->visual.empty)
                    continue;

                window.draw(game.characters[i]->visual.rect);
            }

            if (mainPlayer.allies[0].posX <= -2.0f)
            {
                state = WIN_SCREEN;
            }
        }
        for (int i = 0; i < game.particlesystemcount; ++i)
        {
            for (int j = 0; j < game.particles[i]->particle_count; ++j)
            {
                window.draw(game.particles[i]->particles[j].visual.rect);
            }
        }
        if (!uiKeyHeld && sf::Keyboard::isKeyPressed(sf::Keyboard::Tab))
        {
            showUI = !showUI;
        }
        uiKeyHeld = false;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Tab))
        {
            uiKeyHeld = true;
        }
        if (showUI)
        {
            gui_data.screenDraw(&window, mouseX, mouseY, mousePressed, mouseReleased, delta_time);
        }

        resolutionBlinderTop.Put(0.0f, -massYOffset / massScale);
        resolutionBlinderTop.rect.setScale(256.0f, massYOffset / massScale / resolutionBlinderTop.spriteH);
        window.draw(resolutionBlinderTop.rect);
        resolutionBlinderBottom.Put(0.0f, 128.0f);
        resolutionBlinderBottom.rect.setScale(256.0f, massYOffset * massScale);
        window.draw(resolutionBlinderBottom.rect);

        window.display();
    }

    return 0;
}