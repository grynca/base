#ifndef CONFIG_H
#define CONFIG_H

#include "containers/fast_vector.h"
#include <map>

namespace grynca {

    class CfgValue
    {
    public:
        CfgValue();
        CfgValue(const std::string& val);
        CfgValue(i32 val);
        CfgValue(bool val);
        CfgValue(f32 val);

        CfgValue operator=(const std::string& val);

        operator std::string() { return asString; }
        operator f32() { return asFloat; }
        operator i32() { return asInt; }
        operator bool() { return asBool; }

        std::string asString;
        f32 asFloat;
        i32 asInt;
        bool asBool;
    };

    // config entries are only singleline
    // only endline comments supported

    class Config
    {
    public:
        typedef std::map<std::string/*key*/, CfgValue> ConfigSectionMap;
        friend std::istream& operator>>(std::istream& is, Config& c);          //write to config
        friend std::ostream& operator<<(std::ostream& os, Config& c);          //read from config
    public:
        Config(const std::string& delim="=", const std::string& comment="#", const std::string& section="!!");

        bool loadFromFile(const std::string& filepath, const std::string& section = "");
        void loadFromVector(const fast_vector<std::string>& vec, const std::string& section = "");

        // get:     config.getData("section_name")["screen_width"].asInt;
        // set:     config.accData("section_name")["screen_width"]=CfgValue("800");

        // global section is empty string
        const ConfigSectionMap& getData(const std::string& section = "");
        ConfigSectionMap& accData(const std::string& section = "");

        const std::string& getCurrentSectionName()const;
        void setCurrentSectionName(const std::string &section);
    protected:
        static void trim_(std::string &s);
        void addSourceLine_(const std::string& source_line, ConfigSectionMap*& curr_section);

        std::string delim_;
        std::string comment_;
        std::string section_;

        std::string curr_section_name_;

        fast_vector<std::string> lines_;    // saved source lines (without parsing)
        std::map<std::string, u32> to_line_id_;        // maps key to line id
        std::map<std::string/*section*/, ConfigSectionMap > data_;
    };
}


#include "Config.inl"
#endif //CONFIG_H
