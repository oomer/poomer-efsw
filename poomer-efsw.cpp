#include <iostream>
#include <thread>
#include <chrono>
#include <filesystem>
#include <string>
#include <atomic>

#include "../bella_engine_sdk/src/bella_sdk/bella_engine.h" // For rendering
#include "../bella_engine_sdk/src/dl_core/dl_main.inl" // Core functionality from the Diffuse Logic engine
#include "../oom/oom_license.h"         // common misc code
#include "../oom/oom_file.h"         // common misc code

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

//==============================================================================
// MAIN FUNCTION
//==============================================================================

int DL_main(dl::Args& args) {
    args.add("wd",  "watchdir",   "",   "watch directory for changes");
    args.add("tp",  "thirdparty",   "",   "prints third party licenses");
    args.add("li",  "licenseinfo",   "",   "prints license info");
    
    std::string watchPath = ".";

    // If --help was requested, print help and exit
    if (args.helpRequested()) {
        std::cout << args.help("poomer-efsw © 2025 Harvey Fong","", "1.0") << std::endl;
        return 0;
    }
    
    // Handle other command-line options
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

    if (args.have("--watchdir")) {
        watchPath = args.value("--watchdir").buf();
    }

    // Set up signal handler for clean shutdown
    signal(SIGINT, sigend);

    // Create the file watcher to catch when .bsz files or zip files are created
    oom::file::Watcher oomWatcher({"bsz", "zip"}, {"download"});
    
    // Start watching the current directory
    if (oomWatcher.startWatching(watchPath)) {
        std::cout << "File watcher started" << std::endl;
    } else {
        std::cout << "Failed to start file watcher" << std::endl;
        return 1;
    }

    // Main processing loop
    while(!STOP) {
        // Check for files to process
        std::filesystem::path filePath;
        
        // Check if we have files to delete
        while (oomWatcher.getNextFileToDelete(filePath)) {
            std::cout << "Processing deletion: " << filePath.string() << std::endl;
            // Handle file deletion
            // Add your deletion logic here
        }
        
        // Check if we have files to render
        while (oomWatcher.getNextFileToRender(filePath)) {
            std::cout << "Processing render: " << filePath.string() << std::endl;
            // Handle file rendering
            // Add your rendering logic here
        }
        
        // Sleep to avoid busy waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        std::cout << "Running..." << std::endl;
    }

    // Clean shutdown
    std::cout << "Shutting down file watcher..." << std::endl;
    oomWatcher.stopWatching();
    std::cout << "Shutdown complete" << std::endl;

    return 0;
}
