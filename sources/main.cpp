#include "../headers/dungeon.h"
#include "../headers/system.h"
#include "../headers/gamestate.h"
#include "../headers/collision.h"

float mouseX, mouseY;
bool mousePressed, mouseClicked, mouseReleased;
float massScale = 1.0f, massYOffset = 0.0f;
float delta_time = 0.0f;
bool buttonHovered = false;

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
            if (!p.selected[i] || p.allies[i].visual == nullptr)
                continue;

            p.allies[i].target = nullptr;
            p.allies[i].MoveTo(p.allies[i].visual->rect.getPosition().x - 1.0f * massScale, p.allies[i].visual->rect.getPosition().y, floor);

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
            if (!p.selected[i] || p.allies[i].visual == nullptr)
                continue;

            p.allies[i].target = nullptr;
            p.allies[i].MoveTo(p.allies[i].visual->rect.getPosition().x + 1.0f * massScale, p.allies[i].visual->rect.getPosition().y, floor);
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        for (int i = 0; i < character_limit; ++i)
        {
            if (!p.selected[i] || p.allies[i].visual == nullptr)
                continue;

            p.allies[i].target = nullptr;
            p.allies[i].MoveTo(p.allies[i].visual->rect.getPosition().x, p.allies[i].visual->rect.getPosition().y - 1.0f * massScale, floor);
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        for (int i = 0; i < character_limit; ++i)
        {
            if (!p.selected[i] || p.allies[i].visual == nullptr)
                continue;

            p.allies[i].target = nullptr;
            p.allies[i].MoveTo(p.allies[i].visual->rect.getPosition().x, p.allies[i].visual->rect.getPosition().y + 1.0f * massScale, floor);
        }
    }
    if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
    {
        int selectedCount = 0;
        float averageX = 0, averageY = 0;
        character *targeted = nullptr;

        for (int i = 0; i < game.characterCount; ++i)
        {
            if (game.characters[i] == nullptr)
                continue;

            bool targettedFriend = false;
            for (int j = 0; j < character_limit; ++j)
            {
                if (game.characters[i] == &p.allies[j])
                {
                    targettedFriend = true;
                    break;
                }
            }

            if (targettedFriend)
                continue;

            if (mouseX > game.characters[i]->posX / massScale - game.characters[i]->selectionSize / massScale &&
                mouseX < game.characters[i]->posX / massScale + game.characters[i]->selectionSize / massScale &&
                mouseY > game.characters[i]->posY / massScale + massYOffset - game.characters[i]->selectionSize / massScale &&
                mouseY < game.characters[i]->posY / massScale + massYOffset + game.characters[i]->selectionSize / massScale && game.characters[i]->hp > 0)
            {
                targeted = game.characters[i];
                break;
            }
        }

        // two for-loops seems redundant, please optimize
        for (int i = 0; i < character_limit; ++i)
        {
            if (!p.selected[i] || p.allies[i].visual == nullptr)
                continue;

            averageX += p.allies[i].posX;
            averageY += p.allies[i].posY;
            ++selectedCount;
        }
        averageX /= selectedCount;
        averageY /= selectedCount;
        for (int i = 0; i < character_limit; ++i)
        {
            if (!p.selected[i] || p.allies[i].visual == nullptr)
                continue;

            float offsetX = p.allies[i].posX - averageX;
            float offsetY = p.allies[i].posY - averageY;

            p.allies[i].MoveTo(mouseX * massScale + offsetX,
                               mouseY * massScale + offsetY - massYOffset * massScale, floor);

            p.allies[i].target = targeted;
            if (targeted != nullptr)
            {
                sAttack.Put(mouseX * massScale - 8.0f, mouseY * massScale - 16.0f + massYOffset * massScale);
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
int expAmounts[character_limit];
int abilitySelection[att_limit];
// edit all guis here
void menuData(player &mainP, dungeon &floor)
{
    if (state == prevState)
        return;

    gui_data.elements.clear();

    std::string temp_path;
    switch (state)
    {
    case START_SCREEN:
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
        gui_data.background = sprite("../img/ui/backgrounds/dungeon.png", 0.0f, 0.0f, 256.0f, 128.0f, 1, 1);
        gui_data.background.rect.setColor(sf::Color(255, 255, 255, 0));

        gui_data.elements.push_back(ui_element(UI_IMAGE, "../img/ui/dungeon_ui.png", 0.0f, 0.0f, 256.0f, 128.0f, 1, 1, nullFunc));
        for (int i = 0; i < character_limit; ++i)
        {
            if (mainP.allies[i].visual == nullptr)
                continue;

            mainP.allies[i].posX = floor.spawnLocationX;
            mainP.allies[i].posY = floor.spawnLocationY + i * 16.0f;
            mainP.allies[i].walkToX = floor.spawnLocationX;
            mainP.allies[i].walkToY = floor.spawnLocationY + i * 16.0f;

            temp_path = std::string("../img/ui/icons/" + mainP.allies[i]._class.name + ".png");

            gui_data.elements.push_back(ui_element(UI_IMAGE, temp_path.c_str(), 225.0f, 6.0f + i * 20.0f, 12.0f, 14.0f, 1, 1, nullFunc));

            gui_data.elements.push_back(ui_element(UI_CLICKABLE, "../img/ui/profile_card.png", 224.0f, 5.0f + i * 20.0f, 32.0f, 16.0f, 1, 1, characterMenu, &mainP, nullptr, i));
            gui_data.elements.push_back(ui_element(UI_VALUEISFRAME, "../img/ui/health.png", 238.0f, 9.0f + i * 20.0f, 16.0f, 4.0f, 15, 1, nullFunc, nullptr, nullptr, 0, &hpAmounts[i]));
            gui_data.elements.push_back(ui_element(UI_VALUEISFRAME, "../img/ui/experience.png", 230.0f, 20.0f + i * 20.0f, 24.0f, 2.0f, 25, 1, nullFunc,
                                                   nullptr, nullptr, 0, &expAmounts[i]));
        }
        for (int j = 0; j < att_limit; ++j)
        {
            gui_data.elements.push_back(ui_element(
                UI_CLICKABLE, "../img/ui/ability_card.png", 2.0f + j * 26.0f, 100.0f, 24.0f, 24.0f, 2, 1, characterAbility, &mainP, nullptr, j));

            gui_data.elements.push_back(ui_element(
                UI_VALUEISFRAME, "../img/ui/ability_select.png", 2.0f + j * 26.0f, 100.0f, 24.0f, 24.0f, 2, 1,
                nullptr, nullptr, nullptr, 0, &abilitySelection[j]));
        }
        break;
    default:
        break;
    }

    gui_data.menuBG = animation(&gui_data.background, 0, gui_data.background.framesX - 1, 18.0f);

    prevState = state;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(256, 128), "rambunctious_alpha_0.0");
    sf::View screen(sf::FloatRect(0.0f, 0.0f, 256.0f, 128.0f));

    window.setView(screen);

    sprite brawler("../img/classes/violent/brawler.png", 120.0f, 40.0f, 32.0f, 32.0f, 7, 6); // animation for walking brawler, simple code for looking either way. Animations for enemy, simple code for spawning them in the .sdf file. Animations for icons. Overhaul art with simplistic 4/8-color style
    sprite brawler2("../img/classes/violent/brawler.png", 120.0f, 40.0f, 32.0f, 32.0f, 7, 6);
    sprite brawler3("../img/classes/violent/brawler.png", 120.0f, 40.0f, 32.0f, 32.0f, 7, 6);
    sprite megdrer("../img/enemies/dungeon-1/megdrer.png", 30.0f, 30.0f, 16.0f, 16.0f, 1, 1);

    ch_class brawler_class("brawler", 20, 7, 8.0f, 60.0f, 200.0f, 150, ability_simpleMelee);

    brawler_class.setAbility(0, ability_brawler_hook, 2);
    brawler_class.setAbility(1, ability_brawler_right_hook, 1.1f, 10);
    ch_class meg("megdrer", 140, 2, 14.5f, 120.0f, 400.0f, 40, ability_simpleMelee);

    dungeon currentDungeon("../img/tiles/dungeons/blue/blue.png", 64.0f, 64.0f, massScale, massYOffset);
    currentDungeon.readRoomFile("../dungeons/blue.sdf", massScale, massYOffset);
    player mainPlayer;
    mainPlayer.allies[0] = character(&brawler, CH_PLAYER, brawler_class);
    mainPlayer.allies[1] = character(&brawler2, CH_PLAYER, brawler_class);
    mainPlayer.allies[2] = character(&brawler3, CH_PLAYER, brawler_class);

    character e1(&megdrer, CH_MONSTER, meg);

    sf::Clock sfClock;
    sf::Time sfTime;

    mainPlayer.allies[0].SetAnimation(ANIM_IDLE, 0, 8, 150.0f);
    mainPlayer.allies[0].SetAnimation(ANIM_HURT, 9, 14, 150.0f);
    mainPlayer.allies[0].SetAnimation(ANIM_ABILITY_0, 15, 21, 300.0f);
    mainPlayer.allies[0].SetAnimation(ANIM_ABILITY_1, 23, 41, 300.0f);
    mainPlayer.allies[1].SetAnimation(ANIM_IDLE, 0, 5, 150.0f);
    mainPlayer.allies[2].SetAnimation(ANIM_IDLE, 0, 5, 150.0f);
    e1.SetAnimation(ANIM_IDLE, 0, 0, 150.0f);

    game_system game;
    game.Add(&mainPlayer);
    game.Add(&e1);

    sprite resolutionBlinderTop("../img/ui/resolution_blinder.png", 0.0f, massYOffset * massScale, 1.0f, 1.0f, 1, 1);
    sprite resolutionBlinderBottom("../img/ui/resolution_blinder.png", 0.0f, massYOffset * massScale, 1.0f, 1.0f, 1, 1);

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

        // graphics and code for the leveling up mechanic
        // just have it where killing grants that character xp and enough xp gives bonus dmg and hp
        // yomi hustle time system (everything should be linked)
        // also pause button (probably with space key)

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            window.close();

        window.clear();

        menuData(mainPlayer, currentDungeon);
        if (state == DUNGEON_SCREEN)
        {
            currentDungeon.updateScreenPosition(mouseX, mouseY, delta_time, massScale, massYOffset);

            currentDungeon.draw(&window);
            game.update(currentDungeon, delta_time);
            unitControl(game, mainPlayer, window, &currentDungeon);

            mainPlayer.firstUnitSelected = -1;
            for (int i = 0; i < character_limit; ++i)
            {
                if (mainPlayer.firstUnitSelected == -1 && mainPlayer.selected[i])
                {
                    mainPlayer.firstUnitSelected = i;
                }

                if (mainPlayer.allies[i].visual == nullptr)
                    continue;
                hpAmounts[i] = 14 - static_cast<int>((static_cast<float>(mainPlayer.allies[i].hp) / static_cast<float>(mainPlayer.allies[i]._class.maxHP)) * 14.0f);
                expAmounts[i] = 24 - static_cast<int>((static_cast<float>(mainPlayer.allies[i].experiencePoints) /
                                                       static_cast<float>(mainPlayer.allies[i]._class.experienceToLvlUp)) *
                                                      24.0f);
            }
            for (int i = 0; i < att_limit; ++i)
            {
                abilitySelection[i] = 1;
                if (mainPlayer.firstUnitSelected != -1 && mainPlayer.allies[mainPlayer.firstUnitSelected].nextAbility == i)
                {
                    abilitySelection[i] = 0;
                }
            }

            for (int i = 0; i < game.characterCount; ++i)
            {
                if (game.characters[i]->visual == nullptr)
                {
                    continue;
                }
                window.draw(game.characters[i]->visual->rect);
            }
        }
        gui_data.screenDraw(&window, mouseX, mouseY, mousePressed, mouseReleased, delta_time);

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