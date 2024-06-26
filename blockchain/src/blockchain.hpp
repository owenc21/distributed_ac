/**
 * Declarations and definitions for Blockchain class and related types/macros
 * @author Owen Capell
*/

/**
 * @brief Node definition for Blockchain
*/
#ifndef BLOCKCHAIN
#define BLOCKCHAIN

#include <cstdint>
#include <memory>
#include <ctime>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

/* Macros for Block events and statuses */
#define BLOCK_EVT_START	0
#define BLOCK_EVT_REQUEST 1
#define BLOCK_EVT_INSERT 2

#define BLOCK_STATUS_ACCEPT 0
#define BLOCK_STATUS_REJECT 1

/* Macros for Payload Types */
#define PAYLOAD_EMPTY 0
#define PAYLOAD_SMARTC 1
#define PAYLOAD_REQ 2
#define PAYLOAD_ATTRIBUTE 3

/* Type definitions */


/**
 * @brief Definition for payload struct within block
*/

struct payload_struct{
	payload_struct();
	payload_struct(uint8_t payload_type);
	payload_struct(uint8_t payload_type, std::vector<std::string> attributes, uint64_t size, std::string name);
	uint8_t payload_type;
	std::vector<std::string> attributes;
	std::string name;
	uint64_t size;
};

typedef struct payload_struct block_payload;

/**
 * @brief Definition for Block within Blockchain
*/
struct node{
	node() = delete;
	node(uint8_t event, uint8_t status, uint64_t id, uint64_t block_id);
	std::shared_ptr<struct node> next;
	std::shared_ptr<block_payload> payload;
	std::time_t time_stamp;
	uint8_t event;
	uint8_t status;
	uint64_t id;
	uint64_t block_id;
};

typedef struct node block;

/* Blockchain declaration */

class Blockchain{
public:
	Blockchain();
	~Blockchain() = default;

	/**
	 * @brief Adds the given resource to the Blockchain
	*/
	int add_resource(uint64_t usr_id, std::vector<std::string> attributes, uint64_t size, const std::string& name);

	/**
	 * @brief Adds the given attribute and users associated to the Blockchain
	*/
	int add_attribute(uint64_t usr_id, std::vector<std::string> users, uint64_t num_users, const std::string& attribute);

	/**
	 * @brief Adds the given user (name and ID) to the Blockchain metadata
	*/
	int add_user(uint64_t usr_id, const std::string& name);

	/**
	 * @brief Requests the given resource (if it exists)
	*/
	int request_resource(uint64_t usr_id, const std::string& resource);

	/**
	 * @brief Audits entire blockchain by printing summary of each block to the given stream
	*/
	void audit(std::ostream& outStream);

	/**
	 * @brief Returns the number total simulated size (in bytes) of blockchain (on-chain storage)
	*/
	uint64_t get_onchain_size() { return total_size; }

	/**
	 * @brief Returns the simulated size (in bytes) of blockchain ignoring resource size (off-chain storage)
	*/
	uint64_t get_offchain_size() { return non_payload_size; }

	/**
	 * @brief Returns the total number of resources added
	*/
	uint64_t get_total_resources() { return total_resources; }

	/**
	 * @brief Return the total number of requests made
	*/
	uint64_t get_total_requests() { return total_requests; }

	/**
	 * @brief Returns total number of accepted requests made
	*/
	uint64_t get_accepted_requests() { return accepted_requests; }

	/**
	 * @brief Returns total number of rejected requests made
	*/
	uint64_t get_rejected_requests() { return total_requests - accepted_requests; }

	/**
	 * @brief Returns the associated name of a given user id
	*/
	std::string id2user(uint64_t usr_id) { return id2name[usr_id]; } 

	/**
	 * @brief Returns associted user id of a given name
	*/
	uint64_t user2id(const std::string& name) { return name2id[name]; }

private:
	std::shared_ptr<block> head;
	std::shared_ptr<block> next_free;
	std::unordered_map<uint64_t, std::string> id2name;
	std::unordered_map<std::string, uint64_t> name2id;
	std::unordered_set<std::string> attribute_list;
	std::unordered_set<std::string> resources;
	uint64_t next_id;
	uint64_t total_size;
	uint64_t non_payload_size;
	uint64_t total_resources;
	uint64_t total_requests;
	uint64_t accepted_requests;

	/**
	 * @brief Gets block containing payload with specified name
	*/
	std::shared_ptr<block> get_block_by_payload_name(const std::string& name);

	/**
	 * @brief Gets vector of users included in given attribute (if it exists)
	*/
	int get_attrbute(const std::string& attribute, std::vector<std::string>& users);

};

#endif