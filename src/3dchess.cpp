#include <iostream>
#include <cmath>
#include <fstream>

#include <boost/program_options.hpp>

#include "gui/GuiWindow.h"

#include "misc/helper.h"
#include "core/GameConfiguration.h"
#include "core/Logging.h"

namespace po = boost::program_options;

using namespace std;
using namespace Logging;

int main(int argn, char **argv) {
    // Boost program options for commandline parsing

    po::options_description desc("Test");
    desc.add_options()
            ("help", "Print help message")
            ("width", po::value<int>()->default_value(1024), "Horizontal resolution")
            ("height", po::value<int>()->default_value(768), "Vertical resolution")
            ("loglvl", po::value<string>()->default_value("debug"), "Set loglevel (trace|debug|info|warning|error|fatal)")
            ("fullscreen", "If set program runs in fullscreen")
            ;

    po::variables_map vm;
    po::store(po::parse_command_line(argn, argv, desc), vm);

    if (vm.count("help")) {
        cerr << desc << endl;
        return 1;
    }
    
    auto severity = severityFrom(vm["loglvl"].as<string>());
    if (!severity) {
        cerr << "Invalid severity" << endl;
        cerr << desc << endl;
        return 1;
    }
    
    initializeLogging();
    addLoggingConsoleSink(severity.get());
    addLoggingFileSink("3dchess.log", severity.get());
    
    Logger log = initLogger("main");
    
    GameConfiguration config;
    config.save("config.xml");

    auto configl = GameConfiguration::load("config.xml");
    if (!configl) {
        GLOG(error) << "Failed to load";
    } else {
        GLOG(info) << "Dur: " << (configl->maximumTurnTimeInSeconds) << "s";
    }

    // SDL2/OpenGL for graphics
    const int width = vm["width"].as<int>();
    const int height = vm["height"].as<int>();
    const bool fullscreen = (vm.count("fullscreen") != 0);

    GLOG(info) << "Width:         " << width;
    GLOG(info) << "Height:        " << height;
    GLOG(info) << "Fullscreen:    " << fullscreen;

    GuiWindow window("3D Chess", fullscreen, width, height);
    window.exec();

    return 0;
}
