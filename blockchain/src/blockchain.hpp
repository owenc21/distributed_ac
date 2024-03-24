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

/**
 * @brief Definition for payload struct within block
*/

struct payload_struct{
	payload_struct();
	payload_struct(uint8_t payload_type, std::vector<std::string> attributes, uint64_t size, std::string name);
	uint8_t payload_type;
	std::vector<std::string> attributes;
	std::string name;
	std::string desc;
	uint64_t size;
};

typedef struct payload_struct block_payload;

/* Blockchain declaration */

class Blockchain{
public:
	Blockchain();
	~Blockchain() = default;

	/**
	 * @brief Adds the given resource to the Blockchain
	*/
	int add_resource(uint64_t usr_id, std::vector<std::string> attributes, uint64_t size, std::string name);

	/**
	 * @brief Adds the given attribute and users associated to the Blockchain
	*/
	int add_attribute(uint64_t usr_id, std::vector<std::string> users, uint64_t num_users, std::string attribute);

	/**
	 * @brief Adds the given user (name and ID) to the Blockchain metadata
	*/
	int add_user(uint64_t usr_id, std::string name);

private:
	std::shared_ptr<block> head;
	std::shared_ptr<block> next_free;
	std::unordered_map<uint64_t, std::string> id2name;
	std::unordered_map<std::string, uint64_t> name2id;
	std::unordered_set<std::string> attributes;
	uint64_t next_id;
	uint64_t total_size;
	uint64_t non_payload_size;

};

#endif