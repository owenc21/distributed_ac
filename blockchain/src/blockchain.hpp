/**
 * Declarations and definitions for Blockchain class and related types/macros
 * @author Owen Capell
*/

/**
 * @brief Node definition for Blockchain
*/

#include <cstdint>
#include <memory>
#include <ctime>

/* Macros for Block events and statuses */
#define BLOCK_EVT_START	0
#define BLOCK_EVT_REQUEST 1
#define BLOCK_EVT_INSERT 2

#define BLOCK_STATUS_ACCEPT 0
#define BLOCK_STATUS_REJECT 1

/* Type definitions */

/**
 * @brief Definition for Block within Blockchain
*/
struct node{
	node() = delete;
	node(uint8_t event, uint8_t status, uint64_t id, uint64_t block_id);
	std::shared_ptr<struct node> next;
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
	int add_resource(uint64_t usr_id);

private:
	std::shared_ptr<block> head;
	std::shared_ptr<block> next_free;
	uint64_t next_id;

};