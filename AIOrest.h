/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: AIOrest.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef AIOrest_HEADER
#define AIOrest_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "AIOrest_configuration.hpp"
#include "rapidjson/rapidjson.h"
#include <memory>
#include <chrono>
#include <string>
#include <vector>

class AIOrest : public AppCastingMOOSApp
{
    public:
        AIOrest() {};
        ~AIOrest() {};
        bool notify(const std::string &var, const std::string &val) {
            Notify(var, val);
        };
        bool notify(const std::string &var, const double &val) {
            Notify(var, val);
        };
        bool notify(const std::string &var, const std::vector<unsigned char> &val) {
            Notify(var, val);
        };
        bool registerVar(const std::string &var) {return Register(var);};

    protected: // Standard MOOSApp functions to overload
        bool OnNewMail(MOOSMSG_LIST &NewMail);
        bool Iterate();
        bool OnConnectToServer();
        bool OnStartUp();

    protected: // Standard AppCastingMOOSApp function to overload
        bool buildReport();
        void registerVariables();

    protected: // Own functions
        std::unique_ptr<rapidjson::Document> loadFile(std::string path);
        std::unique_ptr<rapidjson::Document> parseJSON(std::string jsonstring);

    private: // Configuration variables
        std::unique_ptr<AIOconf> conf;
        int pubFrequency = 60;
        int subFrequency = 60;

    private: // State variables
        std::chrono::time_point<std::chrono::system_clock> lastPub;
        std::chrono::time_point<std::chrono::system_clock> lastSub;
};

#endif
