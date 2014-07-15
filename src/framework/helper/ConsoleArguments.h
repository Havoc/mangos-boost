/*
* This file is part of the CMaNGOS Project. See AUTHORS file for Copyright information
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef CONSOLE_ARGUMENTS_H_
#define CONSOLE_ARGUMENTS_H_

#include <boost/program_options.hpp>
#include "Common.h"

namespace cmangos {
namespace helper {

using namespace boost::program_options;

class ConsoleArguments
{
public:
    typedef options_description Options;
    typedef variables_map Variables;

    ConsoleArguments(std::string& configuration_file)
    {
        allowed_options_;
        allowed_options_.add_options()
            ("help,h", "print usage message")
            ("config,c", value<std::string>(&configuration_file)->default_value(configuration_file), "use <arg> as configuration file")
            ;
    }

    void Parse(int ac, char* av[])
    {
        try
        {
            store(command_line_parser(ac, av).options(allowed_options_).allow_unregistered().run(), parsed_options_);
            notify(parsed_options_);
        }
        catch (std::exception& e)
        {
            std::cerr << e.what() << "\n";
        }
    }

    void AddOptions(options_description& options)
    {
        allowed_options_.add(options);
    }

    variables_map Get()
    {
        return &parsed_options_;
    }

    void PrintAllowedOptions()
    {
        std::cout << allowed_options_ << "\n";
    }

private:
    options_description allowed_options_ = options_description("Allowed options");
    variables_map parsed_options_;
};

} // namespace helper
} // namespace cmangos

#endif // CONSOLE_ARGUMENTS_H_