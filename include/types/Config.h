#ifndef CONFIG_H
#define CONFIG_H

#include "containers/fast_vector.h"
#include "containers/HashMap.h"

namespace grynca {

    class CfgValue
    {
    public:
        CfgValue();
        CfgValue(const ustring& val);
        CfgValue(i32 val);
        CfgValue(bool val);
        CfgValue(f32 val);

        CfgValue operator=(const ustring& val);

        operator ustring() { return asString; }
        operator f32() { return asFloat; }
        operator i32() { return asInt; }
        operator bool() { return asBool; }

        ustring asString;
        f32 asFloat;
        i32 asInt;
        bool asBool;
    };

    // config entries are only singleline
    // only endline comments supported

    class Config
    {
    public:
        typedef HashMap<CfgValue, ustring, Hasher<ustring> > ConfigSectionMap;
        friend std::istream& operator>>(std::istream& is, Config& c);          //write to config
        friend std::ostream& operator<<(std::ostream& os, Config& c);          //read from config
    public:
        Config(const ustring& delim="=", const ustring& comment="#", const ustring& section="!!");

        bool loadFromFile(const ustring& filepath, const ustring& section = "");
        void loadFromVector(const fast_vector<ustring>& vec, const ustring& section = "");

        // get:     config.getData("section_name")["screen_width"].asInt;
        // set:     config.accData("section_name")["screen_width"]=CfgValue("800");

        // global section is empty string
        ConfigSectionMap& accData(const ustring& section = "");
        // data of current section
        ConfigSectionMap& accCurrData();

        const ustring& getCurrentSectionName()const;
        void setCurrentSectionName(const ustring &section);
    protected:
        static void trim_(ustring& s);
        void addSourceLine_(const ustring& source_line, ConfigSectionMap*& curr_section);
        ConfigSectionMap& findOraddSectionInner_(const ustring& section_name);

        ustring delim_;
        ustring comment_;
        ustring section_;

        ustring curr_section_name_;

        fast_vector<ustring> lines_;    // saved source lines (without parsing)
        HashMap<u32, ustring, Hasher<ustring> > to_line_id_;        // maps key to line id
        HashMap<ConfigSectionMap, ustring/*section*/, Hasher<ustring>> data_;
    };
}


#include "Config.inl"
#endif //CONFIG_H
