/**
 * Class definition for Blockchain
*/
#include <cstdint>
#include <memory>
#include <ctime>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

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
 * Constructor for payload
 * Accepts payload type, default initializes rest of fields
*/
block_payload::payload_struct(uint8_t payload_type)
	: payload_type(payload_type), size(0)
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
	: total_resources(0), total_requests(0), accepted_requests(0)
{
	head = std::make_shared<block>(BLOCK_EVT_START, BLOCK_STATUS_ACCEPT, 0, 0);
	next_free = head;
	head->payload = std::make_shared<block_payload>();
	next_id = 1;
	total_size = sizeof(head) + sizeof(head->payload);
	non_payload_size = sizeof(head) + sizeof(head->payload);
}

/**
 * Adds the given resource to the blockchain (on the end)
 * Acts as Policy Information Pont (PIP)
 * 
 * @param usr_id	uint64_t of the User ID making the insertion
 * @param attributes	vector of strings that list the attributes which gain access to the resource
 * @param size		Size (in bytes) of resource being added
 * @param name		Name of resource beng added
 * @returns 		negaive integer on failure, non-negative integer on success
*/
int Blockchain::add_resource(uint64_t usr_id, std::vector<std::string> attributes, uint64_t size, const std::string& name){
	/* Check that attributes being added exist and that name of resource not accounted for */
	if(resources.contains(name)) return -1;
	for(std::string& attribute : attributes){
		if(!attribute_list.contains(attribute)) return -1;
	}
	resources.insert(name);

	next_free->next = std::make_shared<block>(BLOCK_EVT_INSERT, BLOCK_STATUS_ACCEPT, usr_id, next_id++);
	next_free = next_free->next;
	next_free->next = nullptr;
	next_free->payload = std::make_shared<block_payload>(PAYLOAD_SMARTC, attributes, size, name);

	/* Include block and resource size for total size */
	uint64_t cur_block_size = sizeof(next_free) + sizeof(next_free->payload);
	total_size += cur_block_size + size;
	non_payload_size += cur_block_size;

	total_resources++;

	return 0;
}

/**
 * Adds the given attribute to the blockchain (on the end)
 * 
 * @param usr_id	uint64_t of User ID making the insersion
 * @param users		vector of strings containing names of users included in attribute (must be non-empty)
 * @param num_users	Number of users contained within attribute
 * @param attribute	Name of attribute (must be unique)
 * @returns			Negative integer on failure, Non-negative integer on success
*/
int Blockchain::add_attribute(uint64_t usr_id, std::vector<std::string> users, uint64_t num_users, const std::string& attribute){
	/* Ensure name of attribure not already added and vector of users non-empty */
	if(attribute_list.contains(attribute)) return -1;
	if(users.empty()) return -1;
	attribute_list.insert(attribute);

	next_free->next = std::make_shared<block>(BLOCK_EVT_INSERT, BLOCK_STATUS_ACCEPT, usr_id, next_id++);
	next_free = next_free->next;
	next_free->next = nullptr;
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
int Blockchain::add_user(uint64_t usr_id, const std::string& name){
	/* Check if name to be inserted already exists */
	if(name2id.find(name) != name2id.end()) return -1;

	name2id[name] = usr_id;
	id2name[usr_id] = name;

	return 0;
}

/**
 * Gets block containing payload with specified name
 * 
 * @param name	String of the payload name
 * @returns 	shared_ptr<block> of the block containing the payload with specified name (null if doesn't exist)
*/
std::shared_ptr<block> Blockchain::get_block_by_payload_name(const std::string& name){
	std::shared_ptr<block> to_return;
	to_return = head;
	while(to_return != nullptr){
		if(to_return->payload->payload_type == PAYLOAD_EMPTY){
			to_return = to_return->next;
			continue;
		}

		std::string payload_name = to_return->payload->name;
		if(name == payload_name){
			break;
		}

		to_return = to_return->next;
	}

	return to_return;
}

/**
 * Retrieves the users contained with the specified attribute (must exist)
 * Stores the users in a provided std::vector<std::string>>&
 * Overwrites all elements
 * 
 * @param attribute		Name of attribute to search for
 * @param users			Reference to vector of strings to store users contained in attribute
 * @returns 			Negative integer on failure, Non-negative integer on success
*/
int Blockchain::get_attrbute(const std::string& attribute, std::vector<std::string>& users){
	/* Ensure that attribute exists */
	if(!attribute_list.contains(attribute)) return -1;

	users.clear();
	
	std::shared_ptr<block> attribute_block = get_block_by_payload_name(attribute);
	users.resize(attribute_block->payload->attributes.size());
	std::copy(attribute_block->payload->attributes.begin(),
	 	attribute_block->payload->attributes.end(),
		users.begin());

	return 0;
}

/**
 * Attempts to retrieve the specified resource
 * Simulates smart contract by findng block in which resource is stored and determining if user has permission
 * Serves as Policy Retrival Point (PRP), Policy Decision Point (PDP) and Policy Enforcement Point (PEP)
 * Assumes requested resource exists; does not check
 * 
 * @param usr_id	User ID of User making request for resource
 * @param resource	Name of resource beng requested
 * @returns			Non-negative integer if successful, negative integer if unsuccessful
*/
int Blockchain::request_resource(uint64_t usr_id, const std::string& resource){
	total_requests++;
	std::string user_name = id2name[usr_id];
	std::shared_ptr<block> resource_block = get_block_by_payload_name(resource);

	/* Add record of request to blockchain */
	next_free->next = std::make_shared<block>(BLOCK_EVT_REQUEST, BLOCK_STATUS_REJECT, usr_id, next_id++);
	next_free = next_free->next;
	next_free->next = nullptr;
	next_free->payload = std::make_shared<block_payload>(PAYLOAD_REQ);

	/* Exclude payload from size calculations for purposes of simulation */
	uint64_t cur_block_size = sizeof(next_free) + sizeof(next_free->payload);
	total_size += cur_block_size;
	non_payload_size += cur_block_size;

	/* Check to see if user is in any of the given attributes specified within resource's smart contract */	
	for(std::string& attribute : resource_block->payload->attributes){
		std::vector<std::string> users_in_attribute;
		if(get_attrbute(attribute, users_in_attribute) < 0) continue;
		/* Check if user exists in the attribute */
		if(std::find(users_in_attribute.begin(), users_in_attribute.end(), user_name) != users_in_attribute.end()){
			next_free->status = BLOCK_STATUS_ACCEPT;
			accepted_requests++;
			return 0;
		}
	}

	return -1;
}