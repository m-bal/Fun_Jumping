#include <SFML/Graphics.hpp>
#include <Eigen/Dense>
#include <iostream>


sf::Glsl::Vec2 res(800, 450);


int main(){

	

	if(!sf::Shader::isAvailable()){
		std::cerr << "NO GRAPHICS CARD!!" << std::endl;
		return 1;
	}
	sf::Shader shader;
	if(!shader.loadFromFile("Fragshader.glsl", sf::Shader::Fragment)){
		std::cerr << "Error reading frag shader" << std::endl;
		return 1;
	}
	//we need to create a rectangle that we can draw on
	sf::RectangleShape rect;
	rect.setSize(res);
	rect.setPosition(0,0);
	
	//set fragment vars
	shader.setUniform("iResolution", res);
	
	//create the window
	sf::RenderWindow window(sf::VideoMode(res.x, res.y), "Graphics Project");
	
	sf::Clock timer;
	while(window.isOpen()){
		sf::Event event;
		while(window.pollEvent(event)){
			if(event.type == sf::Event::Closed){
				window.close();
			}
		}
		
		shader.setUniform("time", timer.getElapsedTime().asSeconds());
		sf::RenderStates states = sf::RenderStates::Default;
		states.shader = &shader;
		
		window.clear();
		window.draw(rect, &shader);
		window.display();
	
	}


	return 0;
}
