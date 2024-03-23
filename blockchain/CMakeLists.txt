add_library(
	blockchain
	STATIC
	src/blockchain.hpp
	src/blockchain.cpp
)

target_include_directories(blockchain PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/src"
)