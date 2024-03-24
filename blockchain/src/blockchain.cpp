/**
 * Class definition for Blockchain
*/
#include <cstdint>
#include <memory>
#include <ctime>
#include <string>
#include <vector>
#include <unordered_map>

#include "blockchain.hpp"

/**
 * Constructor for block
 * Initializes fields with given values
*/
block::node(uint8_t event, uint8_t status, uint64_t id, uint64_t block_id)
	: time_stamp(std::time(nullptr)), event(event), status(status),
	id(id), block_id(block_id), next(nullptr)
{

}

/**
 * Default constructor for payload
 * Initialzes fields as null except for type
*/
block_payload::payload_struct()
	: payload_type(PAYLOAD_EMPTY), size(0)
{

}

/**
 * Construct for payload
 * Accepts arguments and initializes given fields
*/
block_payload::payload_struct(uint8_t payload_type, std::vector<std::string> attributes, uint64_t size, std::string name)
	: payload_type(payload_type), attributes(attributes), size(size), name(name)
{

}

/**
 * Default constructor for Blockchain
 * Initializes underlying structure
*/
Blockchain::Blockchain()
{
	head = std::make_shared<block>(BLOCK_EVT_START, BLOCK_STATUS_ACCEPT, 0, 0);
	next_free = head;
	head->payload = std::make_shared<block_payload>();
	head->payload->payload_type = PAYLOAD_EMPTY;
	next_id = 1;
	total_size = sizeof(head) + sizeof(head->payload);
	non_payload_size = sizeof(head) + sizeof(head->payload);
}

/**
 * Adds the given resource to the blockchain (on the end)
 * 
 * @param usr_id	uint64_t of the User ID making the insertion
 * @param attributes	vector of strings that list the attributes which gain access to the resource
 * @param size		Size (in bytes) of resource being added
 * @param name		Name of resource beng added
 * @returns 		negaive integer on failure, non-negative integer on success
*/
int Blockchain::add_resource(uint64_t usr_id, std::vector<std::string> attributes, uint64_t size, std::string name){
	next_free->next = std::make_shared<block>(BLOCK_EVT_INSERT, BLOCK_STATUS_ACCEPT, usr_id, next_id++);
	next_free = next_free->next;
	next_free->payload = std::make_shared<block_payload>(PAYLOAD_SMARTC, attributes, size, name);

	/* Include block and resource size for total size */
	uint64_t cur_block_size = sizeof(next_free) + sizeof(next_free->payload);
	total_size += cur_block_size + size;
	non_payload_size += cur_block_size;

	return 0;
}

/**
 * Adds the given attribute to the blockchain (on the end)
 * 
 * @param usr_id	uint64_t of User ID making the insersion
 * @param users		vector of strings containing names of users included in attribute
 * @param num_users	Number of users contained within attribute
 * @param attribute	Name of attribute (must be unique)
 * @returns			Negative integer on failure, Non-negative integer on success
*/
int Blockchain::add_attribute(uint64_t usr_id, std::vector<std::string> users, uint64_t num_users, std::string attribute){
	/* Ensure name of attribure not already added */
	if(attributes.contains(attribute)) return -1;

	next_free->next = std::make_shared<block>(BLOCK_EVT_INSERT, BLOCK_STATUS_ACCEPT, usr_id, next_id++);
	next_free = next_free->next;
	next_free->payload = std::make_shared<block_payload>(PAYLOAD_ATTRIBUTE, users, num_users, attribute);

	/* Exclude payload from size calculations for purposes of simulation */
	uint64_t cur_block_size = sizeof(next_free) + sizeof(next_free->payload);
	total_size += cur_block_size;
	non_payload_size += cur_block_size;

	return 0;
}

/**
 * Adds the given user (name and ID) to Blockchain metadata (represented as fields in class)
 * 
 * @param usr_id	uint64_t of User ID of User being added
 * @param name		Name of user being added (must be unique)
 * @returns 		Negative integer on failure, Non-negative integer on success
*/
int Blockchain::add_user(uint64_t usr_id, std::string name){
	/* Check if name to be inserted already exists */
	if(name2id.find(name) == name2id.end()) return -1;

	name2id[name] = usr_id;
	id2name[usr_id] = name;

	return 0;
}