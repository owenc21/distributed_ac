#include<iostream>

#include "blockchain.hpp"

int main(int argc, char** argv){
	
	Blockchain blockchain = Blockchain();

	for(int i=0; i<5; ++i){
		blockchain.add_resource(static_cast<uint64_t>(i));
	}
}