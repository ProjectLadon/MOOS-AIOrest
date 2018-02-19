/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: AIOrest.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef AIOrest_HEADER
#define AIOrest_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class AIOrest : public AppCastingMOOSApp
{
 public:
   AIOrest();
   ~AIOrest();
   bool notifyString(const std::string &var, const std::string &val) {
	   Notify(var, val);
   };
   bool notifyDouble(const std::string &var, const double &val) {
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

 private: // State variables
};

#endif
