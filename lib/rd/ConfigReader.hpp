#pragma once
#include "net/Connector.hpp"
#include "net/ConnectorTcp.h"
#include "net/ConnectorSocks5.hpp"
#include <libconfig.h++>

namespace Rd
{
	struct Config
	{
		std::string imprint;
		std::string secret;
		std::string name;
		std::vector<Net::Connector*> connectors;
		int connectionTimeout = 0;

		Config(const std::string& path)
		{	
			for (;;)
			{
				try
				{
					read(path);
					break;
				}
				catch (std::exception const& e)
				{
					std::cout << "config exception: " <<  e.what() << std::endl;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(2000));
			}
		}

		void read(const std::string& path)
		{
			libconfig::Config cfg;
			cfg.readFile(path.c_str());

			name = cfg.lookup("name").c_str();
			imprint = cfg.lookup("imprint").c_str();
			secret = cfg.lookup("secret").c_str();
			cfg.lookupValue("connectionTimeout", connectionTimeout);

			const libconfig::Setting& root = cfg.getRoot();
			const libconfig::Setting& servers = root["servers"];
			int count = servers.getLength();

			for (int i = 0; i < count; ++i)
			{
				std::string type = servers[i].lookup("type").c_str();
				if (type == "tcp")
				{
					int port = 0;
					std::string address;
					servers[i].lookupValue("address", address);
					servers[i].lookupValue("port", port);
					connectors.push_back(new Net::ConnectorTcp(address, port));
				}

				if (type == "socks5")
				{
					int proxyPort = 0;
					std::string proxyAddress;
					int port = 0;
					std::string address;
					servers[i].lookupValue("address", address);
					servers[i].lookupValue("port", port);
					servers[i].lookupValue("proxyAddress", proxyAddress);
					servers[i].lookupValue("proxyPort", proxyPort);
					connectors.push_back(new Net::ConnectorSocks5(address, port, proxyAddress, proxyPort));
				}
			}
		}


	private:
		
	};
 


}
