#include <rapp/cloud/service_controller.hpp>
#include <rapp/cloud/available_services.hpp>
#include <iostream>
#include <alerror/alerror.h>
#include <alproxies/altexttospeechproxy.h>

int main()
{
    rapp::cloud::platform info = {"rapp.ee.auth.gr", "9001", "rapp_token"}; 
    rapp::cloud::service_controller ctrl(info);
    AL::ALTextToSpeechProxy tts("10.130.3.25", 9559);
    
    auto cb = [&](std::vector<std::pair<std::string, std::string>> services) {
         std::cout << "available services: " << std::endl;
         for (const auto & pair : services) {
            std::cout << pair.first << " " << pair.second << std::endl;
            tts.say(pair.first);
         }   
    };
    ctrl.make_call<rapp::cloud::available_services>(cb);

    exit(0);
}
