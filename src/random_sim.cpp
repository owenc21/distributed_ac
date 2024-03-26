#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdint>

#include "blockchain.hpp"

/**
 * Takes the input file describing the access control specifications (users, roles, files)
 * Outputs the total simulated size (bytes) at each step in total_size.txt
 * Outputs the non-file simulated size (bytes) at each step in trim_size.txt
 * 
 * "USER:USER1:USER2:..."
 * "ROLE:ALLOWEDUSER1:ALLOWEDUSER2:...:ALLOWEDUSERN:N:ROLENAME"
 * "FILE:ALLOWEDROLE1:ALLOWEDROLE2:...:ALLOWEDROLEN:OWNERUSER:SIZE:NAME"
*/
void process_input_file(std::string& file_name, Blockchain& blockchain){
    std::ifstream inputFile(file_name);

    if (inputFile.is_open()) {
        std::string line;
        while (std::getline(inputFile, line)) {
            /* Field Extraction */
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string fieldName = line.substr(0, colonPos);

                std::string argumentsString = line.substr(colonPos + 1); 
                std::vector<std::string> arguments;
                std::stringstream ss(argumentsString);
                std::string arg;
                while (std::getline(ss, arg, ':')) { // Split by ':' delimiter
                    arguments.push_back(arg);
                }

				if(fieldName == "USER"){
					uint64_t user_id = 1;
					for(std::string& user : arguments){
						blockchain.add_user(user_id, user);
						user_id++;
					}
				}
				else if(fieldName == "ROLE"){
					std::string role_name = arguments.back();
					arguments.pop_back();
					uint64_t num_users = static_cast<uint64_t>(std::stoi(arguments.back()));
					arguments.pop_back();

					blockchain.add_attribute(0, arguments, num_users, role_name);
				}
				else if(fieldName == "FILE"){
					std::string attr_name = arguments.back();
					arguments.pop_back();
					uint64_t size = static_cast<uint64_t>(std::stoi(arguments.back()));
					arguments.pop_back();				
					std::string usr_name = arguments.back();
					arguments.pop_back();
					uint64_t usr_id = blockchain.user2id(usr_name);

					std::cout << usr_id << " " << size << " " << attr_name << std::endl;

					blockchain.add_resource(usr_id, arguments, size, attr_name);
				}
            } else {
				std::cerr << "Error parsing policy file!" << std::endl;
				inputFile.close();
				return;
            }            
        }
        inputFile.close();
    } else {
        std::cerr << "Error opening file!" << std::endl;
    }
}

int main(int argc, char** argv){
	if(argc != 4){
		std::cout << "Invalid argument(s) given" << std::endl <<
			"Proper usage: ./random_sim -i <policy_file> -<(v)erbose|(o)utput}|(n)one>" << std::endl;
	}
	
	Blockchain blockchain = Blockchain();

	std::string file_name(argv[2]);
	process_input_file(file_name, blockchain);

	std::string output_choice(argv[3]);
	if(output_choice == "-v"){
		blockchain.audit(std::cout);
	}
	else if(output_choice == "-o"){
		std::ofstream fileStream("blockchain_audit.txt");
		blockchain.audit(fileStream);
	}

	return 0;
}