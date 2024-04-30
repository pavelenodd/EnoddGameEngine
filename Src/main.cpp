#include"Engine/engine.h"
#include "Game/game.h"
int main() { Engine engine; }
/*
//интересный пример отрисовки таблички
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

int main() {
sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Table");

sf::Font font;
if (!font.loadFromFile(
"E:/Projects/Pets/EnoddGameEngine/build/Data/Fonts/font.ttf")) {
// обработка ошибки
return -1;
}

// Данные для таблицы
std::vector<std::string> headers = {"Header 1", "Header 2", "Header 3"};
std::vector<std::vector<std::string>> rows = {
{"Row 1, ColL 1", "Row 1, ColiL1 2", "Row 1, Col 3"},
{"Row 2, Col 1", "Row 2, Col 2", "Row 2, Col 3"},
// Добавьте больше строк по необходимости
};

// Настройки таблицы
float columnWidth = 200.f;
float rowHeight = 50.f;
sf::Vector2f startPosition(50.f, 50.f);  // Начальная позиция таблицы

// Создаем фон для заголовков
sf::RectangleShape headerBackground(
sf::Vector2f(headers.size() * columnWidth, rowHeight));
headerBackground.setFillColor(sf::Color::Cyan);
headerBackground.setPosition(startPosition);

// Создаем текст для заголовков
std::vector<sf::Text> headerTexts;
for (size_t i = 0; i < headers.size(); ++i) {
sf::Text text(headers[i], font, 20);
text.setFillColor(sf::Color::Black);
text.setPosition(startPosition.x + i * columnWidth, startPosition.y);
headerTexts.push_back(text);
}

// Создаем фоны и тексты для строк
std::vector<sf::RectangleShape> rowBackgrounds;
std::vector<std::vector<sf::Text>> rowTexts;
for (size_t i = 0; i < rows.size(); ++i) {
// Фон для строки
sf::RectangleShape rowBackground(
sf::Vector2f(headers.size() * columnWidth, rowHeight));
rowBackground.setFillColor((i % 2 == 0) ? sf::Color::White
                 : sf::Color::Green);
rowBackground.setPosition(startPosition.x,
   startPosition.y + (i + 1) * rowHeight);

rowBackgrounds.push_back(rowBackground);

// Тексты для ячеек
std::vector<sf::Text> texts;
for (size_t j = 0; j < rows[i].size(); ++j) {
sf::Text text(rows[i][j], font, 20);
text.setFillColor(sf::Color::Black);
text.setPosition(startPosition.x + j * columnWidth,
startPosition.y + (i + 1) * rowHeight);
texts.push_back(text);
}
rowTexts.push_back(texts);
}

while (window.isOpen()) {
sf::Event event;
while (window.pollEvent(event)) {
if (event.type == sf::Event::Closed) window.close();
}

window.clear();

// Отрисовка таблицы
window.draw(headerBackground);
for (const auto& text : headerTexts) {
window.draw(text);
}
for (const auto& background : rowBackgrounds) {
window.draw(background);
}
for (const auto& texts : rowTexts) {
for (const auto& text : texts) {
window.draw(text);
}
}

window.display();
}

return 0;
}
*/