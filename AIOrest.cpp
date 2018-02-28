/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: AIOrest.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <chrono>
#include "MBUtils.h"
#include "ACTable.h"
#include "AIOrest.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include "rapidjson/schema.h"
#include "rapidjson/writer.h"
#include "rapidjson/error/en.h"
#include "rapidjson/istreamwrapper.h"

using namespace std;
using namespace std::chrono;
using namespace std::literals::chrono_literals;

//---------------------------------------------------------
// Procedure: OnNewMail

bool AIOrest::OnNewMail(MOOSMSG_LIST &NewMail) {
    AppCastingMOOSApp::OnNewMail(NewMail);

    bool result = false;
    for(auto &msg: NewMail) {
        if (!(conf->procMail(msg)) || (msg.GetKey() == "APPCAST_REQ")) {
            reportRunWarning("Unhandled Mail: " + msg.GetKey());
        } else result = true;
    }
    return result;
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool AIOrest::OnConnectToServer() {
    registerVariables();
    return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool AIOrest::Iterate() {
    AppCastingMOOSApp::Iterate();
    if ((std::chrono::system_clock::now() - lastSub) > ((60/subFrequency) * 1s)) {
        if (conf->poll(this)) lastSub = std::chrono::system_clock::now();
    }
    if ((std::chrono::system_clock::now() - lastPub) > ((60/pubFrequency) * 1s)) {
        if (conf->publish()) lastPub = std::chrono::system_clock::now();
    }
    AppCastingMOOSApp::PostReport();
    return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool AIOrest::OnStartUp() {
    AppCastingMOOSApp::OnStartUp();

    STRING_LIST sParams;
    m_MissionReader.EnableVerbatimQuoting(false);
    if(!m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
        reportConfigWarning("No config block found for " + GetAppName());
        std::exit(-1);
    }

    bool handled = false;
    for (auto &p: sParams) {    // Find and load configuration first
        std::string param = toupper(biteStringX(p, '='));
        if (param == std::string("AIOCONF")) {
            auto confjson = loadFile(p);
            if (confjson) {
                conf = std::unique_ptr<AIOconf>(new AIOconf(*confjson));
                handled = true;
                break;
            } else {
                reportConfigWarning("AIOconf " + p + "contained invalid filename or file contained invalid JSON, exiting");
                std::exit(-1);
            }
        }
    }
    if (!handled) {
        reportConfigWarning("No AIOconf block found for " + GetAppName() + "; exiting");
        std::exit(-1);
    }

    for (auto &p: sParams) {    // process the rest
        bool handled = false;
        std::string orig = p;
        std::string param = toupper(biteStringX(p, '='));
        if (param == std::string("PUBLISH")) {
            auto pub = parseJSON(p);
            if (pub) {
                conf->getPublishers().push_back(move(Publisher::publisherFactory(conf.get(), *pub)));
            } else std::exit(-1);
        } else if (param == std::string("PUBFILE")) {
            auto pubarray = loadFile(p);
            if (pubarray && pubarray->IsArray()) {
                for (auto &c: pubarray->GetArray()) {
                    conf->getPublishers().push_back(move(Publisher::publisherFactory(conf.get(), c)));
                }
            }
        } else if (param == std::string("SUBSCRIBE")) {
            auto sub = parseJSON(p);
            if (sub) {
                conf->getSubscribers().push_back(move(Subscriber::subscriberFactory(conf.get(), *sub)));
            } else std::exit(-1);
        } else if (param == std::string("SUBFILE")) {
            auto subarray = loadFile(p);
            if (subarray && subarray->IsArray()) {
                for (auto &c: subarray->GetArray()) {
                    conf->getSubscribers().push_back(move(Subscriber::subscriberFactory(conf.get(), c)));
                }
            }
        } else if (param == std::string("CONFFILE")) {
            auto confarray = loadFile(p);
            if (confarray && confarray->IsArray()) {
                for (auto &c: confarray->GetArray()) {
                    if (c.HasMember("direction") && (c["direction"].GetString() == std::string("publish"))) {
                        conf->getPublishers().push_back(move(Publisher::publisherFactory(conf.get(), c)));
                    } else if (c.HasMember("direction") && (c["direction"].GetString() == std::string("subscribe"))) {
                        conf->getSubscribers().push_back(move(Subscriber::subscriberFactory(conf.get(), c)));
                    }
                }
            }
        } else if (param == std::string("SUBFREQUENCY")) {
            subFrequency = atof(p.c_str());
        } else if (param == std::string("PUBFREQUENCY")) {
            pubFrequency = atof(p.c_str());
        } else if (param != std::string("AIOCONF")) {
            reportUnhandledConfigWarning(orig);
        }
    }
    registerVariables();
    return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void AIOrest::registerVariables() {
    AppCastingMOOSApp::RegisterVariables();
    conf->subscribe(this);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool AIOrest::buildReport()
{
  m_msgs << "============================================ \n";
  m_msgs << "File: pAIOrest                               \n";
  m_msgs << "============================================ \n";
  m_msgs << "Publishers:                                  \n";
  m_msgs << "============================================ \n";
  for (auto &p: conf->getPublishers()) {
      m_msgs << p->buildReport().getFormattedString();
  }
  for (auto &p: conf->getSubscribers()) {
      m_msgs << p->buildReport().getFormattedString();
  }
  return(true);
}

std::unique_ptr<rapidjson::Document> AIOrest::loadFile(std::string path) {
    std::unique_ptr<rapidjson::Document> result = std::unique_ptr<rapidjson::Document>(new rapidjson::Document);
    ifstream infile(path);
    if (infile.is_open()) {
        rapidjson::IStreamWrapper instream(infile);
        if (result->ParseStream(instream).HasParseError()) {
            cerr << "JSON parse error " << GetParseError_En(result->GetParseError());
            cerr << " in JSON from file " << path << " at offset ";
            cerr << result->GetErrorOffset() << endl;
        } else return result;
    }
    return std::unique_ptr<rapidjson::Document>(nullptr);
}

std::unique_ptr<rapidjson::Document> parseJSON(std::string jsonstring) {
    std::unique_ptr<rapidjson::Document> result = std::unique_ptr<rapidjson::Document>(new rapidjson::Document);
    if (result->Parse(jsonstring.c_str()).HasParseError()) {
        cerr << "JSON parse error " << GetParseError_En(result->GetParseError());
        cerr << " in incoming JSON at offset " << result->GetErrorOffset() << endl;
        cerr << "Faulty JSON: " << jsonstring;
        return std::unique_ptr<rapidjson::Document>(nullptr);
    } else return result;
}
