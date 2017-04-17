#include "Config.h"
#include <fstream>

namespace grynca {

    inline CfgValue::CfgValue()
     : asString(""),asFloat(0.0f),asInt(0),asBool(false)
    {
    }

    inline CfgValue::CfgValue(const std::string& val)
     : asString(val),
       asFloat(0.0f)
    {
        std::string s_upper = val;
        std::string::iterator it;
        for( it = s_upper.begin(); it!=s_upper.end(); it++ )
            *it = toupper(*it);  // make all caps
        if ( !s_upper.compare("FALSE") || !s_upper.compare("NO"))
            s_upper = "0";
        else if ( !s_upper.compare("TRUE") || !s_upper.compare("YES"))
            s_upper = "1";

        // use ss for converting to numbers
        asFloat = ssu::fromString(s_upper, 0.0f);
        asInt = (i32)asFloat;
        asBool= (asFloat!=0.0);
    }

    inline CfgValue::CfgValue(i32 val)
     : asString(ssu::toStringA(val)),
       asFloat(f32(val)),
       asInt(val),
       asBool(bool(val))
    {}

    inline CfgValue::CfgValue(bool val)
    : asString(ssu::toStringA(val)),
      asFloat(f32(val)),
      asInt(i32(val)),
      asBool(val)
    {}

    inline CfgValue::CfgValue(f32 val)
    : asString(ssu::toStringA(val)),
      asFloat(val),
      asInt(i32(val)),
      asBool(val!=0.0f)
    {}

    inline CfgValue CfgValue::operator=(const std::string& val) {
        return CfgValue(val);
    }


    inline Config::Config(const std::string& delim /* ="=" */, const std::string& comment /* ="#" */, const std::string& section /*="!!"*/)
     : delim_(delim), comment_(comment), section_(section)
    {
    }

    inline bool Config::loadFromFile(const std::string& filepath, const std::string& section /*= ""*/) {
        std::ifstream fs(filepath);
        if (!fs.good()) {
            std::cerr << "[Warning] Config: Could not open config file " << filepath << std::endl;
            return false;
        }

        curr_section_name_ = section;
        fs >> (*this);
        return true;
    }

    inline void Config::loadFromVector(const fast_vector<std::string>& vec, const std::string& section /*= ""*/) {
        curr_section_name_ = section;
        ConfigSectionMap* curr_section = &data_[curr_section_name_];

        std::istringstream ss;
        for (size_t i=0; i<vec.size(); ++i) {
            addSourceLine_(vec[i]+"\n", curr_section);
        }
    }

    inline const Config::ConfigSectionMap& Config::getData(const std::string& section /*= ""*/) {
        return data_[section];
    }

    inline Config::ConfigSectionMap& Config::accData(const std::string& section /*= ""*/) {
        return data_[section];
    }

    inline const std::string& Config::getCurrentSectionName()const {
        return curr_section_name_;
    }

    inline void Config::setCurrentSectionName(const std::string &section) {
        curr_section_name_ = section;
    }

    inline void Config::trim_(std::string &s) {
    //static
        // Remove leading and trailing whitespace
        static const char whitespace[] = " \n\t\v\r\f";
        s.erase( 0, s.find_first_not_of(whitespace) );
        s.erase( s.find_last_not_of(whitespace)+1 );
    }

    inline void Config::addSourceLine_(const std::string& source_line, ConfigSectionMap*& curr_section) {
        size_t delim_len = delim_.length();
        size_t section_len = section_.length();


        std::string line_without_comms= source_line.substr( 0, source_line.find(comment_) );    // ignore comments
        size_t delim_pos = line_without_comms.find(delim_);
        if (delim_pos==std::string::npos) {
            std::string sl = source_line;
            trim_(sl);
            if (std::equal(sl.begin(), sl.begin()+section_len, section_.begin())) {
                // change section
                curr_section_name_ = sl.substr(section_len);
                trim_(curr_section_name_);
                curr_section = &data_[curr_section_name_];
            }

            // no delimiter found
            // just save source line
            lines_.push_back(source_line);
            return;
        }
        // extract key
        std::string key = line_without_comms.substr(0, delim_pos);
        std::string val = line_without_comms.substr(delim_pos+delim_len);
        //line.replace(0, delim_pos+delim_len, "");
        // remove whitechars
        Config::trim_(key);
        Config::trim_(val);
        // store to map
        (*curr_section)[key] = CfgValue(val);

        // check if this key was loaded before
        std::map<std::string, u32>::iterator it = to_line_id_.find(key);
        if (it != to_line_id_.end()) {
            // must correct source line
            lines_[it->second].replace(0, lines_[it->second].find(comment_), line_without_comms);
        }
        else {
            // first time seeing this key, add source line
            lines_.push_back(source_line);
        }
    }

    // stream operators
    inline std::istream& operator>>(std::istream& is, Config& c) {
        std::string source_line;
        bool eof;
        Config::ConfigSectionMap* curr_section = &c.data_[c.curr_section_name_];
        do {
            eof = !ssu::getLineA(is, source_line);
            c.addSourceLine_(source_line, curr_section);
        } while (!eof);
        return is;
    }

    inline std::ostream& operator<<(std::ostream& os, Config& c) {
        for(u32 line_id=0; line_id<c.lines_.size(); line_id++)
        {
            os << c.lines_[line_id] << std::endl;
        }
        return os;
    }

}