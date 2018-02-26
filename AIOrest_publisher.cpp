/************************************************************/
/*    NAME:                                               */
/*    ORGN: Project Ladon                                     */
/*    FILE: AIOrest_publisher.cpp                      */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#include <string>
#include <vector>
#include <memory>
#include <cstdlib>
#include "AIOrest.h"
#include "AIOrest_configuration.hpp"
#include "AIOrest_publisher.hpp"
#include "schema/publish_schema.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include "rapidjson/schema.h"
#include "rapidjson/writer.h"
#include "rapidjson/error/en.h"
#include "cppcodec/hex_lower.hpp"
#include "cppcodec/base64_url.hpp"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

using namespace std;
using namespace rapidjson;

std::unique_ptr<Publisher> Publisher::publisherFactory(AIOconf *iface, rapidjson::Value &v) {
    if (!publisher_validator) { // If we haven't created the schema validator yet, create it
        rapidjson::Document d;
        if (d.Parse(reinterpret_cast<char*>(publish_schema_json), publish_schema_json_len)).HasParseError()) {
            cerr << "JSON parse error " << GetParseError_En(d.GetParseError());
            cerr << " in publisher schema at offset " << d.GetErrorOffset() << endl;
            std::exit();
        }
        publisher_schema.reset(new rapidjson::SchemaDocument(d));
        publisher_validator.reset(new rapidjson::Validator(publisher_schema));
    }
    if (!v.Accept(*publisher_validator)) { // Validate the incoming JSON
        StringBuffer sb;
        publisher_validator.GetInvalidSchemaPointer().StringifyUriFragment(sb);
        cerr << "Invalid configuration schema: " << sb.GetString() << endl;
        cerr << "Invalid keyword: " << publisher_validator.GetInvalidSchemaKeyword() << endl;
        sb.Clear();
        publisher_validator.GetInvalidDocumentPointer().StringifyUriFragment(sb);
        cerr << "Invalid document: " << sb.GetString() << endl;
        std::exit();
    }
    if (v["varType"].GetString() == "DOUBLE") {
        return std::unique_ptr<Publisher>(new PublisherDouble(iface, v));
    } else if (v["varType"].GetString() == "STRING") {
        return std::unique_ptr<Publisher>(new PublisherString(iface, v));
    } else if (v["varType"].GetString() == "BINARY") {
        return std::unique_ptr<Publisher>(new PublisherBinary(iface, v));
    }
    return std::unique_ptr<Publisher>(nullptr);
}

bool Publisher::subscribe(AIOrest *a) {return a->registerVar(varName);}

PublisherString::PublisherString(AIOconf *iface, rapidjson::Value &v) : iface(iface) {
    fresh = false;
    if (v.HasMember("updateOnly")) {
        updateOnly = v["updateOnly"].GetBool();
    } else updateOnly = false;
    if (v.HasMember("group")) {
        group = v["group"].GetString();
    } else group = "";
    feed = v["feed"].GetString();
    varName = v["variable"].GetString();
}

bool PublisherString::publish() {
    int code = 0;
    auto result = iface->put(feed, group, data, code);
    if ((code < 200) || (code > 299)) return false;
    if (!result) return false;
    return true;
}

bool PublisherString::procMail(CMOOSMsg &msg) {
    if (msg.GetKey() == varName) {
        data = msg.GetString();
        return true;
    }
    return false;
}

ACTable PublisherString::buildReport() {
    ACTable tab(1);
    tab << varName;
    tab.addHeaderLines();
    tab << data;
    return tab;
}

PublisherDouble::PublisherDouble(AIOconf *iface, rapidjson::Value &v) : iface(iface) {
    fresh = false;
    if (v.HasMember("updateOnly")) {
        updateOnly = v["updateOnly"].GetBool();
    } else updateOnly = false;
    if (v.HasMember("group")) {
        group = v["group"].GetString();
    } else group = "";
    feed = v["feed"].GetString();
    varName = v["variable"].GetString();
}

bool PublisherDouble::publish() {
    int code = 0;
    auto result = iface->put(feed, group, data, code);
    if ((code < 200) || (code > 299)) return false;
    if (!result) return false;
    return true;
}

bool PublisherDouble::PublisherDouble::procMail(CMOOSMsg &msg) {
    if (msg.GetKey() == varName) {
        data = msg.GetDouble();
        return true;
    }
    return false;
}

ACTable PublisherDouble::buildReport() {
    ACTable tab(1);
    tab << varName;
    tab.addHeaderLines();
    tab << to_string(data);
    return tab;
}

PublisherBinary::PublisherBinary(AIOconf *iface, rapidjson::Value &v) : iface(iface) {
    fresh = false;
    if (v.HasMember("updateOnly")) {
        updateOnly = v["updateOnly"].GetBool();
    } else updateOnly = false;
    if (v.HasMember("group")) {
        group = v["group"].GetString();
    } else group = "";
    feed = v["feed"].GetString();
    varName = v["variable"].GetString();
}

bool PublisherBinary::publish() {
    int code = 0;
    auto result = iface->put(feed, group, data, code);
    if ((code < 200) || (code > 299)) return false;
    if (!result) return false;
    return true;
}

bool PublisherBinary::procMail(CMOOSMsg &msg) {
    if (msg.GetKey() == varName) {
        data = msg.GetBinaryDataAsVector();
        return true;
    }
    return false;
}

ACTable PublisherBinary::buildReport() {
    ACTable tab(1);
    tab << varName;
    tab.addHeaderLines();
    tab << cppcodec::hex_lower::encode(data.data(), data.size());
    return tab;
}

std::string PublisherBinary::encode() {
    return cppcodec::base64_url::encode(data.data(), data.size());
}
