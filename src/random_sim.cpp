#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <random>
#include <chrono>

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
void process_input_file(std::string& file_name,
	std::vector<uint64_t>& user_list, std::vector<std::string>& file_list,
	Blockchain& blockchain){
    std::ifstream inputFile(file_name);

    if (inputFile.is_open()) {
		std::ofstream totalSizeFile("total_size_iter.txt");
		std::ofstream trimSizeFile("trim_size_iter.txt");
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
						user_list.push_back(user_id);
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

					blockchain.add_resource(usr_id, arguments, size, attr_name);
					file_list.push_back(attr_name);
				}

				totalSizeFile << blockchain.get_onchain_size() << std::endl;
				trimSizeFile << blockchain.get_offchain_size() << std::endl;
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

/**
 * Simulates for a given number of iterations 
 * Each itertion randonly selects a user to attempt to access a random file
 * Prints stats at end
 * 
 * @param user_list List of users ids
 * @param file_list List of files
 * @param blockchain	Reference to blockchain object from which to request files
 * @param iters 	Number of iterations to execute
*/
void simulate(std::vector<uint64_t>& user_list, std::vector<std::string>& file_list, Blockchain& blockchain, uint64_t iters){
	std::random_device rd1;
	std::mt19937 generator1(rd1());
	std::uniform_int_distribution<> dist1(0, user_list.size()-1);

	std::random_device rd2;
	std::mt19937 generator2(rd2());
	std::uniform_int_distribution<> dist2(0, file_list.size()-1);

	double total_time = 0.0;
	int success = 0;

	for(uint64_t n=iters; n>0; n--){
		int name_ind = dist1(generator1);
		int file_ind = dist2(generator2);

		auto start = std::chrono::high_resolution_clock::now();
		int req = blockchain.request_resource(user_list[name_ind], file_list[file_ind]);
		if(req>=0) success++;
		auto end = std::chrono::high_resolution_clock::now();

		std::chrono::duration<double, std::nano> elapsed_nanoseconds = end-start;
		total_time += elapsed_nanoseconds.count();
	}

	std::cout << "Average time (per access) (ns): " << total_time/iters << std::endl;
	std::cout << "Total accesses: " << iters << std::endl;
	std::cout << "Successful accesses: " << success << std::endl;
	std::cout << "Denied accesses: " << iters-success << std::endl;
}

int main(int argc, char** argv){
	if(argc != 4 && argc != 5){
		std::cout << "Invalid argument(s) given" << std::endl <<
			"Proper usage: ./random_sim -i <policy_file> -<(v)erbose|(o)utput}|(n)one> <num iters (optional)>" << std::endl;
	}
	
	uint64_t iters = 10000000;
	if(argc == 5) iters = static_cast<uint64_t>(std::stoi(argv[4]));

	Blockchain blockchain = Blockchain();

	/* For keeping track of users and resources to simulate */
	std::vector<uint64_t> users_list;
	std::vector<std::string> file_list;

	std::string file_name(argv[2]);
	process_input_file(file_name, users_list, file_list, blockchain);
	simulate(users_list, file_list, blockchain, iters);

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