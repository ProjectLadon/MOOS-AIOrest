/************************************************************/
/*    NAME:                                               */
/*    ORGN: Project Ladon                                     */
/*    FILE: AIOrest_configuration.cpp                      */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#include <string>
#include <vector>
#include <memory>
#include <cstdlib>
#include <sstream>
#include "AIOrest.h"
#include "AIOrest_configuration.hpp"
#include "AIOrest_publisher.hpp"
#include "AIOrest_subscriber.hpp"
#include "schema/configuration_schema.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include "rapidjson/schema.h"
#include "rapidjson/writer.h"
#include "rapidjson/error/en.h"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

extern "C" {
	#include <curl/curl.h>
}

using namespace std;
using namespace rapidjson;

AIOconf::AIOconf(rapidjson::Document &d) {
    rapidjson::Document v;
    if (v.Parse(reinterpret_cast<char*>(configuration_schema_json), configuration_schema_json_len).HasParseError()) {
        cerr << "JSON parse error " << GetParseError_En(v.GetParseError());
        cerr << " in configuration schema at offset " << v.GetErrorOffset() << endl;
        std::exit(-1);
    }
    SchemaDocument sd(v);
    SchemaValidator validator(sd);
    if (d.Accept(validator)) {
        url = d["url"].GetString();
        username = d["username"].GetString();
        key = d["key"].GetString();
        if (d.HasMember("latVar")) latVar = d["latVar"].GetString();
        if (d.HasMember("lonVar")) lonVar = d["lonVar"].GetString();
    } else {
        StringBuffer sb;
        validator.GetInvalidSchemaPointer().StringifyUriFragment(sb);
        cerr << "Invalid configuration schema: " << sb.GetString() << endl;
        cerr << "Invalid keyword: " << validator.GetInvalidSchemaKeyword() << endl;
        sb.Clear();
        validator.GetInvalidDocumentPointer().StringifyUriFragment(sb);
        cerr << "Invalid document: " << sb.GetString() << endl;
        std::exit(-1);
    }
}

std::unique_ptr<rapidjson::Document> AIOconf::get(std::string feed, std::string group) {
    int httpcode;
    std::string reqURL = url + "/" + username + "/" + feed +"/data/last";
    auto response = makeRequest(reqURL, nullptr, &httpcode);
    if (response && (httpcode == CURLE_OK)) return response;
    return std::unique_ptr<rapidjson::Document>(nullptr);
}

std::unique_ptr<rapidjson::Document> AIOconf::put(const std::string feed, const std::string group, const std::string data, int *httpcode) {
    rapidjson::Document d;
    rapidjson::SetValueByPointer(d, "/value", data.c_str());
    if (latVar != "") rapidjson::SetValueByPointer(d, "/lat", to_string(lat).c_str());
    if (lonVar != "") rapidjson::SetValueByPointer(d, "/lon", to_string(lon).c_str());
    std::string reqURL = url + "/" + username + "/" + feed +"/data";
    return makeRequest(reqURL, &d, httpcode);
}

bool AIOconf::publish() {
    if (publishPtr->get()->publish()) {
        publishPtr++;
        if (publishPtr == publishers.end()) publishPtr = publishers.begin();
        return true;
    } else {
        auto oldptr = publishPtr;
        publishPtr++;
        bool result = false;
        while ((publishPtr != oldptr) && (!result)) {
            if (publishPtr == publishers.end()) publishPtr = publishers.begin();
            result = publishPtr->get()->publish();
            publishPtr++;
        }
        return result;
    }
    return false;
}

bool AIOconf::poll(AIOrest *a) {
    bool result = true;
    for (auto &s: subscribers) {
        result &= s->poll(a);
    }
    return result;
}

bool AIOconf::procMail(CMOOSMsg &msg) {
    bool result = false;
    for (auto &p: publishers) {
        result |= p->procMail(msg);
    }
    if ((!result) && (latVar != "") && (msg.GetKey() == latVar)) {
        lat = msg.GetDouble();
        return true;
    } else if ((!result) && (lonVar != "") && (msg.GetKey() == lonVar)) {
        lon = msg.GetDouble();
        return true;
    }
    return result;
}

bool AIOconf::subscribe(AIOrest *a) {
    bool result = true;
    for (auto &p: publishers) {
        result &= p->subscribe(a);
    }
    return result;
}

std::unique_ptr<rapidjson::Document> AIOconf::makeRequest (std::string url, rapidjson::Document *request, int *httpcode) {
    std::unique_ptr<rapidjson::Document> result;
	CURLcode ret;
	CURL *hnd;
	struct curl_slist *slist1;
    char errbuf[1024];
    std::string response;

    // key header string
    std::string keyheader = "X-AIO-Key: " + key;

    // assemble request
	slist1 = nullptr;
    slist1 = curl_slist_append(slist1, keyheader.c_str());

    // initialize request
    hnd = curl_easy_init();
    curl_easy_setopt(hnd, CURLOPT_URL, url.c_str());
    curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/7.38.0");
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
    curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(hnd, CURLOPT_SSH_KNOWNHOSTS, "/home/debian/.ssh/known_hosts");
    curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(hnd, CURLOPT_ERRORBUFFER, errbuf);
    curl_easy_setopt(hnd, CURLOPT_FAILONERROR, 1);

    // if we have some JSON to transmit, make a POST; otherwise, a GET
    if (request) {
        slist1 = curl_slist_append(slist1, "Content-Type: application/json");
        StringBuffer buf;
        Writer<StringBuffer> writer(buf);
        request->Accept(writer);
    	curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, buf.GetString());
    	curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t)strlen(buf.GetString()));
    	curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
    }

	// set up data writer callback
	curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	curl_easy_setopt(hnd, CURLOPT_WRITEDATA, (void *)&response);

	// make request
	ret = curl_easy_perform(hnd);

	// clean up request
	curl_easy_cleanup(hnd);
	hnd = nullptr;
	curl_slist_free_all(slist1);
	slist1 = nullptr;

    // if we have a good status code, record this as last contact
    if (httpcode) *httpcode = ret;
	if (ret == CURLE_OK) {     // If we got a good response, parse the incoming JSON
        if (result->Parse(response.c_str()).HasParseError()) {
            cerr << "JSON parse error " << GetParseError_En(result->GetParseError());
            cerr << " in incoming JSON at offset " << result->GetErrorOffset() << endl;
            cerr << "Faulty JSON: " << response;
            return std::unique_ptr<rapidjson::Document>(nullptr);
        } else return result;
    } else {
        return std::unique_ptr<rapidjson::Document>(nullptr);
    }
}

size_t AIOconf::WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
	string *target = (string *)userp;
	char *ptr = (char *)contents;

	//target->clear();
	size *= nmemb;
	target->reserve(size);
	for (size_t i = 0; i < size; i++) {
		*target += ptr[i];
	}
    return size;
}
