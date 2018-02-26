/************************************************************/
/*    NAME:                                               */
/*    ORGN: Project Ladon                                     */
/*    FILE: AIOrest_subscriber.hpp                      */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef AIOrest_Subscriber_HEADER
#define AIOrest_Subscriber_HEADER

#include <string>
#include <vector>
#include <memory>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class AIOrest;  // forward declared because we only use the pointers here
class AIOconf;  // forward declared because we only use the pointers here

class Subscriber {
    public:
        static std::unique_ptr<Subscriber> subscriberFactory(AIOconf *iface, rapidjson::Value &v);
        virtual bool poll(AIOrest *a) = 0;
        virtual ACTable buildReport() = 0;
        bool isNew(std::string newTime) {return (newTime != lastTime);};
        bool isNew(long int epoch) {return (epoch > lastEpoch);};

    protected:  // methods
        Subscriber() {};
    protected:  // members
        std::string varName;
        std::string feed;
        std::string group;
        std::string lastTime;
        long int lastEpoch;
        AIOconf *iface;
        static std::unique_ptr<rapidjson::SchemaDocument> data_schema;
        static std::unique_ptr<rapidjson::Validator> data_validator;

    private:
        static void createValidators();
        static std::unique_ptr<rapidjson::SchemaDocument> subscriber_schema;
        static std::unique_ptr<rapidjson::Validator> subscriber_validator;
};

class SubscriberString : public Subscriber {
    public:
        SubscriberString(AIOconf *iface, rapidjson::Value &v);
        bool poll(AIOrest* a);
        ACTable buildReport();

    private:    // members
        std::string data;
};

class SubscriberDouble : public Subscriber {
    public:
        SubscriberDouble(AIOconf *iface, rapidjson::Value &v);
        bool poll(AIOrest* a);
        ACTable buildReport();

    private:    // members
        double data;
};

class SubscriberBinary : public Subscriber {
    public:
        SubscriberBinary(AIOconf *iface, rapidjson::Value &v);
        bool poll(AIOrest* a);
        ACTable buildReport();

    private:  // members
        bool decode(std::string incoming);
        std::vector<unsigned char> data;
};
