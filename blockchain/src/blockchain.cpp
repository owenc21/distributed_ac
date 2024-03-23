/**
 * Class definition for Blockchain
*/
#include <cstdint>
#include <memory>
#include <ctime>

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
 * Default constructor for Blockchain
 * Initializes underlying structure
*/
Blockchain::Blockchain(){
	head = std::make_shared<block>(BLOCK_EVT_START, BLOCK_STATUS_ACCEPT, 0, 0);
	next_free = head;	
	next_id = 1;
}

/**
 * Adds the given resource to the blockchain (on the end)
 * 
 * @param usr_id	uint64_t of the User ID making the insertion
 * @returns 		negaive integer on failure, non-negative integer on success
*/
int Blockchain::add_resource(uint64_t usr_id){
	next_free->next = std::make_shared<block>(BLOCK_EVT_INSERT, BLOCK_STATUS_ACCEPT, usr_id, next_id++);

	return 1;
}