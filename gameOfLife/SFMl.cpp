// SFML: Simple Fast Multimedia Library — gives us the window, 
// rendering (Rectangles, Sprites, Text), input events, and timing.
// Docs: https://www.sfml-dev.org/documentation/3.1.0/
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

int main(){
    // Create the main window
    sf::RenderWindow window(sf::VideoMode({800,600}), "SFML window");

    // Load a sprite to display
    const sf::Texture texture("./cute_image.png");
    sf::Sprite sprite(texture);

    //Create a graphical text to disaplay
    const sf::Font font("arial.ttf");
    sf::Text text(font, "Hello SFML", 50);

    // Load a music to play
    sf::Music music("Kuntry Boy - Anno Domini Beats.mp3");

    // Play the music
    music.play();

    // Start the game loop
    while(window.isOpen()){
        //Process events
        while(const std::optional event = window.pollEvent()){
            // Close window: exit
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        // Clear screen
        window.clear();

        // Draw the sprite
        window.draw(sprite);

        // Draw the string
        window.draw(text);

        // Update the window
        window.display();
    }
}