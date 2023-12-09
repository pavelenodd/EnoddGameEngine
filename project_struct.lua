project_root/
|-- src/
|   |-- main.cpp                    // корень , линкует игровой движок с самой игрой 
|   |-- GameEngine/                 //сам игровой движок 
|       |-- Engine.cpp 
|       |-- Engine.h
|       |-- Graphics/                   //рендеры графики
|           |-- 2DRenderer.cpp          //2d рендер
|           |-- 2DRenderer.h
|           |-- 3DRenderer.cpp          //3d ренлер
|           |-- 3DRenderer.h
|       |-- FileManager/                //менеджер работы с файлами 
|           |-- FileManager.cpp
|           |-- FileManager.h
|           |-- Json_Parser/            //парсер json фалов
|               |-- JsonParser.cpp
|               |-- JsonParser.h
|       |-- SoundManager/               //менеджер звука 
|           |-- AudioManager.cpp
|           |-- AudioManager.h
|       |-- NetworkManager/             //менеджер сети 
|           |-- NetworkManager.cpp
|           |-- NetworkManager.h
|       |--SettingsEngine/
|           |-- settings.json           //настройки игрового движка 
|-- Libs/                           //библиотеки исполььзуемые в движке 
|   |-- boost/                      // заголовочные файлы от Boost
|   |-- Game/                       //файлы игры 
|       |--Assets                   // Ресурсы, такие как изображения, модели, звуки, и т. д.
|-- CMakeLists.txt

