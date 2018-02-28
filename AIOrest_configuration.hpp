/************************************************************/
/*    NAME:                                               */
/*    ORGN: Project Ladon                                     */
/*    FILE: AIOrest_configuration.hpp                      */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef AIOrest_Configuration_HEADER
#define AIOrest_Configuration_HEADER

#include <string>
#include <vector>
#include <memory>
#include "AIOrest_publisher.hpp"
#include "AIOrest_subscriber.hpp"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "ACTable.h"

class AIOrest;  // forward declared because we only use the pointers here

class AIOconf {
    public:
        AIOconf(rapidjson::Document &d);
        std::unique_ptr<rapidjson::Document> get(std::string feed, std::string group);
        std::unique_ptr<rapidjson::Document> put(const std::string feed, const std::string group, const std::string data, int *httpcode = nullptr);
        bool publish();
        bool poll(AIOrest *a);
        bool procMail(CMOOSMsg &msg);
        bool subscribe(AIOrest *a);
        std::vector<std::unique_ptr<Publisher>> &getPublishers() {return publishers;};
        std::vector<std::unique_ptr<Subscriber>> &getSubscribers() {return subscribers;};
        const std::string &getURL() {return url;};

    private:    // nethods
        std::unique_ptr<rapidjson::Document> makeRequest (std::string url, rapidjson::Document *request = nullptr, int *httpcode = nullptr);
        static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);

    private:    // members
        std::string url;
        std::string username;
        std::string key;
        std::string latVar;
        std::string lonVar;
        double      lat;
        double      lon;
        std::vector<std::unique_ptr<Publisher>> publishers;
        std::vector<std::unique_ptr<Subscriber>> subscribers;
        std::vector<std::unique_ptr<Publisher>>::iterator publishPtr;
};

#endif
