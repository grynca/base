#ifndef OPTIONS_H
#define OPTIONS_H

#include "containers/fast_vector.h"
#include <map>

namespace grynca {
    class no_argument_for_required : public std::invalid_argument {
    public:
        no_argument_for_required(ustring option_name) :
                std::invalid_argument(std::string("REQUIRED option '") + option_name.c_str() + "' without argument") {}
    };

    class argument_for_none : public std::invalid_argument {
    public:
        argument_for_none(ustring option_name) :
                std::invalid_argument(std::string("NONE option '") + option_name.c_str() + "' with argument") {}
    };

    enum ArgumentType { atNONE, atREQUIRED, atOPTIONAL };

    class OptionsHandler {
    private:

        struct Option {

            c32 short_name;
            ustring long_name;
            ArgumentType type;
            bool multiple;

            Option() {}

            Option( c32         _short_name,
                    ustring  _long_name,
                    ArgumentType _type,
                    bool         _multiple) :
                    short_name(_short_name),
                    long_name(_long_name),
                    type(_type),
                    multiple(_multiple) {}
        };

        // Properties
        fast_vector<Option> declared_options;
        fast_vector<ustring> input;
        std::map<ustring, fast_vector<ustring> > parsed_input;

        // Private Methods
        void update(const Option & option);
        void update_none(const Option & option,
                         fast_vector<ustring>::iterator str);
        void update_required(const Option & option,
                             fast_vector<ustring>::iterator str);
        void update_optional(const Option & option,
                             fast_vector<ustring>::iterator str);

    public:
        OptionsHandler(int argc, char** argv) {
            input.insert(input.begin(), argv+1, argv+argc);
        }

        // Public Methods
        OptionsHandler& add_option(c32 short_name,
                            ustring long_name,
                            ArgumentType type,
                            bool multiple);

        // bool get_option(c32 short_name);
        bool get_option(ustring long_name);

        // ustring get_argument(c32 short_name);
        ustring get_argument(ustring long_name);
        fast_vector<ustring> get_arguments(ustring long_name);

        bool is_short(ustring s) {
            return ((s.at(0) == '-') && (s.at(1) != '-'));
        }
        bool is_long(ustring s)  {
            return ((s.at(0) == '-') && (s.at(1) == '-'));
        }
    };

/**********************************************************
*
* @class: OptionsHandler
* @method: add_option
* @description: Sets a new option to handle from input.
*
**********************************************************/

    inline OptionsHandler& OptionsHandler::add_option(c32 short_name,
                                        ustring long_name,
                                        ArgumentType type,
                                        bool multiple) {

        // Add to declared options
        declared_options.push_back(Option(short_name, long_name, type, multiple));
        // Re-assess input
        update(declared_options.back());
        // Chain adding options
        return *this;
    }

/**********************************************************
*
* @class: OptionsHandler
* @method: get_option
* @description: Returns whether an option has been passed
* from input, given the long name of the option.
*
**********************************************************/

    inline bool OptionsHandler::get_option(ustring name) {
        return ((parsed_input.find(name) != parsed_input.end()) && !parsed_input.empty());
    }

/**********************************************************
*
* @class: OptionsHandler
* @method: get_argument
* @description: Returns the argument passed with a given
* option.
*
**********************************************************/

    inline ustring OptionsHandler::get_argument(ustring name) {
        if (!get_option(name) || parsed_input.at(name).empty())
            return "";
        else
            return parsed_input.at(name).front();
    }

/**********************************************************
*
* @class: OptionsHandler
* @method: get_argument
* @description: Returns the vector of arguments passed with
* a given option.
*
**********************************************************/

    inline fast_vector<ustring> OptionsHandler::get_arguments(ustring name) {
        if (!get_option(name))
            return fast_vector<ustring>();
        else
            return parsed_input.at(name);
    }

/**********************************************************
*
* @class: OptionsHandler
* @method: update
* @description: Given an option, update the parsed argument
* map appropriately.
*
**********************************************************/

    inline void OptionsHandler::update(const Option & option) {
        // Get start of input
        fast_vector<ustring>::iterator str = input.begin();
        while (str != input.end()) {
            // If there was no input
            if ((is_long(*str) || is_short(*str)) &&
                ((*str).at(1) == option.short_name ||
                 (*str).substr(2) == option.long_name)) {

                // Create empty vector if type = none
                if (option.type == atNONE)
                    update_none(option, str);

                if(option.type == atREQUIRED)
                    update_required(option, str);

                if(option.type == atOPTIONAL)
                    update_optional(option, str);

            }
            // Next input
            str += 1;
        }
    }

/**********************************************************
*
* @class: OptionsHandler
* @method: update_none
* @description: If the ArgumentType doesn't require an
* argument, either throw an error (if given an argument)
* or insert it into the map if it doesn't already exist.
*
**********************************************************/

    inline void OptionsHandler::update_none(const Option & option, fast_vector<ustring>::iterator str) {
        if (((str+1) != input.end()) && (!is_long(*(str+1)) && !is_short(*(str+1))))
            throw argument_for_none(option.long_name);

        // Only if it doesn't already exist in map
        if (parsed_input.find(option.long_name) == parsed_input.end()) {
            // Insert empty vector
            parsed_input.insert(
                    std::make_pair(option.long_name, fast_vector<ustring>())
            );
        }
    }

/**********************************************************
*
* @class: OptionsHandler
* @method: update_required
* @description: If the ArgumentType does require an argument,
* throw an error (if not given an argument) or handle it
* like an optional argument.
*
**********************************************************/

    inline void OptionsHandler::update_required(const Option & option, fast_vector<ustring>::iterator str) {
        if (((str+1) == input.end()) || (is_long(*(str+1)) || is_short(*(str+1))))
            throw no_argument_for_required(option.long_name);

        update_optional(option, str);
    }

/**********************************************************
*
* @class: OptionsHandler
* @method: update_optional
* @description: If the the option allows multiple arguments
* passed to an option (Ex. --person Haoran --person Ryan or
* --person Ryan Haoran Guan) then push arguments into a
* vector. Else, the last argument passed will be the
* argument set for the option in the map.
*
**********************************************************/

    inline void OptionsHandler::update_optional(const Option & option, fast_vector<ustring>::iterator str) {
        // Increment to check the argument
        if(str+1 != input.end()) str++;
        // If the iterator is not at the end and we haven't encountered another option
        while(str != input.end() && !is_long(*str) && !is_short(*str)) {
            // If the option doesn't already exist in map
            if(parsed_input.find(option.long_name) == parsed_input.end()) {
                // Insert into parsed options map
                parsed_input.insert(
                        std::make_pair(option.long_name, fast_vector<ustring>(1, *(str)))
                );
            }
            else {
                // If the option allows multiple arguments to it,
                // push into arguments vector
                if(option.multiple)
                    parsed_input.at(option.long_name).push_back(*(str));
                else
                    parsed_input.at(option.long_name)[0] = *str;
            }
            // Next input
            str += 1;
        }
    }
}
#endif //OPTIONS_H
