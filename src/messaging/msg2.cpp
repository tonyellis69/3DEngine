#include "msg2.h"




std::map<int, std::vector < std::shared_ptr<CDispatcherBase> > > slots;

std::unordered_map < std::type_index, int> objIds;