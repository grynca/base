#ifndef CONFIG_H
#define CONFIG_H

#include "containers/fast_vector.h"
#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <fstream>


namespace grynca {

    class CfgValue
    {
    public:
        CfgValue();
        CfgValue(const std::string& val);
        CfgValue operator=(const std::string& val);

        std::string asString;
        float asFloat;
        int asInt;
        bool asBool;
    };

    // config entries are only singleline
    // only endline comments supported
    class Config
    {
    public:
        friend std::istream& operator>>(std::istream& is, Config& c);          //write to config
        friend std::ostream& operator<<(std::ostream& os, Config& c);          //read from config

        Config(std::string delim="=", std::string comment="#");

        bool loadFromFile(const std::string& filepath);
        void loadFromVector(const fast_vector<std::string>& vec);

        std::map<std::string,CfgValue> data;

        // get:     int scr_w = config.data["screen_width"].asInt;
        // set:     config.data["screen_width"]=CfgValue("800");
    protected:
        static void _trim(std::string& s);
        void addSourceLine_(const std::string& source_line);

        std::string _delim;
        std::string _comment;

        fast_vector<std::string> _lines;    // saved source lines (without parsing)
        std::map<std::string, unsigned int> _to_line_id;        // maps key to line id
    };
}


#include "Config.inl"
#endif //CONFIG_H
