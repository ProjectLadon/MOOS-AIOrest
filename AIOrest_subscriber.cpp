/************************************************************/
/*    NAME:                                               */
/*    ORGN: Project Ladon                                     */
/*    FILE: AIOrest_subscriber.cpp                      */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#include <string>
#include <vector>
#include <memory>
#include <cstdlib>
#include "AIOrest.h"
#include "AIOrest_configuration.hpp"
#include "AIOrest_subscriber.hpp"
#include "schema/subscribe_schema.h"
#include "schema/data_response_schema.h"
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

std::unique_ptr<Subscriber> Subscriber::subscriberFactory(AIOconf *iface, rapidjson::Value &v) {
    if (!subscriber_validator) createValidators();
    if (!v.Accept(*subscriber_validator)) { // Validate the incoming JSON
        StringBuffer sb;
        subscriber_validator->GetInvalidSchemaPointer().StringifyUriFragment(sb);
        cerr << "Invalid configuration schema: " << sb.GetString() << endl;
        cerr << "Invalid keyword: " << subscriber_validator->GetInvalidSchemaKeyword() << endl;
        sb.Clear();
        subscriber_validator->GetInvalidDocumentPointer().StringifyUriFragment(sb);
        cerr << "Invalid document: " << sb.GetString() << endl;
        std::exit(-1);
    }
    if (v["varType"].GetString() == "DOUBLE") {
        return std::unique_ptr<Subscriber>(new SubscriberDouble(iface, v));
    } else if (v["varType"].GetString() == "STRING") {
        return std::unique_ptr<Subscriber>(new SubscriberString(iface, v));
    } else if (v["varType"].GetString() == "BINARY") {
        return std::unique_ptr<Subscriber>(new SubscriberBinary(iface, v));
    }
    return std::unique_ptr<Subscriber>(nullptr);
}

void Subscriber::createValidators() {
    rapidjson::Document d, e;
        if (d.Parse(reinterpret_cast<char*>(subscribe_schema_json), subscribe_schema_json_len).HasParseError()) {
            cerr << "JSON parse error " << GetParseError_En(d.GetParseError());
            cerr << " in subscriber schema at offset " << d.GetErrorOffset() << endl;
            std::exit(-1);
        }
        if (e.Parse(reinterpret_cast<char*>(data_response_schema_json), data_response_schema_json_len).HasParseError()) {
            cerr << "JSON parse error " << GetParseError_En(e.GetParseError());
            cerr << " in data response schema at offset " << e.GetErrorOffset() << endl;
            std::exit(-1);
        }
        subscriber_schema.reset(new rapidjson::SchemaDocument(d));
        data_schema.reset(new rapidjson::SchemaDocument(e));
        subscriber_validator.reset(new rapidjson::SchemaValidator(*subscriber_schema));
        data_validator.reset(new rapidjson::SchemaValidator(*data_schema));
}

SubscriberString::SubscriberString(AIOconf *_iface, rapidjson::Value &v) {
    iface = _iface;
    if (v.HasMember("group")) {
        group = v["group"].GetString();
    } else group = "";
    feed = v["feed"].GetString();
    varName = v["variable"].GetString();
}

bool SubscriberString::poll(AIOrest* a) {
    auto result = iface->get(feed, group);
    if (result) {
        if(result->Accept(*data_validator)) {// Validate the incoming JSON
            data = (*result)["value"].GetString();
            lastTime = (*result)["created_at"].GetString();
            lastEpoch = (*result)["created_epoch"].GetInt();
            a->notify(varName, data);
        } else {
            StringBuffer sb;
            data_validator->GetInvalidSchemaPointer().StringifyUriFragment(sb);
            cerr << "Invalid configuration schema: " << sb.GetString() << endl;
            cerr << "Invalid keyword: " << data_validator->GetInvalidSchemaKeyword() << endl;
            sb.Clear();
            data_validator->GetInvalidDocumentPointer().StringifyUriFragment(sb);
            cerr << "Invalid document: " << sb.GetString() << endl;
        }
    }
    return false;
}

ACTable SubscriberString::buildReport() {
    ACTable tab(1);
    tab << varName;
    tab.addHeaderLines();
    tab << data;
    return tab;
}

SubscriberDouble::SubscriberDouble(AIOconf *_iface, rapidjson::Value &v) {
    iface = _iface;
    if (v.HasMember("group")) {
        group = v["group"].GetString();
    } else group = "";
    feed = v["feed"].GetString();
    varName = v["variable"].GetString();
}

bool SubscriberDouble::poll(AIOrest* a) {
    auto result = iface->get(feed, group);
    if (result) {
        if(result->Accept(*data_validator)) {// Validate the incoming JSON
            data = (*result)["value"].GetFloat();
            lastTime = (*result)["created_at"].GetString();
            lastEpoch = (*result)["created_epoch"].GetInt();
            a->notify(varName, data);
        } else {
            StringBuffer sb;
            data_validator->GetInvalidSchemaPointer().StringifyUriFragment(sb);
            cerr << "Invalid configuration schema: " << sb.GetString() << endl;
            cerr << "Invalid keyword: " << data_validator->GetInvalidSchemaKeyword() << endl;
            sb.Clear();
            data_validator->GetInvalidDocumentPointer().StringifyUriFragment(sb);
            cerr << "Invalid document: " << sb.GetString() << endl;
        }
    }
    return false;
}

ACTable SubscriberDouble::buildReport() {
    ACTable tab(1);
    tab << varName;
    tab.addHeaderLines();
    tab << to_string(data);
    return tab;
}

SubscriberBinary::SubscriberBinary(AIOconf *_iface, rapidjson::Value &v) {
    iface = _iface;
    if (v.HasMember("group")) {
        group = v["group"].GetString();
    } else group = "";
    feed = v["feed"].GetString();
    varName = v["variable"].GetString();
}

bool SubscriberBinary::poll(AIOrest* a) {
    auto result = iface->get(feed, group);
    if (result) {
        if(result->Accept(*data_validator)) {   // Validate the incoming JSON
            if (decode((*result)["value"].GetString())) {
                lastTime = (*result)["created_at"].GetString();
                lastEpoch = (*result)["created_epoch"].GetInt();
                a->notify(varName, data);
            }
        } else {
            StringBuffer sb;
            data_validator->GetInvalidSchemaPointer().StringifyUriFragment(sb);
            cerr << "Invalid configuration schema: " << sb.GetString() << endl;
            cerr << "Invalid keyword: " << data_validator->GetInvalidSchemaKeyword() << endl;
            sb.Clear();
            data_validator->GetInvalidDocumentPointer().StringifyUriFragment(sb);
            cerr << "Invalid document: " << sb.GetString() << endl;
        }
    }
    return false;
}

ACTable SubscriberBinary::buildReport() {
    ACTable tab(1);
    tab << varName;
    tab.addHeaderLines();
    tab << cppcodec::hex_lower::encode(data.data(), data.size());
    return tab;
}

bool SubscriberBinary::decode(std::string incoming) {
    data = cppcodec::base64_url::decode(incoming.c_str(), incoming.size());
    return true;
}
