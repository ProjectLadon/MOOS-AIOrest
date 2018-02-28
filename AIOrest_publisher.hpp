/************************************************************/
/*    NAME:                                               */
/*    ORGN: Project Ladon                                     */
/*    FILE: AIOrest_publisher.hpp                      */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef AIOrest_Publisher_HEADER
#define AIOrest_Publisher_HEADER

#include <string>
#include <vector>
#include <memory>
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/schema.h"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "ACTable.h"

class AIOrest;  // forward declared because we only use the pointers here
class AIOconf;  // forward declared because we only use the pointers here

class Publisher {
    public:
        static std::unique_ptr<Publisher> publisherFactory(AIOconf *iface, rapidjson::Value &v);
        virtual bool publish() = 0;
        virtual bool procMail(CMOOSMsg &msg) = 0;
        virtual ACTable buildReport() = 0;
        bool subscribe(AIOrest *a);
        bool isFresh() {return fresh;};
        bool isUpdateOnly() {return updateOnly;};

    protected:  // methods
        Publisher() {};
        void setFresh(bool n) {fresh = n;};
    protected:  // members
        bool        fresh;
        bool        updateOnly;
        std::string varName;
        std::string feed;
        std::string group;
        AIOconf *iface;

    private:
        static std::unique_ptr<rapidjson::SchemaDocument> publisher_schema;
        static std::unique_ptr<rapidjson::SchemaValidator> publisher_validator;
};

class PublisherString : public Publisher {
    public:
        PublisherString(AIOconf *iface, rapidjson::Value &v);
        bool publish();
        bool procMail(CMOOSMsg &msg);
        ACTable buildReport();

    private:    // members
        std::string data;
};

class PublisherDouble : public Publisher {
    public:
        PublisherDouble(AIOconf *iface, rapidjson::Value &v);
        bool publish();
        bool procMail(CMOOSMsg &msg);
        ACTable buildReport();

    private:    // members
        double data;
};

class PublisherBinary : public Publisher {
    public:
        PublisherBinary(AIOconf *iface, rapidjson::Value &v);
        bool publish();
        bool procMail(CMOOSMsg &msg);
        ACTable buildReport();

    private:  // members
        std::string encode();
        std::vector<unsigned char> data;
};

#endif
