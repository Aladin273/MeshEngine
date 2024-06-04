#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>

#include "MeshEngine/Logger/mesh_engine_sink.h"

#include "Application/Application.h"

int main()
{    
    auto fileLogger = std::make_shared<spdlog::sinks::daily_file_sink_st>("Logs/MeshEditor.txt", 23, 55);
    auto engineLogger = std::make_shared<spdlog::sinks::mesh_engine_sink_mt>();
    
    spdlog::default_logger()->sinks().push_back(fileLogger);
    spdlog::default_logger()->sinks().push_back(engineLogger);

    Application* app = Application::instance();
    std::unique_ptr<Model> model = app->loadModel(Settings::dataPath + Settings::model3);
    View* view = app->createView(Settings::title, Settings::width, Settings::height);

    view->setModel(model.get());
    app->run();

    return 0;
}
