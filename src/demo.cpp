#include <iostream>
#include <string>
#include <map>
#include "../include/core_singleton.hpp"

/**
 * MessageService - Example class for logging messages
 */
class MessageService {
public:
    MessageService() { 
        std::cout << "MessageService: Initialized\n"; 
    }
    
    ~MessageService() { 
        std::cout << "MessageService: Shut down\n"; 
    }
    
    void sendMessage(const std::string& level, const std::string& message) const {
        std::cout << "[" << level << "]: " << message << std::endl;
    }
};

/**
 * AppSettings - Example class for application configuration
 */
class AppSettings {
public:
    AppSettings() { 
        std::cout << "AppSettings: Loaded\n"; 
    }
    
    ~AppSettings() { 
        std::cout << "AppSettings: Saved\n"; 
    }
    
    void setOption(const std::string& key, const std::string& value) {
        settings_[key] = value;
        std::cout << "Option set: " << key << " = " << value << std::endl;
    }
    
    std::string getOption(const std::string& key) const {
        auto it = settings_.find(key);
        return (it != settings_.end()) ? it->second : "undefined";
    }
    
private:
    std::map<std::string, std::string> settings_;
};

// Define different singleton types with various combinations of strategies

// Default strategies (new allocation, standard disposal, mutex synchronization)
using StandardLogger = pattern::CoreSingleton<MessageService>;

// Smart pointer allocation with resurrection capability and mutex synchronization
using ConfigManager = pattern::CoreSingleton<
    AppSettings,
    pattern::SmartPointerAllocationStrategy,
    pattern::ResurrectionStrategy,
    pattern::MutexSynchronizationStrategy
>;

// Single-threaded logger with immortal lifetime
using PermanentLogger = pattern::CoreSingleton<
    MessageService,
    pattern::NewAllocationStrategy,
    pattern::ImmortalStrategy,
    pattern::NoSynchronizationStrategy
>;

// Thread-local logger (each thread gets its own instance)
using ThreadLogger = pattern::CoreSingleton<
    MessageService,
    pattern::NewAllocationStrategy,
    pattern::ImmortalStrategy,
    pattern::ThreadLocalStrategy
>;

/**
 * Main demonstration function
 */
int main() {
    std::cout << "========== Orthogonal Singleton Pattern Demonstration ==========\n\n";
    
    // Demonstrate standard logger
    std::cout << "--- Standard Logger Example ---\n";
    StandardLogger::GetInstance().sendMessage("INFO", "Application started");
    
    // Demonstrate configuration manager
    std::cout << "\n--- Configuration Manager Example ---\n";
    ConfigManager::GetInstance().setOption("database.host", "localhost");
    ConfigManager::GetInstance().setOption("database.port", "5432");
    ConfigManager::GetInstance().setOption("app.name", "SingletonDemo");
    
    std::cout << "Database host: " << ConfigManager::GetInstance().getOption("database.host") << std::endl;
    std::cout << "Application name: " << ConfigManager::GetInstance().getOption("app.name") << std::endl;
    
    // Demonstrate permanent logger
    std::cout << "\n--- Permanent Logger Example ---\n";
    PermanentLogger::GetInstance().sendMessage("DEBUG", "This logger will never be destroyed");
    
    // Demonstrate thread logger
    std::cout << "\n--- Thread Logger Example ---\n";
    ThreadLogger::GetInstance().sendMessage("THREAD", "Message from main thread");
    
    // Use standard logger again
    std::cout << "\n--- Standard Logger Again ---\n";
    StandardLogger::GetInstance().sendMessage("INFO", "Application shutting down");
    
    std::cout << "\n========== Demonstration Complete ==========\n";
    return 0;
}