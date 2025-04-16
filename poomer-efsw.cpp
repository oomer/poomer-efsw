#include <iostream>
#include <thread>
#include <chrono>
#include <filesystem>
#include <string>
#include <atomic>
#include <signal.h>

#include "../bella_engine_sdk/src/bella_sdk/bella_engine.h" // For rendering and scene creation in Bella
#include "../bella_engine_sdk/src/dl_core/dl_main.inl" // Core functionality from the Diffuse Logic engine
#include "../oom/oom_license.h" // common misc code
#include "../oom/oom_file.h"    // common misc code
#include "../oom/oom_bella_long.h"    // common misc code
#include "../oom/oom_bella_engine.h"    // common misc code

//==============================================================================
// GLOBAL VARIABLES AND FUNCTIONS
//==============================================================================

// Global flag to indicate program termination
std::atomic<bool> STOP(false);

// Signal handler for clean shutdown
void sigend(int) {
    std::cout << std::endl << "Bye bye" << std::endl;
    STOP = true;
    // Give a short time for cleanup
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    exit(0);  // Force exit after cleanup
}

//void render_thread( dl::bella_sdk::Engine& engine,
//                    MyEngineObserver& engineObserver);

//==============================================================================
// MAIN FUNCTION
//==============================================================================

int DL_main(dl::Args& args) {

    int s_oomBellaLogContext = 0; 
    dl::subscribeLog(&s_oomBellaLogContext, oom::bella::log);
    dl::flushStartupMessages();

    args.add("wd",  "watchdir",   "",   "watch directory for changes");
    args.add("tp",  "thirdparty",   "",   "prints third party licenses");
    args.add("li",  "licenseinfo",   "",   "prints license info");
    args.add("i",  "input",   "",   "prints license info");

    dl::bella_sdk::Engine engine;
    engine.scene().loadDefs();
    oom::bella::MyEngineObserver engineObserver;
    engine.subscribe(&engineObserver);

    if (args.helpRequested()) {
        std::cout << args.help("poomer-efsw © 2025 Harvey Fong","", "1.0") << std::endl;
        return 0;
    }
    
    if (args.have("--licenseinfo")) {
        std::cout << "poomer-efsw © 2025 Harvey Fong" << std::endl;
        std::cout << oom::license::printLicense() << std::endl;
        return 0;
    }
 
    if (args.have("--thirdparty")) {
        std::cout << oom::license::printBellaSDK() << "\n====\n" << std::endl;
        std::cout << oom::license::printEFSW() << "\n====\n" << std::endl;
        return 0;
    }

    std::string watchPath = ".";
    if (args.have("--watchdir")) {
        watchPath = args.value("--watchdir").buf();
    }

    // Set up signal handler/callback for clean shutdown, global space of C standard library
    signal(SIGINT, sigend);

    // Check if watchPath exists before starting the watcher
    if (!std::filesystem::exists(watchPath)) {
        dl::logError("Watch path '%s' does not exist", watchPath.c_str());
        return 1;
    }

    // Global file watcher to catch when .bsz files are created or deleted
    oom::file::Watcher oomWatcher({"bsz" }, {"download"});
    
    // Start watching the directory (which we now know exists)
    if (oomWatcher.startWatching(watchPath)) {
        dl::logInfo("File watcher started on '%s'", watchPath.c_str());
    } else {
        dl::logError("Failed to start file watcher on '%s'", watchPath.c_str());
        return 1;
    }

    while(!STOP) { 
        std::filesystem::path filePath;
        
        // Check if we have files to delete
        while (oomWatcher.getNextFileToDelete(filePath)) {
            dl::logInfo("Processing deletion: %s", filePath.string());
            // Handle file deletion
            // Add your deletion logic here
        }
        
        // Check if we have files to render
        while (oomWatcher.getNextFileToRender(filePath)) {
            dl::logInfo("Processing render: %s", filePath.string().c_str());
            engine.loadScene(filePath.string().c_str());
            engine.start();
            // Handle file rendering
            //render_thread(engine, engineObserver);
        }
        
        // Sleep to avoid busy waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        dl::logInfo("Running...");
    }

    // Clean shutdown
    dl::logInfo("Shutting down file watcher...");
    oomWatcher.stopWatching();
    dl::logInfo("Shutdown complete");

    return 0;
}
