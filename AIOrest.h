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
        bool registerVar(const std::string &var) {Register(var);};

    protected: // Standard MOOSApp functions to overload
        bool OnNewMail(MOOSMSG_LIST &NewMail);
        bool Iterate();
        bool OnConnectToServer();
        bool OnStartUp();

    protected: // Standard AppCastingMOOSApp function to overload
        bool buildReport();

    protected:
        void registerVariables();

    private: // Configuration variables
        std::unique_ptr<AIOconf> conf;
        int pubFrequency = 60;
        int subFrequency = 60;

    private: // State variables
        std::chrono::system_time lastPub;
        std::chrono::system_time lastSub;
};

#endif
