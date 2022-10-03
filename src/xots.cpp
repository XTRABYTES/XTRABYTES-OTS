/*
Copyright 2022 XTRABYTES
Copyright 2022 Zoltan Szabo

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 3 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
*/

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <openssl/rand.h>
#include <iostream>
#include <sstream>

#include "Poco/Util/Application.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"

using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;
using Poco::Util::OptionCallback;

class XotsApp: public Application {
	
public:
	XotsApp(): _helpRequested(false) {}

	void defineOptions(OptionSet& options) {
		
			Application::defineOptions(options);
	
			options.addOption(
				Option("help", "h", "display help information on command line arguments")
					.required(false)
					.repeatable(false)
					.callback(OptionCallback<XotsApp>(this, &XotsApp::handleHelp)));
							
	}
	
   void handleHelp(const std::string& name, const std::string& value)
	{
		_helpRequested = true;		
		stopOptionsProcessing();
	}
	
	
	void displayHelp()
	{
		HelpFormatter helpFormatter(options());
		helpFormatter.setCommand(commandName());
		helpFormatter.setUsage("command OPTIONS");
		helpFormatter.setHeader("This application will demonstrate some of the features of XTRABYTES-XOT");		
		helpFormatter.format(std::cout);
	}
	
		
	int main(const std::vector<std::string>& args)
	{
		if (_helpRequested || args.empty())
		{
			displayHelp();
		}
		else
		{
			for (std::vector<std::string>::const_iterator it = args.begin(); it != args.end(); ++it)
			{}				
		}
		return Application::EXIT_OK;
	}

	
	private:
	
	bool _helpRequested;
};
	
POCO_APP_MAIN(XotsApp)

