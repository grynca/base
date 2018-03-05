#include "Config.h"
#include <fstream>

namespace grynca {

    inline CfgValue::CfgValue()
     : asString(""),asFloat(0.0f),asInt(0),asBool(false)
    {
    }

    inline CfgValue::CfgValue(const ustring& val)
     : asString(val),
       asFloat(0.0f)
    {
        ustring s_upper = val;
        ustring::iterator it;
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

    inline CfgValue CfgValue::operator=(const ustring& val) {
        return CfgValue(val);
    }


    inline Config::Config(const ustring& delim /* ="=" */, const ustring& comment /* ="#" */, const ustring& section /*="!!"*/)
     : delim_(delim), comment_(comment), section_(section)
    {
    }

    inline bool Config::loadFromFile(const ustring& filepath, const ustring& section /*= ""*/) {
        std::ifstream fs(filepath.cpp_str());
        if (!fs.good()) {
            std::cerr << "[Warning] Config: Could not open config file " << filepath << std::endl;
            return false;
        }

        curr_section_name_ = section;
        fs >> (*this);
        return true;
    }

    inline void Config::loadFromVector(const fast_vector<ustring>& vec, const ustring& section /*= ""*/) {
        curr_section_name_ = section;
        ConfigSectionMap * curr_section  = data_.findItem(curr_section_name_);

        std::istringstream ss;
        for (size_t i=0; i<vec.size(); ++i) {
            addSourceLine_(vec[i]+"\n", curr_section);
        }
    }

    inline Config::ConfigSectionMap& Config::accData(const ustring& section /*= ""*/) {
        return findOraddSectionInner_(curr_section_name_);
    }

    inline Config::ConfigSectionMap& Config::accCurrData() {
        return findOraddSectionInner_(curr_section_name_);
    }

    inline const ustring& Config::getCurrentSectionName()const {
        return curr_section_name_;
    }

    inline void Config::setCurrentSectionName(const ustring &section) {
        curr_section_name_ = section;
    }

    inline void Config::trim_(ustring &s) {
    //static
        // Remove leading and trailing whitespace
        static const c32 whitespace[] = U" \n\t\v\r\f";
        s.erase( 0, s.find_first_not_of(whitespace) );
        s.erase( s.find_last_not_of(whitespace)+1 );
    }

    inline void Config::addSourceLine_(const ustring& source_line, ConfigSectionMap*& curr_section) {
        size_t delim_len = delim_.length();
        size_t section_len = section_.length();


        ustring line_without_comms= source_line.substr( 0, source_line.find(comment_) );    // ignore comments
        size_t delim_pos = line_without_comms.find(delim_);
        if (delim_pos==ustring::npos) {
            ustring sl = source_line;
            trim_(sl);
            if (sl.find(section_) == 0) {
                // change section
                curr_section_name_ = sl.substr(section_len);
                trim_(curr_section_name_);
                curr_section = &accCurrData();
            }

            // no delimiter found
            // just save source line
            lines_.push_back(source_line);
            return;
        }
        // extract key
        ustring key = line_without_comms.substr(0, delim_pos);
        ustring val = line_without_comms.substr(delim_pos+delim_len);
        //line.replace(0, delim_pos+delim_len, "");
        // remove whitechars
        Config::trim_(key);
        Config::trim_(val);
        // store to map
        new (curr_section->addItem(key)) CfgValue(val);

        // check if this key was loaded before
        u32* line_id = to_line_id_.findItem(key);
        if (line_id) {
            // must correct source line
            lines_[*line_id].replace(0, lines_[*line_id].find(comment_), line_without_comms);
        }
        else {
            // first time seeing this key, add source line
            lines_.push_back(source_line);
        }
    }

    inline Config::ConfigSectionMap& Config::findOraddSectionInner_(const ustring& section_name) {
        bool was_added;
        Config::ConfigSectionMap* rslt = data_.findOrAddItem(section_name, was_added);
        if (was_added)
            new (rslt) Config::ConfigSectionMap();
        return *rslt;
    }

    // stream operators
    inline std::istream& operator>>(std::istream& is, Config& c) {
        ustring source_line;
        bool eof;
        Config::ConfigSectionMap* curr_section = &c.accCurrData();
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