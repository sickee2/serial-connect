#include <sscom.hpp>

int main(int argc, const char* argv[]){

  auto port = sscom::serial::create(argc, argv);

  // printf("\tbefore connect\n");
  //
  // port->connect();
  //
  // printf("\tafter connect\n");
  return 0;
}
