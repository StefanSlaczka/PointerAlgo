// SFML: Simple Fast Multimedia Library — gives us the window, 
// rendering (Rectangles, Sprites, Text), input events, and timing.
// Docs: https://www.sfml-dev.org/documentation/3.1.0/
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <filesystem>

int main(){
    // Setup
    sf::RenderWindow window(sf::VideoMode({800,600}), "SFML window");
    window.setFramerateLimit(60);

    // Texture
    sf::Texture texture;
    if(!texture.loadFromFile("./cute_image.png")){
        std::cerr << "Failed to load cute_image.png\n";
        return 1;
    }
    sf::Sprite sprite(texture);
    sprite.setPosition({100.f, 100.f});
    sprite.setScale({2.f,2.f});     // adjust to taste

    // Font
    sf::Font font;
    if(!font.openFromFile("./arial.ttf")){
        std::cerr << "Failed to load font\n";
        return 1;
    }
    sf::Text text(font, "Hello SFML", 50);
    text.setPosition({50.f,50.f});
    text.setFillColor(sf::Color::White);

    // Music
    sf::Music music;
    if(!music.openFromFile("Kuntry Boy - Anno Domini Beats.mp3")){
        std::cerr << "Failed to load music\n";
        // No need to go back because its not importent
    }else{
        music.setLooping(true);
        music.play();
    }

    // Per frame state
    float x =100.f; // number.f => number.0
    float speed = 100.f; // Pixels per second

    sf::Clock clock;

    // Start the game loop
    while(window.isOpen()){
        float dt = clock.restart().asSeconds();     // seconds since last frame


        // Events
        while(const std::optional event = window.pollEvent()){
            // Close window: exit
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* key = event->getIf<sf::Event::KeyPressed>()){
                if(key->code == sf::Keyboard::Key::Escape)
                    window.close();
                if(key->code == sf::Keyboard::Key::Space)
                    std::cout << "Space pressed\n";
            }
        }

        // Update
        x+= speed * dt;
        if(x>700.f) x = 100.f;  // wrap
        sprite.setPosition({x,100.f});

        // Draw
        window.clear(sf::Color(30,30,30));
        window.draw(sprite);
        window.draw(text);
        window.display();
    }

    return 0;
}