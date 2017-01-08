#include "Config.h"

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
        std::istringstream ist(s_upper);
        ist >> asFloat;
        asInt = (int)asFloat;
        asBool= (asFloat!=0.0);
    }

    inline CfgValue CfgValue::operator=(const std::string& val) {
        return CfgValue(val);
    }


    inline Config::Config(std::string delim /* ="=" */, std::string comment /* ="#" */)
     : _delim(delim), _comment(comment)
    {
    }

    inline bool Config::loadFromFile(const std::string& filepath) {
        std::ifstream fs(filepath);
        if (!fs.good()) {
            std::cerr << "[Warning] Config: Could not open config file " << filepath << std::endl;
            return false;
        }

        fs >> (*this);
        return true;
    }

    inline void Config::loadFromVector(const fast_vector<std::string>& vec) {
        std::stringstream ss;
        for (size_t i=0; i<vec.size(); ++i) {
            ss << vec[i] << std::endl;
        }
        ss >> (*this);
    }

    inline void Config::_trim(std::string& s) {
    //static
        // Remove leading and trailing whitespace
        static const char whitespace[] = " \n\t\v\r\f";
        s.erase( 0, s.find_first_not_of(whitespace) );
        s.erase( s.find_last_not_of(whitespace)+1 );
    }

    inline void Config::addSourceLine_(const std::string& source_line)
    {
        size_t delim_len = _delim.length();         // length of separator

        std::string line_without_comms= source_line.substr( 0, source_line.find(_comment) );    // ignore comments
        size_t delim_pos = line_without_comms.find(_delim);
        if (delim_pos==std::string::npos)
            // no delimiter found
        {
            // just save source line
            _lines.push_back(source_line);
            return;
        }
        // extract key
        std::string key = line_without_comms.substr(0, delim_pos);
        std::string val = line_without_comms.substr(delim_pos+delim_len);
        //line.replace(0, delim_pos+delim_len, "");
        // remove whitechars
        Config::_trim(key);
        Config::_trim(val);
        // store to map
        data[key] = CfgValue(val);

        // check if this key was loaded before
        std::map<std::string, unsigned int>::iterator it = _to_line_id.find(key);
        if (it != _to_line_id.end())
        {
            // must correct source line
            _lines[it->second].replace(0, _lines[it->second].find(_comment), line_without_comms);
        }
        else
            // first time seeing this key, add source line
        {
            _lines.push_back(source_line);
        }
    }

    // stream operators
    inline std::istream& operator>>(std::istream& is, Config& c)
    {
        std::string source_line;

        while (is)
        {
            std::getline(is,source_line);        //get line
            c.addSourceLine_(source_line);
        }
        return is;
    }
    inline std::ostream& operator<<(std::ostream& os, Config& c)
    {
        for(unsigned int line_id=0; line_id<c._lines.size(); line_id++)
        {
            os << c._lines[line_id] << std::endl;
        }
        return os;
    }

}