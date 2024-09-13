#include "../headers/dungeon.h"
#include "../headers/system.h"
#include "../headers/gamestate.h"
#include "../headers/collision.h"

float mouseX, mouseY;
bool mousePressed, mouseClicked, mouseReleased;
float massScale = 1.0f, massYOffset = 0.0f;
float delta_time = 0.0f;

void unitControl(game_system &game, player &p, sf::RenderWindow &window, dungeon *floor)
{
    static sprite sTopLeft("../img/ui/unit-control/playerselect-c.png", 0.0f, 0.0f, 8.0f, 8.0f, 1, 1);
    static sprite sTopRight("../img/ui/unit-control/playerselect-c-h.png", 0.0f, 0.0f, 8.0f, 8.0f, 1, 1);
    static sprite sBottomLeft("../img/ui/unit-control/playerselect-c-v.png", 0.0f, 0.0f, 8.0f, 8.0f, 1, 1);
    static sprite sBottomRight("../img/ui/unit-control/playerselect-c-vh.png", 0.0f, 0.0f, 8.0f, 8.0f, 1, 1);
    static sprite sMove("../img/ui/unit-control/playertarget-move.png", 0.0f, 0.0f, 16.0f, 16.0f, 1, 1);
    static sprite sAttack("../img/ui/unit-control/playertarget-attack.png", 0.0f, 0.0f, 16.0f, 16.0f, 1, 1);
    static float boxClickX, boxClickY, boxMinX, boxMinY, boxMaxX, boxMaxY;

    if (mouseClicked)
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
    if (mousePressed)
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
    } // selector box also needs to be changed with screen offset
    // if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    // {
    //     for (int i = 0; i < character_limit; ++i)
    //     {
    //         if (!p.selected[i] || p.allies[i].visual == nullptr)
    //             continue;

    //         // p.allies[i].posX -= p.allies[i]._class.runSpeed * delta_time;
    //         // p.allies[i].walkToX = p.allies[i].posX; // why does this have strange results
    //         p.allies[i].target = nullptr;
    //         p.allies[i].MoveTo(p.allies[i].posX - 5.0f * massScale, p.allies[i].posY);

    //         // Options:

    //         // Fix bugs, add keyboard walk feature
    //         // Draw concepts, update a character to 32 bit and do simple animations for all actions
    //         // Design intro with one starter character and make music track for it
    //         // Implement sfml audio support
    //     }
    // }
    if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
    {
        int selectedCount = 0;
        float averageX = 0, averageY = 0;
        character *targeted = nullptr;

        for (int i = 0; i < entity_limit; ++i)
        {
            if (i > game.characterCount)
                break;

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
// edit all guis here
void menuData(player &mainP, dungeon &floor)
{
    if (state == prevState)
        return;

    gui_data.buttons.clear();

    std::string temp_path;
    switch (state)
    {
    case START_SCREEN:
        gui_data.background = sprite("../img/ui/backgrounds/start.png", 0.0f, 0.0f, 256.0f, 128.0f, 1, 1);
        gui_data.background.rect.setColor(sf::Color(255, 255, 255, 255));
        gui_data.buttons.push_back(button("../img/ui/buttons/start.png", 40.0f, 40.0f, 16.0f, 16.0f, startGame));
        break;
    case MENU_SCREEN:
        gui_data.background = sprite("../img/ui/backgrounds/menu.png", 0.0f, 0.0f, 256.0f, 128.0f, 3, 1);
        gui_data.background.rect.setColor(sf::Color(255, 255, 255, 255));
        gui_data.buttons.push_back(button("../img/ui/buttons/play.png", 40.0f, 40.0f, 16.0f, 16.0f, startGame));
        gui_data.buttons.push_back(button("../img/ui/buttons/quit.png", 80.0f, 40.0f, 16.0f, 16.0f, quitGame));
        break;
    case CHARACTER_CREATION_SCREEN:
        gui_data.background = sprite("../img/ui/backgrounds/character.png", 0.0f, 0.0f, 256.0f, 128.0f, 1, 1);
        gui_data.background.rect.setColor(sf::Color(255, 255, 255, 255));
        gui_data.buttons.push_back(button("../img/ui/buttons/play.png", 40.0f, 40.0f, 16.0f, 16.0f, startGame));
        break;
    case DUNGEON_SCREEN:
        gui_data.background = sprite("../img/ui/backgrounds/dungeon.png", 0.0f, 0.0f, 256.0f, 128.0f, 1, 1);
        gui_data.background.rect.setColor(sf::Color(255, 255, 255, 0));

        gui_data.images.push_back(sprite("../img/ui/dungeon_ui.png", 0.0f, 0.0f, 256.0f, 128.0f, 1, 1));
        for (int i = 0; i < character_limit; ++i)
        {
            if (mainP.allies[i].visual == nullptr)
                continue;

            mainP.allies[i].posX = floor.spawnLocationX;
            mainP.allies[i].posY = floor.spawnLocationY + i * 16.0f;
            mainP.allies[i].walkToX = floor.spawnLocationX;
            mainP.allies[i].walkToY = floor.spawnLocationY + i * 16.0f; // plan - arrow keys move selected units

            temp_path = std::string("../img/ui/icons/" + mainP.allies[i]._class.name + ".png");

            gui_data.images.push_back(sprite(temp_path.c_str(), 225.0f, 6.0f + i * 20.0f, 12.0f, 14.0f, 1, 1));

            gui_data.buttons.push_back(button("../img/ui/profile_card.png", 224.0f, 5.0f + i * 20.0f, 32.0f, 16.0f, characterMenu, i, &mainP));
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

    sprite brawler("../img/classes/violent/brawler.png", 120.0f, 40.0f, 32.0f, 32.0f, 5, 4);
    sprite detective("../img/classes/violent/detective.png", 130.0f, 40.0f, 16.0f, 16.0f, 6, 1);
    sprite bloom("../img/classes/violent/bloom.png", 110.0f, 40.0f, 16.0f, 16.0f, 6, 1);
    sprite megdrer("../img/enemies/dungeon-1/megdrer.png", 30.0f, 30.0f, 16.0f, 16.0f, 1, 1);

    ch_class brawler_class("brawler", 20, 8.0f, 60.0f, 200.0f, 150);
    ch_class detective_class("detective", 10, 16.0f, 90.0f, 700.0f, 200);
    ch_class bloom_class("bloom", 15, 10.0f, 80.0f, 300.0f, 150);
    ch_class meg("megdrer", 0, 14.5f, 120.0f, 400.0f, 40);

    dungeon currentDungeon("../img/tiles/dungeons/blue/blue.png", 64.0f, 64.0f, massScale, massYOffset);
    currentDungeon.readRoomFile("../dungeons/blue.sdf", massScale, massYOffset);
    player mainPlayer;
    mainPlayer.allies[0] = character(&brawler, CH_PLAYER, brawler_class);
    mainPlayer.allies[1] = character(&detective, CH_PLAYER, detective_class);
    mainPlayer.allies[2] = character(&bloom, CH_PLAYER, bloom_class);

    character e1(&megdrer, CH_MONSTER, meg);

    sf::Clock sfClock;
    sf::Time sfTime;

    mainPlayer.allies[0].SetAnimation(ANIM_IDLE, 0, 5, 150.0f);
    mainPlayer.allies[0].SetAnimation(ANIM_HURT, 5, 7, 150.0f);
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

        // next up is either animation code stuff, or, since I know you don't want to do that
        // graphics and code for the leveling up mechanic
        // just have it where killing grants that character xp and enough xp gives bonus dmg and hp

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