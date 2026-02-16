#include "sources/SomeIPTelemetrySourceImpl.hpp"
#include <thread>

// ADD THESE INCLUDES - They auto-register when included
#include <v1/log/TelemetryServiceSomeIPProxy.hpp>
#include <v1/log/TelemetryServiceSomeIPDeployment.hpp>

// FORCE REGISTRATION - Add this static block
namespace {
    struct SomeIPRegistration {
        SomeIPRegistration() {
            // This forces the SOME/IP code to be linked and registered
        }
    };
    static SomeIPRegistration forceRegistration;
    
    // Reference symbols to prevent linker from removing them
    static auto& proxyRef = typeid(v1::log::TelemetryServiceSomeIPProxy);
}

bool SomeIPTelemetrySourceImpl::init(){

    std::lock_guard<std::mutex> lock(mutex_);

    if (initialized_) {
        std::cout << "[SomeIPClient] Already initialized\n";
        return true;
    }

    std::cout << "[SomeIPClient] Initializing...\n";

    runtime_ = CommonAPI::Runtime::get(); 

    if (!runtime_) {
        std::cerr << "[SomeIPClient] Failed to get CommonAPI runtime\n";
        return false;
    }

    // NO REGISTRATION LINE NEEDED - just build proxy directly
    proxy_ = runtime_->buildProxy<v1::log::TelemetryServiceProxy>("local","TelemetryService");

    if (!proxy_) {
        std::cerr << "[SomeIPClient] Failed to build proxy\n";
        return false;
    }

    initialized_ = true;
    std::cout << "[SomeIPClient] Initialized successfully\n";
    return true;
}

bool SomeIPTelemetrySourceImpl::connect() {

    if (!initialized_) {
        std::cerr << "[SomeIPClient] Not initialized. Call init() first.\n";
        return false;
    }

    std::cout << "[SomeIPClient] Waiting for service...\n";

    const int maxAttempts = 100;
    int attempts = 0;
    
    while (!proxy_->isAvailable() && attempts < maxAttempts) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        attempts++;
    }

    if (proxy_->isAvailable()) {
        std::lock_guard<std::mutex> lock(mutex_);
        connected_ = true;
        std::cout << "[SomeIPClient] Connected to service\n";
        return true;
    } else {
        std::cerr << "[SomeIPClient] Connection timeout - service not available\n";
        return false;
    }
}

std::string SomeIPTelemetrySourceImpl::requestTelemetry() {

    if (!isConnected()) {
        std::cerr << "[SomeIPClient] Not connected. Cannot request telemetry.\n";
        return "";
    }

    if (!proxy_ || !proxy_->isAvailable()) {
        std::lock_guard<std::mutex> lock(mutex_);
        connected_ = false;
        std::cerr << "[SomeIPClient] Service not available\n";
        return "";
    }

    uint8_t loadValue;
    CommonAPI::CallStatus status;
    proxy_->getLoad(status, loadValue);

    if (status == CommonAPI::CallStatus::SUCCESS) { 
        return std::to_string(loadValue);
    } else {
        return "";
    }
}

void SomeIPTelemetrySourceImpl::disconnect(){
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!connected_ && !initialized_) {
        return;
    }

    std::cout << "[SomeIPClient] Disconnecting...\n";
    connected_ = false;
    proxy_.reset();  
}                            

SomeIPTelemetrySourceImpl& SomeIPTelemetrySourceImpl::getInstance() {          
    static SomeIPTelemetrySourceImpl instance;             
    return instance;                                       
}

SomeIPTelemetrySourceImpl::~SomeIPTelemetrySourceImpl() {
    disconnect();
    std::cout << "[SomeIPClient] Instance destroyed\n";
}

SomeIPTelemetrySourceImpl::SomeIPTelemetrySourceImpl()
    : runtime_(nullptr)
    , proxy_(nullptr)
    , connected_(false)
    , initialized_(false)
{
    std::cout << "[SomeIPClient] Instance created\n";
}

bool SomeIPTelemetrySourceImpl::isConnected() {
    std::lock_guard<std::mutex> lock(mutex_);
    return connected_ && proxy_ && proxy_->isAvailable();
}